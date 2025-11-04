#include "libclang_parser.h"
#include "parser_utils.h"
#include <clang-c/Index.h>
#include <iostream>
#include <stack>
#include <filesystem>
#include <cassert>

using namespace cppuml;
using namespace cppuml::model;
using parser_utils::fromCXType;
using parser_utils::toStringAndDispose;
using parser_utils::cxAccessToAccess;

// State for AST traversal
struct VisitState {
    std::shared_ptr<TranslationUnit> tu;
    std::vector<std::shared_ptr<Namespace>> ns_stack;
    std::vector<std::shared_ptr<Class>> class_stack;
    CXFile main_file = nullptr;
    std::string filename;
    VisitState(const std::string& f)
        : filename(f), tu(std::make_shared<TranslationUnit>()) { tu->filename = f; }
};

static bool isCursorInMainFile(CXCursor cursor, CXFile main_file) {
    if (!main_file) return false;
    CXSourceLocation loc = clang_getCursorLocation(cursor);
    CXFile file; unsigned line, column, offset;
    clang_getSpellingLocation(loc, &file, &line, &column, &offset);
    return file == main_file;
}

// Utility to extract template parameters from a class or method cursor
void extractTemplateParameters(CXCursor cursor, std::vector<std::string>& outParams) {
    clang_visitChildren(cursor, [](CXCursor c, CXCursor, CXClientData client_data) {
        CXCursorKind kind = clang_getCursorKind(c);
        if (kind == CXCursor_TemplateTypeParameter ||
            kind == CXCursor_NonTypeTemplateParameter ||
            kind == CXCursor_TemplateTemplateParameter) {
            auto* params = static_cast<std::vector<std::string>*>(client_data);
            params->push_back(toStringAndDispose(clang_getCursorSpelling(c)));
        }
        return CXChildVisit_Continue;
    }, &outParams);
}

static CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    auto* state = static_cast<VisitState*>(client_data);
    CXCursorKind k = clang_getCursorKind(cursor);

    // Only process nodes from the main file
    if (!isCursorInMainFile(cursor, state->main_file)) {
        return CXChildVisit_Continue;
    }

    if (k == CXCursor_Namespace) {
        std::string ns_name = toStringAndDispose(clang_getCursorSpelling(cursor));
        auto ns = std::make_shared<Namespace>();
        ns->name = ns_name;
        ns->parent = state->ns_stack.empty() ? std::weak_ptr<Namespace>() : state->ns_stack.back();
        if (!state->ns_stack.empty()) state->ns_stack.back()->namespaces.push_back(ns);
        else state->tu->namespaces.push_back(ns);

        state->ns_stack.push_back(ns);
        clang_visitChildren(cursor, visitor, client_data);
        state->ns_stack.pop_back();
        return CXChildVisit_Continue;
    }

    // Classes & Structs
    if (k == CXCursor_ClassDecl || k == CXCursor_StructDecl || k == CXCursor_ClassTemplate) {
        if (clang_isCursorDefinition(cursor) == 0) return CXChildVisit_Continue;
        auto cls = std::make_shared<Class>();
        cls->name = toStringAndDispose(clang_getCursorSpelling(cursor));
        cls->usr  = toStringAndDispose(clang_getCursorUSR(cursor));
        cls->isStruct = (k == CXCursor_StructDecl);

        // Template detection (child traversal)
        if (k == CXCursor_ClassTemplate) {
            cls->isTemplate = true;
            extractTemplateParameters(cursor, cls->templateParams);
        } else {
            // Defensive: for regular class with template parameters (rare), may still have children
            extractTemplateParameters(cursor, cls->templateParams);
            if (!cls->templateParams.empty()) {
                cls->isTemplate = true;
            }
        }

        // Nested classes
        if (!state->class_stack.empty())
            state->class_stack.back()->nested.push_back(cls);

        // Namespace organization
        if (!state->ns_stack.empty())
            state->ns_stack.back()->classes.push_back(cls);
        else {
            if (state->tu->namespaces.empty()) {
                auto global_ns = std::make_shared<Namespace>();
                global_ns->name = "";
                state->tu->namespaces.push_back(global_ns);
            }
            state->tu->namespaces.front()->classes.push_back(cls);
        }
        state->tu->classes_by_usr[cls->usr] = cls;

        state->class_stack.push_back(cls);
        clang_visitChildren(cursor, visitor, client_data);
        state->class_stack.pop_back();
        return CXChildVisit_Continue;
    }

    // Fields
    if (k == CXCursor_FieldDecl) {
        if (state->class_stack.empty()) return CXChildVisit_Continue;
        Field f;
        f.name = toStringAndDispose(clang_getCursorSpelling(cursor));
        f.type = fromCXType(clang_getCursorType(cursor));
        f.access = cxAccessToAccess(clang_getCXXAccessSpecifier(cursor));
        f.usr = toStringAndDispose(clang_getCursorUSR(cursor));
        f.isStatic = clang_Cursor_getStorageClass(cursor) == CX_SC_Static;
        // Default value not extracted for now.
        state->class_stack.back()->fields.push_back(std::move(f));
        return CXChildVisit_Continue;
    }

    // Methods: regular, ctor, dtor
    if (k == CXCursor_CXXMethod || k == CXCursor_Constructor || k == CXCursor_Destructor ||
        k == CXCursor_FunctionTemplate) {
        if (state->class_stack.empty()) return CXChildVisit_Continue;
        Method m;
        m.name = toStringAndDispose(clang_getCursorSpelling(cursor));
        CXType cxtype = clang_getCursorType(cursor);
        if (k == CXCursor_CXXMethod || k == CXCursor_FunctionTemplate)
            m.returnType = fromCXType(clang_getResultType(cxtype));
        else
            m.returnType = TypeName{}; // ctors/dtors: no return type

        m.isConst = clang_CXXMethod_isConst(cursor);
        m.isStatic = clang_CXXMethod_isStatic(cursor);
        m.isVirtual = clang_CXXMethod_isVirtual(cursor);
        m.isPureVirtual = clang_CXXMethod_isPureVirtual(cursor);
        m.access = cxAccessToAccess(clang_getCXXAccessSpecifier(cursor));
        m.usr = toStringAndDispose(clang_getCursorUSR(cursor));
        m.isInline = clang_Cursor_isFunctionInlined(cursor) != 0;

        // Template parameter detection for methods
        if (k == CXCursor_FunctionTemplate) {
            extractTemplateParameters(cursor, m.returnType.templateArgs); // You could add more struct members for template params.
        }

        int numArgs = clang_Cursor_getNumArguments(cursor);
        for (int i = 0; i < numArgs; ++i) {
            CXCursor arg = clang_Cursor_getArgument(cursor, i);
            Parameter param;
            param.name = toStringAndDispose(clang_getCursorSpelling(arg));
            param.type = fromCXType(clang_getCursorType(arg));
            m.params.push_back(std::move(param));
        }
        state->class_stack.back()->methods.push_back(std::move(m));
        return CXChildVisit_Continue;
    }

    // Base classes (inheritance)
    if (k == CXCursor_CXXBaseSpecifier) {
        if (state->class_stack.empty()) return CXChildVisit_Continue;
        CXCursor referenced = clang_getCursorReferenced(cursor);
        std::string base_usr = toStringAndDispose(clang_getCursorUSR(referenced));
        if (!base_usr.empty())
            state->class_stack.back()->bases_usr.push_back(base_usr);
        else {
            std::string ts = toStringAndDispose(clang_getTypeSpelling(clang_getCursorType(cursor)));
            state->class_stack.back()->bases_usr.push_back(ts);
        }
        return CXChildVisit_Continue;
    }

    return CXChildVisit_Recurse;
}

std::shared_ptr<TranslationUnit> LibClangParser::parseFile(
    const std::string& filepath,
    const std::vector<std::string>& compile_args)
{
    CXIndex idx = clang_createIndex(0, 0);

    // Prepare compile flags for libclang (C API expects const char* array)
    std::vector<const char*> cargs;
    for (const auto& s : compile_args) cargs.push_back(s.c_str());

    CXTranslationUnit tu = clang_parseTranslationUnit(
        idx,
        filepath.c_str(),
        cargs.empty() ? nullptr : cargs.data(),
        static_cast<int>(cargs.size()),
        nullptr, 0,
        CXTranslationUnit_None);

    if (!tu) {
        std::cerr << "Failed to parse translation unit: " << filepath << "\n";
        clang_disposeIndex(idx);
        return nullptr;
    }

    VisitState state(filepath);
    state.main_file = clang_getFile(tu, filepath.c_str());

    CXCursor root = clang_getTranslationUnitCursor(tu);
    clang_visitChildren(root, visitor, &state);

    // Diagnostics/logging
    unsigned nd = clang_getNumDiagnostics(tu);
    for (unsigned i = 0; i < nd; ++i) {
        CXDiagnostic d = clang_getDiagnostic(tu, i);
        CXString diag = clang_formatDiagnostic(d, clang_defaultDiagnosticDisplayOptions());
        std::string sdiag = toStringAndDispose(diag);
        if (!sdiag.empty()) std::cerr << "clang diagnostic: " << sdiag << "\n";
        clang_disposeDiagnostic(d);
    }

    clang_disposeTranslationUnit(tu);
    clang_disposeIndex(idx);
    return state.tu;
}
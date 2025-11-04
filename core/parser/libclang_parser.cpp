#include "libclang_parser.h"
#include <clang-c/Index.h>
#include <iostream>
#include <cassert>
#include <stack>
#include <cstring>
#include <filesystem>

using namespace cppuml;

static Access cxAccessToAccess(CX_CXXAccessSpecifier a) {
    switch (a) {
        case CX_CXXPublic: return Access::Public;
        case CX_CXXProtected: return Access::Protected;
        case CX_CXXPrivate: return Access::Private;
        default: return Access::None;
    }
}

static std::string toStringAndDispose(CXString s) {
    const char *c = clang_getCString(s);
    std::string r = c ? c : "";
    clang_disposeString(s);
    return r;
}

struct VisitState {
    std::shared_ptr<TranslationUnit> tu;
    std::vector<std::shared_ptr<Namespace>> ns_stack;
    std::vector<std::shared_ptr<Class>> class_stack;
    CXFile main_file = nullptr; // the CXFile corresponding to parsed input file
    std::string filename;
    VisitState(const std::string& f): filename(f) {
        tu = std::make_shared<TranslationUnit>();
        tu->filename = f;
    }
};

// Returns true if the cursor is located in the main source file being parsed.
// If cursor has no file location, returns false.
static bool isCursorInMainFile(CXCursor cursor, CXFile main_file) {
    if (!main_file) return false;
    CXSourceLocation loc = clang_getCursorLocation(cursor);
    CXFile file;
    unsigned line, column, offset;
    clang_getSpellingLocation(loc, &file, &line, &column, &offset);
    return file == main_file;
}

static CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    auto *state = static_cast<VisitState*>(client_data);
    CXCursorKind k = clang_getCursorKind(cursor);

    // Filter: only visit cursors that are in the main file (so we avoid stdlib/header noise).
    // For declarations like bases, the base class cursor might be in other file - we'll still use referenced USR.
    if (!isCursorInMainFile(cursor, state->main_file)) {
        // still recurse for certain cursor kinds? No: if cursor not in main file, ignore its children here.
        return CXChildVisit_Continue;
    }

    if (k == CXCursor_Namespace) {
        std::string ns_name = toStringAndDispose(clang_getCursorSpelling(cursor));
        auto ns = std::make_shared<Namespace>();
        ns->name = ns_name;
        if (!state->ns_stack.empty()) {
            state->ns_stack.back()->namespaces.push_back(ns);
        } else {
            state->tu->namespaces.push_back(ns);
        }
        state->ns_stack.push_back(ns);
        clang_visitChildren(cursor, visitor, client_data);
        state->ns_stack.pop_back();
        return CXChildVisit_Continue;
    }

    if (k == CXCursor_ClassDecl || k == CXCursor_StructDecl) {
        // only interested in definitions (skip forward declarations)
        if (clang_isCursorDefinition(cursor) == 0) {
            return CXChildVisit_Continue;
        }
        auto cls = std::make_shared<Class>();
        cls->name = toStringAndDispose(clang_getCursorSpelling(cursor));
        cls->usr  = toStringAndDispose(clang_getCursorUSR(cursor));
        cls->isStruct = (k == CXCursor_StructDecl);
        // store class in current namespace or top-level global namespace
        if (!state->ns_stack.empty()) {
            state->ns_stack.back()->classes.push_back(cls);
        } else {
            if (state->tu->namespaces.empty()) {
                auto global_ns = std::make_shared<Namespace>();
                global_ns->name = ""; // global
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

    if (k == CXCursor_FieldDecl) {
        if (state->class_stack.empty()) return CXChildVisit_Continue;
        Field f;
        f.name = toStringAndDispose(clang_getCursorSpelling(cursor));
        CXType t = clang_getCursorType(cursor);
        // Use canonical type spelling for clarity
        CXType ct = clang_getCanonicalType(t);
        f.type = toStringAndDispose(clang_getTypeSpelling(ct));
        f.access = cxAccessToAccess(clang_getCXXAccessSpecifier(cursor));
        f.usr = toStringAndDispose(clang_getCursorUSR(cursor));
        state->class_stack.back()->fields.push_back(std::move(f));
        return CXChildVisit_Continue;
    }

    if (k == CXCursor_CXXMethod || k == CXCursor_Constructor || k == CXCursor_Destructor) {
        if (state->class_stack.empty()) return CXChildVisit_Continue;
        Method m;
        m.name = toStringAndDispose(clang_getCursorSpelling(cursor));
        CXType cxtype = clang_getCursorType(cursor);
        if (k == CXCursor_CXXMethod) {
            CXType ret = clang_getResultType(cxtype);
            m.returnType = toStringAndDispose(clang_getTypeSpelling(ret));
        } else {
            m.returnType = ""; // constructors/destructors
        }
        m.isConst = clang_CXXMethod_isConst(cursor);
        m.isStatic = clang_CXXMethod_isStatic(cursor);
        m.isVirtual = clang_CXXMethod_isVirtual(cursor);
        m.isPureVirtual = clang_CXXMethod_isPureVirtual(cursor);
        m.access = cxAccessToAccess(clang_getCXXAccessSpecifier(cursor));
        m.usr = toStringAndDispose(clang_getCursorUSR(cursor));
        int numArgs = clang_Cursor_getNumArguments(cursor);
        for (int i = 0; i < numArgs; ++i) {
            CXCursor arg = clang_Cursor_getArgument(cursor, i);
            std::string pname = toStringAndDispose(clang_getCursorSpelling(arg));
            CXType atype = clang_getCursorType(arg);
            CXType act = clang_getCanonicalType(atype);
            std::string tname = toStringAndDispose(clang_getTypeSpelling(act));
            m.params.emplace_back(tname, pname);
        }
        state->class_stack.back()->methods.push_back(std::move(m));
        return CXChildVisit_Continue;
    }

    if (k == CXCursor_CXXBaseSpecifier) {
        if (state->class_stack.empty()) return CXChildVisit_Continue;
        // base class referenced cursor
        CXCursor referenced = clang_getCursorReferenced(cursor);
        std::string base_usr = toStringAndDispose(clang_getCursorUSR(referenced));
        if (!base_usr.empty()) {
            state->class_stack.back()->bases_usr.push_back(base_usr);
        } else {
            // fallback to spelled type
            CXType t = clang_getCursorType(cursor);
            std::string ts = toStringAndDispose(clang_getTypeSpelling(t));
            state->class_stack.back()->bases_usr.push_back(ts);
        }
        return CXChildVisit_Continue;
    }

    // continue recursion for other cursor kinds (to find nested classes, methods, fields ...)
    return CXChildVisit_Recurse;
}

std::shared_ptr<TranslationUnit> LibClangParser::parseFile(const std::string &filepath,
                                                           const std::vector<std::string> &compile_args) {
    // create index
    CXIndex idx = clang_createIndex(0, 0);

    // prepare args for C API
    std::vector<const char*> cargs;
    for (auto &s : compile_args) cargs.push_back(s.c_str());

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

    // prepare state
    VisitState state(filepath);

    // obtain CXFile for the main source file (for filtering)
    CXFile main_file = clang_getFile(tu, filepath.c_str());
    state.main_file = main_file;

    // visit translation unit
    CXCursor root = clang_getTranslationUnitCursor(tu);
    clang_visitChildren(root, visitor, &state);

    // diagnostics (log them; do not fail unless parse failed)
    unsigned nd = clang_getNumDiagnostics(tu);
    for (unsigned i = 0; i < nd; ++i) {
        CXDiagnostic d = clang_getDiagnostic(tu, i);
        CXString diag = clang_formatDiagnostic(d, clang_defaultDiagnosticDisplayOptions());
        std::string sdiag = toStringAndDispose(diag);
        // only log, but useful for user to supply includes/flags if necessary
        if (!sdiag.empty()) {
            std::cerr << "clang diagnostic: " << sdiag << "\n";
        }
        clang_disposeDiagnostic(d);
    }

    clang_disposeTranslationUnit(tu);
    clang_disposeIndex(idx);
    return state.tu;
}
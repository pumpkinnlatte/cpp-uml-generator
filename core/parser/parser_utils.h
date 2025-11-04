#pragma once
/**
 * @file parser_utils.h
 * @brief Utilities for converting libclang types and cursors into unified model structures.
 *
 * Helpers to translate CXType/CXCursor to cppuml::model types (TypeName, Access, etc).
 */

#include <clang-c/Index.h>
#include <string>
#include <vector>
#include <memory>
#include "../model/UnifiedModel.h"

namespace cppuml {
namespace parser_utils {

using cppuml::model::TypeName;
using cppuml::model::Access;

/**
 * @brief Converts clang's C++ access specifier to internal Access enum.
 */
inline Access cxAccessToAccess(CX_CXXAccessSpecifier a) {
    switch (a) {
        case CX_CXXPublic:    return Access::Public;
        case CX_CXXProtected: return Access::Protected;
        case CX_CXXPrivate:   return Access::Private;
        default:              return Access::None;
    }
}

/**
 * @brief Converts a CXType into a TypeName, supporting pointers, references, const, and simple template arguments.
 */
inline TypeName fromCXType(CXType cxType) {
    TypeName tn;
    // Name
    CXString spelling = clang_getTypeSpelling(cxType);
    tn.name = clang_getCString(spelling);
    clang_disposeString(spelling);

    // Const qualifier
    tn.isConst = clang_isConstQualifiedType(cxType);

    // Reference (lvalue or rvalue)
    CXTypeKind kind = clang_getCanonicalType(cxType).kind;
    tn.isReference = (kind == CXType_LValueReference || kind == CXType_RValueReference);

    // Pointer depth
    CXType t = cxType;
    while (t.kind == CXType_Pointer) {
        tn.pointerDepth++;
        t = clang_getPointeeType(t);
    }

    // Template arguments (libclang's support is limited)
    int nTemplateArgs = clang_Type_getNumTemplateArguments(cxType);
    if (nTemplateArgs > 0 && nTemplateArgs != -1) {
        for (int i = 0; i < nTemplateArgs; ++i) {
            CXType templateArg = clang_Type_getTemplateArgumentAsType(cxType, i);
            if (templateArg.kind != CXType_Invalid) {
                tn.templateArgs.push_back(fromCXType(templateArg));
            }
        }
    }
    return tn;
}

/**
 * @brief Converts a CXString to std::string and disposes the CXString.
 */
inline std::string toStringAndDispose(CXString s) {
    const char *c = clang_getCString(s);
    std::string r = c ? c : "";
    clang_disposeString(s);
    return r;
}

} // namespace parser_utils
} // namespace cppuml
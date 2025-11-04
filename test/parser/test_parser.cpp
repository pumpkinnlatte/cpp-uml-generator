#include <iostream>
#include <filesystem>
#include <fstream>
#include <cassert>
#include <vector>
#include <string>
#include <memory>

#include "../core/parser/libclang_parser.h"
#include "../core/model/model.h"

using namespace cppuml;
namespace fs = std::filesystem;

static void fail(const std::string &msg) {
    std::cerr << "FAIL: " << msg << "\n";
    std::exit(1);
}

static void pass(const std::string &msg) {
    std::cout << "PASS: " << msg << "\n";
}

static std::shared_ptr<TranslationUnit> parseWithStd17(const fs::path &p) {
    LibClangParser parser;
    std::vector<std::string> args = {"-std=c++17"};
    auto tu = parser.parseFile(p.string(), args);
    if (!tu) fail("parse returned nullptr for " + p.string());
    return tu;
}

// Test 1: clase simple con campos públicos/privados y un método con parámetros.
// Uses repository file "level1_simple_class.cpp"
static void test_simple_class(const fs::path &repo_root) {
    fs::path p = repo_root / "level1_simple_class.cpp";
    if (!fs::exists(p)) fail("test_simple_class: sample file not found: " + p.string());
    auto tu = parseWithStd17(p);

    // find class Person by name (search classes_by_usr map values)
    std::shared_ptr<Class> person;
    for (auto &kv : tu->classes_by_usr) {
        auto &c = kv.second;
        if (c->name == "Person") { person = c; break; }
    }
    if (!person) fail("Person class not found");

    // Check fields: name (public), age (public), active (private)
    bool found_name = false, found_age = false, found_active = false;
    for (auto &f : person->fields) {
        if (f.name == "name") found_name = true;
        if (f.name == "age") found_age = true;
        if (f.name == "active") found_active = true;
    }
    if (!found_name) fail("field 'name' not found in Person");
    if (!found_age) fail("field 'age' not found in Person");
    if (!found_active) fail("field 'active' not found in Person");

    // Check method greet exists and is const
    bool found_greet = false;
    for (auto &m : person->methods) {
        if (m.name == "greet") {
            found_greet = true;
            if (!m.isConst) fail("method greet should be const");
        }
    }
    if (!found_greet) fail("method greet not found");
    pass("test_simple_class");
}

// Test 2: herencia simple (class A {}; class B : public A {};).
// We create a temporary file with minimal content.
static void test_simple_inheritance(const fs::path &repo_root) {
    fs::path tmp = repo_root / "tmp_inheritance_test.cpp";
    std::ofstream ofs(tmp);
    ofs << "class A {};\nclass B : public A {};\n";
    ofs.close();

    auto tu = parseWithStd17(tmp);

    // find classes by name
    std::shared_ptr<Class> Acls, Bcls;
    for (auto &kv : tu->classes_by_usr) {
        auto &c = kv.second;
        if (c->name == "A") Acls = c;
        if (c->name == "B") Bcls = c;
    }
    if (!Acls) { fs::remove(tmp); fail("A not found"); }
    if (!Bcls) { fs::remove(tmp); fail("B not found"); }

    // B should have base pointing to A (by USR). We check that B->bases_usr references A's USR.
    bool found_base = false;
    for (auto &busr : Bcls->bases_usr) {
        if (busr == Acls->usr) { found_base = true; break; }
    }
    if (!found_base) {
        // If base stored as fallback spelling, we allow check of name too
        for (auto &busr : Bcls->bases_usr) {
            if (busr.find("A") != std::string::npos) { found_base = true; break; }
        }
    }
    fs::remove(tmp);
    if (!found_base) fail("B does not list A as base");
    pass("test_simple_inheritance");
}

// Test 3: namespaces anidados
// Uses repository file "level2_namespace_nested.cpp"
static void test_nested_namespaces(const fs::path &repo_root) {
    fs::path p = repo_root / "level2_namespace_nested.cpp";
    if (!fs::exists(p)) fail("test_nested_namespaces: sample file not found: " + p.string());
    auto tu = parseWithStd17(p);

    // locate top-level namespace "ui"
    std::shared_ptr<Namespace> ui_ns;
    for (auto &ns : tu->namespaces) {
        if (ns->name == "ui") { ui_ns = ns; break; }
    }
    if (!ui_ns) {
        // maybe global namespace contains a namespace called "ui"
        for (auto &ns : tu->namespaces) {
            for (auto &sub : ns->namespaces) {
                if (sub->name == "ui") { ui_ns = sub; break; }
            }
        }
    }
    if (!ui_ns) fail("namespace 'ui' not found");

    // in ui, there should be class Window
    bool found_window = false;
    for (auto &c : ui_ns->classes) {
        if (c->name == "Window") { found_window = true; break; }
    }
    if (!found_window) fail("class Window not found in namespace ui");

    pass("test_nested_namespaces");
}

int main(int argc, char** argv) {
    // assume running from repository root; attempt to get current path
    fs::path repo_root = fs::current_path();

    std::cout << "Running cpp-uml-parser tests from: " << repo_root << "\n";

    try {
        test_simple_class(repo_root);
        test_simple_inheritance(repo_root);
        test_nested_namespaces(repo_root);
    } catch (const std::exception &e) {
        fail(std::string("exception: ") + e.what());
    }
    std::cout << "ALL TESTS PASSED\n";
    return 0;
}
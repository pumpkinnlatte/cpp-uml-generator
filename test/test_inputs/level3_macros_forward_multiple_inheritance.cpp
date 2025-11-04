// Macros, forward declarations, multiple and virtual inheritance, enum class, typedefs/using
#include <string>
#include <iostream>

#define DECLARE_MSG(name) \
    static const char* msg_##name() { return #name; }

class IPrintable {
public:
    virtual ~IPrintable() = default;
    virtual std::string toString() const = 0;
};

class Logger {
public:
    void log(const std::string& msg) { std::cout << "LOG: " << msg << '\n'; }
};

class Shape; // forward declaration

enum class Color : unsigned char { Red = 0, Green = 1, Blue = 2 };

// Avoid clashing with system typedef 'id_t' by using a different name for tests
using my_id_t = unsigned long;

class BaseA {
public:
    virtual ~BaseA() = default;
    DECLARE_MSG(BaseA)
};

class BaseB {
public:
    virtual ~BaseB() = default;
    static int kind() { return 42; }
};

// Diamond inheritance example (virtual inheritance)
class Drawable : public virtual BaseA, public BaseB, public IPrintable {
public:
    Drawable() = default;
    virtual void draw() {}
    std::string toString() const override { return "Drawable"; }
};

class ComplexShape : public Drawable {
public:
    ComplexShape(my_id_t id, Color c) : id(id), color(c) {}
    std::string toString() const override {
        return "ComplexShape #" + std::to_string(id);
    }

    my_id_t id;
    Color color;
    Shape* partner = nullptr; // raw pointer to forward-declared type
};

// Definition of forward-declared class to complete the example
class Shape {
public:
    Shape() {}
    void setPartner(ComplexShape* s) { partner = s; }
private:
    ComplexShape* partner = nullptr;
};
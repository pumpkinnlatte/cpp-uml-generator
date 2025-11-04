// Simple class with public/private members and methods
#include <string>
#include <iostream>

class Person {
public:
    Person(std::string name, int age) : name(std::move(name)), age(age) {}
    ~Person() = default;

    void greet() const {
        std::cout << "Hello, my name is " << name << " and I'm " << age << " years old.\n";
    }

    std::string name;
    int age;

private:
    bool active = true;
};
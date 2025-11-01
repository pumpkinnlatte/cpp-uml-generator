// Inheritance, virtual methods, protected/private members, static members
#include <string>
#include <iostream>

class Animal {
public:
    Animal(std::string species) : species(std::move(species)) {}
    virtual ~Animal() = default;

    virtual void speak() const {
        std::cout << species << " makes a sound.\n";
    }

    std::string species;

protected:
    int age = 0;

private:
    bool alive = true;
};

class Dog : public Animal {
public:
    Dog(std::string name) : Animal("Dog"), name(std::move(name)) {}
    ~Dog() override = default;

    void speak() const override {
        std::cout << name << " says: Woof!\n";
    }

    void setAge(int a) { age = a; } // tests protected access

    static int instanceCount;
private:
    std::string name;
};

int Dog::instanceCount = 0;
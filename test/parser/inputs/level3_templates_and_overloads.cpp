// Templates, method templates, specialization-like behavior, function overloads
#include <vector>
#include <iostream>
#include <memory>
#include <string>

template <typename T, typename Alloc = std::allocator<T>>
class Container {
public:
    using value_type = T;
    using allocator_type = Alloc;

    Container() = default;
    explicit Container(const Alloc& a) : alloc(a) {}

    void add(const T& v) { data.push_back(v); }
    void add(T&& v) { data.push_back(std::move(v)); }

    template <typename... Args>
    void emplace(Args&&... args) {
        data.emplace_back(std::forward<Args>(args)...);
    }

    T get(size_t i) const {
        return data.at(i);
    }

    size_t size() const noexcept { return data.size(); }

private:
    std::vector<T, Alloc> data;
    Alloc alloc;
};

// Overloads and free functions
void print(const Container<int>& c) {
    std::cout << "Container<int> size=" << c.size() << '\n';
}

template <typename T>
void print(const Container<T>& c) {
    std::cout << "Container<generic> size=" << c.size() << '\n';
}

void print(int v) {
    std::cout << "int: " << v << '\n';
}

// Example usage (not needed for parser but keeps the file self-contained)
struct X {
    int a;
    X(int v) : a(v) {}
};

int main() {
    Container<int> ci;
    ci.add(1);
    ci.add(2);
    print(ci);

    Container<X> cx;
    cx.emplace(5);
    print(cx);
    return 0;
}
// Simple struct (default public) with methods and typedef
#include <cmath>

struct Point {
    double x{0.0};
    double y{0.0};

    using value_type = double;

    Point() = default;
    Point(double x_, double y_) : x(x_), y(y_) {}

    double distance() const {
        return std::sqrt(x * x + y * y);
    }
};
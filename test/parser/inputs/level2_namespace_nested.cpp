// Namespaces, nested classes, friend, static members, using
#include <vector>
#include <string>

namespace ui {

class Window {
public:
    Window() = default;

    class Layout {
    public:
        void addComponent(const std::string& id) { components.push_back(id); }
        std::vector<std::string> components;
    };

    void add(const std::string& id) { layout.addComponent(id); }

    static int defaultWidth;
private:
    Layout layout;

    void render() noexcept {}
    friend class Debugger; // friend declaration
};

int Window::defaultWidth = 800;

// short alias
using Win = Window;

} // namespace ui
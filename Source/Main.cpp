#include "../Headers/Application.hpp"
#include "../Headers/Menu.hpp"

#include <stdexcept>

int main()
{
    Menu menu;
    Application app(menu);
    
    try
    {
        app.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
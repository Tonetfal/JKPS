#include "../Headers/Application.hpp"

#include <stdexcept>

int main()
{
    Application app;
    
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
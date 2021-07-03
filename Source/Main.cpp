#include "../Headers/Application.hpp"

#include <stdexcept>
#include <iostream>

int main()
{
    Application app;
    
    try
    {
        app.run();    
    }
    catch(const std::exception &ex)
    {
        std::cerr << ex.what() << '\n';
    }
    
    
    return 0;
}
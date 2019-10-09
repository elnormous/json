#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "json.hpp"

namespace
{
    void testNull()
    {
        json::Data d("null");
        if (!d.isNull())
            throw std::runtime_error("Expected a null");
    }

    void testString()
    {
        json::Data d("\"\"");
        if (d.getType() != json::Value::Type::String)
            throw std::runtime_error("Expected a string");
    }

    void testObject()
    {
        json::Data d("{}");
        if (d.getType() != json::Value::Type::Object)
            throw std::runtime_error("Expected an object");
    }
}

int main()
{
    try
    {
        testNull();
        testString();
        testObject();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Success\n";
    return EXIT_SUCCESS;
}

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "json.hpp"

namespace
{
    class TestRunner
    {
    public:
        template <class T, class ...Args>
        void run(T test, Args ...args)
        {
            try
            {
                test(args...);
            }
            catch (std::exception& e)
            {
                std::cerr << e.what() << '\n';
                result = false;
            }
        }

        bool getResult() const noexcept { return result; }

    private:
        bool result = true;
    };

    void testNull()
    {
        json::Data d("null");
        if (!d.isNull())
            throw std::runtime_error("Expected a null");
    }

    void testInteger()
    {
        json::Data d("0");
        if (d.getType() != json::Value::Type::Integer)
            throw std::runtime_error("Expected an integer");

        if (d.as<int>() != 0)
            throw std::runtime_error("Expected 0");
    }

    void testFloat()
    {
        json::Data d("0.0");
        if (d.getType() != json::Value::Type::Float)
            throw std::runtime_error("Expected a float");

        if (d.as<float>() != 0.0F)
            throw std::runtime_error("Expected 0.0");
    }

    void testBoolean()
    {
        json::Data d("false");
        if (d.getType() != json::Value::Type::Boolean)
            throw std::runtime_error("Expected a boolean");

        if (d.as<bool>() != false)
            throw std::runtime_error("Expected true");
    }

    void testString()
    {
        json::Data d("\"\"");
        if (d.getType() != json::Value::Type::String)
            throw std::runtime_error("Expected a string");
    }

    void testEmptyObject()
    {
        json::Data d("{}");
        if (d.getType() != json::Value::Type::Object)
            throw std::runtime_error("Expected an object");


    }

    void testObject()
    {
        json::Data d;
        d = json::Data("{\"a\":\"b\"}");
        if (d.getType() != json::Value::Type::Object)
            throw std::runtime_error("Expected an object");

        if (!d.hasMember("a"))
            throw std::runtime_error("Expected a member \"a\"");

        if (d["a"].getType() != json::Value::Type::String)
            throw std::runtime_error("Expected a string member");

        if (d["a"].as<std::string>() != "b")
        throw std::runtime_error("Expected a value \"b\"");
    }

    void testEmptyArray()
    {
        json::Data d("[]");
        if (d.getType() != json::Value::Type::Array)
            throw std::runtime_error("Expected an array");

        if (!d.as<json::Value::Array>().empty())
            throw std::runtime_error("Expected an empty array");
    }

    void testArray()
    {
        json::Data d = json::Data("[1, 2]");
        if (d.getType() != json::Value::Type::Array)
            throw std::runtime_error("Expected an array");

        if (d.as<json::Value::Array>().size() != 2)
            throw std::runtime_error("Expected an array with 2 elements");

        if (d[0].as<int>() != 1 || d[1].as<int>() != 2)
            throw std::runtime_error("Expected elements 1 and 2");
    }
}

int main()
{
    TestRunner testRunner;
    testRunner.run(testNull);
    testRunner.run(testInteger);
    testRunner.run(testFloat);
    testRunner.run(testBoolean);
    testRunner.run(testString);
    testRunner.run(testEmptyObject);
    testRunner.run(testObject);
    testRunner.run(testEmptyArray);
    testRunner.run(testArray);

    if (testRunner.getResult())
        std::cout << "Success\n";

    return testRunner.getResult() ? EXIT_SUCCESS : EXIT_FAILURE;
}

#include <iostream>
#include "json.hpp"

namespace
{
    class TestError final: public std::logic_error
    {
    public:
        explicit TestError(const std::string& str): std::logic_error(str) {}
        explicit TestError(const char* str): std::logic_error(str) {}
    };

    class TestRunner final
    {
    public:
        template <class T, class ...Args>
        void run(T test, Args ...args) noexcept
        {
            try
            {
                test(args...);
            }
            catch (const TestError& e)
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
        json::Value d = json::parse("null");
        if (!d.isNull())
            throw TestError("Expected a null");
    }

    void testInteger()
    {
        json::Value d = json::parse("0");
        if (d.getType() != json::Value::Type::Integer)
            throw TestError("Expected an integer");

        if (d.as<int>() != 0)
            throw TestError("Expected 0");
    }

    void testFloat()
    {
        json::Value d = json::parse("0.0");
        if (d.getType() != json::Value::Type::Float)
            throw TestError("Expected a float");

        if (d.as<float>() != 0.0F)
            throw TestError("Expected 0.0");

        d = json::parse("0.1e1");
        if (d.as<float>() != 1.0F)
            throw TestError("Expected 1.0");
    }

    void testBoolean()
    {
        json::Value d = json::parse("false");
        if (d.getType() != json::Value::Type::Boolean)
            throw TestError("Expected a boolean");

        if (d.as<bool>() != false)
            throw TestError("Expected true");
    }

    void testString()
    {
        json::Value d = json::parse("\"\"");
        if (d.getType() != json::Value::Type::String)
            throw TestError("Expected a string");
    }

    void testEmptyObject()
    {
        json::Value d = json::parse("{}");
        if (d.getType() != json::Value::Type::Object)
            throw TestError("Expected an object");
    }

    void testObject()
    {
        json::Value d;
        d = json::parse("{\"a\":\"b\"}");
        if (d.getType() != json::Value::Type::Object)
            throw TestError("Expected an object");

        if (!d.hasMember("a"))
            throw TestError("Expected a member \"a\"");

        if (d["a"].getType() != json::Value::Type::String)
            throw TestError("Expected a string member");

        if (d["a"].as<std::string>() != "b")
            throw TestError("Expected a value \"b\"");
    }

    void testEmptyArray()
    {
        json::Value d = json::parse("[]");
        if (d.getType() != json::Value::Type::Array)
            throw TestError("Expected an array");

        if (!d.as<json::Value::Array>().empty())
            throw TestError("Expected an empty array");
    }

    void testArray()
    {
        json::Value d = json::parse("[1, 2]");
        if (d.getType() != json::Value::Type::Array)
            throw TestError("Expected an array");

        if (d.as<json::Value::Array>().size() != 2)
            throw TestError("Expected an array with 2 elements");

        if (d[0].as<int>() != 1 || d[1].as<int>() != 2)
            throw TestError("Expected elements 1 and 2");
    }

    void testUnicode()
    {
        json::Value d = json::parse("\"ē–\"");
        if (d.getType() != json::Value::Type::String)
            throw TestError("Expected an array");

        if (d.as<std::string>() != "ē–")
            throw TestError("Wrong value");
    }

    void testEncoding()
    {
        json::Value d = json::Value::Object{
            {"n", nullptr},
            {"i", 1},
            {"f", 2.0F},
            {"s", "foo"},
            {"b", true},
            {"a", json::Value::Array{
                true, 1, 2.0F, "3",
                json::Value::Array{1, 2, 3}}
            }
        };

        if (json::encode(d) != "{\"a\":[true,1,2.000000,\"3\",[1,2,3]],\"b\":true,\"f\":2.000000,\"i\":1,\"n\":null,\"s\":\"foo\"}")
            throw TestError("Wrong encoded result");

        if (json::encode(d, true) != "{\n\t\"a\":[\n\t\ttrue,\n\t\t1,\n\t\t2.000000,\n\t\t\"3\",\n\t\t[\n\t\t\t1,\n\t\t\t2,\n\t\t\t3\n\t\t]\n\t],\n\t\"b\":true,\n\t\"f\":2.000000,\n\t\"i\":1,\n\t\"n\":null,\n\t\"s\":\"foo\"\n}")
            throw TestError("Wrong encoded result");
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
    testRunner.run(testUnicode);
    testRunner.run(testEncoding);

    if (testRunner.getResult())
        std::cout << "Success\n";

    return testRunner.getResult() ? EXIT_SUCCESS : EXIT_FAILURE;
}

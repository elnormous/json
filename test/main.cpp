#include <chrono>
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
        TestRunner(int argc, char** argv) noexcept:
            argumentCount(argc), arguments(argv) {}
        TestRunner(const TestRunner&) = delete;
        TestRunner& operator=(const TestRunner&) = delete;
        ~TestRunner()
        {
            if (result)
                std::cout << "Success, total duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms\n";
        }

        template <class T, class ...Args>
        void run(const std::string& name, T test, Args ...args) noexcept
        {
            for (int i = 1; i < argumentCount; ++i)
                if (name == arguments[i]) break;
                else if (i == argumentCount - 1) return;

            try
            {
                std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
                test(args...);
                std::chrono::steady_clock::time_point finish = std::chrono::steady_clock::now();

                duration += finish - start;

                std::cerr << name << " succeeded, duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms\n";
            }
            catch (const TestError& e)
            {
                std::cerr << name << " failed: " << e.what() << '\n';
                result = false;
            }
        }

        bool getResult() const noexcept { return result; }
        std::chrono::steady_clock::duration getDuration() const noexcept { return duration; }

    private:
        int argumentCount;
        char** arguments;
        bool result = true;
        std::chrono::steady_clock::duration duration = std::chrono::milliseconds(0);
    };

    void testNull()
    {
        json::Value d = json::parse("null");
        if (!d.isNull())
            throw TestError("Expected a null");
    }

    void testInteger()
    {
        json::Value d = json::parse("10");
        if (d.getType() != json::Value::Type::integer)
            throw TestError("Expected an integer");

        if (d.as<int>() != 10)
            throw TestError("Expected 0");
    }

    void testFloat()
    {
        json::Value d = json::parse("0.5");
        if (d.getType() != json::Value::Type::floatingPoint)
            throw TestError("Expected a float");

        if (d.as<float>() != 0.5F)
            throw TestError("Expected 0.5");

        d = json::parse("0.1e1");
        if (d.as<float>() != 1.0F)
            throw TestError("Expected 1.0");
    }

    void testBoolean()
    {
        json::Value d = json::parse("false");
        if (d.getType() != json::Value::Type::boolean)
            throw TestError("Expected a boolean");

        if (d.as<bool>() != false)
            throw TestError("Expected true");
    }

    void testString()
    {
        json::Value d = json::parse("\"\"");
        if (d.getType() != json::Value::Type::string)
            throw TestError("Expected a string");
    }

    void testEmptyObject()
    {
        json::Value d = json::parse("{}");
        if (d.getType() != json::Value::Type::object)
            throw TestError("Expected an object");
    }

    void testObject()
    {
        json::Value d;
        d = json::parse("{\"a\":\"b\", \"c\": [1, 2], \"d\": {\"x\": \"y\"}}");
        if (d.getType() != json::Value::Type::object)
            throw TestError("Expected an object");

        if (!d.hasMember("a"))
            throw TestError("Expected a member \"a\"");

        if (d["a"].getType() != json::Value::Type::string)
            throw TestError("Expected a string member");

        if (d["a"].as<std::string>() != "b")
            throw TestError("Expected a value \"b\"");

        if (!d.hasMember("c"))
            throw TestError("Expected a member \"c\"");

        if (d["c"].getType() != json::Value::Type::array)
            throw TestError("Expected an array member");

        if (!d.hasMember("d"))
            throw TestError("Expected a member \"d\"");

        if (d["d"].getType() != json::Value::Type::object)
            throw TestError("Expected an object member");
    }

    void testEmptyArray()
    {
        json::Value d = json::parse("[]");
        if (d.getType() != json::Value::Type::array)
            throw TestError("Expected an array");

        if (!d.as<json::Value::Array>().empty())
            throw TestError("Expected an empty array");
    }

    void testArray()
    {
        json::Value d = json::parse("[1, 2]");
        if (d.getType() != json::Value::Type::array)
            throw TestError("Expected an array");

        if (d.as<json::Value::Array>().size() != 2)
            throw TestError("Expected an array with 2 elements");

        if (d[0].getType() != json::Value::Type::integer)
            throw TestError("Expected an integer");

        if (d[1].getType() != json::Value::Type::integer)
            throw TestError("Expected an integer");

        if (d[0].as<int>() != 1 || d[1].as<int>() != 2)
            throw TestError("Expected elements 1 and 2");
    }

    void testUnicode()
    {
        json::Value d = json::parse("\"ē–\"");
        if (d.getType() != json::Value::Type::string)
            throw TestError("Expected a string");

        if (d.as<std::string>() != "ē–")
            throw TestError("Wrong value");
    }

    void testEscape()
    {
        json::Value d = json::parse("\"\\b\\f\\n\\r\\t\"");
        if (d.getType() != json::Value::Type::string)
            throw TestError("Expected a string");

        if (d.as<std::string>() != "\b\f\n\r\t")
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

    enum class byte: unsigned char {};

    void testByte()
    {
        std::vector<byte> data = {
            static_cast<byte>('{'),
            static_cast<byte>('}')
        };

        json::Value d = json::parse(data);
        if (d.getType() != json::Value::Type::object)
            throw TestError("Expected an object");
    }
}

int main(int argc, char* argv[])
{
    TestRunner testRunner(argc, argv);
    testRunner.run("testNull", testNull);
    testRunner.run("testInteger", testInteger);
    testRunner.run("testFloat", testFloat);
    testRunner.run("testBoolean", testBoolean);
    testRunner.run("testString", testString);
    testRunner.run("testEmptyObject", testEmptyObject);
    testRunner.run("testObject", testObject);
    testRunner.run("testEmptyArray", testEmptyArray);
    testRunner.run("testArray", testArray);
    testRunner.run("testUnicode", testUnicode);
    testRunner.run("testEscape", testEscape);
    testRunner.run("testEncoding", testEncoding);
    testRunner.run("testByte", testByte);

    return testRunner.getResult() ? EXIT_SUCCESS : EXIT_FAILURE;
}

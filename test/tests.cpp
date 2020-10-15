#include <cstddef>
#include <string>
#include <vector>
#include "catch2/catch.hpp"
#include "json.hpp"

TEST_CASE("Null", "[null]")
{
    const json::Value d = json::parse("null");
    REQUIRE(d.isNull());
}

TEST_CASE("Integer", "[integer]")
{
    const json::Value d = json::parse("10");
    REQUIRE(d.getType() == json::Value::Type::integer);
    REQUIRE(d.as<int>() == 10);
}

TEST_CASE("Float", "[float]")
{
    SECTION("Float")
    {
        const json::Value d = json::parse("0.5");
        REQUIRE(d.getType() == json::Value::Type::floatingPoint);
        REQUIRE(d.as<float>() == Approx(0.5F));
    }

    SECTION("Exponent")
    {
        const auto e = json::parse("0.1e1");
        REQUIRE(e.as<float>() == Approx(1.0F));
    }
}

TEST_CASE("Bool", "[bool]")
{
    SECTION("True")
    {
        const json::Value d = json::parse("true");
        REQUIRE(d.getType() == json::Value::Type::boolean);
        REQUIRE(d.as<bool>());
    }

    SECTION("False")
    {
        const json::Value d = json::parse("false");
        REQUIRE(d.getType() == json::Value::Type::boolean);
        REQUIRE_FALSE(d.as<bool>());
    }
}

TEST_CASE("String", "[string]")
{
    const json::Value d = json::parse("\"\"");
    REQUIRE(d.getType() == json::Value::Type::string);
}

TEST_CASE("Empty object", "[empty_object]")
{
    const json::Value d = json::parse("{}");
    REQUIRE(d.getType() == json::Value::Type::object);
}

TEST_CASE("Object", "[object]")
{
    json::Value d;
    d = json::parse("{\"a\" : \"b\", \"c\": [1, 2 ], \"d\": {\"x\"\r: \"y\"} }");
    REQUIRE(d.getType() == json::Value::Type::object);
    REQUIRE(d.hasMember("a"));
    REQUIRE(d["a"].getType() == json::Value::Type::string);
    REQUIRE(d["a"].as<std::string>() == "b");
    REQUIRE(d.hasMember("c"));
    REQUIRE(d["c"].getType() == json::Value::Type::array);
    REQUIRE(d.hasMember("d"));
    REQUIRE(d["d"].getType() == json::Value::Type::object);
}

TEST_CASE("EmptyArray", "[empty_array]")
{
    const json::Value d = json::parse("[]");
    REQUIRE(d.getType() == json::Value::Type::array);
    REQUIRE(d.as<json::Value::Array>().empty());
}

TEST_CASE("Array", "[array]")
{
    const json::Value d = json::parse("[1, 2\t , {}\n, \"\"]");
    REQUIRE(d.getType() == json::Value::Type::array);
    REQUIRE(d.as<json::Value::Array>().size() == 4);
    REQUIRE(d[0].getType() == json::Value::Type::integer);
    REQUIRE(d[1].getType() == json::Value::Type::integer);
    REQUIRE(d[0].as<int>() == 1);
    REQUIRE(d[1].as<int>() == 2);
    REQUIRE(d[2].getType() == json::Value::Type::object);
    REQUIRE(d[3].getType() == json::Value::Type::string);
}

TEST_CASE("Unicode", "[unicode]")
{
    const json::Value d = json::parse("\"ē–\"");
    REQUIRE(d.getType() == json::Value::Type::string);
    REQUIRE(d.as<std::string>() == "ē–");
}

TEST_CASE("Escape", "[escape]")
{
    const json::Value d = json::parse("\"\\b\\f\\n\\r\\t\"");
    REQUIRE(d.getType() == json::Value::Type::string);
    REQUIRE(d.as<std::string>() == "\b\f\n\r\t");
}

TEST_CASE("Encoding", "[encoding]")
{
    const json::Value d = json::Value::Object{
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

    REQUIRE(json::encode(d) == "{\"a\":[true,1,2.000000,\"3\",[1,2,3]],\"b\":true,\"f\":2.000000,\"i\":1,\"n\":null,\"s\":\"foo\"}");

    REQUIRE(json::encode(d, true) ==            "{\n\t\"a\":[\n\t\ttrue,\n\t\t1,\n\t\t2.000000,\n\t\t\"3\",\n\t\t[\n\t\t\t1,\n\t\t\t2,\n\t\t\t3\n\t\t]\n\t],\n\t\"b\":true,\n\t\"f\":2.000000,\n\t\"i\":1,\n\t\"n\":null,\n\t\"s\":\"foo\"\n}");
}

TEST_CASE("Byte", "[byte]")
{
    const std::vector<std::byte> data = {
        static_cast<std::byte>('{'),
        static_cast<std::byte>('}')
    };

    const json::Value d = json::parse(data);
    REQUIRE(d.getType() == json::Value::Type::object);
}

TEST_CASE("ParseBraceError", "[parse_brace_error]")
{
    REQUIRE_THROWS_AS(json::parse("{"), json::ParseError);
}

TEST_CASE("ParseObjectError", "[parse_object_error]")
{
    REQUIRE_THROWS_AS(json::parse("{\"\"}"), json::ParseError);
}

TEST_CASE("Constructors")
{
    SECTION("Null")
    {
        json::Value v = nullptr;
        REQUIRE(v.getType() == json::Value::Type::null);
        REQUIRE(v.isNull());
    }

    SECTION("Integer")
    {
        json::Value v = 10;
        REQUIRE(v.getType() == json::Value::Type::integer);
        REQUIRE(v.as<int>() == 10);
    }

    SECTION("FloatingPoint")
    {
        json::Value v = 10.0;
        REQUIRE(v.getType() == json::Value::Type::floatingPoint);
        REQUIRE(v.as<double>() == 10.0);
    }

    SECTION("String")
    {
        json::Value v = "s";
        REQUIRE(v.getType() == json::Value::Type::string);
        REQUIRE(v.as<std::string>() == "s");
    }

    SECTION("Object")
    {
        json::Value v = std::map<std::string, json::Value>{};
        REQUIRE(v.getType() == json::Value::Type::object);
        REQUIRE(v.as<json::Value::Object>().empty());
    }

    SECTION("Array")
    {
        json::Value v = std::vector<json::Value>{};
        REQUIRE(v.getType() == json::Value::Type::array);
        REQUIRE(v.as<json::Value::Array>().empty());
    }

    SECTION("Boolean")
    {
        json::Value v = false;
        REQUIRE(v.getType() == json::Value::Type::boolean);
        REQUIRE(v.as<bool>() == false);
    }
}

TEST_CASE("Setters")
{
    SECTION("Null")
    {
        json::Value v;
        v = nullptr;
        REQUIRE(v.getType() == json::Value::Type::null);
        REQUIRE(v.isNull());
    }

    SECTION("Integer")
    {
        json::Value v;
        v = 10;
        REQUIRE(v.getType() == json::Value::Type::integer);
        REQUIRE(v.as<int>() == 10);
    }

    SECTION("FloatingPoint")
    {
        json::Value v;
        v = 10.0;
        REQUIRE(v.getType() == json::Value::Type::floatingPoint);
        REQUIRE(v.as<double>() == 10.0);
    }

    SECTION("String")
    {
        json::Value v;
        v = "s";
        REQUIRE(v.getType() == json::Value::Type::string);
        REQUIRE(v.as<std::string>() == "s");
    }

    SECTION("Object")
    {
        json::Value v;
        v = std::map<std::string, json::Value>{};
        REQUIRE(v.getType() == json::Value::Type::object);
        REQUIRE(v.as<json::Value::Object>().empty());
    }

    SECTION("Array")
    {
        json::Value v;
        v = std::vector<json::Value>{};
        REQUIRE(v.getType() == json::Value::Type::array);
        REQUIRE(v.as<json::Value::Array>().empty());
    }

    SECTION("Boolean")
    {
        json::Value v;
        v = false;
        REQUIRE(v.getType() == json::Value::Type::boolean);
        REQUIRE(v.as<bool>() == false);
    }
}

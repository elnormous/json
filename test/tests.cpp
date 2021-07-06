#include <cstddef>
#include <string>
#include <vector>
#include "catch2/catch.hpp"
#include "json.hpp"

TEST_CASE("Parse null", "[parsing]")
{
    const json::Value d = json::parse("null");
    REQUIRE(d.is<std::nullptr_t>());
}

TEST_CASE("Parse integer", "[parsing]")
{
    const json::Value d = json::parse("10");
    REQUIRE(d.is<double>());
    REQUIRE(d.as<int>() == 10);
}

TEST_CASE("Parse float", "[parsing]")
{
    const json::Value d = json::parse("0.5");
    REQUIRE(d.is<double>());
    REQUIRE(d.as<float>() == Approx(0.5F));
}

TEST_CASE("Parse float with exponent", "[parsing]")
{
    const auto e = json::parse("0.1e1");
    REQUIRE(e.as<float>() == Approx(1.0F));
}

TEST_CASE("Parse true", "[parsing]")
{
    const json::Value d = json::parse("true");
    REQUIRE(d.is<bool>());
    REQUIRE(d.as<bool>());
}

TEST_CASE("Parse false", "[parsing]")
{
    const json::Value d = json::parse("false");
    REQUIRE(d.is<bool>());
    REQUIRE_FALSE(d.as<bool>());
}

TEST_CASE("Parse string", "[parsing]")
{
    const json::Value d = json::parse("\"\"");
    REQUIRE(d.is<json::String>());
}

TEST_CASE("Parse empty object", "[parsing]")
{
    const json::Value d = json::parse("{}");
    REQUIRE(d.is<json::Object>());
}

TEST_CASE("Parse object", "[parsing]")
{
    json::Value d;
    d = json::parse("{\"a\" : \"b\", \"c\": [1, 2 ], \"d\": {\"x\"\r: \"y\"} }");
    REQUIRE(d.is<json::Object>());
    REQUIRE(d.hasMember("a"));
    REQUIRE(d["a"].is<json::String>());
    REQUIRE(d["a"].as<std::string>() == "b");
    REQUIRE(d.hasMember("c"));
    REQUIRE(d["c"].is<json::Array>());
    REQUIRE(d.hasMember("d"));
    REQUIRE(d["d"].is<json::Object>());
}

TEST_CASE("Parse empty array", "[parsing]")
{
    const json::Value d = json::parse("[]");
    REQUIRE(d.is<json::Array>());
    REQUIRE(d.as<json::Array>().empty());
}

TEST_CASE("Parse array", "[parsing]")
{
    const json::Value d = json::parse("[1, 2\t , {}\n, \"\"]");
    REQUIRE(d.is<json::Array>());
    REQUIRE(d.as<json::Array>().size() == 4);
    REQUIRE(d[0].is<double>());
    REQUIRE(d[1].is<double>());
    REQUIRE(d[0].as<int>() == 1);
    REQUIRE(d[1].as<int>() == 2);
    REQUIRE(d[2].is<json::Object>());
    REQUIRE(d[3].is<json::String>());
}

TEST_CASE("Parse unicode", "[parsing]")
{
    const json::Value d = json::parse("\"ē–\"");
    REQUIRE(d.is<json::String>());
    REQUIRE(d.as<std::string>() == "ē–");
}

TEST_CASE("Parse escape sequence", "[parsing]")
{
    const json::Value d = json::parse("\"\\b\\f\\n\\r\\t\"");
    REQUIRE(d.is<json::String>());
    REQUIRE(d.as<std::string>() == "\b\f\n\r\t");
}

TEST_CASE("Encoding", "[encoding]")
{
    const json::Value d = json::Object{
        {"n", nullptr},
        {"i", 1},
        {"f", 2.1F},
        {"s", "foo"},
        {"b", true},
        {"a", json::Array{
            true, 1, 2.1F, "3",
            json::Array{1, 2, 3}}
        }
    };

    REQUIRE(json::encode(d) == "{\"a\":[true,1,2.100000,\"3\",[1,2,3]],\"b\":true,\"f\":2.100000,\"i\":1,\"n\":null,\"s\":\"foo\"}");

    REQUIRE(json::encode(d, true) == "{\n\t\"a\":[\n\t\ttrue,\n\t\t1,\n\t\t2.100000,\n\t\t\"3\",\n\t\t[\n\t\t\t1,\n\t\t\t2,\n\t\t\t3\n\t\t]\n\t],\n\t\"b\":true,\n\t\"f\":2.100000,\n\t\"i\":1,\n\t\"n\":null,\n\t\"s\":\"foo\"\n}");
}

TEST_CASE("Byte", "[byte]")
{
    const std::vector<std::byte> data = {
        static_cast<std::byte>('{'),
        static_cast<std::byte>('}')
    };

    const json::Value d = json::parse(data);
    REQUIRE(d.is<json::Object>());
}

TEST_CASE("Parse brace error", "[errors]")
{
    REQUIRE_THROWS_AS(json::parse("{"), json::ParseError);
}

TEST_CASE("Parse object error", "[errors]")
{
    REQUIRE_THROWS_AS(json::parse("{\"\"}"), json::ParseError);
}

TEST_CASE("Null type error", "[errors]")
{
    const json::Value v = nullptr;
    REQUIRE_THROWS_AS(v.as<std::string>(), json::TypeError);
    REQUIRE_THROWS_AS(v[0], json::TypeError);
}

TEST_CASE("Integer type error", "[errors]")
{
    const json::Value v = 0;
    REQUIRE_THROWS_AS(v.as<std::string>(), json::TypeError);
}

TEST_CASE("String type error", "[errors]")
{
    const json::Value v = "";
    REQUIRE_THROWS_AS(v.as<bool>(), json::TypeError);
    REQUIRE_THROWS_AS(v.as<int>(), json::TypeError);
    REQUIRE_THROWS_AS(v.as<float>(), json::TypeError);
    REQUIRE_THROWS_AS(v[0], json::TypeError);
}

TEST_CASE("Range error", "[errors]")
{
    const json::Value v = json::Array{};
    REQUIRE_THROWS_AS(v[0], json::RangeError);
}

TEST_CASE("Null constructor", "[constructors]")
{
    const json::Value v = nullptr;
    REQUIRE(v.is<std::nullptr_t>());
}

TEST_CASE("Integer constructor", "[constructors]")
{
    const json::Value v = 10;
    REQUIRE(v.is<double>());
    REQUIRE(v.is<int>());
    REQUIRE(v.as<int>() == 10);
}

TEST_CASE("Floating point constructor", "[constructors]")
{
    const json::Value v = 10.0;
    REQUIRE(v.is<double>());
    REQUIRE(v.is<float>());
    REQUIRE(v.as<double>() == 10.0);
}

TEST_CASE("String constructor", "[constructors]")
{
    const json::Value v = "s";
    REQUIRE(v.is<json::String>());
    REQUIRE(v.is<const char*>());
    REQUIRE(v.as<std::string>() == "s");
}

TEST_CASE("Char array constructor", "[constructors]")
{
    char s[] = "s";
    const json::Value v = s;
    REQUIRE(v.is<json::String>());
    REQUIRE(v.as<std::string>() == "s");
}

TEST_CASE("Object constructor", "[constructors]")
{
    const  json::Value v = json::Object{};
    REQUIRE(v.is<json::Object>());
    REQUIRE(v.as<json::Object>().empty());
}

TEST_CASE("Array constructor", "[constructors]")
{
    const json::Value v = json::Array{};
    REQUIRE(v.is<json::Array>());
    REQUIRE(v.as<json::Array>().empty());
}

TEST_CASE("Boolean constructor", "[constructors]")
{
    const json::Value v = false;
    REQUIRE(v.is<bool>());
    REQUIRE_FALSE(v.as<bool>());
}

TEST_CASE("Null setter", "[setters]")
{
    json::Value v;
    v = nullptr;
    REQUIRE(v.is<std::nullptr_t>());
}

TEST_CASE("Integer setter", "[setters]")
{
    json::Value v;
    v = 10;
    REQUIRE(v.is<double>());
    REQUIRE(v.as<int>() == 10);
}

TEST_CASE("Floating point setter", "[setters]")
{
    json::Value v;
    v = 10.0;
    REQUIRE(v.is<double>());
    REQUIRE(v.as<double>() == 10.0);
}

TEST_CASE("String setter", "[setters]")
{
    json::Value v;
    v = "s";
    REQUIRE(v.is<json::String>());
    REQUIRE(v.as<std::string>() == "s");
}

TEST_CASE("Char array setter", "[setters]")
{
    char s[] = "s";
    json::Value v;
    v = s;
    REQUIRE(v.is<json::String>());
    REQUIRE(v.as<std::string>() == "s");
}

TEST_CASE("Object setter", "[setters]")
{
    json::Value v;
    v = json::Object{};
    REQUIRE(v.is<json::Object>());
    REQUIRE(v.as<json::Object>().empty());
}

TEST_CASE("Array setter", "[setters]")
{
    json::Value v;
    v = json::Array{};
    REQUIRE(v.is<json::Array>());
    REQUIRE(v.as<json::Array>().empty());
}

TEST_CASE("Array push back", "[setters]")
{
    json::Value v;
    v = json::Array{};
    v.pushBack(10);
    REQUIRE(!v.isEmpty());
    REQUIRE(v.getSize() == 1);
    REQUIRE(v[0].is<double>());
}

TEST_CASE("Boolean setter", "[setters]")
{
    json::Value v;
    v = false;
    REQUIRE(v.is<bool>());
    REQUIRE_FALSE(v.as<bool>());
}

TEST_CASE("Zero float to bool cast", "[casts]")
{
    const json::Value v = 0.0;
    REQUIRE_FALSE(v.as<bool>());
}

TEST_CASE("Positive float to bool cast", "[casts]")
{
    const json::Value v = 1.0;
    REQUIRE(v.as<bool>());
}

TEST_CASE("Zero integer to bool cast", "[casts]")
{
    const json::Value v = 0;
    REQUIRE_FALSE(v.as<bool>());
}

TEST_CASE("Positive integer to bool cast", "[casts]")
{
    const json::Value v = 1;
    REQUIRE(v.as<bool>());
}

TEST_CASE("Mutable range loop", "[range_loop]")
{
    json::Value v = json::Array{json::Value{0}, json::Value{1}};

    int counter = 0;

    for (json::Value& i : v)
        REQUIRE(i.as<int>() == counter++);

    REQUIRE(counter == 2);
}

TEST_CASE("Constant range loop", "[range_loop]")
{
    const json::Value v = json::Array{json::Value{0}, json::Value{1}};

    int counter = 0;

    for (const json::Value& i : v)
        REQUIRE(i.as<int>() == counter++);

    REQUIRE(counter == 2);
}

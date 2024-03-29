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

TEST_CASE("Parse negative integer", "[parsing]")
{
    const json::Value d = json::parse("-10");
    REQUIRE(d.is<double>());
    REQUIRE(d.as<int>() == -10);
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

TEST_CASE("Parse float with exponent only", "[parsing]")
{
    const auto e = json::parse("1e1");
    REQUIRE(e.as<float>() == Approx(10.0F));
}

TEST_CASE("Parse float with capital E exponent only", "[parsing]")
{
    const auto e = json::parse("1E1");
    REQUIRE(e.as<float>() == Approx(10.0F));
}

TEST_CASE("Parse float with positive exponent", "[parsing]")
{
    const auto e = json::parse("0.5e+1");
    REQUIRE(e.as<float>() == Approx(5.0F));
}

TEST_CASE("Parse float with positive exponent only", "[parsing]")
{
    const auto e = json::parse("10e+1");
    REQUIRE(e.as<float>() == Approx(100.0F));
}

TEST_CASE("Parse float with negative exponent", "[parsing]")
{
    const auto e = json::parse("0.5e-1");
    REQUIRE(e.as<float>() == Approx(0.05F));
}

TEST_CASE("Parse float with negative exponent only", "[parsing]")
{
    const auto e = json::parse("10e-1");
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

TEST_CASE("Parse object with empty key", "[parsing]")
{
    const json::Value d = json::parse("{\"\":\"a\"}");
    REQUIRE(d.is<json::Object>());
    REQUIRE(d[""].as<std::string>() == "a");
}

TEST_CASE("Parse object with duplicate keys", "[parsing]")
{
    const json::Value d = json::parse("{\"a\":\"a\",\"a\":\"b\"}");
    REQUIRE(d.is<json::Object>());
    REQUIRE(d["a"].as<std::string>() == "b");
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

TEST_CASE("Parse unicode escape sequence", "[parsing]")
{
    const json::Value d = json::parse("\"\\u0060\\u012a\\u12AB\"");
    REQUIRE(d.is<json::String>());
    REQUIRE(d.as<std::string>() == "\u0060\u012a\u12AB");
}

TEST_CASE("Parse unicode newline escape sequence", "[parsing]")
{
    const json::Value d = json::parse("\"\\u000A\"");
    REQUIRE(d.is<json::String>());
    REQUIRE(d.as<std::string>() == "\n");
}

TEST_CASE("Null encoding", "[encoding]")
{
    const json::Value d = nullptr;

    REQUIRE(json::encode(d) == "null");
}

TEST_CASE("Bool false encoding", "[encoding]")
{
    const json::Value d = false;

    REQUIRE(json::encode(d) == "false");
}

TEST_CASE("Bool true encoding", "[encoding]")
{
    const json::Value d = true;

    REQUIRE(json::encode(d) == "true");
}

TEST_CASE("Integer encoding", "[encoding]")
{
    const json::Value d = 10;

    REQUIRE(json::encode(d) == "10");
}

TEST_CASE("Float encoding", "[encoding]")
{
    const json::Value d = 10.0;

    REQUIRE(json::encode(d) == "10.000000");
}

TEST_CASE("String encoding", "[encoding]")
{
    const json::Value d = "a";

    REQUIRE(json::encode(d) == "\"a\"");
}

TEST_CASE("Array encoding", "[encoding]")
{
    const json::Value d = json::Array{
        false, 1, "2"
    };

    SECTION("Without whitespaces")
    {
        REQUIRE(json::encode(d) == "[false,1,\"2\"]");
    }

    SECTION("With whitespaces")
    {
        REQUIRE(json::encode(d, true) == "[\n\tfalse,\n\t1,\n\t\"2\"\n]");
    }
}

TEST_CASE("Object encoding", "[encoding]")
{
    const json::Value d = json::Object{
        {"n", nullptr},
        {"i", 1},
        {"f", 2.1F},
        {"s", "foo"},
        {"bf", false},
        {"bt", true},
        {"a", json::Array{
            true, 1, 2.1F, "3",
            json::Array{1, 2, 3}}
        }
    };

    SECTION("Without whitespaces")
    {
        REQUIRE(json::encode(d) == "{\"a\":[true,1,2.100000,\"3\",[1,2,3]],\"bf\":false,\"bt\":true,\"f\":2.100000,\"i\":1,\"n\":null,\"s\":\"foo\"}");
    }

    SECTION("With whitespaces")
    {
        REQUIRE(json::encode(d, true) == "{\n\t\"a\":[\n\t\ttrue,\n\t\t1,\n\t\t2.100000,\n\t\t\"3\",\n\t\t[\n\t\t\t1,\n\t\t\t2,\n\t\t\t3\n\t\t]\n\t],\n\t\"bf\":false,\n\t\"bt\":true,\n\t\"f\":2.100000,\n\t\"i\":1,\n\t\"n\":null,\n\t\"s\":\"foo\"\n}");
    }
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

TEST_CASE("Parse space error", "[errors]")
{
    REQUIRE_THROWS_AS(json::parse(" "), json::ParseError);
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

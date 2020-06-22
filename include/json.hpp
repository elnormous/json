// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef JSON_HPP
#define JSON_HPP

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace json
{
    class ParseError final: public std::logic_error
    {
    public:
        explicit ParseError(const std::string& str): std::logic_error(str) {}
        explicit ParseError(const char* str): std::logic_error(str) {}
    };

    class TypeError final: public std::runtime_error
    {
    public:
        explicit TypeError(const std::string& str): std::runtime_error(str) {}
        explicit TypeError(const char* str): std::runtime_error(str) {}
    };

    class RangeError final: public std::runtime_error
    {
    public:
        explicit RangeError(const std::string& str): std::runtime_error(str) {}
        explicit RangeError(const char* str): std::runtime_error(str) {}
    };

    class Value final
    {
    public:
        using Array = std::vector<Value>;
        using Object = std::map<std::string, Value>;

        enum class Type
        {
            null,
            integer,
            floatingPoint,
            string,
            object,
            array,
            boolean
        };

        Value() = default;

        Value(const Type initType): type(initType) {}

        template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
        Value(const T value): type(Type::floatingPoint), doubleValue(std::isfinite(value) ? static_cast<double>(value) : 0.0) {}

        template <typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
        Value(const T value): type(Type::integer), intValue(static_cast<std::int64_t>(value)) {}

        Value(const std::string& value): type(Type::string), stringValue(value) {}

        Value(const char* value): type(Type::string), stringValue(value) {}

        Value(const bool value): type(Type::boolean), boolValue(value) {}

        Value(const std::nullptr_t): type(Type::null) {}

        Value(const Array& value): type(Type::array), arrayValue(value) {}

        Value(const Object& value): type(Type::object), objectValue(value) {}

        Value& operator=(const Type newType) noexcept
        {
            type = newType;
            return *this;
        }

        template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
        Value& operator=(const T value) noexcept
        {
            type = Type::floatingPoint;
            doubleValue = std::isfinite(value) ? static_cast<double>(value) : 0.0;
            return *this;
        }

        template <typename T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
        Value& operator=(const T value) noexcept
        {
            type = Type::integer;
            intValue = static_cast<std::int64_t>(value);
            return *this;
        }

        Value& operator=(const std::string& value)
        {
            type = Type::string;
            stringValue = value;
            return *this;
        }

        Value& operator=(const char* value)
        {
            type = Type::string;
            stringValue = value;
            return *this;
        }

        Value& operator=(const bool value) noexcept
        {
            type = Type::boolean;
            boolValue = value;
            return *this;
        }

        Value& operator=(std::nullptr_t) noexcept
        {
            type = Type::null;
            objectValue.clear();
            return *this;
        }

        Value& operator=(const Array& value)
        {
            type = Type::array;
            arrayValue = value;
            return *this;
        }

        Value& operator=(const Object& value)
        {
            type = Type::object;
            objectValue = value;
            return *this;
        }

        Type getType() const noexcept { return type; }

        template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
        std::string& as() noexcept
        {
            type = Type::string;
            return stringValue;
        }

        template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
        const std::string& as() const
        {
            if (type != Type::string) throw TypeError("Wrong type");
            return stringValue;
        }

        template <typename T, typename std::enable_if<std::is_same<T, const char*>::value>::type* = nullptr>
        T as() const
        {
            if (type != Type::string) throw TypeError("Wrong type");
            return stringValue.c_str();
        }

        template <typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
        T as() const
        {
            if (type != Type::boolean && type != Type::integer && type != Type::floatingPoint)
                throw TypeError("Wrong type");
            if (type == Type::boolean) return boolValue;
            else if (type == Type::integer) return intValue != 0;
            else return doubleValue != 0.0;
        }

        template <typename T, typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value>::type* = nullptr>
        T as() const
        {
            if (type != Type::boolean && type != Type::integer && type != Type::floatingPoint)
                throw TypeError("Wrong type");
            if (type == Type::boolean) return boolValue;
            else if (type == Type::integer) return static_cast<T>(intValue);
            else return static_cast<T>(doubleValue);
        }

        template <typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
        T& as() noexcept
        {
            type = Type::object;
            return objectValue;
        }

        template <typename T, typename std::enable_if<std::is_same<T, Object>::value>::type* = nullptr>
        const T& as() const
        {
            if (type != Type::object) throw TypeError("Wrong type");
            return objectValue;
        }

        template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
        T& as() noexcept
        {
            type = Type::array;
            return arrayValue;
        }

        template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
        const T& as() const
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue;
        }

        Array::iterator begin()
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue.begin();
        }

        Array::iterator end()
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue.end();
        }

        Array::const_iterator begin() const
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue.begin();
        }

        Array::const_iterator end() const
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue.end();
        }

        bool isNull() const noexcept
        {
            return type == Type::null;
        }

        bool hasMember(const std::string& member) const
        {
            if (type != Type::object) throw TypeError("Wrong type");
            return objectValue.find(member) != objectValue.end();
        }

        Value& operator[](const std::string& member)
        {
            type = Type::object;
            return objectValue[member];
        }

        const Value& operator[](const std::string& member) const
        {
            if (type != Type::object) throw TypeError("Wrong type");

            auto i = objectValue.find(member);
            if (i != objectValue.end())
                return i->second;
            else
                throw RangeError("Member does not exist");
        }

        Value& operator[](std::size_t index)
        {
            type = Type::array;
            if (index >= arrayValue.size()) arrayValue.resize(index + 1);
            return arrayValue[index];
        }

        const Value& operator[](std::size_t index) const
        {
            if (type != Type::array) throw TypeError("Wrong type");

            if (index < arrayValue.size())
                return arrayValue[index];
            else
                throw RangeError("Index out of range");
        }

        std::size_t getSize() const
        {
            if (type != Type::array) throw TypeError("Wrong type");
            return arrayValue.size();
        }

        void resize(std::size_t size)
        {
            if (type != Type::array) throw TypeError("Wrong type");
            arrayValue.resize(size);
        }

        void pushBack(const Value& value)
        {
            if (type != Type::array) throw TypeError("Wrong type");
            arrayValue.push_back(value);
        }

    private:
        Type type = Type::object;
        union
        {
            bool boolValue = false;
            std::int64_t intValue;
            double doubleValue;
        };
        Object objectValue;
        Array arrayValue;
        std::string stringValue;
    };

    inline namespace detail
    {
        constexpr std::uint8_t utf8ByteOrderMark[] = {0xEF, 0xBB, 0xBF};
    }

    template <class Iterator>
    Value parse(Iterator begin, Iterator end)
    {
        class Parser final
        {
        public:
            static Value parse(Iterator begin, Iterator end)
            {
                const std::vector<Token> tokens = tokenize(hasByteOrderMark(begin, end) ? begin + 3 : begin, end);
                auto iterator = tokens.begin();
                return parse(iterator, tokens.end());
            }

        private:
            static bool hasByteOrderMark(Iterator begin, Iterator end) noexcept
            {
                for (auto i = std::begin(utf8ByteOrderMark); i != std::end(utf8ByteOrderMark); ++i)
                    if (begin == end || static_cast<std::uint8_t>(*begin++) != *i)
                        return false;
                return true;
            }

            struct Token final
            {
                enum class Type
                {
                    literalInteger, // integer
                    literalFloat, // float
                    literalString, // string
                    keywordTrue, // true
                    keywordFalse, // false
                    keywordNull, // null
                    leftBrace, // {
                    rightBrace, // }
                    leftBracket, // [
                    rightBracket, // ]
                    comma, // ,
                    colon // :
                };

                Type type;
                std::string value;
            };

            static std::vector<Token> tokenize(Iterator begin, Iterator end)
            {
                std::vector<Token> tokens;

                static const std::map<std::string, typename Token::Type> keywordMap{
                    {"true", Token::Type::keywordTrue},
                    {"false", Token::Type::keywordFalse},
                    {"null", Token::Type::keywordNull}
                };

                for (auto iterator = begin; iterator != end;)
                {
                    Token token;

                    if (static_cast<char>(*iterator) == '-' ||
                        (static_cast<char>(*iterator) >= '0' &&
                         static_cast<char>(*iterator) <= '9'))
                    {
                        token.type = Token::Type::literalInteger;

                        if (static_cast<char>(*iterator) == '-')
                        {
                            token.value.push_back(static_cast<char>(*iterator));
                            if (++iterator == end ||
                                static_cast<char>(*iterator) < '0' ||
                                static_cast<char>(*iterator) > '9')
                                throw ParseError("Invalid number");
                        }

                        while (iterator != end &&
                               (static_cast<char>(*iterator) >= '0' &&
                                static_cast<char>(*iterator) <= '9'))
                        {
                            token.value.push_back(static_cast<char>(*iterator));
                            ++iterator;
                        }

                        if (iterator != end &&
                            static_cast<char>(*iterator) == '.')
                        {
                            token.type = Token::Type::literalFloat;

                            token.value.push_back(static_cast<char>(*iterator));
                            ++iterator;

                            while (iterator != end &&
                                   (static_cast<char>(*iterator) >= '0' &&
                                    static_cast<char>(*iterator) <= '9'))
                            {
                                token.value.push_back(static_cast<char>(*iterator));
                                ++iterator;
                            }
                        }

                        // parse exponent
                        if (iterator != end &&
                            (static_cast<char>(*iterator) == 'e' ||
                             static_cast<char>(*iterator) == 'E'))
                        {
                            token.value.push_back(static_cast<char>(*iterator));

                            if (++iterator == end)
                                throw ParseError("Invalid exponent");

                            if (static_cast<char>(*iterator) == '+' ||
                                static_cast<char>(*iterator) == '-')
                                token.value.push_back(static_cast<char>(*iterator++));

                            if (iterator == end ||
                                static_cast<char>(*iterator) < '0' ||
                                static_cast<char>(*iterator) > '9')
                                throw ParseError("Invalid exponent");

                            while (iterator != end &&
                                   static_cast<char>(*iterator) >= '0' &&
                                   static_cast<char>(*iterator) <= '9')
                            {
                                token.value.push_back(static_cast<char>(*iterator));
                                ++iterator;
                            }
                        }
                    }
                    else if (static_cast<char>(*iterator) == '"') // string literal
                    {
                        token.type = Token::Type::literalString;

                        for (;;)
                        {
                            if (++iterator == end)
                                throw ParseError("Unterminated string literal");

                            if (static_cast<char>(*iterator) == '"')
                            {
                                ++iterator;
                                break;
                            }
                            else if (static_cast<char>(*iterator) == '\\')
                            {
                                if (++iterator == end)
                                    throw ParseError("Unterminated string literal");

                                switch (static_cast<char>(*iterator))
                                {
                                    case '"': token.value.push_back('"'); break;
                                    case '\\': token.value.push_back('\\'); break;
                                    case '/': token.value.push_back('/'); break;
                                    case 'b': token.value.push_back('\b'); break;
                                    case 'f': token.value.push_back('\f'); break;
                                    case 'n': token.value.push_back('\n'); break;
                                    case 'r': token.value.push_back('\r'); break;
                                    case 't': token.value.push_back('\t'); break;
                                    case 'u':
                                    {
                                        char32_t c = 0;

                                        for (std::uint32_t i = 0; i < 4; ++i)
                                        {
                                            if (iterator == end)
                                                throw ParseError("Unexpected end of data");

                                            std::uint8_t code = 0;

                                            if (static_cast<char>(*iterator) >= '0' && static_cast<char>(*iterator) <= '9')
                                                code = static_cast<std::uint8_t>(*iterator) - '0';
                                            else if (static_cast<char>(*iterator) >= 'a' && static_cast<char>(*iterator) <='f')
                                                code = static_cast<std::uint8_t>(*iterator) - 'a' + 10;
                                            else if (static_cast<char>(*iterator) >= 'A' && static_cast<char>(*iterator) <='F')
                                                code = static_cast<std::uint8_t>(*iterator) - 'A' + 10;
                                            else
                                                throw ParseError("Invalid character code");

                                            c = (c << 4) | code;

                                            ++iterator;
                                        }

                                        if (c <= 0x7F)
                                            token.value.push_back(static_cast<char>(c));
                                        else if (c <= 0x7FF)
                                        {
                                            token.value.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
                                            token.value.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                                        }
                                        else if (c <= 0xFFFF)
                                        {
                                            token.value.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
                                            token.value.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                                            token.value.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                                        }
                                        else
                                        {
                                            token.value.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
                                            token.value.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
                                            token.value.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                                            token.value.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                                        }

                                        break;
                                    }
                                    default:
                                        throw ParseError("Unrecognized escape character");
                                }
                            }
                            else if (static_cast<std::uint8_t>(*iterator) <= 0x1F) // control char
                                throw ParseError("Unterminated string literal");
                            else
                                token.value.push_back(static_cast<char>(*iterator));
                        }
                    }
                    else if ((static_cast<char>(*iterator) >= 'a' && static_cast<char>(*iterator) <= 'z') ||
                             (static_cast<char>(*iterator) >= 'A' && static_cast<char>(*iterator) <= 'Z') ||
                             static_cast<char>(*iterator) == '_')
                    {
                        while (iterator != end &&
                               ((static_cast<char>(*iterator) >= 'a' && static_cast<char>(*iterator) <= 'z') ||
                                (static_cast<char>(*iterator) >= 'A' && static_cast<char>(*iterator) <= 'Z') ||
                                static_cast<char>(*iterator) == '_' ||
                                (static_cast<char>(*iterator) >= '0' && static_cast<char>(*iterator) <= '9')))
                        {
                            token.value.push_back(static_cast<char>(*iterator));
                            ++iterator;
                        }

                        auto keywordIterator = keywordMap.find(token.value);

                        if (keywordIterator != keywordMap.end())
                            token.type = keywordIterator->second;
                        else
                            throw ParseError("Unknown keyword " + token.value);
                    }
                    else if (static_cast<char>(*iterator) == ' ' ||
                             static_cast<char>(*iterator) == '\t' ||
                             static_cast<char>(*iterator) == '\r' ||
                             static_cast<char>(*iterator) == '\n') // whitespace or newline
                    {
                        ++iterator;
                        continue;
                    }
                    else if (static_cast<char>(*iterator) == '\0')
                        break;
                    else
                    {
                        switch (static_cast<char>(*iterator))
                        {
                            case '{': token.type = Token::Type::leftBrace; break;
                            case '}': token.type = Token::Type::rightBrace; break;
                            case '[': token.type = Token::Type::leftBracket; break;
                            case ']': token.type = Token::Type::rightBracket; break;
                            case ',': token.type = Token::Type::comma; break;
                            case ':': token.type = Token::Type::colon; break;
                            default: throw ParseError("Unknown character");
                        }

                        ++iterator;
                    }

                    tokens.push_back(token);
                }

                return tokens;
            }

            static Value parse(typename std::vector<Token>::const_iterator& iterator,
                               typename std::vector<Token>::const_iterator end)
            {
                if (iterator == end)
                    throw ParseError("Unexpected end of data");

                Value result;

                if (iterator->type == Token::Type::leftBrace)
                {
                    result = Value::Type::object;

                    ++iterator; // skip the left brace

                    bool first = true;

                    for (;;)
                    {
                        if (iterator == end)
                            throw ParseError("Unexpected end of data");

                        if (iterator->type == Token::Type::rightBrace)
                        {
                            ++iterator;// skip the right brace
                            break;
                        }

                        if (first)
                            first = false;
                        else
                        {
                            if (iterator->type != Token::Type::comma)
                                throw ParseError("Expected a comma");

                            if (++iterator == end)
                                throw ParseError("Unexpected end of data");
                        }

                        if (iterator->type != Token::Type::literalString)
                            throw ParseError("Expected a string literal");

                        const std::string& key = iterator->value;

                        if (result.hasMember(key))
                            throw ParseError("Duplicate key value " + key);

                        if (++iterator == end)
                            throw ParseError("Unexpected end of data");

                        if (iterator->type != Token::Type::colon)
                            throw ParseError("Expected a colon");

                        if (++iterator == end)
                            throw ParseError("Unexpected end of data");

                        result[key] = parse(iterator, end);
                    }
                }
                else if (iterator->type == Token::Type::leftBracket)
                {
                    result = Value::Type::array;

                    ++iterator; // skip the left bracket

                    bool first = true;

                    for (;;)
                    {
                        if (iterator == end)
                            throw ParseError("Unexpected end of data");

                        if (iterator->type == Token::Type::rightBracket)
                        {
                            ++iterator;// skip the right bracket
                            break;
                        }

                        if (first)
                            first = false;
                        else
                        {
                            if (iterator->type != Token::Type::comma)
                                throw ParseError("Expected a comma");

                            if (++iterator == end)
                                throw ParseError("Unexpected end of data");
                        }

                        result.pushBack(parse(iterator, end));
                    }
                }
                else if (iterator->type == Token::Type::literalInteger)
                {
                    result = std::stoll(iterator->value);
                    ++iterator;
                }
                else if (iterator->type == Token::Type::literalFloat)
                {
                    result = std::stod(iterator->value);
                    ++iterator;
                }
                else if (iterator->type == Token::Type::literalString)
                {
                    result = iterator->value;
                    ++iterator;
                }
                else if (iterator->type == Token::Type::keywordTrue ||
                         iterator->type == Token::Type::keywordFalse)
                {
                    result = iterator->type == Token::Type::keywordTrue;
                    ++iterator;
                }
                else if (iterator->type == Token::Type::keywordNull)
                {
                    result = nullptr;
                    ++iterator;
                }
                else
                    throw ParseError("Expected a value");

                return result;
            }
        };

        return Parser::parse(begin, end);
    }

    inline Value parse(const char* data)
    {
        const char* end = data;
        while (*end) ++end;
        return parse(data, end);
    }

    template <class Data>
    Value parse(const Data& data)
    {
        return parse(std::begin(data), std::end(data));
    }

    inline std::string encode(const Value& value, bool whitespaces = false, bool byteOrderMark = false)
    {
        class Encoder final
        {
        public:
            static std::string encode(const Value& value, bool whitespaces, bool byteOrderMark)
            {
                std::string result;
                if (byteOrderMark) result.assign(std::begin(utf8ByteOrderMark),
                                                 std::end(utf8ByteOrderMark));
                encode(value, result, whitespaces);
                return result;
            }

        private:
            static void encode(const std::string& str, std::string& result)
            {
                for (const auto c : str)
                    if (c == '"') result.insert(result.end(), {'\\', '"'});
                    else if (c == '\\') result.insert(result.end(), {'\\', '\\'});
                    else if (c == '/') result.insert(result.end(), {'\\', '/'});
                    else if (c == '\b') result.insert(result.end(), {'\\', 'b'});
                    else if (c == '\f') result.insert(result.end(), {'\\', 'f'});
                    else if (c == '\n') result.insert(result.end(), {'\\', 'n'});
                    else if (c == '\r') result.insert(result.end(), {'\\', 'r'});
                    else if (c == '\t') result.insert(result.end(), {'\\', 't'});
                    else if (c <= 0x1F)
                    {
                        result.insert(result.end(), {'\\', 'u'});

                        constexpr char digits[] = "0123456789abcdef";
                        for (std::uint32_t p = 0; p < 4; ++p)
                            result.push_back(digits[(c >> (12 - p * 4)) & 0x0F]);
                    }
                    else
                        result.push_back(c);
            }

            static void encode(const Value& value, std::string& result, bool whitespaces, size_t level = 0)
            {
                switch (value.getType())
                {
                    case Value::Type::null:
                    {
                        result.insert(result.end(), {'n', 'u', 'l', 'l'});
                        break;
                    }
                    case Value::Type::integer:
                    {
                        const auto str = std::to_string(value.as<std::int64_t>());
                        result.insert(result.end(), str.begin(), str.end());
                        break;
                    }
                    case Value::Type::floatingPoint:
                    {
                        const auto str = std::to_string(value.as<double>());
                        result.insert(result.end(), str.begin(), str.end());
                        break;
                    }
                    case Value::Type::string:
                        result.push_back('"');
                        encode(value.as<std::string>(), result);
                        result.push_back('"');
                        break;
                    case Value::Type::object:
                    {
                        result.push_back('{');
                        if (whitespaces) result.push_back('\n');

                        const auto& entries = value.as<Value::Object>();

                        for (auto entryIterator = entries.begin(); entryIterator != entries.end();)
                        {
                            const auto& entry = *entryIterator;
                            if (whitespaces) result.insert(result.end(), level + 1, '\t');
                            result.push_back('"');
                            encode(entry.first, result);
                            result.insert(result.end(), {'"', ':'});
                            encode(entry.second, result, whitespaces, level + 1);
                            if (++entryIterator != entries.end()) result.push_back(',');
                            if (whitespaces) result.push_back('\n');
                        }

                        if (whitespaces) result.insert(result.end(), level, '\t');
                        result.push_back('}');
                        break;
                    }
                    case Value::Type::array:
                    {
                        result.push_back('[');
                        if (whitespaces) result.push_back('\n');

                        const auto& entries = value.as<Value::Array>();

                        for (auto entryIterator = entries.begin(); entryIterator != entries.end();)
                        {
                            const auto& entry = *entryIterator;
                            if (whitespaces) result.insert(result.end(), level + 1, '\t');
                            encode(entry, result, whitespaces, level + 1);
                            if (++entryIterator != entries.end()) result.push_back(',');
                            if (whitespaces) result.push_back('\n');
                        }

                        if (whitespaces) result.insert(result.end(), level, '\t');
                        result.push_back(']');
                        break;
                    }
                    case Value::Type::boolean:
                        if (value.as<bool>()) result.insert(result.end(), {'t', 'r', 'u', 'e'});
                        else result.insert(result.end(), {'f', 'a', 'l', 's', 'e'});
                        break;
                    default:
                        throw ParseError("Unknown value type");
                }
            }
        };

        return Encoder::encode(value, whitespaces, byteOrderMark);
    }
} // namespace json

#endif // JSON_HPP

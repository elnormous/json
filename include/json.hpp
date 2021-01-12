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
#include <variant>
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

        Value() = default;

        template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>* = nullptr>
        Value(const T v): value{static_cast<double>(v)} {}

        Value(const std::string& v): value{v} {}

        Value(const char* v): value{v} {}

        Value(const bool v): value{v} {}

        Value(const std::nullptr_t): value{nullptr} {}

        Value(const Array& v): value(v) {}

        Value(const Object& v): value{v} {}

        template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>* = nullptr>
        Value& operator=(const T v) noexcept
        {
            value = static_cast<double>(v);
            return *this;
        }

        Value& operator=(const std::string& v)
        {
            value = v;
            return *this;
        }

        Value& operator=(const char* v)
        {
            value = v;
            return *this;
        }

        Value& operator=(const bool v) noexcept
        {
            value = v;
            return *this;
        }

        Value& operator=(std::nullptr_t) noexcept
        {
            return *this;
        }

        Value& operator=(const Array& v)
        {
            value = v;
            return *this;
        }

        Value& operator=(const Object& v)
        {
            value = v;
            return *this;
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, std::string>>* = nullptr>
        std::string& as() noexcept
        {
            if (auto* result = std::get_if<std::string>(&value))
                return *result;
            else
            {
                value = std::string{};
                return *std::get_if<std::string>(&value);
            }
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, std::string>>* = nullptr>
        const std::string& as() const
        {
            if (auto* result = std::get_if<std::string>(&value))
                return *result;
            else
                throw TypeError("Wrong type");
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, const char*>>* = nullptr>
        T as() const
        {
            if (auto* result = std::get_if<std::string>(&value))
                return result->c_str();
            else
                throw TypeError("Wrong type");
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, bool>>* = nullptr>
        T as()
        {
            if (auto* result = std::get_if<bool>(&value))
                return *result;
            else
            {
                value = false;
                return *std::get_if<bool>(&value);
            }
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, bool>>* = nullptr>
        T as() const
        {
            if (auto* result = std::get_if<bool>(&value))
                return *result;
            else
                throw TypeError("Wrong type");
        }

        template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>* = nullptr>
        T as()
        {
            if (auto* result = std::get_if<double>(&value))
                return static_cast<T>(*result);
            else
            {
                value = 0.0;
                return static_cast<T>(*std::get_if<double>(&value));
            }
        }

        template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>* = nullptr>
        T as() const
        {
            if (auto* result = std::get_if<double>(&value))
                return static_cast<T>(*result);
            else
                throw TypeError("Wrong type");
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Object>>* = nullptr>
        T& as() noexcept
        {
            if (auto* result = std::get_if<Object>(&value))
                return *result;
            else
            {
                value = Object{};
                return *std::get_if<Object>(&value);
            }
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Object>>* = nullptr>
        const T& as() const
        {
            if (auto* result = std::get_if<Object>(&value))
                return *result;
            else
                throw TypeError("Wrong type");
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Array>>* = nullptr>
        T& as() noexcept
        {
            if (auto* result = std::get_if<Array>(&value))
                return *result;
            else
            {
                value = Array{};
                return *std::get_if<Array>(&value);
            }
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Array>>* = nullptr>
        const T& as() const
        {
            if (auto* result = std::get_if<Array>(&value))
                return *result;
            else
                throw TypeError("Wrong type");
        }

        Array::iterator begin()
        {
            if (auto* result = std::get_if<Array>(&value))
                return result->begin();
            else
                throw TypeError("Wrong type");
        }

        Array::iterator end()
        {
            if (auto* result = std::get_if<Array>(&value))
                return result->end();
            else
                throw TypeError("Wrong type");
        }

        Array::const_iterator begin() const
        {
            if (auto* result = std::get_if<Array>(&value))
                return result->begin();
            else
                throw TypeError("Wrong type");
        }

        Array::const_iterator end() const
        {
            if (auto* result = std::get_if<Array>(&value))
                return result->end();
            else
                throw TypeError("Wrong type");
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, std::nullptr_t>>* = nullptr>
        bool is() const noexcept
        {
            return std::holds_alternative<std::nullptr_t>(value);
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, std::string>>* = nullptr>
        bool is() const noexcept
        {
            return std::holds_alternative<std::string>(value);
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, const char*>>* = nullptr>
        bool is() const noexcept
        {
            return std::holds_alternative<std::string>(value);
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, bool>>* = nullptr>
        bool is() const noexcept
        {
            return std::holds_alternative<bool>(value);
        }

        template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>* = nullptr>
        bool is() const noexcept
        {
            return std::holds_alternative<double>(value);
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Object>>* = nullptr>
        bool is() const noexcept
        {
            return std::holds_alternative<Object>(value);
        }

        template <typename T, typename std::enable_if_t<std::is_same_v<T, Array>>* = nullptr>
        bool is() const noexcept
        {
            return std::holds_alternative<Array>(value);
        }

        bool hasMember(const std::string& member) const
        {
            if (auto* object = std::get_if<Object>(&value))
                return object->find(member) != object->end();
            else
                throw TypeError("Wrong type");
        }

        Value& operator[](const std::string& member) &
        {
            if (auto* object = std::get_if<Object>(&value))
                return (*object)[member];
            else
            {
                value = Object{};
                return (*std::get_if<Object>(&value))[member];
            }
        }

        const Value& operator[](const std::string& member) const&
        {
            if (auto* object = std::get_if<Object>(&value))
            {
                auto i = object->find(member);
                if (i != object->end())
                    return i->second;
                else
                    throw RangeError("Member does not exist");
            }
            else
                throw TypeError("Wrong type");
        }

        Value& operator[](std::size_t index) &
        {
            if (auto* array = std::get_if<Array>(&value))
            {
                if (index >= array->size()) array->resize(index + 1);
                return (*array)[index];
            }
            else
            {
                value = Array(index + 1);
                return (*std::get_if<Array>(&value))[index];
            }
        }

        const Value& operator[](std::size_t index) const&
        {
            if (auto* array = std::get_if<Array>(&value))
            {
                if (index < array->size())
                    return (*array)[index];
                else
                    throw RangeError("Index out of range");
            }
            else
                throw TypeError("Wrong type");
        }

        std::size_t getSize() const
        {
            if (auto* array = std::get_if<Array>(&value))
                return array->size();
            else
                throw TypeError("Wrong type");
        }

        void resize(std::size_t size) &
        {
            if (auto* array = std::get_if<Array>(&value))
                array->resize(size);
            else
                throw TypeError("Wrong type");
        }

        void pushBack(const Value& v) &
        {
            if (auto* array = std::get_if<Array>(&value))
                array->push_back(v);
            else
                throw TypeError("Wrong type");
        }

    private:
        std::variant<std::nullptr_t, bool, double, Object, Array, std::string> value;
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
                auto iterator = hasByteOrderMark(begin, end) ? begin + 3 : begin;
                auto [result, valueIterator] = parseValue(iterator, end);
                iterator = skipWhitespaces(valueIterator, end);
                if (iterator != end)
                    throw ParseError("Unexpected data");

                return result;
            }

        private:
            static bool hasByteOrderMark(Iterator begin, Iterator end) noexcept
            {
                for (auto i = std::begin(utf8ByteOrderMark); i != std::end(utf8ByteOrderMark); ++i)
                    if (begin == end || static_cast<std::uint8_t>(*begin) != *i)
                        return false;
                    else
                        ++begin;
                return true;
            }

            static constexpr bool isWhitespace(const char c) noexcept
            {
                return c == ' ' || c == '\t' || c == '\r' || c == '\n';
            }

            static Iterator skipWhitespaces(Iterator begin, Iterator end)
            {
                for (auto i = begin; i != end; ++i)
                    if (!isWhitespace(static_cast<char>(*i))) return i;
                return end;
            }

            static std::pair<bool, Iterator> isSame(Iterator begin, Iterator end,
                                                    const char* expectedBegin, const char* expectedEnd)
            {
                auto iterator = begin;

                while (iterator != end && expectedBegin != expectedEnd)
                {
                    if (static_cast<char>(*iterator) != *expectedBegin)
                        return std::pair(false, iterator);

                    ++iterator;
                    ++expectedBegin;
                }

                return std::pair(true, iterator);
            }

            static std::pair<Value, Iterator> parseValue(Iterator begin, Iterator end)
            {
                Iterator iterator = skipWhitespaces(begin, end);

                if (iterator == end)
                    throw ParseError("Unexpected end of data");

                if (static_cast<char>(*iterator) == '{')
                {
                    ++iterator;

                    Value result = Value::Object{};

                    bool firstValue = true;

                    while ((iterator = skipWhitespaces(iterator, end)) != end &&
                           static_cast<char>(*iterator) != '}')
                    {
                        if (firstValue)
                            firstValue = false;
                        else
                        {
                            if (static_cast<char>(*iterator++) != ',')
                                throw ParseError("Invalid object");

                            iterator = skipWhitespaces(iterator, end);
                        }

                        auto [key, stringIterator] = parseString(iterator, end);
                        iterator = skipWhitespaces(stringIterator, end);

                        if (static_cast<char>(*iterator++) != ':')
                            throw ParseError("Invalid object");

                        iterator = skipWhitespaces(iterator, end);

                        auto [value, valueIterator] = parseValue(iterator, end);
                        iterator = valueIterator;
                        result[key] = std::move(value);
                    }

                    if (iterator == end || static_cast<char>(*iterator) != '}')
                        throw ParseError("Invalid object");

                    ++iterator;

                    return std::pair(result, iterator);
                }
                else if (static_cast<char>(*iterator) == '[')
                {
                    ++iterator;

                    Value result = Value::Array{};

                    bool firstValue = true;

                    while ((iterator = skipWhitespaces(iterator, end)) != end &&
                           static_cast<char>(*iterator) != ']')
                    {
                        if (firstValue)
                            firstValue = false;
                        else
                        {
                            if (static_cast<char>(*iterator++) != ',')
                                throw ParseError("Invalid object");

                            iterator = skipWhitespaces(iterator, end);
                        }

                        auto [value, valueIterator] = parseValue(iterator, end);
                        iterator = valueIterator;
                        result.pushBack(value);
                    }

                    if (iterator == end || static_cast<char>(*iterator) != ']')
                        throw ParseError("Invalid array");

                    ++iterator;

                    return std::pair(result, iterator);
                }
                else if (static_cast<char>(*iterator) == '-' ||
                         (static_cast<char>(*iterator) >= '0' &&
                          static_cast<char>(*iterator) <= '9'))
                {
                    std::string value;

                    if (static_cast<char>(*iterator) == '-')
                    {
                        value.push_back('-');

                        if (++iterator == end ||
                            static_cast<char>(*iterator) < '0' ||
                            static_cast<char>(*iterator) > '9')
                            throw ParseError("Invalid number");
                    }

                    while (iterator != end &&
                           (static_cast<char>(*iterator) >= '0' &&
                            static_cast<char>(*iterator) <= '9'))
                    {
                        value.push_back(static_cast<char>(*iterator));
                        ++iterator;
                    }

                    if (iterator != end &&
                        static_cast<char>(*iterator) == '.')
                    {
                        value.push_back(static_cast<char>(*iterator));
                        ++iterator;

                        while (iterator != end &&
                               (static_cast<char>(*iterator) >= '0' &&
                                static_cast<char>(*iterator) <= '9'))
                        {
                            value.push_back(static_cast<char>(*iterator));
                            ++iterator;
                        }
                    }
                    else
                        return std::pair(Value{std::stoll(value)}, iterator);

                    // parse exponent
                    if (iterator != end &&
                        (static_cast<char>(*iterator) == 'e' ||
                         static_cast<char>(*iterator) == 'E'))
                    {
                        value.push_back(static_cast<char>(*iterator));

                        if (++iterator == end)
                            throw ParseError("Invalid exponent");

                        if (static_cast<char>(*iterator) == '+' ||
                            static_cast<char>(*iterator) == '-')
                            value.push_back(static_cast<char>(*iterator++));

                        if (iterator == end ||
                            static_cast<char>(*iterator) < '0' ||
                            static_cast<char>(*iterator) > '9')
                            throw ParseError("Invalid exponent");

                        while (iterator != end &&
                               static_cast<char>(*iterator) >= '0' &&
                               static_cast<char>(*iterator) <= '9')
                        {
                            value.push_back(static_cast<char>(*iterator));
                            ++iterator;
                        }
                    }

                    return std::pair(Value{std::stod(value)}, iterator);
                }
                else if (static_cast<char>(*iterator) == '"')
                {
                    auto [stringValue, stringIterator] = parseString(iterator, end);
                    iterator = stringIterator;
                    return std::pair(Value{stringValue}, iterator);
                }
                else
                {
                    constexpr char trueString[] = {'t', 'r', 'u', 'e'};
                    constexpr char falseString[] = {'f', 'a', 'l', 's', 'e'};
                    constexpr char nullString[] = {'n', 'u', 'l', 'l'};

                    auto [isTrue, trueIterator] = isSame(iterator, end, std::begin(trueString), std::end(trueString));
                    iterator = trueIterator;
                    if (isTrue)
                        return std::pair(Value{true}, iterator);

                    auto [isFalse, falseIterator] = isSame(iterator, end, std::begin(falseString), std::end(falseString));
                    iterator = falseIterator;
                    if (isFalse)
                        return std::pair(Value{false}, iterator);

                    auto [isNull, nullIterator] = isSame(iterator, end, std::begin(nullString), std::end(nullString));
                    iterator = nullIterator;
                    if (isNull)
                        return std::pair(Value{nullptr}, iterator);

                    throw ParseError("Unexpected identifier");
                }
            }

            static std::pair<std::string, Iterator> parseString(Iterator begin, Iterator end)
            {
                std::string result;
                Iterator iterator = begin;

                if (iterator == end || static_cast<char>(*iterator) != '"')
                    throw ParseError("Invalid string");

                ++iterator;

                while (iterator != end && static_cast<char>(*iterator) != '"')
                {
                    if (static_cast<char>(*iterator) == '\\')
                    {
                        if (++iterator == end)
                            throw ParseError("Unterminated string literal");

                        switch (static_cast<char>(*iterator))
                        {
                            case '"': result.push_back('"'); break;
                            case '\\': result.push_back('\\'); break;
                            case '/': result.push_back('/'); break;
                            case 'b': result.push_back('\b'); break;
                            case 'f': result.push_back('\f'); break;
                            case 'n': result.push_back('\n'); break;
                            case 'r': result.push_back('\r'); break;
                            case 't': result.push_back('\t'); break;
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
                                    result.push_back(static_cast<char>(c));
                                else if (c <= 0x7FF)
                                {
                                    result.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
                                    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                                }
                                else if (c <= 0xFFFF)
                                {
                                    result.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
                                    result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                                    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                                }
                                else
                                {
                                    result.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
                                    result.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
                                    result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                                    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
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
                        result.push_back(static_cast<char>(*iterator));

                    ++iterator;
                }

                if (iterator == end || static_cast<char>(*iterator) != '"')
                    throw ParseError("Invalid string");

                ++iterator;

                return std::pair(result, iterator);
            }
        };

        return Parser::parse(begin, end);
    }

    inline Value parse(const char* data)
    {
        auto end = data;
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
                if (value.is<std::nullptr_t>())
                    result.insert(result.end(), {'n', 'u', 'l', 'l'});
                else if (value.is<double>())
                {
                    const auto d = value.as<double>();
                    const auto str = (std::floor(d) == d) ? std::to_string(static_cast<std::int64_t>(d)) : std::to_string(d);
                    result.insert(result.end(), str.begin(), str.end());
                }
                else if (value.is<std::string>())
                {
                    result.push_back('"');
                    encode(value.as<std::string>(), result);
                    result.push_back('"');
                }
                else if (value.is<Value::Object>())
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
                }
                else if (value.is<Value::Array>())
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
                }
                else if (value.is<bool>())
                {
                    if (value.as<bool>()) result.insert(result.end(), {'t', 'r', 'u', 'e'});
                    else result.insert(result.end(), {'f', 'a', 'l', 's', 'e'});
                }
                else
                    throw ParseError("Unknown value type");
            }
        };

        return Encoder::encode(value, whitespaces, byteOrderMark);
    }
} // namespace json

#endif // JSON_HPP

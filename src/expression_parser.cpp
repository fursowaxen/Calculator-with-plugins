#include "expression_parser.h"

#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <string>

ExpressionParser::ExpressionParser(std::string_view input, const PluginManager& plugins) : input_(input), plugins_(plugins) {}

double ExpressionParser::parse()
{
    skipSpaces();
    if (end()) {
        throw std::runtime_error("Empty expression");
    }

    const double result = parseExpression();
    skipSpaces();

    if (!end()) {
        throw std::runtime_error("Unexpected character at position " + std::to_string(position_));
    }

    return result;
}

double ExpressionParser::parseExpression()
{
    double left = parseTerm();

    while (true) {
        skipSpaces();
        if (match('+'))      left += parseTerm();
        else if (match('-')) left -= parseTerm();
        else break;
    }

    return left;
}

double ExpressionParser::parseTerm()
{
    double left = parseUnary();

    while (true) {
        skipSpaces();
        if (match('*')) {
            left *= parseUnary();
        }
        else if (match('/')) {
            const double right = parseUnary();
            if (right == 0.0) {
                throw std::runtime_error("Division by zero");
            }
            left /= right;
        }
        else {
            break;
        }
    }

    return left;
}

double ExpressionParser::parseUnary()
{
    skipSpaces();
    if (match('+')) return parseUnary();
    if (match('-')) return -parseUnary();
    return parsePrimary();
}

double ExpressionParser::parsePrimary()
{
    skipSpaces();

    if (match('(')) {
        const double result = parseExpression();
        expect(')');
        return result;
    }

    if (position_ < input_.size()) {
        const unsigned char c = static_cast<unsigned char>(input_[position_]);

        if (std::isdigit(c) || input_[position_] == '.') {
            return parseNumber();
        }

        if (std::isalpha(c) || input_[position_] == '_') {
            const std::string name = parseIdentifier();

            skipSpaces();
            if (!match('(')) {
                throw std::runtime_error("Expected '(' after function name '" + name + "'");
            }

            const double argument = parseExpression();
            expect(')');

            auto result = plugins_.call(name, argument);
            if (!result.has_value()) {
                throw std::runtime_error("Cannot evaluate '" + name + "' (unknown function or plugin error)");
            }
            return *result;
        }
    }

    throw std::runtime_error("Unexpected character at position " + std::to_string(position_));
}

void ExpressionParser::skipSpaces()
{
    while (!end() &&
        std::isspace(static_cast<unsigned char>(input_[position_]))) {
        ++position_;
    }
}

bool ExpressionParser::match(char c)
{
    skipSpaces();
    if (!end() && input_[position_] == c) {
        ++position_;
        return true;
    }
    return false;
}

void ExpressionParser::expect(char c)
{
    if (!match(c)) {
        throw std::runtime_error(std::string("Expected '") + c + "' at position " + std::to_string(position_));
    }
}

bool ExpressionParser::end() const
{
    return position_ >= input_.size();
}

double ExpressionParser::parseNumber()
{
    skipSpaces();

    const std::size_t start = position_;
    const char* begin = input_.data() + position_;
    char* endPtr = nullptr;

    const double value = std::strtod(begin, &endPtr);
    if (endPtr == begin) {
        throw std::runtime_error("Invalid number at position " + std::to_string(start));
    }

    position_ += static_cast<std::size_t>(endPtr - begin);
    return value;
}

std::string ExpressionParser::parseIdentifier()
{
    skipSpaces();

    const std::size_t start = position_;
    while (!end()) {
        const unsigned char c = static_cast<unsigned char>(input_[position_]);
        if (!std::isalnum(c) && input_[position_] != '_') break;
        ++position_;
    }

    return std::string(input_.substr(start, position_ - start));
}
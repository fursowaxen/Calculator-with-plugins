#pragma once

#include "plugin_manager.h"
#include <string_view>

class ExpressionParser {
public:
    ExpressionParser(std::string_view input, const PluginManager& plugins);
    double parse();

private:
    double parseExpression();
    double parseTerm();
    double parseUnary();
    double parsePrimary();

    double parseNumber();
    std::string parseIdentifier();

    void skipSpaces();
    bool match(char c);
    void expect(char c);
    bool end() const;

    std::string_view input_;
    const PluginManager& plugins_;
    std::size_t position_{ 0 };
};
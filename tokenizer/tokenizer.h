#pragma once

#include <optional>
#include <variant>
#include "error.h"
#include <istream>

struct SymbolToken {
    std::string name;
    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value = 0;
    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    }
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in) : str_(in) {
        Next();
    }

    bool IsEnd() {
        return end_;
    }

    void Next() {
        while (std::isspace(str_->peek())) {
            str_->get();
        }

        if (str_->peek() == EOF) {
            end_ = true;
            return;
        }

        char c = str_->get();

        if (c == '(') {
            token_.emplace<BracketToken>(BracketToken::OPEN);
        } else if (c == ')') {
            token_.emplace<BracketToken>(BracketToken::CLOSE);
        } else if (c == '\'') {
            token_.emplace<QuoteToken>();
        } else if (c == '.') {
            token_.emplace<DotToken>();
        } else if (IsNumber(c, str_->peek())) {
            std::string value;
            value += c;
            while (std::isdigit(str_->peek())) {
                value += str_->get();
            }
            token_.emplace<ConstantToken>(ConstantToken{std::stoi(value)});
        } else if (IsOperator(c)) {
            std::string oper;
            oper += c;
            while (IsOther(str_->peek())) {
                oper += str_->get();
            }
            token_.emplace<SymbolToken>(SymbolToken{oper});
        } else {
            throw SyntaxError("");
        }
    }

    Token GetToken() {
        return token_;
    }

private:
    bool IsOperator(char c) const {
        static const std::string kOperChars = "<=>*/#+-";
        return std::isalpha(c) || kOperChars.find(c) != std::string::npos;
    }

    bool IsOther(char c) const {
        static const std::string kOtherChars = "<=>*/#?!-";
        return std::isalnum(c) || kOtherChars.find(c) != std::string::npos;
    }

    bool IsNumber(char c, char next_char) const {
        return std::isdigit(c) || ((c == '+' || c == '-') && std::isdigit(next_char));
    }

    bool end_ = false;
    std::istream* str_;
    Token token_;
};
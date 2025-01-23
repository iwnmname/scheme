#include <parser.h>

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("");
    }

    Token token = tokenizer->GetToken();

    if (std::holds_alternative<ConstantToken>(token)) {
        const auto* num = std::get_if<ConstantToken>(&token);
        tokenizer->Next();
        return std::make_shared<Number>(num->value);
    }

    if (std::holds_alternative<SymbolToken>(token)) {
        const auto* oper = std::get_if<SymbolToken>(&token);
        tokenizer->Next();
        return std::make_shared<Symbol>(oper->name);
    }

    if (token == Token{BracketToken::OPEN}) {
        return ReadList(tokenizer);
    }

    if (token == Token{DotToken{}}) {
        tokenizer->Next();
        return std::make_shared<Symbol>(".");
    }

    throw SyntaxError("");
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    tokenizer->Next();
    std::shared_ptr<Cell> ans = nullptr;
    std::shared_ptr<Cell> tmp = nullptr;
    while (!tokenizer->IsEnd()) {
        Token token = tokenizer->GetToken();
        if (token == Token{BracketToken::CLOSE}) {
            tokenizer->Next();
            return ans;
        }
        if (token == Token{DotToken{}}) {
            tokenizer->Next();
            if (!tmp) {
                throw SyntaxError("");
            }
            if (tokenizer->IsEnd()) {
                throw SyntaxError("");
            }
            std::shared_ptr<Object> stmp = Read(tokenizer);
            tmp->second_ = stmp;
            if (tokenizer->IsEnd()) {
                throw SyntaxError("");
            }
            if (tokenizer->GetToken() != Token{BracketToken::CLOSE}) {
                throw SyntaxError("");
            }
            tokenizer->Next();
            return ans;
        }
        std::shared_ptr<Object> elem = Read(tokenizer);
        std::shared_ptr<Cell> n_tmp = std::make_shared<Cell>();
        n_tmp->first_ = elem;
        ans ? (tmp->second_ = n_tmp) : (ans = n_tmp);
        tmp = n_tmp;
    }
    throw SyntaxError("");
}
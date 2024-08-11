// SPDX-FileCopyrightText: 2024 Conner Tenn
//
// SPDX-License-Identifier: 0BSD

#include "parser.hpp"

#include <spdlog/spdlog.h>
// #include <iostream>

Token::Token() : token("") {
}
Token::Token(std::string str) : token(str) {
}

std::ostream& operator<<(std::ostream &out, const Token &t) {
    out << t.str();
    return out;
}
std::ostream& operator<<(std::ostream &out, const std::vector<Token> &t) {
    out << "Token{";
    for (const auto &token : t) {
        out << token << ", ";
    }
    out << "}";
    return out;
}

bool Token::operator==(const Token& other) const {
    return this->token == other.token;
}

size_t Token::len() const {
    return this->token.length();
}

std::string Token::str() const {
    return this->token;
}

void Token::append(char character) {
    this->token.push_back(character);
}


std::vector<Token> tokenize(std::string expression) {
    std::vector<Token> token_list;
    Token token_accum;

    auto captureToken = [&]() {
        if (token_accum.len() > 0) {
            token_list.push_back(token_accum);
        }
        token_accum = Token();
    };

    for (const auto &character : expression) {
        if (isspace(character)) {
            captureToken();
        }
        else if (isalnum(character)) {
            token_accum.append(character);
        }
        else {
            switch (character)
            {
            case '&':
            case '|':
            case '+':
            case '-':
            case '!':
            case '(':
            case ')':
                captureToken();
                token_accum.append(character);
                captureToken();
                break;

            default:
                spdlog::trace("Unexpected Character {0}", character);
                break;
            }
        }
    }
    captureToken();
    return token_list;
}


AstNode *parse(std::vector<Token> tokens) {
    std::vector<AstNode *> rpn;




}


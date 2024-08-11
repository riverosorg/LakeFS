// SPDX-FileCopyrightText: 2024 Conner Tenn
//
// SPDX-License-Identifier: 0BSD
#pragma once

#include "ast.hpp"

class Token {
private:
    std::string token;

public:
    Token();
    Token(std::string str);

    friend std::ostream& operator<<(std::ostream &out, const Token &t);
    friend std::ostream& operator<<(std::ostream &out, const std::vector<Token> &t);
    bool operator==(const Token& other) const;

    size_t len() const;
    std::string str() const;
    void append(char character);

};


std::vector<Token> tokenize(std::string expression);
AstNode *parse(std::string expression);

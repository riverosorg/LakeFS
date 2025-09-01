// SPDX-FileCopyrightText: 2024 Conner Tenn
// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <memory>
#include <expected>

#include "ast.hpp"

class Token {
private:
    std::string token;

public:
    Token();
    Token(std::string str);

    bool operator==(const Token& other) const;

    size_t len() const;
    std::string str() const;
    void append(char character);

    friend std::ostream& operator<<(std::ostream &out, const Token &token);
};

std::ostream& operator<<(std::ostream &out, const std::vector<Token> &tokens);

std::vector<Token> tokenize(std::string expression);
std::optional<std::shared_ptr<AstNode>> parse(std::string expression);

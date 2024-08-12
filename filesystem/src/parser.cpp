// SPDX-FileCopyrightText: 2024 Conner Tenn
//
// SPDX-License-Identifier: 0BSD

#include "parser.hpp"

#include <sstream>


Token::Token() : token("") {
}
Token::Token(std::string str) : token(str) {
}

std::ostream& operator<<(std::ostream &out, const Token &token) {
    out << token.str();
    return out;
}
std::ostream& operator<<(std::ostream &out, const std::vector<Token> &tokens) {
    out << "Tokens{";
    for (const auto &token : tokens) {
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
            case '*':
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
                std::ostringstream msg;
                msg << "Unexpected Character " << character;
                throw std::runtime_error(msg.str());
                break;
            }
        }
    }
    captureToken();
    return token_list;
}


AstNode *parse(std::string expression) {
    std::vector<Token> tokens = tokenize(expression);

    std::vector<AstNode *> rpn;
    std::vector<Operator *> stack;

    auto putStack = [&](Operator *op) {
        if (stack.size() > 0) {
            Operator *last = stack.back();
            //Check if the last operator is equal or higher precedence.
            //This is done when equal in order to preserve argument ordering.
            //This ensures {1+2+3 --> 1 2 + 3 +} instead of {1+2+3 --> 1 2 3 + +}
            if (*last >= *op) {
                rpn.push_back(last);
                stack.pop_back();
            }
        }
        stack.push_back(op);
    };

    for (const auto &token : tokens) {
        std::string token_str = token.str();
        std::cout << token_str << std::endl;

        if (token_str == "&" || token_str == "*") {
            putStack(new Intersection());
        }
        else if (token_str == "|" || token_str == "+") {
            putStack(new Union());
        }
        else if (token_str == "!" || token_str == "-") {
            putStack(new Negation());
        }
        else if (token_str == "(") {
        }
        else if (token_str == ")") {
        }
        else {
            AstNode *node = new Tag(token_str);
            rpn.push_back(node);
        }
    }

    //Remove all remaining items from the stack
    while (stack.size() > 0) {
        rpn.push_back(stack.back());
        stack.pop_back();
    }

    std::cout << rpn << std::endl;
    std::cout << stack << std::endl;

    return 0;
}


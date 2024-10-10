// SPDX-FileCopyrightText: 2024 Conner Tenn
// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include "parser.hpp"

#include <sstream>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

Token::Token() 
    : token("") {}

Token::Token(std::string str) 
    : token(str) {}

std::ostream& operator<<(std::ostream &out, const Token &token) {
    out << token.str();
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
        else if (isalnum(character) || character == '_') {
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

std::vector<std::shared_ptr<AstNode>> parseRpn(std::vector<Token>::iterator* token_iter, std::vector<Token>::iterator end_iter) {
    std::vector<std::shared_ptr<AstNode>> rpn;
    std::vector<std::shared_ptr<Operator>> stack;

    auto putStack = [&](std::shared_ptr<Operator> op) {
        if (stack.size() > 0) {
            auto last = stack.back();
            //Check if the last operator is equal or higher precedence.
            //This is done when equal in order to preserve argument ordering.
            //This ensures {1+2+3 --> 1 2 + 3 +} instead of {1+2+3 --> 1 2 3 + +}
            if (last >= op) {
                rpn.push_back(last);
                stack.pop_back();
            }
        }
        stack.push_back(op);
    };

    //Loop through the tokens
    while(*token_iter != end_iter) {
        Token token = **token_iter;
        (*token_iter)++;

        std::string token_str = token.str();

        if (token_str == "&" || token_str == "*") {
            putStack(std::make_shared<Intersection>());
        }
        else if (token_str == "|" || token_str == "+") {
            putStack(std::make_shared<Union>());
        }
        else if (token_str == "!" || token_str == "-") {
            putStack(std::make_shared<Negation>());
        }
        else if (token_str == "(") {
            //Parse the sub-expression
            auto sub_expr = parseRpn(token_iter, end_iter);
            rpn.insert(rpn.end(), sub_expr.begin(), sub_expr.end());
        }
        else if (token_str == ")") {
            //Exit the for loop
            break;
        }
        else {
            auto node = std::make_shared<Tag>(token_str);
            rpn.push_back(node);
        }
    }

    //Remove all remaining items from the stack
    while (stack.size() > 0) {
        rpn.push_back(stack.back());
        stack.pop_back();
    }

    return rpn;
}

std::shared_ptr<AstNode> parse(std::string expression) {
    std::vector<Token> tokens = tokenize(expression);
    std::vector<Token>::iterator token_iter = tokens.begin();

    //Parse the expression and convert to RPN
    std::vector<std::shared_ptr<AstNode>> rpn = parseRpn(&token_iter, tokens.end());
    
    //Convert the RPN representation to an AST
    std::vector<std::shared_ptr<AstNode>>::iterator rpn_iter = rpn.begin();
    while (rpn_iter != rpn.end()) {
        spdlog::trace("RPN: {0}", (*rpn_iter)->str());

        // TODO: manipulating an iterator like this (deleting elements) is undefined behaviour
        (*rpn_iter)->assembleAST(&rpn, &rpn_iter);
        
        rpn_iter += 1;
    }

    spdlog::trace("AST: {0}", rpn.back()->str());

    return rpn.back();
}

std::ostream& operator<<(std::ostream &out, const std::vector<Token> &tokens) {
    out << "Tokens{";
    for (const auto &token : tokens) {
        out << token << ", ";
    }
    out << "}";
    return out;
}
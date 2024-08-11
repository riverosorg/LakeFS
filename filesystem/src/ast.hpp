// SPDX-FileCopyrightText: 2024 Conner Tenn
//
// SPDX-License-Identifier: 0BSD
#pragma once

#include <vector>
#include <string>

class AstNode
{
private:

public:
    AstNode();
};


class Operator : public AstNode {
private:
    int precedence;

public:

    Operator(int precedence);

    bool operator>=(const Operator& other) const;
};


class BinaryOperator : public Operator {
private:
    AstNode *left_node;
    AstNode *right_node;

public:

    BinaryOperator(int precedence);
};


class UnaryOperator : public Operator {
private:
    AstNode *node;

public:

    UnaryOperator(int precedence);
};


class Union : public BinaryOperator {

public:
    Union();

};


class Intersection : public BinaryOperator {

public:
    Intersection();

};


class Negation : public UnaryOperator {

public:
    Negation();

};


class Tag : public AstNode {
private:
    std::string name;

public:
    Tag(std::string name);
};




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
    virtual ~AstNode();
};

class BinaryOperator : public AstNode {
private:
    AstNode *left_node;
    AstNode *right_node;

public:

    BinaryOperator();
    ~BinaryOperator();
};


class UnaryOperator : public AstNode {
private:
    AstNode *node;

public:

    UnaryOperator();
    ~UnaryOperator();
};


class Union : public BinaryOperator {

public:
    Union();
    ~Union();

};


class Intersection : public BinaryOperator {

public:
    Intersection();
    ~Intersection();

};


class Negation : public UnaryOperator {

public:
    Negation();
    ~Negation();

};


class Tag : public AstNode {
private:
    std::string name;

public:
    Tag();
    ~Tag();
};




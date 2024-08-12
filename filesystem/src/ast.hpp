// SPDX-FileCopyrightText: 2024 Conner Tenn
//
// SPDX-License-Identifier: 0BSD
#pragma once

#include <vector>
#include <string>
#include <iostream>

class AstNode
{
private:

public:
    AstNode();

    virtual std::string str() const = 0;
    friend std::ostream& operator<<(std::ostream &out, const AstNode &node);
    friend std::ostream& operator<<(std::ostream &out, const std::vector<AstNode *> &nodes);
};


class Operator : public AstNode {
private:
    int precedence;

public:

    Operator(int precedence);

    virtual std::string str() const;
    friend std::ostream& operator<<(std::ostream &out, const std::vector<Operator *> &nodes);

    bool operator>=(const Operator& other) const;
};


class BinaryOperator : public Operator {
private:
    AstNode *left_node;
    AstNode *right_node;

public:

    BinaryOperator(int precedence);

    virtual std::string str() const;
};


class UnaryOperator : public Operator {
private:
    AstNode *node;

public:

    UnaryOperator(int precedence);

    virtual std::string str() const;
};


class Union : public BinaryOperator {

public:
    Union();

    virtual std::string str() const;
};


class Intersection : public BinaryOperator {

public:
    Intersection();

    virtual std::string str() const;
};


class Negation : public UnaryOperator {

public:
    Negation();

    virtual std::string str() const;
};


class Tag : public AstNode {
private:
    std::string name;

public:
    Tag(std::string name);

    virtual std::string str() const;
};




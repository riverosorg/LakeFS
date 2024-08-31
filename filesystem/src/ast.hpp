// SPDX-FileCopyrightText: 2024 Conner Tenn
// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <vector>
#include <string>
#include <iostream>

class AstNode {
private:

public:
    AstNode();

    virtual std::string str() const = 0;
    
    virtual bool match(const AstNode *other) const = 0;
    virtual void assembleAST(std::vector<AstNode *> *rpn, std::vector<AstNode *>::iterator *rpn_iter) = 0;
    
    friend std::ostream& operator<<(std::ostream &out, const AstNode &node);
};


class Operator : public AstNode {
private:
    int precedence;

public:

    Operator(int precedence);

    virtual std::string str() const;

    bool operator>=(const Operator& other) const;
};


class BinaryOperator : public Operator {
public:
    AstNode *left_node;
    AstNode *right_node;

    BinaryOperator(int precedence);
    BinaryOperator(int precedence, AstNode *left, AstNode *right);

    virtual std::string str() const;

    void assembleAST(std::vector<AstNode *> *rpn, std::vector<AstNode *>::iterator *rpn_iter);
};


class UnaryOperator : public Operator {
public:
    AstNode *node;

    UnaryOperator(int precedence);
    UnaryOperator(int precedence, AstNode *node);

    virtual std::string str() const;

    void assembleAST(std::vector<AstNode *> *rpn, std::vector<AstNode *>::iterator *rpn_iter);
};


class Union : public BinaryOperator {

public:
    Union();
    Union(AstNode *left, AstNode *right);

    virtual std::string str() const;
    virtual bool match(const AstNode *other) const;
};


class Intersection : public BinaryOperator {

public:
    Intersection();
    Intersection(AstNode *left, AstNode *right);

    virtual std::string str() const;
    virtual bool match(const AstNode *other) const;
};


class Negation : public UnaryOperator {

public:
    Negation();
    Negation(AstNode *node);

    virtual std::string str() const;
    virtual bool match(const AstNode *other) const;
};


class Tag : public AstNode {
public:
    std::string name;

    Tag(std::string name);

    virtual std::string str() const;
    virtual bool match(const AstNode *other) const;

    void assembleAST(std::vector<AstNode *> *rpn, std::vector<AstNode *>::iterator *rpn_iter);
};

std::ostream& operator<<(std::ostream &out, const std::vector<AstNode *> &nodes);
std::ostream& operator<<(std::ostream &out, const std::vector<Operator *> &nodes);

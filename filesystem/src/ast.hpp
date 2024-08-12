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
    virtual bool match(const AstNode *other) const = 0;

    virtual void assembleAST(std::vector<AstNode *> *rpn, std::vector<AstNode *>::iterator *rpn_iter) = 0;
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
protected:
    AstNode *left_node;
    AstNode *right_node;

public:

    BinaryOperator(int precedence);
    BinaryOperator(int precedence, AstNode *left, AstNode *right);

    virtual std::string str() const;

    void assembleAST(std::vector<AstNode *> *rpn, std::vector<AstNode *>::iterator *rpn_iter);
};


class UnaryOperator : public Operator {
protected:
    AstNode *node;

public:

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
private:
    std::string name;

public:
    Tag(std::string name);

    virtual std::string str() const;
    virtual bool match(const AstNode *other) const;

    void assembleAST(std::vector<AstNode *> *rpn, std::vector<AstNode *>::iterator *rpn_iter);
};




// SPDX-FileCopyrightText: 2024 Conner Tenn
// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class AstNode
{
  private:
  public:
    AstNode();

    virtual std::string str() const = 0;

    virtual bool match(const std::shared_ptr<AstNode> other) const = 0;
    virtual bool assembleAST(std::vector<std::shared_ptr<AstNode>>* rpn,
                             std::vector<std::shared_ptr<AstNode>>::iterator* rpn_iter) = 0;

    friend std::ostream& operator<<(std::ostream& out, const std::shared_ptr<AstNode> node);
};

class Operator : public AstNode
{
  private:
    int precedence;

  public:
    Operator(int precedence);

    virtual std::string str() const;

    bool operator>=(const std::shared_ptr<Operator> other) const;
};

class BinaryOperator : public Operator
{
  public:
    std::shared_ptr<AstNode> left_node;
    std::shared_ptr<AstNode> right_node;

    BinaryOperator(int precedence);
    BinaryOperator(int precedence, std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right);

    virtual std::string str() const;

    bool assembleAST(std::vector<std::shared_ptr<AstNode>>* rpn,
                     std::vector<std::shared_ptr<AstNode>>::iterator* rpn_iter);
};

class UnaryOperator : public Operator
{
  public:
    std::shared_ptr<AstNode> node;

    UnaryOperator(int precedence);
    UnaryOperator(int precedence, std::shared_ptr<AstNode> node);

    virtual std::string str() const;

    bool assembleAST(std::vector<std::shared_ptr<AstNode>>* rpn,
                     std::vector<std::shared_ptr<AstNode>>::iterator* rpn_iter);
};

class Union : public BinaryOperator
{

  public:
    Union();
    Union(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right);

    virtual std::string str() const;
    virtual bool match(const std::shared_ptr<AstNode> other) const;
};

class Intersection : public BinaryOperator
{

  public:
    Intersection();
    Intersection(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right);

    virtual std::string str() const;
    virtual bool match(const std::shared_ptr<AstNode> other) const;
};

class Negation : public UnaryOperator
{

  public:
    Negation();
    Negation(std::shared_ptr<AstNode> node);

    virtual std::string str() const;
    virtual bool match(const std::shared_ptr<AstNode> other) const;
};

class Tag : public AstNode
{
  public:
    std::string name;

    Tag(std::string name);

    virtual std::string str() const;
    virtual bool match(const std::shared_ptr<AstNode> other) const;

    bool assembleAST(std::vector<std::shared_ptr<AstNode>>* rpn,
                     std::vector<std::shared_ptr<AstNode>>::iterator* rpn_iter);
};

std::ostream& operator<<(std::ostream& out, const std::vector<std::shared_ptr<AstNode>>& nodes);
std::ostream& operator<<(std::ostream& out, const std::vector<std::shared_ptr<Operator>>& nodes);

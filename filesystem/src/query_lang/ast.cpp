// SPDX-FileCopyrightText: 2024 Conner Tenn
// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include <cassert>
#include <iterator>
#include <spdlog/spdlog.h>

#include "ast.hpp"

//== AstNode ==

AstNode::AstNode() {}

std::ostream& operator<<(std::ostream& out, const std::shared_ptr<AstNode> node)
{
    out << node->str();

    return out;
}

//== Operator ==

Operator::Operator(int precedence) : precedence(precedence) {}

std::string Operator::str() const { return "Operator"; }

bool Operator::operator>=(const std::shared_ptr<Operator> other) const
{
    return this->precedence >= other->precedence;
}

//== BinaryOperator ==

BinaryOperator::BinaryOperator(int precedence)
    : Operator(precedence), left_node(nullptr), right_node(nullptr)
{
}

BinaryOperator::BinaryOperator(int precedence, std::shared_ptr<AstNode> left,
                               std::shared_ptr<AstNode> right)
    : Operator(precedence), left_node(left), right_node(right)
{
}

std::string BinaryOperator::str() const
{
    std::string left_str = "Null";
    std::string right_str = "Null";

    if (this->left_node != nullptr)
    {
        left_str = this->left_node->str();
    }

    if (this->right_node != nullptr)
    {
        right_str = this->right_node->str();
    }

    return "BinaryOperator{" + left_str + "," + right_str + "}";
}

// TODO: I cant remember why these are raw pointers but they should be smart
bool BinaryOperator::assembleAST(std::vector<std::shared_ptr<AstNode>>* rpn,
                                 std::vector<std::shared_ptr<AstNode>>::iterator* rpn_iter)
{
    assert(rpn != nullptr);
    assert(rpn_iter != nullptr);

    // Check that memory access will be SAFE
    if (std::distance(rpn->begin(), *rpn_iter) < 2)
    {
        spdlog::error("Incorrect AST provided!");
        return false; // PANIC
    }

    // Collect the arguments
    this->left_node = *((*rpn_iter) - 2);
    this->right_node = *((*rpn_iter) - 1);

    rpn->erase((*rpn_iter) - 2, (*rpn_iter));
    (*rpn_iter) -= 2; // 2 elements removed

    return true;
}

//== UnaryOperator ==

UnaryOperator::UnaryOperator(int precedence) : Operator(precedence), node(nullptr) {}

UnaryOperator::UnaryOperator(int precedence, std::shared_ptr<AstNode> node)
    : Operator(precedence), node(node)
{
}

std::string UnaryOperator::str() const
{
    std::string node_str = "Null";

    if (this->node != nullptr)
    {
        node_str = this->node->str();
    }

    return "UnaryOperator{" + node_str + "}";
}

bool UnaryOperator::assembleAST(std::vector<std::shared_ptr<AstNode>>* rpn,
                                std::vector<std::shared_ptr<AstNode>>::iterator* rpn_iter)
{
    assert(rpn != nullptr);
    assert(rpn_iter != nullptr);

    // Check that memory access will be SAFE
    if (std::distance(rpn->begin(), *rpn_iter) < 1)
    {
        spdlog::error("Incorrect AST provided!");
        return false; // PANIC
    }

    // Collect the arguments
    this->node = *((*rpn_iter) - 1);

    rpn->erase((*rpn_iter) - 1, (*rpn_iter));
    (*rpn_iter) -= 1; // 1 element removed

    return true;
}

//== Union ==

Union::Union() : BinaryOperator(0) {}

Union::Union(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right)
    : BinaryOperator(0, left, right)
{
}

std::string Union::str() const { return "Union_" + BinaryOperator::str(); }

bool Union::match(const std::shared_ptr<AstNode> other) const
{
    const auto other_union = std::dynamic_pointer_cast<Union>(other);

    if (other_union != nullptr)
    {
        return this->left_node->match(other_union->left_node) &&
               this->right_node->match(other_union->right_node);
    }

    return false;
}

//== Intersection ==

Intersection::Intersection() : BinaryOperator(1) {}

Intersection::Intersection(std::shared_ptr<AstNode> left, std::shared_ptr<AstNode> right)
    : BinaryOperator(0, left, right)
{
}

std::string Intersection::str() const { return "Intersection_" + BinaryOperator::str(); }

bool Intersection::match(const std::shared_ptr<AstNode> other) const
{
    const auto other_intersection = std::dynamic_pointer_cast<Intersection>(other);

    if (other_intersection != nullptr)
    {
        return this->left_node->match(other_intersection->left_node) &&
               this->right_node->match(other_intersection->right_node);
    }

    return false;
}

//== Negation ==

Negation::Negation() : UnaryOperator(2) {}

Negation::Negation(std::shared_ptr<AstNode> node) : UnaryOperator(0, node) {}

std::string Negation::str() const { return "Negation_" + UnaryOperator::str(); }

bool Negation::match(const std::shared_ptr<AstNode> other) const
{
    const auto other_negation = std::dynamic_pointer_cast<Negation>(other);

    if (other_negation != nullptr)
    {
        return this->node->match(other_negation->node);
    }

    return false;
}

//== Tag ==

Tag::Tag(std::string name) : name(name) {}

std::string Tag::str() const { return "Tag{" + this->name + "}"; }

bool Tag::assembleAST(std::vector<std::shared_ptr<AstNode>>* rpn,
                      std::vector<std::shared_ptr<AstNode>>::iterator* rpn_iter)
{
    // Nothing needs to be done

    return true;
}

bool Tag::match(const std::shared_ptr<AstNode> other) const
{
    const auto other_tag = std::dynamic_pointer_cast<Tag>(other);

    if (other_tag != nullptr)
    {
        return this->name == other_tag->name;
    }

    return false;
}

// == Loose functions ==

std::ostream& operator<<(std::ostream& out, const std::vector<std::shared_ptr<AstNode>>& nodes)
{
    out << "AstNodes{";

    for (const auto& node : nodes)
    {
        out << node << ", ";
    }

    out << "}";

    return out;
}

std::ostream& operator<<(std::ostream& out, const std::vector<std::shared_ptr<Operator>>& nodes)
{
    out << "Operators{";

    for (const auto& node : nodes)
    {
        out << node << ", ";
    }

    out << "}";

    return out;
}
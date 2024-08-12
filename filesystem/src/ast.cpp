// SPDX-FileCopyrightText: 2024 Conner Tenn
//
// SPDX-License-Identifier: 0BSD

#include "ast.hpp"

AstNode::AstNode() {
}

std::ostream& operator<<(std::ostream &out, const AstNode &node) {
    out << node.str();
    return out;
}
std::ostream& operator<<(std::ostream &out, const std::vector<AstNode *> &nodes) {
    out << "AstNodes{";
    for (const auto &node : nodes) {
        out << *node << ", ";
    }
    out << "}";
    return out;
}


Operator::Operator(int precedence) : precedence(precedence) {
}

std::string Operator::str() const {
    return "Operator";
}

std::ostream& operator<<(std::ostream &out, const std::vector<Operator *> &nodes) {
    out << "Operators{";
    for (const auto &node : nodes) {
        out << *node << ", ";
    }
    out << "}";
    return out;
}

bool Operator::operator>=(const Operator& other) const {
    return this->precedence >= other.precedence;
}


BinaryOperator::BinaryOperator(int precedence) : Operator(precedence), left_node(NULL), right_node(NULL) {
}

std::string BinaryOperator::str() const {
    std::string left_str = "Null";
    std::string right_str = "Null";
    if (this->left_node != NULL) {
        left_str = this->left_node->str();
    }
    if (this->right_node != NULL) {
        right_str = this->right_node->str();
    }
    return "BinaryOperator{" + left_str + "," + right_str + "}";
}


UnaryOperator::UnaryOperator(int precedence) : Operator(precedence), node(NULL) {
}

std::string UnaryOperator::str() const {
    std::string node_str = "Null";
    if (this->node != NULL) {
        node_str = this->node->str();
    }
    return "UnaryOperator{" + node_str + "}";
}


Union::Union() : BinaryOperator(0) {
}

std::string Union::str() const {
    return "Union_" + BinaryOperator::str();
}


Intersection::Intersection() : BinaryOperator(1) {
}

std::string Intersection::str() const {
    return "Intersection_" + BinaryOperator::str();
}


Negation::Negation() : UnaryOperator(2) {
}

std::string Negation::str() const {
    return "Negation_" + UnaryOperator::str();
}


Tag::Tag(std::string name) : name(name) {
}

std::string Tag::str() const {
    return "Tag{" + this->name + "}";
}


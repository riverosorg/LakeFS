// SPDX-FileCopyrightText: 2024 Conner Tenn
//
// SPDX-License-Identifier: 0BSD

#include "ast.hpp"

AstNode::AstNode() {
}

Operator::Operator(int precedence) : precedence(precedence) {
}
bool Operator::operator>=(const Operator& other) const {
    return this->precedence >= other.precedence;
}

BinaryOperator::BinaryOperator(int precedence) : Operator(precedence) {
}

UnaryOperator::UnaryOperator(int precedence) : Operator(precedence) {
}

Union::Union() : BinaryOperator(0) {
}

Intersection::Intersection() : BinaryOperator(1) {
}

Negation::Negation() : UnaryOperator(2) {
}


Tag::Tag(std::string name) : name(name) {
}


// SPDX-FileCopyrightText: 2024 Conner Tenn
// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include "ast.hpp"

//== AstNode ==

AstNode::AstNode() {}

std::ostream& operator<<(std::ostream &out, const AstNode &node) {
    out << node.str();
    
    return out;
}

//== Operator ==

Operator::Operator(int precedence) 
    : precedence(precedence) {}

std::string Operator::str() const {
    return "Operator";
}

bool Operator::operator>=(const Operator& other) const {
    return this->precedence >= other.precedence;
}


//== BinaryOperator ==

BinaryOperator::BinaryOperator(int precedence) 
    : Operator(precedence), left_node(nullptr), right_node(nullptr) {}

BinaryOperator::BinaryOperator(int precedence, AstNode *left, AstNode *right) 
    : Operator(precedence), left_node(left), right_node(right) {}

std::string BinaryOperator::str() const {
    std::string left_str = "Null";
    std::string right_str = "Null";
    
    if (this->left_node != nullptr) {
        left_str = this->left_node->str();
    }
    
    if (this->right_node != nullptr) {
        right_str = this->right_node->str();
    }
    
    return "BinaryOperator{" + left_str + "," + right_str + "}";
}

void BinaryOperator::assembleAST(std::vector<AstNode *> *rpn, std::vector<AstNode *>::iterator *rpn_iter) {
    //Collect the arguments
    this->left_node = *((*rpn_iter)-2);
    this->right_node = *((*rpn_iter)-1);

    rpn->erase((*rpn_iter)-2, (*rpn_iter));
    (*rpn_iter) -= 2; //2 elements removed
}


//== UnaryOperator ==

UnaryOperator::UnaryOperator(int precedence) 
    : Operator(precedence), node(nullptr) {}

UnaryOperator::UnaryOperator(int precedence, AstNode *node) 
    : Operator(precedence), node(node) {}

std::string UnaryOperator::str() const {
    std::string node_str = "Null";
    
    if (this->node != nullptr) {
        node_str = this->node->str();
    }
    
    return "UnaryOperator{" + node_str + "}";
}

void UnaryOperator::assembleAST(std::vector<AstNode *> *rpn, std::vector<AstNode *>::iterator *rpn_iter) {
    //Collect the arguments
    this->node = *((*rpn_iter)-1);

    rpn->erase((*rpn_iter)-1, (*rpn_iter));
    (*rpn_iter) -= 1; //1 element removed
}


//== Union ==

Union::Union() 
    : BinaryOperator(0) {}

Union::Union(AstNode *left, AstNode *right) 
    : BinaryOperator(0, left, right) {}

std::string Union::str() const {
    return "Union_" + BinaryOperator::str();
}

bool Union::match(const AstNode *other) const {
    const Union *other_union = dynamic_cast<const Union *>(other);
    
    if (other_union != nullptr) {
        return this->left_node->match(other_union->left_node) 
            && this->right_node->match(other_union->right_node);
    }
    
    return false;
}


//== Intersection ==

Intersection::Intersection() 
    : BinaryOperator(1) {}

Intersection::Intersection(AstNode *left, AstNode *right) 
    : BinaryOperator(0, left, right) {}

std::string Intersection::str() const {
    return "Intersection_" + BinaryOperator::str();
}

bool Intersection::match(const AstNode *other) const {
    const Intersection *other_intersection = dynamic_cast<const Intersection *>(other);
    
    if (other_intersection != nullptr) {
        return this->left_node->match(other_intersection->left_node) 
            && this->right_node->match(other_intersection->right_node);
    }
    
    return false;
}


//== Negation ==

Negation::Negation() 
    : UnaryOperator(2) {}

Negation::Negation(AstNode *node) 
    : UnaryOperator(0, node) {}

std::string Negation::str() const {
    return "Negation_" + UnaryOperator::str();
}

bool Negation::match(const AstNode *other) const {
    const Negation *other_negation = dynamic_cast<const Negation *>(other);
    
    if (other_negation != nullptr) {
        return this->node->match(other_negation->node);
    }
    
    return false;
}


//== Tag ==

Tag::Tag(std::string name) 
    : name(name) {}

std::string Tag::str() const {
    return "Tag{" + this->name + "}";
}

void Tag::assembleAST(std::vector<AstNode *> *rpn, std::vector<AstNode *>::iterator *rpn_iter) {
    //Nothing needs to be done
}

bool Tag::match(const AstNode *other) const {
    const Tag *other_tag = dynamic_cast<const Tag *>(other);
    
    if (other_tag != nullptr) {
        return this->name == other_tag->name;
    }
    
    return false;
}

// == Loose functions ==

std::ostream& operator<<(std::ostream &out, const std::vector<AstNode *> &nodes) {
    out << "AstNodes{";
    
    for (const auto &node : nodes) {
        out << *node << ", ";
    }
    
    out << "}";
    
    return out;
}

std::ostream& operator<<(std::ostream &out, const std::vector<Operator *> &nodes) {
    out << "Operators{";
    
    for (const auto &node : nodes) {
        out << *node << ", ";
    }
    
    out << "}";
    
    return out;
}
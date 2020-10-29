#include "abstractsyntaxtree.h"
using std::string;
/**
 * Calculates the value from an AST (Abstract Syntax Tree). To parse numbers from strings, please use std::stod
 * @return A double representing the calculated value from the expression transformed into an AST
 */
double AbstractSyntaxTree::eval() const {
    // @TODO Your code goes here...

    return eval(root);
}


double AbstractSyntaxTree::eval(Node* subRoot) const{
    string s = subRoot->elem;
    if(s=="+"){
      return eval(subRoot->left) + eval(subRoot->right);
    }
    if(s=="-"){
      return eval(subRoot->left) - eval(subRoot->right);
    }
    if(s=="/"){
      return eval(subRoot->left) / eval(subRoot->right);
    }
    if(s=="*"){
      return eval(subRoot->left) * eval(subRoot->right);
    }
    return std::stod(s);
}

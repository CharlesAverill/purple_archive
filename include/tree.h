/**
 * @file
 * @author CharlesAverill
 * @date   06-Oct-2021
 * @brief Function headers for AST operations
*/

#ifndef TREE_H
#define TREE_H

#include <stdio.h>

#include "definitions.h"

AST_Node *make_ast_node(Token_Type ttype, AST_Node *left, AST_Node *mid, AST_Node *right, int value);
AST_Node *make_ast_leaf(Token_Type ttype, int value);
AST_Node *make_unary_ast_node(Token_Type ttype, AST_Node *child, int value);

#endif

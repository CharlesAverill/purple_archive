/**
 * @file
 * @author CharlesAverill
 * @date   06-Oct-2021
 * @brief Functions for defining an Abstract Syntax Tree (AST)
*/

#include <stdlib.h>

#include "tree.h"

/**
 * Constructs a new AST Node with the provided values
 * @param  ttype               Token_Type of the new AST Node
 * @param  left                Left child subtree of the new AST Node
 * @param  right               Right child subtree of the new AST Node
 * @param  value               If Token_Type == T_INTLIT, then the value of the integer literal
 * @return       The pointer to a new AST Node with the provided values
 */
AST_Node *make_ast_node(Token_Type ttype, AST_Node *left, AST_Node *right, int value)
{
    AST_Node *out;

    // Allocate memory for new node
    out = (AST_Node *)malloc(sizeof(AST_Node));
    if (out == NULL) {
        fprintf(stderr, "Unable to allocate memory for new AST Node");
        shutdown(1);
    }

    // Assign values
    out->ttype = ttype;
    out->left = left;
    out->right = right;
    out->v.value = value;

    return out;
}

/**
 * Constructs a new AST Leaf Node with the provided values
 * @param  ttype               Token_Type of the new AST Node
 * @param  value               If Token_Type == T_INTLIT, then the value of the integer literal
 * @return       The pointer to a new AST Leaf Node with the provided values
 */
AST_Node *make_ast_leaf(Token_Type ttype, int value)
{
    return make_ast_node(ttype, NULL, NULL, value);
}

/**
 * Constructs a new AST Unary Parent Node with the provided values
 * @param  ttype               Token_Type of the new AST Node
 * @param  child               The AST Node's single child
 * @param  value               If Token_Type == T_INTLIT, then the value of the integer literal
 * @return       The pointer to a new AST Unary Parent Node with the provided values
 */
AST_Node *make_unary_ast_node(Token_Type ttype, AST_Node *child, int value)
{
    return make_ast_node(ttype, child, NULL, value);
}

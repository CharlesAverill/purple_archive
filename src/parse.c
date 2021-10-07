/**
 * @file
 * @author charlesaverill
 * @date   06-Oct-2021
 * @brief Expression parsing logic
*/

#include "parse.h"

/**
 * Build a terminal AST Node for a given token, exit if not a valid primary token
 * @param  t               The token to check and build
 * @return   An AST Node built from the provided token, or an error if the token is non-terminal
 */
static AST_Node *build_terminal_node(token t)
{
    AST_Node *out;

    if (t._token == T_INTLIT) {
        out = make_ast_leaf(T_INTLIT, t.value);
    } else {
        fprintf(stderr, "Syntax error on line %d\n", D_LINE_NUMBER);
        exit(1);
    }

    return out;
}

/**
 * Given an initial token, recursively parse binary expressions into an AST
 * @param  t               The current token to be parsed
 * @return   An AST or AST Subtree of the binary expressions in D_INPUT_FILE
 */
AST_Node *parse_binary_expression(token t)
{
    AST_Node *out;
    AST_Node *left;
    AST_Node *right;
    Token_Type ttype;

    // Get the intlit on the left
    left = build_terminal_node(t);

    // Scan the next token
    scan(&t);

    // Check for EOF
    if (t._token == T_EOF) {
        return left;
    }

    // This token should be an operator, so store its value
    ttype = t._token;

    // Scan the next token
    scan(&t);

    // Recursively build the right AST subtree
    right = parse_binary_expression(t);

    // Build and return the output node
    out = make_ast_node(ttype, left, right, 0);
    return out;
}

/**
 * Interprets a given AST Node *without* operator precedence. Will print intermediate steps if D_DEBUG is 1
 * @param  n               AST Node to interpret
 * @return   The interpreted value of the AST Node
 */
int interpret_AST(AST_Node *n)
{
    int left;
    int right;

    // Compute left and right subtrees
    if (n->left) {
        left = interpret_AST(n->left);
    }
    if (n->right) {
        right = interpret_AST(n->right);
    }

    if (D_DEBUG) {
        if (n->ttype == T_INTLIT) {
            printf("int %d\n", n->value);
        } else {
            printf("%d %s %d\n", left, token_strings[n->ttype], right);
        }
    }

    // Compute expression value without precedence
    switch (n->ttype) {
    case T_PLUS:
        return left + right;
    case T_MINUS:
        return left - right;
    case T_STAR:
        return left * right;
    case T_SLASH:
        return left / right;
    case T_INTLIT:
        return n->value;
    default:
        fprintf(stderr, "Unknown operator with ttype %d\n", n->ttype);
        exit(1);
    }
}

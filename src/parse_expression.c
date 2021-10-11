/**
 * @file
 * @author charlesaverill
 * @date   06-Oct-2021
 * @brief Function for parsing expressions into ASTs
*/

#include "parse.h"

/**
 * Determine the precedence of a provided operator, with syntax checking
 * @param  ttype           The token type of the operator
 * @return   An integer defining the precedence of the given operator
 */
static int operator_precedence(Token_Type ttype)
{
    int prec = token_precedence[ttype];

    if (prec == 0) {
        fprintf(stderr, "Syntax error on line %d, token %d\n", D_LINE_NUMBER, ttype);
        exit(1);
    }

    return prec;
}

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
 * Given the precedence of the previous token, recursively parse binary expressions into an AST
 * @param  previous_token_precedence          The integer precedence value of the previous token
 * @return   An AST or AST Subtree of the binary expressions in D_INPUT_FILE
 */
AST_Node *parse_binary_expression(int previous_token_precedence)
{
    AST_Node *left;
    AST_Node *right;

    // Get the intlit on the left and scan the next token
    left = build_terminal_node(GToken);
    scan(&GToken);

    // Check for EOF
    Token_Type current_ttype = GToken._token;
    if (current_ttype == T_SEMICOLON) {
        return left;
    }

    // While current token has greater precedence than previous token
    while (operator_precedence(current_ttype) > previous_token_precedence) {
        // Scan the next token
        scan(&GToken);

        // Recursively build the right AST subtree
        right = parse_binary_expression(token_precedence[current_ttype]);

        // Join right subtree with current left subtree
        left = make_ast_node(current_ttype, left, right, 0);

        // Update current_ttype and check for EOF
        current_ttype = GToken._token;
        if (current_ttype == T_SEMICOLON) {
            break;
        }
    }

    return left;
}

/**
 * Interprets a given AST Node with operator precedence. Will print intermediate steps if D_DEBUG is 1
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

/**
 * @file
 * @author CharlesAverill
 * @date   06-Oct-2021
 * @brief Function for parsing expressions into ASTs
*/

#include "parse.h"
#include "symbol_table.h"

/**
 * Determine the precedence of a provided operator, with syntax checking
 * @param  ttype           The token type of the operator
 * @return   An integer defining the precedence of the given operator
 */
static int operator_precedence(Token_Type ttype)
{
    int prec = token_precedence[ttype];

    if (prec == 0) {
        fprintf(stderr, "Syntax error on line %d, token \"%s\"\n", D_LINE_NUMBER,
                token_strings[ttype]);
        shutdown(1);
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
    int position;

    switch (t._token) {
    case T_INTLIT:
        out = make_ast_leaf(T_INTLIT, t.value);
        break;
    case T_IDENTIFIER:
        position = symbol_exists(D_GLOBAL_SYMBOL_TABLE, D_IDENTIFIER_BUFFER);
        if (position == -1) {
            fprintf(stderr, "Unknown variable %s on line %d\n", D_IDENTIFIER_BUFFER, D_LINE_NUMBER);
            shutdown(1);
        }

        out = make_ast_leaf(T_IDENTIFIER, position);
        break;
    default:
        fprintf(stderr, "Syntax error on line %d, token \"%s\"\n", D_LINE_NUMBER, token_strings[t._token]);
        shutdown(1);
    }

    scan(&GToken);

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

    // Check for EOF
    Token_Type current_ttype = GToken._token;
    if (current_ttype == T_SEMICOLON || current_ttype == T_RIGHT_PARENTHESIS || current_ttype == T_AS) {
        return left;
    }

    // While current token has greater precedence than previous token
    while (operator_precedence(current_ttype) > previous_token_precedence) {
        // Scan the next token
        scan(&GToken);

        // Recursively build the right AST subtree
        right = parse_binary_expression(token_precedence[current_ttype]);

        // Ensure we're dealing with an operator
        if (current_ttype > T_EOF && current_ttype < T_INTLIT) {
            // Join right subtree with current left subtree
            left = make_ast_node(current_ttype, left, NULL, right, 0);
        } else {
            fprintf(stderr, "Expected operator on line %d, got %s instead\n", D_LINE_NUMBER,
                    token_strings[current_ttype]);
            shutdown(1);
        }

        // Update current_ttype and check for EOF
        current_ttype = GToken._token;
        if (current_ttype == T_SEMICOLON || current_ttype == T_RIGHT_PARENTHESIS || current_ttype == T_AS) {
            break;
        }
    }

    return left;
}

/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Relevant definitions
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/**
 * @enum Token_Type
 * @brief This enum contains the various kinds of permitted tokens
 */
typedef enum Token_Type {
    // Arithmetic
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_INTLIT
} Token_Type;

/**
 * @struct token
 * @brief A structure to hold token information
 */
typedef struct token {
    Token_Type _token;
    int value;
} token;

/**
 * @struct AST_Node
 * @brief This structure is used to build the AST for parsing
 * @var AST_Node::ttype The Token_Type of the given token
 * @var AST_Node::left The left child of the AST Node
 * @var AST_Node::right The right child of the AST Node
 * @var AST_Node::value If the AST Node contains an int, this field holds its value
 */
typedef struct AST_Node {
    Token_Type ttype; /**The Token_Type of the given token*/
    struct AST_Node *left;
    struct AST_Node *right;
    int value;
} AST_Node;

/**
 * List of printable tokens
 */
static char *token_strings[] = {"+", "-", "*", "/", "integer literal"};

#endif

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
    T_INTLIT /**Integer literal*/
} Token_Type;

/**
 * @struct token
 * @brief A structure to hold token information
 */
typedef struct token {
    Token_Type _token; /**The token's type*/
    int value; /**If an integer literal, the token's value*/
} token;

/**
 * @struct AST_Node
 * @brief This structure is used to build the AST for parsing
 */
typedef struct AST_Node {
    Token_Type ttype; /**The Token_Type of the given token*/
    struct AST_Node *left; /**The left child of the AST Node*/
    struct AST_Node *right; /**The right child of the AST Node*/
    int value; /**If the AST Node contains an int, this field holds its value*/
} AST_Node;

/**
 * List of printable tokens
 */
static char *token_strings[] = {"+", "-", "*", "/", "integer literal"};

#endif

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
    /**Integer literal*/
    T_INTLIT
} Token_Type;

/**
 * @struct token
 * @brief A structure to hold token information
 */
typedef struct token {
    /**The token's type*/
    Token_Type _token;
    /**If an integer literal, the token's value*/
    int value;
} token;

/**
 * @struct AST_Node
 * @brief This structure is used to build the AST for parsing
 */
typedef struct AST_Node {
    /**The Token_Type of the given token*/
    Token_Type ttype;
    /**The left child of the AST Node*/
    struct AST_Node *left;
    /**The right child of the AST Node*/
    struct AST_Node *right;
    /**If the AST Node contains an int, this field holds its value*/
    int value;
} AST_Node;

/**
 * List of printable tokens
 */
static char *token_strings[] = {"+", "-", "*", "/", "integer literal"};

#endif

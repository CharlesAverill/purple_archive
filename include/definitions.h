/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Relevant project-wide definitions
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/**
 * @enum Token_Type
 * @brief This enum contains the various kinds of permitted tokens
 */
typedef enum Token_Type {
    // File IO
    T_EOF,
    // Arithmetic
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    /**Integer literal*/
    T_INTLIT,
    // Syntax
    T_SEMICOLON,
    T_PRINT,
} Token_Type;

/**
 * Token string equivalents
 */
static char *token_strings[] = {"EOF", "+", "-", "*", "/", "integer literal", ";", "print"};

/**
 * Token precedence values
 */
static int token_precedence[] = {0, 10, 10, 20, 20, 0};

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
    /**@brief The Token_Type of the given token*/
    Token_Type ttype;
    /**The left child of the AST Node*/
    struct AST_Node *left;
    /**The right child of the AST Node*/
    struct AST_Node *right;
    /**If the AST Node contains an int, this field holds its value*/
    int value;
} AST_Node;

#endif

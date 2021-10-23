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

    // Comparison
    T_EQUALS,
    T_NOT_EQUALS,
    T_LESS,
    T_GREATER,
    T_LESS_EQUAL,
    T_GREATER_EQUAL,

    // Literals
    T_INTLIT,

    // Syntax
    T_SEMICOLON,
    T_ASSIGNMENT,
    T_LEFT_PARENTHESIS,
    T_RIGHT_PARENTHESIS,
    T_LEFT_BRACE,
    T_RIGHT_BRACE,
    /**Identifier name*/
    T_IDENTIFIER,

    // Types
    T_INT,

    // Keywords
    T_AS,
    T_IF,
    T_ELSE,
	T_FOR,
    T_PRINT,
    T_WHILE,
    T_WITH,

    // AST-specific Types
    T_AST_LEFT_VALUE_IDENTIFIER,
    /**Glues ASTs together*/
    T_AST_GLUE,
    /**New Scope*/
    T_SCOPE,
} Token_Type;

/**
 * Token string equivalents
 */
static char *token_strings[] = {"EOF",        "+",    "-",
                                "*",          "/",    "==",
                                "!=",         "<",    ">",
                                "<=",         ">=",   "integer literal",
                                ";",          "=",    "(",
                                ")",          "{",    "}",
                                "identifier", "int",  "as",
                                "if",         "else", "for", "print",
                                "while",      "with", "LValue Identifier",
                                "AST Glue", "Scope"};

/**
 * Operator precedence values. Precedence ranges from 0-15, 15 being the first to be computed
 */
static int token_precedence[] = {
    0, // EOF

    11, // PLUS
    11, // MINUS
    12, // STAR
    12, // SLASH

    8, // EQUALS
    8, // NOT EQUALS

    9, // LESS
    9, // GREATER
    9, // LESS EQUAL
    9, // GREATER EQUAL

    0, // INTLIT
};

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


// Max symbol length in Purple is 63 characters and a null terminator
#define MAX_SYMBOL_LEN 63

/**
 * @struct symbol
 * @brief This structure contains data for a single symbol in a Symbol Table
 */
typedef struct symbol {
    /**Maximum symbol length is 63 characters and a null terminator*/
    char name[MAX_SYMBOL_LEN + 1];
    /**This symbol's position on the stack
     * If this is a global it should be -1 */
    int stack_offset;
    /**The size of this symbol's data in bytes*/
    int size;
} symbol;

typedef struct symbol_table {
    symbol *symbols;
    struct symbol_table *parent;
    int max_length;
    int cur_length;
    //size of all of the elments of this symbol table in bytes
    int stack_offset;
} symbol_table;

/**
 * @struct AST_Node
 * @brief This structure is used to build the AST for parsing
 */
typedef struct AST_Node {
    /**@brief The Token_Type of the given token*/
    Token_Type ttype;
    /**The left child of the AST Node*/
    struct AST_Node *left;
    /**The middle child of the AST Node*/
    struct AST_Node *mid;
    /**The right child of the AST Node*/
    struct AST_Node *right;
    /**Union containing either the value of an integer literal, or the identifier of a symbol*/
    union {
        int value;
        char *identifier;
        symbol_table *scope_symbol_table;
    } v;
} AST_Node;

/**Enum defining comparison modes for assembly generation*/
typedef enum Comparison_Mode { CMP_LT, CMP_LE, CMP_GT, CMP_GE, CMP_EQ, CMP_NE } Comparison_Mode;

// Return NO_REGISTER if PIR functions do not return a register
#define NO_REGISTER -1

/**Enum defining the size of each datatype*/
typedef enum Datatype_Sizes {
    SIZE_INT = 4,
} Datatype_Sizes;

void shutdown(int exit_code);

#endif

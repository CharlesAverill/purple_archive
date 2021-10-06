/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Relevant definitions
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// Token structure
struct token {
    int _token;
    int value;
};

// Token types
enum {
    // Arithmetic
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_INTLIT
};

// List of printable tokens
static char *token_strings[] = {"+", "-", "*", "/", "integer literal"};

#endif

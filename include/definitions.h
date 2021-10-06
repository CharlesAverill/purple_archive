/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Relevant definitions
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// Token structure
typedef struct token {
    int _token;
    int value;
} token;

// Token types
enum {
    // Arithmetic
    T_ADD,
    T_SUB,
    T_MUL,
    T_DIV,
    T_INT
}

#endif

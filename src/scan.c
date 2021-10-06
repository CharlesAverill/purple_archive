/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Lexical Scanner logic
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "definitions.h"
#include "scan.h"

/**
 * Get next valid character from input_file
 * @return  Next valid character from input_file
 */
static char next(void)
{
    // Get next valid character from file

    char c;

    // If we have to put a character back into the stream
    if (put_back) {
        c = put_back;
        put_back = 0;
        return c;
    }

    // Get next character from file
    c = fgetc(input_file);

    // Check line increment
    if (c == '\n') {
        line_number++;
    }

    return c;
}

/**
 * Define the character to be put back into the input stream
 * @param c  Character to be put back
 */
static void put_back_into_stream(char c) { put_back = c; }

/**
 * Skip whitespace tokens
 * @return  Next non-whitespace token
 */
static char skip(void)
{
    char c;

    // Get next character until not hit space, tab, newline, carriage return, form feed
    do {
        c = next();
    } while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f');

    return c;
}

/**
 * Return the index of c in s
 * @param  s               String to search in
 * @param  c               Character to search for
 * @return   Index of c in s if s contains c, else -1
 */
static int index_of(char *s, int c)
{
    char *p = strchr(s, c);

    if (p) {
        return p - s;
    } else {
        return -1;
    }
}

/**
 * Scan and return integer literal from input_file
 * @param  c               Current character
 * @return   Next scanned integer literal
 */
static int scanint(char c)
{
    int k = 0;   // Current digit
    int val = 0; // Output

    while ((k = index_of("0123456789", c)) >= 0) {
        val = val * 10 + k;
        c = next();
    }

    // Loop has terminated at a non-integer value, so put it back
    put_back_into_stream(c);

    return val;
}

/**
 * Scan and fill the next token
 * @param  t               token to be filled
 * @return   1 if valid token returned, 0 if no tokens left
 */
int scan(token *t)
{
    char c;

    // Skip whitespace
    c = skip();

    // Fill token
    switch (c) {
    case EOF:
        return 0;
    case '+':
        t->_token = T_PLUS;
        break;
    case '-':
        t->_token = T_MINUS;
        break;
    case '*':
        t->_token = T_STAR;
        break;
    case '/':
        t->_token = T_SLASH;
        break;
    default:
        // Check if c is an integer
        if (isdigit(c)) {
            t->value = scanint(c);
            t->_token = T_INTLIT;
        } else {
            fprintf(stderr, "Unrecognized character %c on line %d\n", c, line_number);
            exit(1);
        }
    }

    return 1;
}

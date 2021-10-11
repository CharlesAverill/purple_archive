/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Functions for lexical scanning from D_INPUT_FILE
*/

#include "scan.h"

/**
 * Get next valid character from D_INPUT_FILE
 * @return  Next valid character from D_INPUT_FILE
 */
static char next(void)
{
    // Get next valid character from file

    char c;

    // If we have to put a character back into the stream
    if (D_PUT_BACK) {
        c = D_PUT_BACK;
        D_PUT_BACK = 0;
        return c;
    }

    // Get next character from file
    c = fgetc(D_INPUT_FILE);

    // Check line increment
    if (c == '\n') {
        D_LINE_NUMBER++;
    }

    return c;
}

/**
 * Define the character to be put back into the input stream
 * @param c  Character to be put back
 */
static void put_back_into_stream(char c) { D_PUT_BACK = c; }

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

static int scan_identifier(char c, char *buf, int character_limit){
    int i = 0;
    
    // Identifiers permit alphanumeric characters and underscores
    while(isalpha(c) || isdigit(c) || c == '_'){
        if(i == character_limit - 1){
            fprintf(stderr, "Identifier too long on line %d\n", D_LINE_NUMBER);
            exit(1);
        } else if(i < character_limit - 1){
            buf[i++] = c;
        }
        
        c = next();
    }
    
    // Invalid character was reached
    put_back_into_stream(c);
    
    // Null-terminate buffer
    buf[i] = '\0';
    
    return i;
}

static Token_Type string_to_keyword(char *str){
    switch(str[0]){
        case 'p':
            if(!strcmp(str, "print")){
                return T_PRINT;
            }
            break;
    }
    
    return -1;
}

/**
 * Scan and return integer literal from D_INPUT_FILE
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
    t->value = 0;
    if (c == EOF || c == 255) {
        t->_token = T_EOF;
        return 0;
    }

    switch (c) {
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
    case ';':
        t->_token = T_SEMICOLON;
        break;
    default:
        // Check if c is an integer
        if (isdigit(c)) {
            t->value = scanint(c);
            t->_token = T_INTLIT;
        // Check if c is an identifier
        } else if(isalpha(c) || c == '_'){
            scan_identifier(c, D_IDENTIFIER_BUFFER, D_MAX_IDENTIFIER_LENGTH);
            
            Token_Type ttype = string_to_keyword(D_IDENTIFIER_BUFFER);
            if(ttype != -1){
                t->_token = ttype;
                break;
            }
            
            fprintf(stderr, "Unrecognized symbol %s on line %d\n", D_IDENTIFIER_BUFFER, D_LINE_NUMBER);
            exit(1);
        } else {
            fprintf(stderr, "Unrecognized character %c on line %d\n", c, D_LINE_NUMBER);
            exit(1);
        }
    }

    return 1;
}

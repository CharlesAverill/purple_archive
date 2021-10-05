/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Lexical Scanner logic
*/

#include <stdio.h>

#include "data"
#include "definitions.h"

/**
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
static void put_back_into_stream(char c)
{
    put_back = c;
}

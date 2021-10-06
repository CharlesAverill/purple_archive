/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Main compiler script
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Undef extern_ to transfer ownership to purple.c
#define extern_
#include "data.h"
#undef extern_

#include "definitions.h"
#include "scan.h"

/**
 * Initialize compiler values
 */
static void init(void)
{
    line_number = 1;
    put_back = '\n';
}

/**
 * Print an error message describing the proper usage
 * @param program_name  Program name called by user
 */
static void usage(char *program_name) { fprintf(stderr, "Usage: %s <input_file>\n", program_name); }

/**
 * Scan input_file and print out tokens and their details
 */
static void scan_file()
{
    token T;

    while (scan(&T)) {
        printf("Token %s", token_strings[T._token]);
        if (T._token == T_INTLIT) {
            printf(", value %d", T.value);
        }
        printf("\n");
    }
}

/**
 * Main compiler function
 */
int main(int argc, char *argv[])
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    scan_file();

    return 0;
}
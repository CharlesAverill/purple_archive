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

/**Define, then undef extern_ to transfer ownership to purple.c*/
#define extern_
#include "data.h"
#undef extern_

#include "arguments.h"
#include "definitions.h"
#include "info.h"
#include "parse.h"
#include "scan.h"
#include "translation/translate.h"
#include "tree.h"

/**
 * Initialize compiler values
 */
static void init(int argc, char *argv[])
{
    D_DEBUG = 0;
    D_LINE_NUMBER = 1;
    D_PUT_BACK = '\n';

    // Argument parsing
    args = malloc(sizeof(purple_args));
    parse_args(args, argc, argv);

    D_INPUT_FILE = fopen(args->filenames[0], "r");
    if (D_INPUT_FILE == NULL) {
        fprintf(stderr, "Unable to open %s: %s\n", args->filenames[0], strerror(errno));
        exit(1);
    }
}

/**
 * Scan D_INPUT_FILE and print out tokens and their details
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
    init(argc, argv);

    generate_pir();

    free(args);

    exit(0);
}

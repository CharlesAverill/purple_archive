/**
 * @file
 * @author CharlesAverill
 * @date   16-Oct-2021
 * @brief Symbol Table logic
*/

#include "symbol_table.h"

/**Global symbol table length*/
int global_symbol_table_length = 64;

/**Next available global symbol table position*/
int next_global_symbol_table_position = 0;

/**Overall stack offset*/
int stack_offset = 0;

/**
 * Code for initializing the Global symbol table
 */
void init_symbol_table(void)
{
    D_GLOBAL_SYMBOL_TABLE = malloc(global_symbol_table_length * sizeof(symbol));
    if (D_GLOBAL_SYMBOL_TABLE == NULL) {
        fprintf(stderr, "Unable to allocate memory for new AST Node");
        shutdown(1);
    }
}

void print_symbol_table(void)
{
    printf("-----BEGIN SYMBOL TABLE-----\n");
    for (int i = 0; i < next_global_symbol_table_position; i++) {
        symbol sym = D_GLOBAL_SYMBOL_TABLE[i];

        printf("---SYMBOL %d---\n", i);
        printf("   Name:         %s\n", sym.name);
        printf("   Stack Offset: %d\n", sym.stack_offset);
    }
    printf("-----END SYMBOL TABLE-----\n");
}


/**
 * Return the length of the global symbol table
 */
int global_symbol_table_len() {
    return next_global_symbol_table_position;
}

/**
 * Binary search implementation to find a given symbol in the Global symbol table
 * @return       The index of the symbol, if it exists. Otherwise -1.
 */
static int _global_search(int low, int high, char *value)
{
    if (high < low) {
        return -1;
    }

    // Compare value and symbol at mid
    int mid = (low + high) / 2;
    if (!strcmp(value, D_GLOBAL_SYMBOL_TABLE[mid].name)) {
        return mid;
    }

    // Compare first characters of value and symbol at mid
    if (strcmp(value, D_GLOBAL_SYMBOL_TABLE[mid].name) > 0) {
        return _global_search(mid + 1, high, value);
    }

    return _global_search(low, mid - 1, value);
}

/**
 * Determine if a given symbol exists in the Global symbol table
 * @param  name               The symbol name to search for
 * @return      The index of the symbol, if it exists. Otherwise -1.
 */
int global_symbol_exists(char *name)
{
    return _global_search(0, next_global_symbol_table_position - 1, name);
}

/**
 * Double the size of the global symbol table
 */
static void _expand_global_symbol_table(void)
{
    // Check for length overflow
    if (global_symbol_table_length * 2 < global_symbol_table_length) {
        fprintf(stderr, "Global symbol table length overflow, too many symbols\n");
        shutdown(1);
    }

    // Double the length of the symbol table every time its bounds are exceeded
    global_symbol_table_length *= 2;

    D_GLOBAL_SYMBOL_TABLE = (symbol *)realloc(D_GLOBAL_SYMBOL_TABLE, global_symbol_table_length);
    if (D_GLOBAL_SYMBOL_TABLE == NULL) {
        fprintf(stderr, "Error when attempting to resize global symbol table to %d elements\n",
                global_symbol_table_length);
        shutdown(1);
    }
}

/**
 * Insert a new symbol into the Global symbol table
 * @param  name               Name of the symbol to create
 * @param  datatype           Data type of the symbol to create
 * @return      The position of the symbol in the Global symbol table
 */
int insert_global_symbol(char *name, Token_Type datatype)
{
    if (strlen(name) > MAX_SYMBOL_LEN) {
        fprintf(stderr, "Symbol %s on line %d is too long\n", name, D_LINE_NUMBER);
        shutdown(1);
    }
    int position;

    // Check if symbol already exists
    position = global_symbol_exists(name);
    if (position >= 0) {
        return position;
    }

    // Get next available table position
    position = next_global_symbol_table_position++;
    if (position >= global_symbol_table_length) {
        _expand_global_symbol_table();
    }

    // Shift items until we find the proper place for the symbol name
    int i;
    for (i = position - 1; i >= 0 && *D_GLOBAL_SYMBOL_TABLE[i].name > *name; i--) {
        D_GLOBAL_SYMBOL_TABLE[i + 1] = D_GLOBAL_SYMBOL_TABLE[i];
    }

    // Update position to correct place in Global symbol table sorted by name
    position = i + 1;

    // Set symbol data
    strcpy(D_GLOBAL_SYMBOL_TABLE[position].name, name);
    D_GLOBAL_SYMBOL_TABLE[position].stack_offset = stack_offset;

    int set_size;
    switch (datatype) {
    case T_INT:
        set_size = SIZE_INT;
        break;
    }
    D_GLOBAL_SYMBOL_TABLE[position].size = set_size;

    // Update overall stack offset
    stack_offset += set_size;

    return position;
}

int remove_global_symbol(char *name)
{
    int position = global_symbol_exists(name);
    if (position == -1) {
        fprintf(stderr, "Attempt to remove nonexistent symbol from symbol table on line %d\n",
                D_LINE_NUMBER);
        shutdown(1);
    }

    next_global_symbol_table_position -= 1;

    // Shift every symbol down the list
    for (int i = next_global_symbol_table_position; i > position; i--) {
        D_GLOBAL_SYMBOL_TABLE[i - 1] = D_GLOBAL_SYMBOL_TABLE[i];
    }

    return 0;
}

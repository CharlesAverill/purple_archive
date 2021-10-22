/**
 * @file
 * @author CharlesAverill
 * @date   16-Oct-2021
 * @brief Symbol Table logic
*/

#include "symbol_table.h"

/**Overall stack offset*/
int stack_offset = 0;

/**
 * Code for initializing the Global symbol table
 */
void init_global_symbol_table(void)
{
    D_GLOBAL_SYMBOL_TABLE = malloc(global_symbol_table_length * sizeof(symbol));
    if (D_GLOBAL_SYMBOL_TABLE == NULL) {
        fprintf(stderr, "Unable to allocate memory for new AST Node");
        shutdown(1);
    }
}

/**
 * Code to make a symbol table
 * If the symbol table is the root symbol table (i.e. the global symbol table) parent should be null
 */
void make_symbol_table(symbol_table *parent) {
    symbol_table *symtab = malloc(sizeof(symbol_table));
    if(symtab == NULL) {
        printf(stderr, "Unable to allocate memory for symbol table");
        shutdown(1);
    }
    symtab->length = 64;
    symtab->symbols = malloc(sizeof(symbol) * symtab->length);
    if(symtab->symbols == NULL) {
        printf(stderr, "Unable to allocate memory for symbol table array");
        shutdown(1);
    }
    symtab->next_position = 0;
    symtab->parent = parent;
}

void print_symbol_table(symbol_table *table)
{
    printf("-----BEGIN SYMBOL TABLE-----\n");
    for (int i = 0; i < table->next_position; i++) {
        symbol sym = table->symbols[i];

        printf("---SYMBOL %d---\n", i);
        printf("   Name:         %s\n", sym.name);
        printf("   Stack Offset: %d\n", sym.stack_offset);
    }
    printf("-----END SYMBOL TABLE-----\n");
}

/**
 * Binary search implementation to find a given symbol in a symbol table
 * @return       The index of the symbol, if it exists. Otherwise -1.
 */
static int _search(symbol_table *symtab, int low, int high, char *value)
{
    if (high < low) {
        return -1;
    }

    // Compare value and symbol at mid
    int mid = (low + high) / 2;
    if (!strcmp(value, symtab->symbols[mid].name)) {
        return mid;
    }

    // Compare first characters of value and symbol at mid
    if (strcmp(value, symtab->symbols[mid].name) > 0) {
        return _search(mid + 1, high, value);
    }

    return _search(low, mid - 1, value);
}

/**
 * Determine if a given symbol exists in a symbol table
 * @param  name               The symbol name to search for
 * @return      The index of the symbol, if it exists. Otherwise -1.
 */
int symbol_exists(symbol_table *symtab, char *name)
{
    return _search(symtab, 0, symtab->next_position-1, name);
}

/**
 * Double the size of a symbol table
 */
static void _expand_symbol_table(symbol_table *symtab)
{
    // Check for length overflow
    if (symbol_table->length * 2 < symbol->table->length) {
        fprintf(stderr, "Symbol table length overflow, too many symbols\n");
        shutdown(1);
    }

    // Double the length of the symbol table every time its bounds are exceeded
    symbol_table->length *= 2;

    symbol_table->symbols = (symbol *)realloc(symbol_table->symbols, symbol_table->length);
    if (symbol_table->symbols == NULL) {
        fprintf(stderr, "Error when attempting to resize symbol table to %d elements\n",
                symbol_table->length);
        shutdown(1);
    }
}

/**
 * Insert a new symbol into a symbol table
 * @param  name               Name of the symbol to create
 * @param  datatype           Data type of the symbol to create
 * @return      The position of the symbol in the symbol table
 */
int insert_symbol(symbol_table *symtab, char *name, Token_Type datatype)
{
    if (strlen(name) > MAX_SYMBOL_LEN) {
        fprintf(stderr, "Symbol %s on line %d is too long\n", name, D_LINE_NUMBER);
        shutdown(1);
    }
    int position;

    // Check if symbol already exists
    position = symbol_exists(symtab, name);
    if (position >= 0) {
        return position;
    }

    // Get next available table position
    position = symtab->next_position++;
    if (position >= symtab->length) {
        _expand_symbol_table(symtab);
    }

    // Shift items until we find the proper place for the symbol name
    int i;
    for (i = position - 1; i >= 0 && *symtab->symbols[i].name > *name; i--) {
        symtab->symbols[i + 1] = symtab->symbols[i];
    }

    // Update position to correct place in symbol table sorted by name
    position = i + 1;

    // Set symbol data
    strcpy(symtab->symbols[position].name, name);
    symtab->symbols[position].stack_offset = stack_offset;

    int set_size;
    switch (datatype) {
    case T_INT:
        set_size = SIZE_INT;
        break;
    }
    symtab->symbols[position].size = set_size;

    // Update overall stack offset
    stack_offset += set_size;

    return position;
}
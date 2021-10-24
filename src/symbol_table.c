/**
 * @file
 * @author CharlesAverill
 * @date   16-Oct-2021
 * @brief Symbol Table logic
*/

#include "symbol_table.h"

/**
 * Code to make a symbol table
 * If the symbol table is the root symbol table (i.e. the global symbol table) parent should be null
 */
symbol_table *make_symbol_table(symbol_table *parent)
{
    symbol_table *symtab = malloc(sizeof(symbol_table));
    if (symtab == NULL) {
        fprintf(stderr, "Unable to allocate memory for symbol table");
        shutdown(1);
    }
    symtab->max_length = 64;
    symtab->symbols = malloc(sizeof(symbol) * symtab->max_length);
    if (symtab->symbols == NULL) {
        fprintf(stderr, "Unable to allocate memory for symbol table array");
        shutdown(1);
    }
    symtab->cur_length = 0;
    symtab->stack_offset = 0;
    symtab->parent = parent;
    return symtab;
}

/**
 * Code for initializing the Global symbol table
 */
void init_global_symbol_table(void)
{
    D_GLOBAL_SYMBOL_TABLE = make_symbol_table(NULL);
    if (D_GLOBAL_SYMBOL_TABLE == NULL) {
        fprintf(stderr, "Unable to allocate memory for global symbol table");
        shutdown(1);
    }
}

void print_symbol_table(symbol_table *table)
{
    printf("-----BEGIN SYMBOL TABLE-----\n");
    for (int i = 0; i < table->cur_length; i++) {
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
        return _search(symtab, mid + 1, high, value);
    }

    return _search(symtab, low, mid - 1, value);
}

/**
 * Determine if a given symbol exists in a symbol table
 * @param  name               The symbol name to search for
 * @return      The index of the symbol, if it exists. Otherwise -1.
 */
int symbol_exists(symbol_table *symtab, char *name)
{
    return _search(symtab, 0, symtab->cur_length - 1, name);
}

/**
 * Double the size of a symbol table
 */
static void _expand_symbol_table(symbol_table *symtab)
{
    // Check for length overflow
    if (symtab->max_length * 2 < symtab->max_length) {
        fprintf(stderr, "Symbol table length overflow, too many symbols\n");
        shutdown(1);
    }

    // Double the length of the symbol table every time its bounds are exceeded
    symtab->max_length *= 2;

    symtab->symbols = (symbol *)realloc(symtab->symbols, symtab->max_length);
    if (symtab->symbols == NULL) {
        fprintf(stderr, "Error when attempting to resize symbol table to %d elements\n",
                symtab->max_length);
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
    position = symtab->cur_length++;
    if (position >= symtab->max_length) {
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
    symtab->symbols[position].stack_offset = symtab->stack_offset;

    int set_size;
    switch (datatype) {
    case T_INT:
        set_size = SIZE_INT;
        break;
    }

    // Check if our size is too small for 64-bit architectures
    if (D_CURRENT_ASM_MODE == X86_64) {
        set_size = get_64_bit_size(set_size);
    }

    symtab->symbols[position].size = set_size;

    // Update overall stack offset
    symtab->stack_offset += set_size;

    return position;
}

/**
 * Get a symbol from the symbol table tree recursively. Return the first match found
 * (i.e. the most specific scope a variable is declared in)
 * @param  stack_top   top of the symbol table stack to traverse
 * @param  name        the name of the symbol
 * @return             the symbol found
 */
symbol *get_symbol(symbol_table *stack_top, char *name)
{
    while (stack_top != NULL) {
        int position = _search(stack_top, 0, stack_top->cur_length - 1, name);
        if (position != -1)
            return &stack_top->symbols[position];
        stack_top = stack_top->parent;
    }
    fprintf(stderr, "Unable to find symbol %s", name);
    shutdown(1);
}

/**
 * Get the stack offset of a symbol table, aligned to 16 bytes
 * @param  symtab               The symbol table to get the size of
 * @return        The aligned stack offset of the symbol table
 */
int get_symbol_aligned_stack_offset(symbol_table *symtab)
{
    return ((symtab->stack_offset + STACK_ALIGN - 1) / STACK_ALIGN) * STACK_ALIGN;
}

/**
 * Some 64-bit architectures like x86_64 have a minimum operand size of 64 bits. This method
 * returns the 64-bit size of a given symbol in bytes
 * @param  sym               The size to resize
 * @return     The proper 64-bit size of the input size
 */
int get_64_bit_size(int size) { return ((size + 7) / 8) * 8; }

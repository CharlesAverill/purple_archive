/**
 * @file
 * @author CharlesAverill
 * @date   17-Oct-2021
 * @brief Logic for variable declarations
*/

#include "declaration.h"

/**
 * Parse a variable declaration statement
 */
void variable_declaration(void)
{
    // Ensure we have a type definition and identifier definition
    Token_Type datatype = match_datatype();
    match(T_IDENTIFIER);

    // Add symbol name to table
    int position = insert_symbol(D_GLOBAL_SYMBOL_TABLE, D_IDENTIFIER_BUFFER, datatype);

    // Ensure semicolon
    match(T_SEMICOLON);
}

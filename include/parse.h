/**
 * @file
 * @author charlesaverill
 * @date   06-Oct-2021
 * @brief Expression parsing headers
*/

#include "data.h"
#include "definitions.h"
#include "scan.h"
#include "tree.h"

AST_Node *parse_binary_expression(token t);
int interpret_AST(AST_Node *n);

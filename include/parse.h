/**
 * @file
 * @author CharlesAverill
 * @date   06-Oct-2021
 * @brief Expression parsing headers
*/

#ifndef PARSE_H
#define PARSE_H

#include "data.h"
#include "declaration.h"
#include "definitions.h"
#include "scan.h"
#include "translation/translate.h"
#include "tree.h"

AST_Node *parse_binary_expression(int previous_token_precedence);
void parse_statements(void);

#endif

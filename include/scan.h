/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Lexical Scanner function headers
*/

#ifndef SCAN_H
#define SCAN_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "definitions.h"

int scan(struct token *t);
void match(Token_Type ttype);
Token_Type match_datatype(void);

#endif

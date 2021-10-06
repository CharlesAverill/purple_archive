/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Important project-wide variables
*/

#ifndef DATA_H
#define DATA_H

// extern_ will be undefined in purple.c, causing purple.c to "own" these variables
#ifndef extern_
    #define extern_ extern
#endif

extern_ int line_number;
extern_ char put_back;

extern_ FILE *input_file;

#endif

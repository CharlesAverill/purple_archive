/**
 * @file
 * @author CharlesAverill
 * @date   05-Oct-2021
 * @brief Important project-wide variables
*/

#ifndef DATA_H
#define DATA_H

/**extern_ will be undefined in purple.c, causing purple.c to "own" these variables*/
#ifndef extern_
    #define extern_ extern
#endif

/**Current line number of the Scanner*/
extern_ int line_number;
/**If defined, this character will be put back into the input stream*/
extern_ char put_back;
/**The pointer to the open filestream for the Scanner*/
extern_ FILE *input_file;

#endif

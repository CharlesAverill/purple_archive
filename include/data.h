 /**
 * @Author: CharlesAverill
 * @Date:   05-Oct-2021
 * @Filename: data.h
 * @Last modified by:   CharlesAverill
 * @Last modified time: 05-Oct-2021
 * @Description: Important project-wide variables
 */

#ifndef DATA_H
#define DATA_H

// extern_ will be undefined in main.c, causing main.c to "own" these variables
#ifndef extern_
    #define extern_ extern
#endif

extern_ int line_number;
extern_ int put_back;

extern_ FILE *input_file;

#endif

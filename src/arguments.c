/**
 * @file
 * @author CharlesAverill
 * @date   12-Oct-2021
 * @brief Logic for handling command line arguments. Based on this argp example: https://www.gnu.org/software/libc/manual/html_node/Argp-Example-3.html
*/

#include "arguments.h"
#include "translation/translate.h"

const char *argp_program_version = PROJECT_NAME_AND_VERS;
const char *argp_program_bug_address = "charlesaverill20@gmail.com";
static char doc[] = "A compiler for the Purple programming language";
static char args_doc[] = "PROGRAM";

static struct argp_option options[] = {
    {"quiet", 'q', 0, 0, "Don't produce any output", 0},
    {"output", 'o', "FILE", 0, "Path to the generated assembly file", 0},
    {"force_asm", -1, "ASM_LANG", 0, "Set this option to force compilation into a specified assembly language.\nSupported: {x86, MIPS}"},
    {0},
};

error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    purple_args *arguments = state->input;

    switch (key) {
    case 'q':
        arguments->quiet = 1;
        break;
    case 'o':
        arguments->filenames[1] = arg;
        break;
    case -1:
        for(int i = 0; i < N_SUPPORTED_ASM_MODES; i++){
            if(!strcmp(asm_mode_names[i], arg)){
                arguments->forced_assembly_mode = i;
            }
        }
    case ARGP_KEY_ARG:
        // Check for too many arguments
        if (state->arg_num > 1) {
            argp_usage(state);
        }

        arguments->filenames[0] = arg;

        break;
    case ARGP_KEY_END:
        // Check for not enough arguments
        if (state->arg_num < 1) {
            argp_usage(state);
        }
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

void parse_args(purple_args *args, int argc, char *argv[])
{
    args->quiet = 0;
    args->forced_assembly_mode = 0;
    args->filenames[1] = "a.s";

    argp_parse(&argp, argc, argv, 0, 0, args);
}

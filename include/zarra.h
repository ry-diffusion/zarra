#pragma once
#include <limits.h>

typedef unsigned char bool;
typedef const char *restrict Text;
static const bool true = 1;
static const bool false = 0;

typedef struct
{
	char input[PATH_MAX];
	char output[PATH_MAX];
} CLIOptions;

/*
 * Parses cli arguments
 * @param argc The number of arguments
 * @param argc The values of arguments
 * @returns 1 on success, otherwise returns 0.
 */
bool ParseCLI(CLIOptions *opts, int argc, char **argv);

/*
 * Checks if `ffmpeg` can be run.
 */
bool IsFFMPEGInstalled(void);

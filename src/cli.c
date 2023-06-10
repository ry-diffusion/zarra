#include "zarra.h"
#include "zarra_static.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static struct option OPTIONS[] = {{"input", required_argument, 0, 'i'},
				  {"output", required_argument, 0, 'o'},
				  {"framerate", required_argument, 0, 'f'},
				  {"help", no_argument, 0, 'h'},
				  {0, 0, 0, 0}};

static const Text HelpText =
    "Zarra v" ZARRA_VERSION " A simple screen recorder\n"
    "Usage: \n"
    "   -i (--input) [ARG]: Set input device\n"
    "   -f (--framerate) [ARG]: Set recording's framerate\n"
    "   -o (--output) <ARG>: Set output device";

bool ParseCLI(CLIOptions *opts, int argc, char **argv)
{
	int optionIndex = 0;
	struct stat inputStat;
	char c;

	while ((c = getopt_long(argc, argv, "i:o:h:f:", OPTIONS,
				&optionIndex)) != -1)
	{
		switch (c)
		{
		case 'i':
			strncpy(opts->input, optarg, strlen(optarg));
			break;

		case 'o':
			strncpy(opts->output, optarg, strlen(optarg));
			break;

		case 'f':
			opts->framerate = atoi(optarg);
			break;

		case 0:
		case ':':
		case '?':
		default:
			goto error;
		}
	}

	if (!*opts->output)
		strncpy(opts->output, defaultOutputFileName,
			strlen(defaultOutputFileName) + 1);

	if (!*opts->input)
		goto error;

	if (!opts->framerate)
		opts->framerate = 60;

	if (stat(opts->input, &inputStat) < 0)
	{
		perror(opts->input);
		goto error;
	}

	return true;

error:
	puts(HelpText);
	return false;
}

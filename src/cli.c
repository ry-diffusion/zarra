#include "zarra.h"
#include "zarra_static.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

static struct option Options[] = {{"input", required_argument, 0, 'i'},
				  {"output", required_argument, 0, 'o'},
				  {"help", required_argument, 0, 'h'},
				  {0, 0, 0, 0}};

static const Text HelpText =
    "Zarra v" ZARRA_VERSION " A simple screen recorder\n"
    "Usage: \n"
    "   -i (--input) [ARG]: Set input device\n"
    "   -o (--output) <ARG>: Set output device";

bool ParseCLI(CLIOptions *opts, int argc, char **argv)
{

	int optionIndex = 0;
	char c = 0;

	while (1)
	{
		c = getopt_long(argc, argv, "i:o:h", Options, &optionIndex);
		if (c == -1)
			break;

		switch (c)
		{
		case 0:
			break;
		case 'i':
			strncpy(opts->input, optarg, strlen(optarg));
			break;
		case 'o':
			strncpy(opts->output, optarg, strlen(optarg));
			break;
		case '?':
			return false;
		default:
			puts(HelpText);
			return false;
		}
	}
	return true;
}

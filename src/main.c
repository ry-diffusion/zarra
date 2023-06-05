#include "zarra.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	CLIOptions options = {0};
	if (!ParseCLI(&options, argc, argv))
		return 1;

	if (!IsFFMPEGInstalled())
	{
		fputs("Error: FFMPEG isn't installed. Please install it and "
		      "try again\n",
		      stderr);
		return 1;
	}

	printf("Input = %s: Output = %s\n", options.input, options.output);

	return 0;
}

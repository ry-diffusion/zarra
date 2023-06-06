#include "zarra.h"
#include <signal.h>
#include <stdio.h>

/*
 * TODO: Add STDIN reading option.
 * DROP: Terminate
 * TODO: Add pause option (SIGSTOP to FFMPEG)
 * TODO: `zarra-list-devices` command.
 * DROP: Global variables
 * TODO: CLI Framerate
 */

int main(int argc, char **argv)
{
	CLIOptions options = {0};

	if (!ParseCLI(&options, argc, argv))
		return 1;

	return 0;
}

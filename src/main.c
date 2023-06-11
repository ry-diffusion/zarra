#include "zarra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define ARG_MAX 256

/*
 * TODO: Add pause option (SIGSTOP to FFMPEG)
 * TODO: `zarra-list-devices` command.
 * DROP: Global variables
 */

void spawnRootAgent(char **agentArguments, char **argv)
{
	char stringUid[10] = {0};
	uint currentIdx = 0;

#define next agentArguments[currentIdx++]

	snprintf(stringUid, 10, "%u", getuid());
	puts("Respawning Zarra with a root agent.");

	next = "pkexec";
	next = *argv++;
	next = "--agent";
	next = stringUid;

	while (*argv)
		next = *argv++;

	next = NULL;

	execvp("pkexec", agentArguments);

#undef next
}

int main(int argc, char **argv)
{
	CLIOptions options = {0};
	char *agentArguments[argc + 3];

	if (!ParseCLI(&options, argc, argv))
		return 1;

	if (options.wantsRootAgent == -1)
	{
		spawnRootAgent((char **)agentArguments, argv);
		perror("Failed to spawn root agent! Too bad!");
	}

	return 0;
}

#include "zarra.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	CLIOptions options = {0};
	TaskManager taskManager = {{{0}}, 0};
	char pathBuffer[PATH_MAX] = {0};
	// why not?
	char cwd[PATH_MAX - 100] = {0};

	if (!ParseCLI(&options, argc, argv))
		return 1;

	if (!IsFFMPEGInstalled())
	{
		fputs("Error: FFMPEG isn't installed. Please install it and "
		      "try again\n",
		      stderr);
		return 1;
	}

	getcwd(cwd, PATH_MAX);
	sprintf(pathBuffer, "%s/ZarraRawVideoInput.mp4", cwd);

	SpawnVideoTask(&taskManager, options.input, pathBuffer);

	sprintf(pathBuffer, "%s/ZarraRawAudioInput.flac", cwd);
	SpawnAudioTask(&taskManager,
		       "alsa_output.pci-0000_00_1b.0.analog-stereo.monitor",
		       pathBuffer);

	while (IsAllTasksGood(&taskManager))
		;

	TerminateAllTasks(&taskManager);

	return 0;
}

#include "zarra.h"
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

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
	TaskManager taskManager = {{{0}}, 0};
	UIState us = {0};
	char cwd[PATH_MAX >> 1] = {0};
	char rawAudioPath[PATH_MAX] = {0};
	char rawVideoPath[PATH_MAX] = {0};

	if (!ParseCLI(&options, argc, argv))
		return 1;

	printf("Framerate: %u\n", options.framerate);
	printf("Input: %s\n", options.input);
	printf("Output: %s\n", options.output);

	if (!getcwd(cwd, PATH_MAX >> 1))
	{
		fputs("Unable to get current cwd: buffer too small!\n", stderr);
		return 1;
	}

	sprintf(rawAudioPath, "%s/zarra-raw-audio.flac", cwd);
	sprintf(rawVideoPath, "%s/zarra-raw-video.raw", cwd);

	SpawnVideoTask(&taskManager, options.input, rawVideoPath);
	SpawnAudioTask(&taskManager, "default", rawAudioPath);

	if (!IsAllTasksGood(&taskManager))
	{
		puts("Unable to start recording!");
	}
	else
	{
		UITask(&us, &taskManager);
	}

	TerminateAllTasks(&taskManager);
	remove(rawAudioPath);
	remove(rawVideoPath);

	return 0;
}

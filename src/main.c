#include "zarra.h"
#include <limits.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * TODO: Add STDIN reading option.
 * DROP: Terminate
 * TODO: Add pause option (SIGSTOP to FFMPEG)
 * TODO: `zarra-list-devices` command.
 * DROP: Global variables
 * TODO: CLI Framerate
 */
UIState ui = {0};

void quitRequested(int signal)
{
	(void)signal;

	ui.terminateRequested = true;
}

int main(int argc, char **argv)
{
	CLIOptions options = {0};
	TaskManager taskManager = {{{0}}, 0};

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

	signal(SIGTERM, quitRequested);
	signal(SIGINT, quitRequested);

	puts("Press C to stop recording...");

	UIPrepareTerm(&ui);

	if (!IsAllTasksGood(&taskManager))
	{
		puts("Unable to start recording!");
		goto error;
	}
	else
	{
		UITask(&ui, &taskManager);
	}

	TerminateAllTasks(&taskManager);
	ui.terminateRequested = false;

	taskManager.currentRunning = 0;
	SpawnProcessingTask(&taskManager, rawVideoPath, rawAudioPath,
			    options.output);

	puts("Processing!\n");

	wait(NULL);
	UITask(&ui, &taskManager);

	UIRestoreTerm(&ui);
	remove(rawAudioPath);
	remove(rawVideoPath);

	return 0;
error:
	UIRestoreTerm(&ui);
	remove(rawAudioPath);
	remove(rawVideoPath);

	return 1;
}

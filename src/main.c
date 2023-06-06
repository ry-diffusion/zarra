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
	puts("Spawn videotask");
	SpawnVideoTask(&taskManager, options.input, pathBuffer);
	puts("Spawn audiotask");
	sprintf(pathBuffer, "%s/ZarraRawAudioInput.flac", cwd);
	SpawnAudioTask(&taskManager,
		       "alsa_output.pci-0000_00_1b.0.analog-stereo.monitor",
		       pathBuffer);
	puts("check");

	while (IsAllTasksGood(&taskManager))
	{
		uint currentTaskIdx = 0;
		for (currentTaskIdx = 0;
		     currentTaskIdx < taskManager.currentRunning;
		     ++currentTaskIdx)
		{
			Task *task = &taskManager.running[currentTaskIdx];
			printf("Task: %s\n", task->type == TaskRecordAudio
						 ? "Record Audio"
						 : "Record Video");
			ParseFFmpegOutput(task);

			printf(" Speed: %f\n Rate: %f\n", task->speed,
			       task->rate);
		}
	}

	TerminateAllTasks(&taskManager);

	return 0;
}

#include "zarra.h"
#include <linux/limits.h>
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

TaskManager taskManager = {{{0}}, 0};
char rawVideoPath[PATH_MAX], rawAudioPath[PATH_MAX];
Text outputPath;
bool isProcessing = false;

void Terminate(int Signal)
{
	if (isProcessing)
		return;
	else
		isProcessing = true;
	(void)Signal;

	TerminateAllTasks(&taskManager);

	for (uint currentIdx = 0; currentIdx < taskManager.currentRunning;
	     ++currentIdx)
	{
		Task task = {0, 0, 0, 0, 0, 0};
		taskManager.running[currentIdx] = task;
	}

	taskManager.currentRunning = 0;

	puts("Processing!");
	SpawnProcessingTask(&taskManager, rawVideoPath, rawAudioPath,
			    outputPath);

	while (IsAllTasksGood(&taskManager))
	{
	}

	_exit(0);
}

int main(int argc, char **argv)
{
	CLIOptions options = {0};

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

	outputPath = options.output;

	getcwd(cwd, PATH_MAX);
	sprintf(rawVideoPath, "%s/ZarraRawVideoInput.raw", cwd);

	puts("Spawn Video Record Task");

	SpawnVideoTask(&taskManager, options.input, rawVideoPath);

	puts("Spawning Audio Record Task");
	sprintf(rawAudioPath, "%s/ZarraRawAudioInput.flac", cwd);
	SpawnAudioTask(&taskManager,
		       "alsa_output.pci-0000_00_1b.0.analog-stereo.monitor",
		       rawAudioPath);

	signal(SIGTERM, Terminate);
	signal(SIGINT, Terminate);
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

			printf(" Speed: %f\n FPS: %f\n Rate: %f\n", task->speed,
			       task->framesPerSecond, task->bitrate);
		}
	}

	Terminate(-1);
	return 0;
}

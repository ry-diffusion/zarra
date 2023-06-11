#include "zarra.h"
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

UIState globalUiState = {0};

void onQuitRequested(int signal)
{
	(void)signal;
	globalUiState.terminateRequested = true;
}

bool RunAgent(CLIOptions options)
{
	TaskManager taskManager = {0};

	char rawAudioPath[pathMax] = {0};
	char rawVideoPath[pathMax] = {0};
	char cwd[cwdMax] = {0};

	if (!getcwd(cwd, cwdMax))
		return false;

	snprintf(rawAudioPath, pathMax, "%s/zarra-raw-audio.flac", cwd);
	snprintf(rawVideoPath, pathMax, "%s/zarra-raw-video.raw", cwd);

	signal(SIGINT, onQuitRequested);
	signal(SIGTERM, onQuitRequested);

	SpawnAudioTask(&taskManager,
		       "alsa_output.pci-0000_00_1b.0.analog-stereo.monitor",
		       rawAudioPath, options.wantsRootAgent);

	SpawnVideoTask(&taskManager, options.input, rawVideoPath);

	UIPrepareTerm(&globalUiState);
	UITask(&globalUiState, &taskManager);
	TerminateAllTasks(&taskManager);

	globalUiState.terminateRequested = false;
	taskManager.currentRunning = 0;

	UIClear();
	puts("Processing!");
	wait(0);

	SpawnProcessingTask(&taskManager, rawVideoPath, rawAudioPath,
			    options.output);
	UITask(&globalUiState, &taskManager);

	wait(0);
	UIRestoreTerm(&globalUiState);

	remove(rawAudioPath);
	remove(rawVideoPath);

	return true;
}
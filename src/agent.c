#include "zarra.h"
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

bool RunAgent(CLIOptions options)
{
	TaskManager taskManager = {0};
	UIState uiState = {0};
	char rawAudioPath[pathMax] = {0};
	char rawVideoPath[pathMax] = {0};
	char cwd[cwdMax] = {0};

	if (!getcwd(cwd, cwdMax))
		return false;

	snprintf(rawAudioPath, pathMax, "%s/zarra-raw-audio.flac", cwd);
	snprintf(rawVideoPath, pathMax, "%s/zarra-raw-video.raw", cwd);

	SpawnAudioTask(&taskManager,
		       "alsa_output.pci-0000_00_1b.0.analog-stereo.monitor",
		       rawAudioPath, options.wantsRootAgent);

	UIPrepareTerm(&uiState);
	UITask(&uiState, &taskManager);
	UIRestoreTerm(&uiState);

	TerminateAllTasks(&taskManager);

	wait(0);
	return true;
}
#include "zarra.h"
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

void updateState(UIState *uiState, TaskManager *taskManager)
{
	Task *task;
	uint current;

	for (current = 0; current < taskManager->currentRunning; ++current)
	{
		task = &taskManager->running[current];
		ParseFFmpegOutput(task);

		switch (task->type)
		{
		case TaskRecordAudio:
			uiState->lastAudioBitrate = task->bitrate;
			break;
		case TaskRecordVideo:
			uiState->lastVideoFramerate = task->framesPerSecond;
			break;
		default:
			break;
		}
	}
}

void UIClear(void)
{
	printf("\33[2K\r");
	fflush(stdout);
}

void UIWrite(Text output)
{
	UIClear();

	printf("%s\r", output);
	fflush(stdout);
}

void UITask(UIState *uiState, TaskManager *taskManager)
{
	char textBuffer[512] = {0};
	fd_set readset;
	struct timeval tv;

	do
	{
		memset(textBuffer, '\0', 512);

		tv.tv_sec = 0;
		tv.tv_usec = 1;

		FD_ZERO(&readset);
		FD_SET(fileno(stdin), &readset);
		select(fileno(stdin) + 1, &readset, NULL, NULL, &tv);

		if (FD_ISSET(fileno(stdin), &readset))
		{
			switch (fgetc(stdin))
			{
			case 'c':
				return;
			}
		}

		updateState(uiState, taskManager);
		snprintf(textBuffer, 512, "video: fps(%u); audio: bitrate(%f)",
			 uiState->lastVideoFramerate,
			 uiState->lastAudioBitrate);

		UIWrite(textBuffer);
	} while (IsAllTasksGood(taskManager) && !uiState->terminateRequested);
}

void UIPrepareTerm(UIState *uiState)

{
	struct termios ttystate;
	tcgetattr(STDIN_FILENO, &ttystate);
	uiState->ttyOldstate = ttystate;
	ttystate.c_lflag &= ~(ICANON | ECHO);
	ttystate.c_cc[VMIN] = 1;

	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void UIRestoreTerm(UIState *ui)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &ui->ttyOldstate);
}

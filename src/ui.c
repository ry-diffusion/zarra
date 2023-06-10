#include "zarra.h"
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

Text space = " ";

void updateState(UIState *us, TaskManager *tm)
{
	Task *task;
	uint current;

	for (current = 0; current < tm->currentRunning; ++current)
	{
		task = &tm->running[current];
		ParseFFmpegOutput(task);

		switch (task->type)
		{
		case TaskRecordAudio:
			us->lastAudioBitrate = task->bitrate;
			break;
		case TaskRecordVideo:
			us->lastVideoFramerate = task->framesPerSecond;
			break;
		default:
			break;
		}
	}
}

void updateOutput(UIState *us, Text output)
{
	fwrite(space, sizeof(char), us->outputWritten, stdout);
	fputc('\r', stdout);
	us->outputWritten = printf("%s\r", output);
	fflush(stdout);
}

void UITask(UIState *us, TaskManager *tm)
{
	char textBuffer[512] = {0};
	fd_set readset;
	struct timeval tv;

	do
	{
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

		updateState(us, tm);
		snprintf(textBuffer, 512, "video: fps(%u); audio: bitrate(%f)",
			 us->lastVideoFramerate, us->lastAudioBitrate);
		updateOutput(us, textBuffer);
	} while (IsAllTasksGood(tm) && !us->terminateRequested);
}

void UIPrepareTerm(UIState *ui)

{
	struct termios ttystate;
	tcgetattr(STDIN_FILENO, &ttystate);
	ui->ttyOldstate = ttystate;
	ttystate.c_lflag &= ~(ICANON | ECHO);
	ttystate.c_cc[VMIN] = 1;

	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void UIRestoreTerm(UIState *ui)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &ui->ttyOldstate);
}

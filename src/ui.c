#include "zarra.h"
#include <stdio.h>
#include <string.h>

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

	do
	{
		updateState(us, tm);
		snprintf(textBuffer, 512, "video: fps(%u); audio: bitrate(%f)",
			 us->lastVideoFramerate, us->lastAudioBitrate);
		updateOutput(us, textBuffer);
	} while (IsAllTasksGood(tm));
}

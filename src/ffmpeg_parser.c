#include "zarra.h"
#include <stdlib.h>
#include <unistd.h>

void parseKV(Task *task, Text keyBuffer, Text valueBuffer)
{
	if (StartsWith(keyBuffer, "fps"))
		task->framesPerSecond = atof(valueBuffer);
	else if (StartsWith(keyBuffer, "bitrate"))
		task->bitrate = atof(valueBuffer);
	else if (StartsWith(keyBuffer, "speed"))
		task->speed = atof(valueBuffer);

	ClearBuffer(keyBuffer, keyBufferSize);
	ClearBuffer(valueBuffer, valueBufferSize);
}

void ParseFFmpegOutput(Task *task)
{
	char keyBuffer[keyBufferSize] = {0};
	char valueBuffer[valueBufferSize] = {0};
	char inputBuffer[1] = {0};
	uint bufferIdx = 0;
	bool isReadingValue = false;
	bool hasParsedFrame = false;

	while (read(task->stdoutFd, inputBuffer, 1) > 0)
	{
		if (StartsWith(keyBuffer, "progress") && hasParsedFrame)
			return;
		else if (StartsWith(keyBuffer, "progress"))
			hasParsedFrame = true;

		switch (*inputBuffer)
		{
		case '\n':
			valueBuffer[bufferIdx] = '\0';
			parseKV(task, keyBuffer, valueBuffer);
			isReadingValue = 0;
			bufferIdx = 0;
			continue;
		case '=':
			keyBuffer[bufferIdx] = '\0';
			bufferIdx = 0;
			isReadingValue = true;
			continue;
		}

		if (isReadingValue && bufferIdx < valueBufferSize)
			valueBuffer[bufferIdx++] = *inputBuffer;
		else if (bufferIdx < keyBufferSize)
		{
			keyBuffer[bufferIdx++] = *inputBuffer;
		}
	}
}

#pragma once
#include <limits.h>

#define keyBufferSize 24
#define valueBufferSize 24

typedef unsigned char bool;
typedef const char *restrict Text;
typedef char *restrict TextMut;
typedef unsigned char u8;
typedef unsigned int uint;

static const bool true = 1;
static const bool false = 0;

typedef enum
{
	TaskRecordAudio,
	TaskRecordVideo,
	TaskProcessRecording,
	TaskMax
} TaskType;

typedef struct
{
	TaskType type;
	float rate, speed;
	int stdoutFd, pid;
} Task;

typedef struct
{
	Task running[TaskMax];
	u8 currentRunning;
} TaskManager;

typedef struct
{
	char input[PATH_MAX];
	char output[PATH_MAX];
} CLIOptions;

/*
 * Parses cli arguments
 * @param argc The number of arguments
 * @param argc The values of arguments
 * @returns 1 on success, otherwise returns 0.
 */
bool ParseCLI(CLIOptions *opts, int argc, char **argv);

/*
 * Checks if `ffmpeg` can be run.
 */
bool IsFFMPEGInstalled(void);

/*
 * Spawns a Video Recording task.
 * @param taskManager The Task Manager
 * @param input The input file path
//  * @returns Negative integer whens fails to spawn it, otherwise 1 on
//  * success.
 */
void SpawnVideoTask(TaskManager *taskManager, Text input, Text output);

/*
 * Spawns a Audio Recording task.
 * @param taskManager The Task Manager
 * @param input The input file path
 */
void SpawnAudioTask(TaskManager *taskManager, Text device, Text output);

/*
 * Verify if str starts with prefix
 * @param prefix The prefix
 * @return Returns 0 if not matches, otherwise returns 1.
 */
unsigned char StartsWith(Text str, Text prefix);

void ClearBuffer(char *restrict buffer, uint size);
bool IsAllTasksGood(TaskManager *taskManager);
void TerminateAllTasks(TaskManager *taskManager);
void ParseFFmpegOutput(Task *task);

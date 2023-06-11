#pragma once
#include <limits.h>
#include <stdatomic.h>
#include <termios.h>

#define keyBufferSize 24
#define valueBufferSize 24
#define defaultOutputFileName "zarra-recording.mp4"

typedef unsigned char bool;
typedef unsigned char u8;
typedef unsigned int uint;
typedef char *restrict Text;

static const bool true = 1;
static const bool false = 0;

#if !defined(PATH_MAX)
#define pathMax 4096
#else
#define pathMax PATH_MAX
#endif

#define cwdMax pathMax >> 1

typedef enum TaskType
{
	TaskRecordAudio,
	TaskRecordVideo,
	TaskProcessRecording,
	TaskMax
} TaskType;

typedef struct Task
{
	TaskType type;
	float framesPerSecond, bitrate, speed;
	int stdoutFd, pid;
} Task;

typedef struct TaskManager
{
	Task running[TaskMax];
	u8 currentRunning;
} TaskManager;

typedef struct CLIOptions
{
	char input[pathMax];
	char audioSource[200];
	char output[pathMax];
	uint framerate;

	/**
	 * -1 when not, and >= 1, when it wants
	 */
	int wantsRootAgent;
} CLIOptions;

typedef struct UIState
{
	uint lastVideoFramerate;
	float lastAudioBitrate;
	struct termios ttyOldstate;
	atomic_bool terminateRequested;
} UIState;

/**
 * Parses cli arguments
 * @param argc The number of arguments
 * @param argc The values of arguments
 * @returns 1 on success, otherwise returns 0.
 */
bool ParseCLI(CLIOptions *opts, int argc, char **argv);

/**
 * Checks if `ffmpeg` can be run.
 */
bool IsFFMPEGInstalled(void);

/**
 * Spawns a Video Recording task.
 * @param taskManager The Task Manager
 * @param input The input file path
 */
void SpawnVideoTask(TaskManager *taskManager, Text input, Text output);

/**
 * Spawns a Audio Recording task.
 * @param taskManager The Task Manager
 * @param input The input file path
 * @param uid The uid to record audio
 */
void SpawnAudioTask(TaskManager *taskManager, Text device, Text output,
		    uint uid);

/**
 * Spawns a processing task.
 * Merges video and a audio to a single file.
 * @param taskManager The Task Manager
 * @param videoPath The raw video path
 * @param audioPath The raw audio path
 * @param output The output path
 */
void SpawnProcessingTask(TaskManager *taskManager, Text videoPath,
			 Text audioPath, Text outputPath);

/**
 * Verify if str starts with prefix
 * @param prefix The prefix
 * @return Returns 0 if not matches, otherwise returns 1.
 */
unsigned char StartsWith(Text str, Text prefix);

void ClearBuffer(char *restrict buffer, uint size);
bool IsPipeClosed(uint fd);

bool IsAllTasksGood(TaskManager *taskManager);
void TerminateAllTasks(TaskManager *taskManager);
void ParseFFmpegOutput(Task *task);

void UITask(UIState *uiState, TaskManager *taskManager);
void UIPrepareTerm(UIState *uiState);
void UIRestoreTerm(UIState *uiStae);
void UIWrite(Text output);
void UIClear(void);

bool RunAgent(CLIOptions options);
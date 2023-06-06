#include "zarra.h"
#include <stddef.h>
#include <sys/wait.h>
#include <unistd.h>

void SpawnVideoTask(TaskManager *taskManager, Text input, Text output)
{
	int pipedes[2] = {0, 0};
	pipe(pipedes);

	Text cmdline[] = {
	    "pkexec",	 "ffmpeg",
	    "-y",	 "-hide_banner",
	    "-loglevel", "error",
	    "-progress", "-",
	    "-device",	 input,
	    "-f",	 "kmsgrab",
	    "-i",	 "-",
	    "-filter:v", "hwmap=derive_device=vaapi,scale_vaapi=format=nv12",
	    "-codec:v",	 "h264_vaapi",
	    "-f",	 "rawvideo",
	    output,	 NULL};

	pid_t pid = fork();

	if (!pid)
	{
		close(pipedes[0]);
		close(STDIN_FILENO);
		dup2(pipedes[1], STDOUT_FILENO);
		execvp("pkexec", (char **)cmdline);
	}

	close(pipedes[1]);

	Task task = {TaskRecordVideo, 0.0f, 0.0f, pipedes[0], pid};
	taskManager->running[taskManager->currentRunning++] = task;
}

void SpawnAudioTask(TaskManager *taskManager, Text input, Text output)
{
	int pipedes[2] = {0, 0};
	pipe(pipedes);

	Text cmdline[] = {
	    "ffmpeg", "-y", "-hide_banner", "-loglevel", "error", "-progress",
	    "-",      "-f", "pulse",	    "-ac",	 "1",	  "-i",
	    input,    "-f", "flac",	    output,	 NULL};

	pid_t pid = fork();

	if (!pid)
	{
		close(pipedes[0]);
		close(STDIN_FILENO);
		dup2(pipedes[1], STDOUT_FILENO);
		execvp("ffmpeg", (char **)cmdline);
	}

	close(pipedes[1]);

	Task task = {TaskRecordAudio, 0.0f, 0.0f, pipedes[0], pid};
	taskManager->running[taskManager->currentRunning++] = task;
}

bool IsAllTasksGood(TaskManager *taskManager)
{
	int stat, rc;
	u8 currentTask;
	Task task;

	for (currentTask = 0; currentTask < taskManager->currentRunning;
	     ++currentTask)
	{
		task = taskManager->running[currentTask];

		rc = waitpid(task.pid, &stat, WUNTRACED | WCONTINUED | WNOHANG);
		if (rc == -1)
			return false;
		if (WIFEXITED(stat))
			return WEXITSTATUS(stat) == 0;
	}

	return true;
}

void TerminateAllTasks(TaskManager *taskManager)
{
	u8 currentTask;
	Task task;

	for (currentTask = 0; currentTask < taskManager->currentRunning;
	     ++currentTask)
	{
		task = taskManager->running[currentTask];

		kill(SIGTERM, task.pid);
	}
}

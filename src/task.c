#include "zarra.h"
#include <stddef.h>
#include <sys/wait.h>
#include <unistd.h>

void SpawnVideoTask(TaskManager *taskManager, Text input, Text output)
{
	Text cmdline[] = {
	    "pkexec",	 "ffmpeg",
	    "-device",	 input,
	    "-f",	 "kmsgrab",
	    "-i",	 "-",
	    "-filter:v", "hwmap=derive_device=vaapi,scale_vaapi=format=nv12",
	    "-codec:v",	 "h264_vaapi",
	    "-f",	 "rawvideo",
	    "-y",	 output,
	    NULL};

	pid_t pid = fork();

	if (!pid)
	{
		execvp("pkexec", (char **)cmdline);
	}

	Task task = {TaskRecordVideo, STDOUT_FILENO, STDERR_FILENO, pid};
	taskManager->running[taskManager->currentRunning++] = task;
}

void SpawnAudioTask(TaskManager *taskManager, Text input, Text output)
{
	Text cmdline[] = {"ffmpeg", "-f", "pulse", "-ac", "1",	  "-i",
			  input,    "-f", "flac",  "-y",  output, NULL};

	pid_t pid = fork();

	if (!pid)
	{
		execvp("ffmpeg", (char **)cmdline);
	}

	Task task = {TaskRecordVideo, STDOUT_FILENO, STDERR_FILENO, pid};
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

		rc = waitpid(task.pid, &stat, WUNTRACED | WCONTINUED);
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

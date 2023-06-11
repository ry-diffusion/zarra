#include "zarra.h"
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void SpawnVideoTask(TaskManager *taskManager, Text input, Text output)
{
	int pipedes[2] = {0, 0};
	pipe(pipedes);

	Text cmdline[] = {
	    "pkexec",	  "ffmpeg",
	    "-y",	  "-hide_banner",
	    "-loglevel",  "error",
	    "-progress",  "-",
	    "-device",	  input,
	    "-framerate", "60",
	    "-f",	  "kmsgrab",
	    "-i",	  "-",
	    "-filter:v",  "hwmap=derive_device=vaapi,scale_vaapi=format=nv12",
	    "-codec:v",	  "h264_vaapi",
	    "-f",	  "rawvideo",
	    output,	  NULL};

	pid_t pid = fork();
	if (!pid)
	{
		close(pipedes[0]);
		close(STDIN_FILENO);
		dup2(pipedes[1], STDOUT_FILENO);
		execvp("pkexec", (char **)cmdline);
	}

	close(pipedes[1]);

	Task task = {TaskRecordVideo, 0.0f, 0.0f, 0.0f, pipedes[0], pid};
	taskManager->running[taskManager->currentRunning++] = task;
}

void SpawnAudioTask(TaskManager *taskManager, Text input, Text output, uint uid)
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
		char buffer[30] = {0};

		if (setuid(uid) < 0)
		{
			fprintf(stderr,
				"AudioTask(error): unable to setuid to "
				"%u. Aborting!\n ",
				uid);

			_exit(1);
		}

		snprintf(buffer, 30, "/var/run/user/%u/pulse", uid);
		setenv("PULSE_RUNTIME_PATH", buffer, 1);

		close(pipedes[0]);
		close(STDIN_FILENO);
		dup2(pipedes[1], STDOUT_FILENO);
		execvp("ffmpeg", (char **)cmdline);
	}
	else
	{
		close(pipedes[1]);

		Task task = {TaskRecordAudio, 0.0f, 0.0f, 0.0f,
			     pipedes[0],      pid};
		taskManager->running[taskManager->currentRunning++] = task;
	}
}

void SpawnProcessingTask(TaskManager *taskManager, Text videoPath,
			 Text audioPath, Text outputPath)
{
	int pipedes[2] = {0, 0};
	pipe(pipedes);
	Text cmdline[] = {
	    "ffmpeg",	 "-y",	 "-hide_banner", "-loglevel", "error",
	    "-progress", "-",	 "-i",		 videoPath,   "-i",
	    audioPath,	 "-c:v", "copy",	 "-acodec",   "copy",
	    outputPath,	 NULL};

	pid_t pid = fork();

	if (!pid)
	{
		close(pipedes[0]);
		close(STDIN_FILENO);
		dup2(pipedes[1], STDOUT_FILENO);
		execvp("ffmpeg", (char **)cmdline);
	}

	close(pipedes[1]);

	Task task = {TaskProcessRecording, 0.0f, 0.0f, 0.0f, pipedes[0], pid};
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

		switch (task.type)
		{
		case TaskRecordVideo:
			if (IsPipeClosed(task.stdoutFd))
				return false;
			break;
		default:
			rc = waitpid(task.pid, &stat, WUNTRACED | WNOHANG);

			switch (rc)
			{
			case -1:
				return false;
			case 0:
				continue;
			}

			if (WIFEXITED(stat) && WEXITSTATUS(stat) != 0)
				return false;
		}
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

		kill(task.pid, SIGTERM);
		// close(task.stdoutFd);
	}
}

#include "zarra.h"
#include <sys/wait.h>
#include <unistd.h>

int WaitPid(pid_t pid)
{
	int stat, rc;
	do
	{
		rc = waitpid(pid, &stat, WUNTRACED | WCONTINUED);
		if (rc == -1)
			return false;
		if (WIFEXITED(stat))
			return WEXITSTATUS(stat) == 0;

	} while (!WIFEXITED(stat) && !WIFSIGNALED(stat));

	return false;
}

bool IsFFMPEGInstalled(void)
{
	char *args[] = {"ffmpeg", "-version", NULL};
	pid_t pid = fork();

	if (!pid)
	{
		close(STDOUT_FILENO);
		_exit(execvp("ffmpeg", args));
	}

	return WaitPid(pid);
}

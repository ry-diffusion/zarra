#include "zarra.h"
#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>

void ClearBuffer(char *restrict buffer, uint size)
{
	uint idx = 0;

	while (idx++ != size)
	{
		buffer[idx] = 0;
	}
}

unsigned char StartsWith(Text str, Text prefix)
{
	register unsigned char eq = 0;
	while (*prefix && (eq = *prefix++ == *str++))
		;
	return eq;
}

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

bool IsPipeClosed(uint fd)
{
	struct pollfd pfd = {
	    .fd = fd,
	    .events = POLLOUT,
	};

	if (poll(&pfd, 1, 1) < 0)
	{
		return false;
	}

	return pfd.revents & POLLERR;
}

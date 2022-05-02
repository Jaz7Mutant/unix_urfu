#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

static int SUCCESSFUL_LOCKS = 0;

int open_file(char *path, int flags, int mode)
{
    int lockFileFd = open(path, flags, mode);
    if (lockFileFd == -1)
    {
        perror("Error while opening file");
        exit(-1);
    }
    return lockFileFd;
}

void close_file(int lockFileFd)
{
    int unlinkResult = close(lockFileFd);
    if (unlinkResult == -1)
    {
        perror("Error while closing file");
    }
}

int read_file(int fd, char *buf, int count)
{
    int readCount = read(fd, buf, count);
    if (readCount == -1)
    {
        perror("Error while reading file");
        exit(-1);
    }
    return readCount;
}

void write_file(int lockFileFd, char *buf, int count)
{
    int unlinkResult = write(lockFileFd, buf, count);
    if (unlinkResult == -1)
    {
        perror("Error while writing file");
        exit(-1);
    }
}

void lseek_file(int lockFileFd, int offset, int whence)
{
    int unlinkResult = lseek(lockFileFd, offset, whence);
    if (unlinkResult == -1)
    {
        perror("Error while lseek in file");
        exit(-1);
    }
}

void sigintHandler(int signum)
{ 
	int lockFileFd = open_file("statistics", O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
    char* buffer = malloc(200);
	if (buffer == NULL)
    {
        printf("Error while allocating buffer with 'malloc'\n");
        exit(-1);
    }
	snprintf(buffer, 200, "Successful locks: %d\n", SUCCESSFUL_LOCKS);
	write_file(lockFileFd, buffer, 200);
	exit(0);
}


int main(int argc, char *argv[])
{
	if (signal(SIGINT, sigintHandler) == SIG_ERR)
	{
		printf("Error in setting SIGINT handler\n");
		return -1;
	}

    char *inputFilePath = NULL;
    int inputFD = 0;
    if (optind < argc)
    {
        inputFilePath = argv[optind++];
    }
    else
    {
        printf("Missing file path\n");
        return -1;
    }

    char *lockPath = malloc(strlen(inputFilePath) + strlen(".lck") + 1);
    if (lockPath == NULL)
    {
        printf("Error while allocating buffer with 'malloc'\n");
        exit(-1);
    }
    strcpy(lockPath, inputFilePath);
    strcat(lockPath, ".lck");

	while (1)
	{
		int lockFileFd = -1;
		while (lockFileFd == -1)
		{
			lockFileFd = open(lockPath, O_CREAT | O_EXCL | O_RDWR, S_IRWXU);
		}
		SUCCESSFUL_LOCKS++;
		
		int pid = getpid();
		int pidLength = snprintf(NULL, 0, "%d", pid);
		char* pidBuffer = malloc(pidLength + 1);
		if (pidBuffer == NULL)
		{
			printf("Error while allocating buffer with 'malloc'\n");
            exit(-1);
		}
		snprintf(pidBuffer, pidLength + 1, "%d", pid);
		write_file(lockFileFd, pidBuffer, pidLength);

		int fileFd = open_file(inputFilePath, O_RDWR, 0);		
		sleep(1);
		close_file(fileFd);

		int unlinkResult = access(lockPath, F_OK);
		if (unlinkResult == -1)
		{
			printf("No lock file for pid: %d\n", pid);
			free(pidBuffer);
			exit(-1);
		}
		
        // todo
		lseek_file(lockFileFd, 0, SEEK_SET);
        // todo
		int readCount = read_file(lockFileFd, pidBuffer, pidLength + 1);
		int newPid = atoi(pidBuffer);
		free(pidBuffer);
		if (newPid != pid)
		{
			printf("Expected pid: %d, actual pid: %d in lock file\n", pid, newPid);
			return -1;
		}
		
		close_file(lockFileFd);
		unlinkResult = unlink(lockPath);
		if (unlinkResult == -1)
		{
            perror("Error while unlinking lock file");
			return -1;
		}
	}
	return 0;
}

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

int parse_block_size(int argc, char *argv[])
{
    int argument;
    int blockSize = 4096;
    while ((argument = getopt(argc, argv, ":b:")) != -1)
    {
        switch (argument)
        {
        case 'b':
            blockSize = atoi(optarg);
            break;
        case '?':
            printf("Unknown argument: %c\n", optopt);
            exit(-1);
        case ':':
            printf("Missing argument for %c\n", optopt);
            exit(-1);
        }
    }
    return blockSize;
}

int open_file(char *path, int flags, int mode)
{
    int fd = open(path, flags, mode);
    if (fd == -1)
    {
        perror("Error while opening file");
        exit(-1);
    }
    return fd;
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

void lseek_file(int fd, int offset, int whence)
{
    int result = lseek(fd, offset, whence);
    if (result == -1)
    {
        perror("Error while lseek in file");
        exit(-1);
    }
    return;
}

void write_file(int fd, char *buf, int count)
{
    int result = write(fd, buf, count);
    if (result == -1)
    {
        perror("Error while writing file");
        exit(-1);
    }
    return;
}

void close_file(int fd)
{
    int result = close(fd);
    if (result == -1)
    {
        perror("Error while closing file");
    }
    // return;
}

int main(int argc, char *argv[])
{
    char *inputFilePath = NULL;
    char *outputFilePath = NULL;
    int inputFD = 0;
    int outputFD = 0;
    int blockSize = parse_block_size(argc, argv);
    if (optind < argc)
    {
        inputFilePath = argv[optind++];
    }
    else
    {
        printf("Missing file path\n");
        return -1;
    }
    if (optind < argc)
    {
        outputFilePath = argv[optind++];
    }
    if (outputFilePath == NULL)
    {
        outputFD = open_file(inputFilePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    }
    else
    {
        inputFD = open_file(inputFilePath, O_RDONLY, 0);
        outputFD = open_file(outputFilePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    }

    char isEmpty;
    int zeroBytesCount = 0;
    int readCount;
    char *buffer = (char *)malloc(blockSize);
    if (buffer == NULL)
    {
        printf("Error while allocating buffer with 'malloca'\n");
        exit(-1);
    }
    while ((readCount = read_file(inputFD, buffer, blockSize)) != 0)
    {
        isEmpty = 1;
        for (int i = 0; i < readCount; i++)
        {
            if (buffer[i] != 0)
            {
                isEmpty = 0;
                break;
            }
        }
        if (isEmpty != 0)
        {
            zeroBytesCount += readCount;
            continue;
        }
        if (zeroBytesCount != 0)
        {
            lseek_file(outputFD, zeroBytesCount, SEEK_CUR);
            zeroBytesCount = 0;
        }
        write_file(outputFD, buffer, readCount);
    }
    if (zeroBytesCount != 0)
    {
        lseek_file(outputFD, zeroBytesCount, SEEK_CUR);
    }
    if (inputFD != 0)
    {
        close_file(inputFD);
    }
    close_file(outputFD);
    return 0;
}

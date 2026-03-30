#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

void server(int, int);
void client(int, int);

int main() {
    int p1[2], p2[2], pid;

    pipe(p1);
    pipe(p2);

    pid = fork();

    if (pid == 0) {
        // Child process → server
        close(p1[1]); // close write end of pipe1
        close(p2[0]); // close read end of pipe2
        server(p1[0], p2[1]);
        exit(0);
    }

    // Parent process → client
    close(p1[0]); // close read end of pipe1
    close(p2[1]); // close write end of pipe2
    client(p1[1], p2[0]);

    wait(NULL);
    return 0;
}

void server(int rfd, int wfd) {
    int n;
    char fname[2000], buff[2000];

    // Read filename from client
    n = read(rfd, fname, sizeof(fname));
    fname[n] = '\0';

    int fd = open(fname, O_RDONLY);

    if (fd < 0) {
        write(wfd, "can't open", 10);
    } else {
        n = read(fd, buff, sizeof(buff));
        write(wfd, buff, n);
        close(fd);
    }
}

void client(int wfd, int rfd) {
    int n;
    char fname[2000], buff[2000];

    printf("Enter filename: ");
    scanf("%s", fname);

    // Send filename to server
    write(wfd, fname, strlen(fname) + 1);

    // Read response from server
    n = read(rfd, buff, sizeof(buff));
    buff[n] = '\0';

    // Print output
    write(1, buff, n);
}

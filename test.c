#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("test.txt", O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char *text = "Hello, file system!\n";
    write(fd, text, 21);
    close(fd);

    return 0;
}

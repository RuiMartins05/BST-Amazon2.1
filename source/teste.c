#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char str[] = "<hostname>:2";
    char *hostname = strtok((char *)str, ":");
    int port = atoi(strtok(NULL,":"));

    printf("%s\n", hostname);
    printf("%d\n", port);
}
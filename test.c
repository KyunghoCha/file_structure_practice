//
// Created by gcha792 on 10/21/25.
//

#include <stdio.h>
#include <pthread.h>

void* my_thread_func(void* arg) {
    printf("Hello from thread!\n");
    return NULL;
}

int main() {
    pthread_t tid;

    if (pthread_create(&tid, NULL, my_thread_func, NULL) != 0) {
        perror("pthread_create failed");
        return 1;
    }

    pthread_join(tid, NULL);
    printf("Thread finished.\n");

    return 0;
}


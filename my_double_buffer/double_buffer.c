//
// Created by gcha792 on 10/22/25.
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define BUFFER_SIZE 4096

typedef struct {
    char *data[2];
    bool is_full[2];
    int front;
    int back;

    pthread_mutex_t lock;
    pthread_cond_t can_produce;
    pthread_cond_t can_consume;
} Buffer, *pBuffer;

void init_buffer(pBuffer);
void swap_buffer(pBuffer);
void *producer(void *);
void *consumer(void *);

int main() {
    Buffer buffer = {
        .data = {NULL, NULL},
        .is_full = {false, false},
        .front = 0,
        .back = 1
    };

    pthread_mutex_init(&buffer.lock, NULL);
    pthread_cond_init(&buffer.can_produce, NULL);
    pthread_cond_init(&buffer.can_consume, NULL);

    init_buffer(&buffer);

    pthread_t prod_thread, cons_thread;
    pthread_create(&prod_thread, NULL, producer, &buffer);
    pthread_create(&cons_thread, NULL, consumer, &buffer);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    pthread_mutex_destroy(&buffer.lock);
    pthread_cond_destroy(&buffer.can_produce);
    pthread_cond_destroy(&buffer.can_consume);

    for (int i = 0; i < 2; i++) {
        free(buffer.data[i]);
    }

    return 0;
}

void init_buffer(pBuffer buffer) {
    for (int i = 0; i < 2; i++) {
        buffer->data[i] = calloc(1, BUFFER_SIZE);
        if (!buffer->data[i]) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }
    }
}

// Only call while holding buffer->lock!
void swap_buffer(pBuffer buffer) {
    int temp = buffer->front;
    buffer->front = buffer->back;
    buffer->back = temp;
}

void *producer(void *arg) {
    pBuffer buffer = (pBuffer)arg;
    while (1) {
        pthread_mutex_lock(&buffer->lock);

        while (buffer->is_full[buffer->back]) {
            pthread_cond_wait(&buffer->can_produce, &buffer->lock);
        }

        snprintf(buffer->data[buffer->back], BUFFER_SIZE, "Produced at %ld", time(NULL));
        buffer->is_full[buffer->back] = true;

        swap_buffer(buffer);

        pthread_cond_signal(&buffer->can_consume);
        pthread_mutex_unlock(&buffer->lock);

        sleep(1);  // 생산은 1초마다
    }
    return NULL;
}

void *consumer(void *arg) {
    pBuffer buffer = (pBuffer)arg;
    while (1) {
        pthread_mutex_lock(&buffer->lock);

        while (!buffer->is_full[buffer->front]) {
            pthread_cond_wait(&buffer->can_consume, &buffer->lock);
        }

        printf("Consumed: %s\n", buffer->data[buffer->front]);
        buffer->is_full[buffer->front] = false;

        pthread_cond_signal(&buffer->can_produce);
        pthread_mutex_unlock(&buffer->lock);

        usleep(100000);  // 소비는 약간 빠르게
    }
    return NULL;
}

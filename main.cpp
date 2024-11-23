#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct {
    int data;
} MyData;

 
pthread_cond_t cond= PTHREAD_COND_INITIALIZER;
 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
 int ready = 0;
 MyData *datas = NULL;


void *provider(void *arg) {
    for(int i=0; i<10;i++) {
        sleep(1);  
        pthread_mutex_lock(&mutex);
        if (datas != NULL) free(datas);  
        datas = (MyData*) malloc(sizeof(MyData));
        datas->data = rand();  
        printf("provided: %d\n", datas->data);
        ready = 1;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg) {
    for(int i=0; i<10;i++) {
        pthread_mutex_lock(&mutex);
 
        while (ready == 0) {
            pthread_cond_wait(&cond, &mutex);  
        }
        printf("awoke: %d\n", datas->data);
        ready = 0;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t provider_thread, consumer_thread;
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&provider_thread, NULL, provider, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    pthread_join(provider_thread, NULL);
    pthread_join(consumer_thread, NULL);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    return 0;
}
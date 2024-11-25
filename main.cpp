#include <iostream>
#include <pthread.h>
using namespace std;

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int ready1 = 0;  
int ready2 = 0;  

void* provider(void* arg) {
  pthread_mutex_lock(&lock);
  while(ready2 == 1) {  
      pthread_cond_wait(&cond1, &lock);
  }
  ready1 = 1;
  ready2 = 0;  
  cout << "provided" << endl;
  pthread_cond_signal(&cond1);
  pthread_mutex_unlock(&lock);
  return 0;
}

void* consumer(void* arg) {
  pthread_mutex_lock(&lock);
  while (ready1 == 0) {
    pthread_cond_wait(&cond1, &lock);
    cout << "awoke" << endl;
  }
  ready1 = 0;
  ready2 = 1;  
  cout << "consumed" << endl;
  pthread_mutex_unlock(&lock);
  return 0;
}

int main() {
  pthread_t thread_provider, thread_consumer;
  pthread_create(&thread_provider, nullptr, provider, nullptr);
  pthread_create(&thread_consumer, nullptr, consumer, nullptr);
  pthread_join(thread_provider, nullptr);
  pthread_join(thread_consumer, nullptr);
  pthread_cond_destroy(&cond1);
  pthread_mutex_destroy(&lock);  
  return 0;
}

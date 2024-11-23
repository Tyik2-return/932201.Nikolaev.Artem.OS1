#include <iostream>
#include <pthread.h>
using namespace std;

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int ready = 0;

void* provider(void* arg) {
  pthread_mutex_lock(&lock);
  ready = 1; 
  cout << "provided" << endl;
  pthread_cond_signal(&cond1); 
  pthread_mutex_unlock(&lock);
  return nullptr;
}

void* consumer(void* arg) {
  pthread_mutex_lock(&lock);
  while (ready == 0) {
    pthread_cond_wait(&cond1, &lock);
    cout << "awoke" << endl;
  }
  ready = 0;
  cout << "consumed" << endl;
  pthread_mutex_unlock(&lock);
  return nullptr;
}

int main() {
  pthread_t thread_provider, thread_consumer;
  pthread_create(&thread_provider, nullptr, provider, nullptr);
  pthread_create(&thread_consumer, nullptr, consumer, nullptr);
  pthread_join(thread_provider, nullptr);
  pthread_join(thread_consumer, nullptr);
  return 0;
}

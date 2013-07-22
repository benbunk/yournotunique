#include <stdio.h>
#include <pthread.h>

#define NUMTHRDS 4
pthread_mutex_t mutex;
pthread_t pth[NUMTHRDS];  // this is our thread identifier
int t_ids[NUMTHRDS];

#define BUFSIZE 3
int buffer_pos_max = 255;
int buffer_current = 0;
unsigned char buffer[BUFSIZE];

void next_buffer(unsigned char buffer[], int *buffer_current, int buffer_pos_max) {
	int i;
	for (i = *buffer_current; i >= 0; i--) {
		if (buffer[i] < buffer_pos_max) {
			buffer[i]++;
			break;
		}
		if (buffer[i] == buffer_pos_max) {
			buffer[i] = 0;
		}

		if (i == 0 && buffer[i] == 0) {
			(*buffer_current)++;
		}
	}
}

void *doWork(void *arg) {
  int* str = (int *)arg;
  int local = *str;
	int j;

	
	while (buffer_current <= BUFSIZE) {
		pthread_mutex_lock(&mutex);
		printf("\nThread %d: ", local);
		for (j = 0; j <= buffer_current; j++) {
			printf("%2x ", buffer[j]);
		}
		next_buffer(buffer, &buffer_current, buffer_pos_max);
		pthread_mutex_unlock(&mutex);
		usleep(100);
	}
}

int main (void) {
	// Initialize the buffer.
	int i; 
	for (i = 0; i < BUFSIZE; i++) {
		buffer[i] = 0;
	}

	printf("Initiating Mutex\n");
  pthread_mutex_init(&mutex, NULL);

	int j, c;
	for (j = 0; j < NUMTHRDS; j++) {
		printf("Starting Thread: %d\n", j);
		pthread_create(&pth[j], NULL, doWork, &j);
	}

  for (j = 0; j < 4; j++) {
  	pthread_join(pth[j],NULL);
  }
}
#include "CircularQueue.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

//------------------------------------------------------------------------------------------
// Allocates space for circular queue 'q' having 'capacity' number of elements
// Initializes semaphores & mutex needed to implement the producer-consumer paradigm
// Initializes indexes of the head and tail of the queue
int queue_init(CircularQueue** q, unsigned int capacity) {
	*q = (CircularQueue*) malloc(sizeof(CircularQueue));
	if (*q == NULL) {
		fprintf(stderr, "Error: Failed to allocate space for the circular queue: %s\n", strerror(errno));
		return -1;
	}

	if (sem_init(&((*q)->empty), 0, capacity) != 0) {
		fprintf(stderr, "Error: Failed to initialize empty semaphore: %s\n", strerror(errno));
		return -1;
	}

	if (sem_init(&((*q)->full), 0, 0) != 0) {
		fprintf(stderr, "Error: Failed to initialize full semaphore: %s\n", strerror(errno));
		return -1;
	}

	if (pthread_mutex_init(&((*q)->mutex), NULL) != 0) {
		fprintf(stderr, "Error: Failed to initialize circular queue mutex: %s\n", strerror(errno));
		return -1;
	}

	(*q)->v = (QueueElem*) malloc(capacity * sizeof(QueueElem));
	if ((*q)->v == NULL) {
		fprintf(stderr, "Error: Failed to allocate space for the circular queue buffer: %s\n", strerror(errno));
		return -1;
	}

	(*q)->capacity = capacity;
	(*q)->first = 0;
	(*q)->last = 0;

	return 0;
}

//------------------------------------------------------------------------------------------
// Inserts 'value' at the tail of queue 'q'
void queue_put(CircularQueue* q, QueueElem value) {
	if (sem_wait(&q->empty) != 0)
		fprintf(stderr, "Error: Failed when waiting for empty semaphore: %s\n", strerror(errno));
	if (pthread_mutex_lock(&q->mutex) != 0)
		fprintf(stderr, "Error: Failed when locking circular queue mutex: %s\n", strerror(errno));

	q->v[q->last] = value;
	// printf("Added %d to queue.\n", (int) q->v[q->last]);

	// updating tail position
	q->last++;
	if (q->last >= q->capacity)
		q->last = 0;

	if (pthread_mutex_unlock(&q->mutex) != 0)
		fprintf(stderr, "Error: Failed when unlocking circular queue mutex: %s\n", strerror(errno));
	if (sem_post(&q->full) != 0)
		fprintf(stderr, "Error: Failed when posting full semaphore: %s\n", strerror(errno));
}

//------------------------------------------------------------------------------------------
// Removes element at the head of queue 'q' and returns its 'value'
QueueElem queue_get(CircularQueue* q) {
	if (sem_wait(&q->full) != 0)
		fprintf(stderr, "Error: Failed when waiting for full semaphore: %s\n", strerror(errno));
	if (pthread_mutex_lock(&q->mutex) != 0)
		fprintf(stderr, "Error: Failed when locking circular queue mutex: %s\n", strerror(errno));

	QueueElem value = q->v[q->first];
	// printf("Removed %d from queue.\n", (int) q->v[q->first]);

	// updating head position
	q->first++;
	if (q->first >= q->capacity)
		q->first = 0;

	if (pthread_mutex_unlock(&q->mutex) != 0)
		fprintf(stderr, "Error: Failed when unlocking circular queue mutex: %s\n", strerror(errno));
	if (sem_post(&q->empty) != 0)
		fprintf(stderr, "Error: Failed when posting empty semaphore: %s\n", strerror(errno));

	return value;
}

//------------------------------------------------------------------------------------------
// Frees space allocated for the queue elements and auxiliary management data
// Must be called when the queue is no more needed
void queue_destroy(CircularQueue* q) {
	sem_destroy(&q->empty);
	sem_destroy(&q->full);
	pthread_mutex_destroy(&q->mutex);
	free(q->v);
	free(q);
}

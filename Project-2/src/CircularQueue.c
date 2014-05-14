#include "CircularQueue.h"

//------------------------------------------------------------------------------------------
// Allocates space for circular queue 'q' having 'capacity' number of elements
// Initializes semaphores & mutex needed to implement the producer-consumer paradigm
// Initializes indexes of the head and tail of the queue
int queue_init(CircularQueue** q, unsigned int capacity) {
	*q = (CircularQueue*) malloc(sizeof(CircularQueue));

	if (sem_init(&((*q)->empty), 0, capacity) != 0)
		return -1;

	if (sem_init(&((*q)->full), 0, 0) != 0)
		return -1;

	if (pthread_mutex_init(&((*q)->mutex), NULL) != 0)
		return -1;

	(*q)->v = (QueueElem*) malloc(capacity * sizeof(QueueElem));
	(*q)->capacity = capacity;
	(*q)->first = 0;
	(*q)->last = 0;

	return 0;
}

//------------------------------------------------------------------------------------------
// Inserts 'value' at the tail of queue 'q'
void queue_put(CircularQueue* q, QueueElem value) {
	sem_wait(&q->empty);
	pthread_mutex_lock(&q->mutex);

	q->v[q->last] = value;
	// printf("Added %d to queue.\n", (int) q->v[q->last]);

	// updating tail position
	q->last++;
	if (q->last >= q->capacity)
		q->last = 0;

	pthread_mutex_unlock(&q->mutex);
	sem_post(&q->full);
}

//------------------------------------------------------------------------------------------
// Removes element at the head of queue 'q' and returns its 'value'
QueueElem queue_get(CircularQueue* q) {
	sem_wait(&q->full);
	pthread_mutex_lock(&q->mutex);

	QueueElem value = q->v[q->first];
	// printf("Removed %d from queue.\n", (int) q->v[q->first]);

	// updating head position
	q->first++;
	if (q->first >= q->capacity)
		q->first = 0;

	pthread_mutex_unlock(&q->mutex);
	sem_post(&q->empty);

	return value;
}

//------------------------------------------------------------------------------------------
// Frees space allocated for the queue elements and auxiliary management data
// Must be called when the queue is no more needed
void queue_destroy(CircularQueue* q) {
	free(q->v);
	free(q);
}

#pragma once

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

//------------------------------------------------------------------------------------------
// Type of the circular queue elements
typedef unsigned long QueueElem;

//------------------------------------------------------------------------------------------
// Struct for representing a "circular queue"
// Space for the queue elements will be allocated dinamically by queue_init()
typedef struct {
	// pointer to the queue buffer
	QueueElem *v;

	// queue capacity
	unsigned int capacity;

	// head of the queue
	unsigned int first;

	// tail of the queue
	unsigned int last;

	// semaphores and mutex for implementing the
	// producer-consumer paradigm
	sem_t empty;
	sem_t full;
	pthread_mutex_t mutex;
} CircularQueue;

void queue_init(CircularQueue **q, unsigned int capacity);
void queue_put(CircularQueue *q, QueueElem value);
QueueElem queue_get(CircularQueue *q);
void queue_destroy(CircularQueue *q);

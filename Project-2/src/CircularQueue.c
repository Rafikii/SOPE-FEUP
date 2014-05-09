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

//------------------------------------------------------------------------------------------
// Allocates space for circular queue 'q' having 'capacity' number of elements
// Initializes semaphores & mutex needed to implement the producer-consumer paradigm
// Initializes indexes of the head and tail of the queue
// TO DO BY STUDENTS: ADD ERROR TESTS TO THE CALLS & RETURN a value INDICATING (UN)SUCESS
void queue_init(CircularQueue **q, unsigned int capacity) {
	*q = (CircularQueue *) malloc(sizeof(CircularQueue));
	sem_init(&((*q)->empty), 0, capacity);
	sem_init(&((*q)->full), 0, 0);
	pthread_mutex_init(&((*q)->mutex), NULL);
	(*q)->v = (QueueElem *) malloc(capacity * sizeof(QueueElem));
	(*q)->capacity = capacity;
	(*q)->first = 0;
	(*q)->last = 0;

	// TODO change return value
}

//------------------------------------------------------------------------------------------
// Inserts 'value' at the tail of queue 'q'
void queue_put(CircularQueue *q, QueueElem value) {
	// TODO BY STUDENTS
}

//------------------------------------------------------------------------------------------
// Removes element at the head of queue 'q' and returns its 'value'
QueueElem queue_get(CircularQueue *q) {
	// TODO BY STUDENTS
	return (QueueElem)NULL;
}

//------------------------------------------------------------------------------------------
// Frees space allocated for the queue elements and auxiliary management data
// Must be called when the queue is no more needed
void queue_destroy(CircularQueue *q) {
	// TODO BY STUDENTS
}

//==========================================================================================
// EXAMPLE: Creation of a circular queue using queue_init()
// TODO: test your program using different queue sizes
/*
#define QUEUE_SIZE 10
CircularQueue *q;
queue_init(&q,QUEUE_SIZE);
*/

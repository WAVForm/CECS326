/* myShm.h */
/* Header file to be used with master.c and slave.c
*/
#include <semaphore.h> //for sem_t

struct CLASS {
int index; // index to next available report slot
int report[10]; // each child writes its child number here
sem_t mutex_sem;
};
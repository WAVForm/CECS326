/* myShm.h */
/* Header file to be used with master.c and slave.c
*/
struct CLASS {
int index; // index to next available report slot
int report[10]; // each child writes its child number here
};
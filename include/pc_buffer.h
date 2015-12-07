#ifndef __PC_BUFFER_H__
#define __PC_BUFFER_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    volatile uint32_t produce_count;
    volatile uint32_t consume_count;
    uint16_t BUFFER_SIZE;
    volatile char *array;
} PC_Buffer ;

//*****************************************************************************
// Initializes the producer-consumer buffer structure
//*****************************************************************************
void pc_buffer_init(PC_Buffer *buffer, uint16_t buffer_size);

//*****************************************************************************
// Adds a character to the producer consumer buffer
//*****************************************************************************
void pc_buffer_add(PC_Buffer *buffer, char data);

//*****************************************************************************
// Removes a character from teh producer consumer buffer.  Returned in data
//*****************************************************************************
void pc_buffer_remove(PC_Buffer *buffer, char *data);

//*****************************************************************************
// Returns if the producer consumer buffer is empty
//*****************************************************************************
bool pc_buffer_empty(PC_Buffer *buffer);

//*****************************************************************************
// Returns if the producer consumer buffer is full
//*****************************************************************************
bool pc_buffer_full(PC_Buffer *buffer);

#endif

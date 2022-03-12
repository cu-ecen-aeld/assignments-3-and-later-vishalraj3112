/**
 * @file aesd-circular-buffer.c
 * @brief Functions and data related to a circular buffer imlementation
 *
 * @author Dan Walkes
 * @date 2020-03-01
 * @copyright Copyright (c) 2020
 *
 */

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

#include "aesd-circular-buffer.h"

/**
 * @param buffer the buffer to search for corresponding offset.  Any necessary locking must be performed by caller.
 * @param char_offset the position to search for in the buffer list, describing the zero referenced
 *      character index if all buffer strings were concatenated end to end
 * @param entry_offset_byte_rtn is a pointer specifying a location to store the byte of the returned aesd_buffer_entry
 *      buffptr member corresponding to char_offset.  This value is only set when a matching char_offset is found
 *      in aesd_buffer. 
 * @return the struct aesd_buffer_entry structure representing the position described by char_offset, or
 * NULL if this position is not available in the buffer (not enough data is written).
 */
struct aesd_buffer_entry *aesd_circular_buffer_find_entry_offset_for_fpos(struct aesd_circular_buffer *buffer,
			size_t char_offset, size_t *entry_offset_byte_rtn )
{
    /**
    * TODO: implement per description
    */


	size_t sum = 0, last_size = 0;
    //start with current read location
    uint8_t entry_idx = buffer->out_offs;

    //printf("out:%d in:%d\n",buffer->out_offs,buffer->in_offs);

    //iterate till in_idx
    do{
        
        sum += buffer->entry[entry_idx].size;
        last_size = buffer->entry[entry_idx].size;;
        
        //if found in 1st block
        if(char_offset <= (sum-1))
        {
            *entry_offset_byte_rtn = char_offset - (sum - last_size);
            return &buffer->entry[entry_idx];
        }

        entry_idx++;

        if(entry_idx == AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)
            entry_idx = 0;
            
    }while(entry_idx != buffer->in_offs);


    return NULL;
}

/**
* Adds entry @param add_entry to @param buffer in the location specified in buffer->in_offs.
* If the buffer was already full, overwrites the oldest entry and advances buffer->out_offs to the
* new start location.
* Any necessary locking must be handled by the caller
* Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
*/
const char* aesd_circular_buffer_add_entry(struct aesd_circular_buffer *buffer, const struct aesd_buffer_entry *add_entry)
{
    /**
    * TODO: implement per description 
    */

   const char* ret_ptr = NULL;

   //On entry check if buffer is full, then overwrite and increment out_offs
   if(buffer->full == true){
       //before overwriting, store the current location
       ret_ptr = buffer->entry[buffer->in_offs].buffptr;
       //add string and size data to the buffer(overwrite)
        buffer->entry[buffer->in_offs] = *(add_entry);
        //increment both the head and tail
        buffer->in_offs++;
        buffer->out_offs++;

        return;
   }
    
    //add string and size data to the buffer
    buffer->entry[buffer->in_offs] = *(add_entry);

    //increment the head
    buffer->in_offs++;

    //If in_offs reaches end, roll back(reset in_offs)
    if(buffer->in_offs == AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)
        buffer->in_offs = 0;

    //Check if full, i.e out_offs == in_offs
    if(buffer->in_offs == buffer->out_offs)
        buffer->full = true;
    else
        buffer->full = false;
}

/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    memset(buffer,0,sizeof(struct aesd_circular_buffer));
}

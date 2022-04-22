#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#define MAX_BUFF_SIZE   10

//in_offs = tail
//out_offs = head

typedef struct{
    int data[MAX_BUFF_SIZE];
    int out_offs;
    int in_offs;
    int *start;
    bool full;
}circular_buffer;

void push_buffer(circular_buffer* buff, int data){
    
    //Check for full buffer here
    if(buff->full == true){
        // printf("Buffer is full!\n");
        buff->data[buff->in_offs] = data;
        buff->in_offs++;
        buff->out_offs++;
        return;
    }
    
    buff->data[buff->in_offs] = data;
    buff->in_offs++; //careful with this
    
    //If in_offs reaches end, roll back
    if(buff->in_offs == MAX_BUFF_SIZE)
        buff->in_offs = 0;
    
    //Check if full,either out_offs == in_offs
    if(buff->in_offs == buff->out_offs)
        buff->full = true;
 
}

int pop_buffer(circular_buffer* buff){
    
    //If buffer is empty, send nothing
    if((buff->out_offs == buff->in_offs) && (!buff->full)){
        printf("buffer is empty cant pop!\n");
        return -1;
    }
    
    //get the value, increment the out_offs and unset the flag
    int ret_val = buff->data[buff->out_offs];
    buff->data[buff->out_offs] = 0;
    buff->out_offs++; //careful with this
    buff->full = false; //will be immediately unfulled if out_offs goes aout_offs
    
    //If out_offs reaches end, roll back
    if(buff->out_offs == MAX_BUFF_SIZE)
        buff->out_offs = 0;
        
    return ret_val;
    
}

void print_buffer(circular_buffer* buff){
    
    for(int i=0;i<MAX_BUFF_SIZE;i++){
        printf("%d ",buff->data[i]);
    }
    printf("\n");
}

int main(void)
{
    circular_buffer buffer;
    
    //initialize the buffer
    memset(&buffer,0,sizeof(circular_buffer));
    
    //initialize the buffer parameters
    buffer.full = false;
    
    //pop_buffer(&buffer);
    push_buffer(&buffer, 1);
    push_buffer(&buffer, 2);
    push_buffer(&buffer, 3);
    push_buffer(&buffer, 4);
    push_buffer(&buffer, 5);
    push_buffer(&buffer, 6);
    push_buffer(&buffer, 7);
    push_buffer(&buffer, 8);
    push_buffer(&buffer, 9);
    push_buffer(&buffer, 10);
    pop_buffer(&buffer);
    pop_buffer(&buffer);
    push_buffer(&buffer, 11);
    // push_buffer(&buffer, 12);
    push_buffer(&buffer, 13);
    push_buffer(&buffer, 14);
    push_buffer(&buffer, 15);
    
    
    print_buffer(&buffer);
    

    return 0;
}
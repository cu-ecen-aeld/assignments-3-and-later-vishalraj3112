#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    
    //get thread parameter
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    thread_func_args->thread_complete_success = true;
    
    //sleep
    int rc = usleep(thread_func_args->wait_to_obtain_ms*1000);
    if(rc == -1){
    	ERROR_LOG("usleep for mutex obtain failed");
    	thread_func_args->thread_complete_success = false;
    	return thread_param;
    }
    
    //lock
    rc = pthread_mutex_lock(thread_func_args->mutex_thread);
    if(rc !=0 ){
    	printf("pthread_mutex_lock failed with %d\n",rc);
    	ERROR_LOG("pthread_mutex_lock failed");
    	thread_func_args->thread_complete_success = false;
    	return thread_param;
    }
    
    //sleep
    rc = usleep(thread_func_args->wait_to_release_ms*1000);
    if(rc == -1){
    	ERROR_LOG("usleep for mutex release failed");
    	thread_func_args->thread_complete_success = false;
    	return thread_param;
    }
    
    rc = pthread_mutex_unlock(thread_func_args->mutex_thread);
    if(rc !=0 ){
    	printf("pthread_mutex_unlock failed with %d\n",rc);
    	ERROR_LOG("pthread_mutex_unlock failed");
    	thread_func_args->thread_complete_success = false;
    	return thread_param;
    }
    
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     * 
     * See implementation details in threading.h file comment block
     */
     
    
	struct thread_data* thread_data_params = (struct thread_data*) malloc(sizeof(struct thread_data));
	
	if(thread_data_params == NULL){
		ERROR_LOG("Malloc failed");
		return false;
	}
	
	//assign struct parameters
	thread_data_params->wait_to_obtain_ms = wait_to_obtain_ms;
	thread_data_params->wait_to_release_ms = wait_to_release_ms;
	thread_data_params->mutex_thread = mutex;
	
	int rc = pthread_create(thread, NULL, threadfunc,(void *) thread_data_params);
	
	if(rc != 0){
		ERROR_LOG("Thread creation failed");
		return false;
	}else{
		DEBUG_LOG("Thread created successfully");
		return true;
	}
	
	//check thread exit here and free the thread_data_params
	/*int *thread_ret_code;
	
	pthread_join(*thread,(void**)&thread_ret_code);
	if(*thread_ret_code == 0){
		DEBUG_LOG("Thread exit success");
		free(thread_data_params);
		return true;
	}else{
		ERROR_LOG("Thread exit faliure");
		free(thread_data_params);
		return false;
	}
	*/
	
	//return false;
}


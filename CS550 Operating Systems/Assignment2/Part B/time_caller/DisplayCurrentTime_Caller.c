#include <stdio.h>  //Required for std input and output
#include <stdlib.h> //Required for struct timespec
#include <unistd.h> //Required for syscall
#define __NR_sys_my_xtime 326 //Macro Identifier of syscall

int main()
{  
    //Current time will be stored in below structure element
    //Values are also initialized to zero
    struct timespec current_time = {0,0};
    //Status of system call will be stored in below variable
    int sys_status;
    
    //Make system call to display current time
    sys_status = syscall(__NR_sys_my_xtime,&current_time);
    
    //If return value is not zero then system call failed
    if(sys_status != 0)
    {
        printf("Error occurred while executing system call. Return Value: %d\n",sys_status); 
    }
    else
    {
        //Display current time
	printf("Current time is %10i.%09i\n",(int)current_time.tv_sec,(int)current_time.tv_nsec);
    }

    return 0;
}

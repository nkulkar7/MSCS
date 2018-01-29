#include<stdio.h>
#include<stdlib.h> //For struct timeval
#include<fcntl.h>  //For open()
#include<unistd.h> //For read()
#include<sys/time.h> //For gettimeofday()
#define N 5 //Total number of times the time will be fetched
int main()
{
    int fd,bytes_read,i;
    struct timeval gtodTimes[N];
    char *procClockTimes[N];

    //Allocate memory to the char buffer that will passed to read syscall
    for(i=0;i<N;i++)
    {
        procClockTimes[i] = (char *)malloc(100*sizeof(char *));
        if(procClockTimes[i] == NULL)
        {
            printf("Unable to allocate memory to character buffer\n");
            exit(1);
        }
    }
    
    //Use open syscall to open the file
    fd = open("/dev/mytime",O_RDONLY);
    if(fd < 0)
    {
        printf("Unable to open driver /dev/mytime\n");
        exit(1);
    }

    for(i=0;i<N;i++)
    {
        //gettimeofday() gives time in number of seconds and microseconds
        gettimeofday(&gtodTimes[i],0);

        //Use read syscall to read the file
        bytes_read = read(fd,procClockTimes[i],sizeof(procClockTimes[i]));
        if(bytes_read == -1)
        {
            printf("Unable to perform read operation\n");
            exit(1);
        }

     }

     //Use close syscall to close the file
     close(fd);

    for(i=0;i<N;i++)
    {
        //Print time obtained from function gettimeofday() and from driver /dev/mytime
        printf("gettimeofday: %i %i\n%s \n\n",(int)gtodTimes[i].tv_sec,(int)gtodTimes[i].tv_usec,procClockTimes[i]);
    }

    return 0;
}

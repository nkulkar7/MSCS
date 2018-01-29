#include<linux/kernel.h>
#include<linux/time.h>
#include<linux/errno.h>
#include<linux/syscalls.h>

int sys_my_xtime(struct timespec __user *current_time)
{
 //Declare xtime of type timespec
 struct timespec xtime;

 //Execute function current_kernel_time() to get current time
 xtime = current_kernel_time();

 //Copy current time (xtime) into user address space pointed by current_time
 //Return -EFAULT incase of error
 if(copy_to_user(current_time,&xtime,sizeof(xtime)))
 return -EFAULT;
 
 //Print Current Time to Console (Check Syslog or /var/log/messages or dmesg)
 printk(KERN_ALERT "Current Time is %10i.%09i\n",(int)xtime.tv_sec,(int)xtime.tv_nsec); 
 
 //return 0 on success
 return 0;
}

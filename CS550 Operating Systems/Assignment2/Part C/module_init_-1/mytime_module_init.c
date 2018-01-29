#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/miscdevice.h>
#include<linux/module.h>
#include<linux/string.h> //For strlen()
#include<linux/errno.h> //For EFAULT
#include<linux/syscalls.h> //For copy_to_user()
#include<linux/slab.h> //For kmalloc()

int myopen(struct inode *inode,struct file *fp)
{
    printk("In myopen\n");
    return 0;
}

int myclose(struct inode *inode,struct file *fp)
{
    printk("In myclose\n");
    return 0;
}

static ssize_t myread(struct file *file,char __user *out,size_t size,loff_t *off)
{
    struct timespec current_time,time_of_day;
    char timeString[100];
    current_time = current_kernel_time();
    getnstimeofday(&time_of_day);
    
    printk("In myread\n");

    //Store output of current_kernel_time() and getnstimeofday() in single string
    snprintf(timeString,sizeof(timeString),"current_kernel_time: %i %i \ngetnstimeofday: %i %i",(int)current_time.tv_sec,(int)current_time.tv_nsec,(int)time_of_day.tv_sec,(int)time_of_day.tv_nsec);

    //Copy string from kernel space into user address space 
    //Return -EFAULT incase of error
    if(copy_to_user(out,&timeString,sizeof(timeString)))
    {
       return -EFAULT;
    }

    //return bytes read
    return sizeof(timeString);
}

struct file_operations myfops = {
    .owner = THIS_MODULE,
    .open = myopen,
    .release = myclose,
    .read = myread,
};

static struct miscdevice mytime = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "mytime",
    .fops = &myfops,
};


static int __init char_dev_init(void)
{
    int dev_err;
    dev_err = misc_register(&mytime);
    if(dev_err)
    {
        printk(KERN_ALERT "Error in registering device mytime\n");  
        return dev_err;
    }
    printk("Device /dev/mytime initialized\n");
    return -1;
}

void __exit char_dev_exit(void)
{   
    misc_deregister(&mytime);
    printk("Exiting from device /dev/mytime\n");
}

MODULE_LICENSE("GPL");
module_init(char_dev_init);
module_exit(char_dev_exit);

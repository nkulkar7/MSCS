#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/miscdevice.h>
#include<linux/module.h>
#include<linux/string.h> //For strlen()
#include<linux/errno.h> //For EFAULT
#include<linux/syscalls.h> //For copy_to_user()
#include<linux/slab.h> //For kmalloc()
#include<linux/semaphore.h> //For semaphore related operations
#include<linux/mutex.h> //For mutex related operations

int buffer_size;
module_param(buffer_size,int,0000);
char **buffer;
int prod_buffer_size = 0;
int cons_buffer_size = 0;

static struct semaphore empty;
static struct semaphore full;
static struct mutex mutex;

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

static ssize_t myread(struct file *filp,char __user *user_buf,size_t size,loff_t *offset)
{
    int bytes_read=0;
    char read_char[50];
    printk("In myread\n");

    if(down_interruptible(&full)!=0)
    {
        printk("Error while executing down_interruptible\n");
        return -EPERM;
    }

    //Acquire lock
    if(mutex_lock_interruptible(&mutex)!=0)
    {
        printk("Error while executing mutex_lock_interruptible\n");
        return -EPERM;
    }
    
    //Reset consumer counter
    if(cons_buffer_size == buffer_size)
    {
        cons_buffer_size = 0;
    }
    
    //Critical section starts here
    sprintf(read_char,buffer[cons_buffer_size]);
    if(copy_to_user(user_buf,read_char,strlen(read_char)+1))
    {
        printk("Error in copy_to_user\n");		
        return -EFAULT;
    }
    
    //Release lock
    mutex_unlock(&mutex);
    bytes_read = strlen(read_char) + 1;
    //Critical section ends here
    up(&empty);
    
    cons_buffer_size++;
    return size;
}

static ssize_t mywrite(struct file *filp,const char *user_buf,size_t size,loff_t *offset)
{
    printk("In mywrite\n");

    //Perform semaphore down operation on empty
    if(down_interruptible(&empty)!=0)
    {
        printk("Error while executing down_interruptible\n");
        return -EPERM;
    }

    //Critical Section starts here
    //Acquire lock
    if(mutex_lock_interruptible(&mutex)!=0)
    {
        printk("Error while executing mutex_lock_interruptible\n");
        return -EPERM;
    }
    
    //Reset producer counter
    if(prod_buffer_size == buffer_size)
    {
       prod_buffer_size = 0;
    }

    buffer[prod_buffer_size] = kzalloc(sizeof(user_buf),GFP_KERNEL);
    if(!buffer[prod_buffer_size])
    {
        printk("Error in allocating memory\n");  
        return -ENOMEM;
    }

    if(copy_from_user(buffer[prod_buffer_size],user_buf,strlen(user_buf)+1)!=0)
    {
        printk("Error in copy_from_user\n");		
        return -EFAULT;
    }

    //Critical Section ends here
    //Release lock
    mutex_unlock(&mutex);
    
    //Perform semaphore up operation on full
    up(&full);
    prod_buffer_size++;
    return size;
}


struct file_operations myfops = {
    .owner = THIS_MODULE,
    .open = myopen,
    .release = myclose,
    .read = myread,
    .write = mywrite,
};

static struct miscdevice mypipe = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "numpipe",
    .fops = &myfops,
};


static int __init char_dev_init(void)
{
    int dev_err;
    dev_err = misc_register(&mypipe);
    if(dev_err)
    {
        printk("Error in registering device /dev/numpipe\n");  
        return dev_err;
    }
    buffer = (char**)kmalloc(buffer_size,GFP_KERNEL);
    if(!buffer)
    {
        printk("Error in allocating memory\n");  
        return -ENOMEM;
    }
    sema_init(&empty,buffer_size);
    sema_init(&full,0);
    mutex_init(&mutex);
    printk("Device /dev/numpipe initialized\n");

    return 0;
}

void __exit char_dev_exit(void)
{   
    misc_deregister(&mypipe);
    kfree(buffer);
    printk("Exiting from device /dev/numpipe\n");
}

MODULE_LICENSE("GPL");
module_init(char_dev_init);
module_exit(char_dev_exit);

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/semaphore.h>

dev_t dev;
int CHAR_DEV_open(struct inode *__inode, struct file *__file);
int CHAR_DEV_release(struct inode *__inode, struct file *__file);
ssize_t CHAR_DEV_read(struct file *__file, char *__ubuff, size_t __nbytes, loff_t *offset);
ssize_t CHAR_DEV_write(struct file *__file, const char *__ubuff, size_t __nbytes, loff_t *offset);


struct file_operations fops1 =
    {
        .owner = THIS_MODULE,
        .open = CHAR_DEV_open,
        .write = CHAR_DEV_write,
        .read = CHAR_DEV_read,
        .release = CHAR_DEV_release,
};
struct file_operations fops2 =
    {
        .owner = THIS_MODULE,
        .open = CHAR_DEV_open,
        .write = CHAR_DEV_write,
        .read = CHAR_DEV_read,
        .release = CHAR_DEV_release,
};

static struct cdev my_cdev[2];

static char *kbuff[50];

static struct semaphore *mysemaphore;

static int __init driver2_init(void)
{
int retval;
    int i = 0;

        if((alloc_chrdev_region(&dev, 0, 1, "Chardriver")) <0)
        {
                pr_info("Cannot allocate major number\n");
                return -1;
        }
        
	    int major = MAJOR(device_number);
	    dev_t my_device;
	    my_class = class_create(THIS_MODULE, "my_driver_class");
	    for (i = 0; i <2; i++) 
	    {
		    my_device = MKDEV(major, i);
		    cdev_init(&my_cdev[i], &fops);
		    retval = cdev_add(&my_cdev[i], my_device, 1);
		    if (retval==NULL) 
		    {
			   pr_info("Cannot create the Device 1\n");
		    }
		    else 
		    {
			    device_create(my_class, NULL, my_device, NULL, "my_device%d", i);
		    }
	    }
    sema_init(mysemaphore);

    PRINT("My Module init end");
    return 0;
}

static void MY_MODULE_exit(void)
{
	int i = 0;
	int major = MAJOR(device_number);
	dev_t my_device;
	for (i = 0; i < MAX_DEVICES; i++) 
	{
		my_device = MKDEV(major, i);
		cdev_del(&my_cdev[i]);
		device_destroy(my_class, my_device);
	}
	class_destroy(my_class);
	unregister_chrdev_region(device_number,2);
	pr_info("unregistered\n");
}

module_init(MY_MODULE_init);
module_exit(MY_MODULE_exit);

int CHAR_DEV_open(struct inode *__inode, struct file *__file)
{
    PRINT("CHARDEV_open");
    return 0;
}
int CHAR_DEV_release(struct inode *__inode, struct file *__file)
{
    PRINT("CHARDEV_release");
    return 0;
}

ssize_t CHAR_DEV_read(struct file *__file, char *__ubuff, size_t __nbytes, loff_t *offset)
{
    int numtoread, copyres;
    down(mysemaphore);
    PRINT("Char_dev_read lock\n")

    numtoread = __nbytes > KBUFFSIZE ? KBUFFSIZE : __nbytes;

    copyres = copy_to_user(__ubuff, kbuff, numtoread);

    if (copyres == 0)
    {
        PRINT("Copied %d bytes to user space", numtoread);
        return numtoread;
    }
    PRINT("Char_dev_read unlock\n")
    up(mysemaphore);
    return 0;
}
ssize_t CHAR_DEV_write(struct file *__file, const char *__ubuff, size_t __nbytes, loff_t *offset)
{
    int numtowrite, copyres;
    down(mysemaphore);
    PRINT("Char_dev_write lock\n")

    numtowrite = __nbytes > KBUFFSIZE ? KBUFFSIZE : __nbytes;

    copyres = copy_from_user(kbuff, __ubuff, numtowrite);

    if (copyres == 0)
    {
        PRINT("Copied %d bytes from user space to kernel space", numtowrite);
        return numtowrite;
    }
    PRINT("Char_dev_write unlocl\n")
    up(mysemaphore);
    return 0;
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("SAI SRUJAN");

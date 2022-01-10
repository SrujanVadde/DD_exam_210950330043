#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>               
#include<linux/uaccess.h>
#include <linux/ioctl.h>             
 
#define mem_size        1024         
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev driver1_cdev;
uint8_t *kernel_buffer;

#define GETSTATUS _IOR('a','b',int32_t*)


static int      __init driver1_init(void);
static void     __exit driver1_exit(void);
static int      driver1_open(struct inode *inode, struct file *file);
static int      driver1_release(struct inode *inode, struct file *file);
static ssize_t  driver1_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  driver1_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long     driver1_ioctl(struct file *file, unsigned int cmd, unsigned long arg);


static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = driver1_read,
        .write          = driver1_write,
        .open           = driver1_open,
        .release        = driver1_release,
};
 

static int driver1_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}


static int driver1_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}


static ssize_t driver1_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        if( copy_to_user(buf, kernel_buffer, mem_size) )
        {
                pr_err("Data Read : Err!\n");
        }
        pr_info("Data Read : Done!\n");
        return mem_size;
}


static ssize_t driver1_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        if( copy_from_user(kernel_buffer, buf, len) )
        {
                pr_err("Data Write : Err!\n");
        }
        pr_info("Data Write : Done!\n");
        return len;
}


static long driver1_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case GETSTATUS:
                         	 printk("Size of kernel buffer is: %d\n",sizeof(*kernel_buffer));
                         	 printk("The data in buffer is: %s\n ",*kernel_buffer);
                       	 break;
                default:
                        pr_info("Default\n");
                        break;
        }
        return 0;
}





static int __init driver1_init(void)
{
        if((alloc_chrdev_region(&dev, 0, 1, "Chardriver")) <0)
        {
                pr_info("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 

        cdev_init(&driver1_cdev,&fops);
 

        if((cdev_add(&driver1_cdev,dev,1)) < 0)
        {
            pr_info("Cannot add the device to the system\n");
            goto r_class;
        }
 

        if((dev_class = class_create(THIS_MODULE,"driver1_class")) == NULL)
        {
            pr_info("Cannot create the struct class\n");
            goto r_class;
        }
 

        if((device_create(dev_class,NULL,dev,NULL,"Mydriver1")) == NULL)
        {
            pr_info("Cannot create the Device 1\n");
            goto r_device;
        }
        

        if((kernel_buffer = kmalloc(mem_size , GFP_KERNEL)) == 0)
        {
            pr_info("Cannot allocate memory in kernel\n");
            goto r_device;
        }
        
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}


static void __exit driver1_exit(void)
{
        kfree(kernel_buffer);
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&driver1_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(driver1_init);
module_exit(driver1_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SRUJAN");

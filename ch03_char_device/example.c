/*************************************************************************
	> File Name: example.c
	> Author: leey.lee
	> 
	> Motto: Let the Bug come harder~
 ************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "my_char__dev"
static int major;
static int minor;
static struct cdev my_cdev;

static int my_open(struct inode *inode, struct file *file){
	pr_info("Device opened\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *file){
	pr_info("Device closed\n");
	return 0;
}

static ssize_t my_read(struct file *file, char __user *buf,
						size_t count, loff_t *ppos){
	pr_info("Device read\n");
	return 0;
}

static ssize_t my_write(struct file *file, const char __user *buf,
						size_t count, loff_t *ppos){
	pr_info("Device write\n");
	return count;
}

static struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
	.read = my_read,
	.write = my_write,
};

static int __init my_init(void){
	dev_t dev;
	int ret;
	ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
	if(ret < 0){
		pr_err("Failed to allocate char device region\n");
		return ret;
	}
	major = MAJOR(dev);
	pr_info("Allocated char device region with major %d\n", major);
	minor = MINOR(dev);
	pr_info("Allocated char device region with minor %d\n", minor);

	cdev_init(&my_cdev, &my_fops);
	my_cdev.owner = THIS_MODULE;
	ret = cdev_add(&my_cdev, dev, 1);
	if(ret < 0){
		pr_err("Failed to add char device\n");
		unregister_chrdev_region(dev, 1);
		return ret;
	}

	pr_info("Char device added successfully\n");
	return 0;
}

static void __exit my_exit(void){
	dev_t dev = MKDEV(major, 0);

	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("Char device unregistered successfully\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("leey.lee");
MODULE_DESCRIPTION("A simple char device example");
MODULE_VERSION("1.0");

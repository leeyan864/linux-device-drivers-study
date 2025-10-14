/*************************************************************************
	> File Name: example.c
	> Author: leey.lee
	> 
	> Motto: Let the Bug come harder~
 ************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "my_char_dev"

static int device_open_count = 0;

static int my_open(struct inode *inode, struct file *filp){
	if(device_open_count){
		pr_err("Device is already open");
		return -EBUSY;
	}

	++device_open_count;
	return 0;
}

static int my_release(struct inode *inode, struct file *filp){
	--device_open_count;
	pr_info("Device is release!");
	return 0;
}

struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
};

dev_t dev_no;

static int __init my_init(void){
	pr_info("cdev_no init Enter");
	int ret;
	ret = alloc_chrdev_region(&dev_no, 0, 1, DEVICE_NAME);
	if(ret < 0){
		pr_err("alloc cdev_no failed!");
		return ret;
	}
	pr_info("cdev_no init leave");
	return 0;
}

static void __exit my_exit(void){
	pr_info("my_exit Enter");
	unregister_chrdev_region(dev_no, 1);
	pr_info("my_exit leave");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("leey.lee");
MODULE_VERSION("1.0");
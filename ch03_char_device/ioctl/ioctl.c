/*************************************************************************
	> File Name: ioctl.c
	> Author: leey.lee
	> 
	> Motto: Let the Bug come harder~
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

// 幻数：用来标识设备的唯一标识符，防止命令冲突
#define MY_IOC_MAGIC 'k'

// 定义命令
#define MY_IOC_RESET _IO(MY_IOC_MAGIC, 0)
#define MY_IOC_GET_STATUS _IOR(MY_IOC_MAGIC, 1, int)
#define MY_IOC_SET_VALUE _IOW(MY_IOC_MAGIC, 2, int)

// 定义最大命令号 
#define MY_IOC_MAXNR 2

#define DEVICE_NAME "mydev"
static int major;
static struct cdev my_cdev;

// 示例设备操作函数
void dev_reset(void) {
	pr_info("Device reset\n");
}

int dev_get_status(void) {
    return 42; // 示例状态值
}

void dev_set_value(int val) {
	pr_info("Device value set to %d\n", val);
}

static long my_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	int ret = 0;
	int value;

	// 检查设备类型和命令号
	if (_IOC_TYPE(cmd) != MY_IOC_MAGIC) {
		return -ENOTTY; /* 不是我们的命令 */
	}
	if (_IOC_NR(cmd) > MY_IOC_MAXNR) {
		return -ENOTTY; /* 命令号超出范围 */
	}

	// 处理命令
	switch (cmd) {
		case MY_IOC_RESET:/* 重置设备 */
			dev_reset();
			break;
		case MY_IOC_GET_STATUS:/* 获取设备状态 */
			value = dev_get_status();
			if (copy_to_user((int __user *)arg, &value, sizeof(value))) {
				ret = -EFAULT;
			}
			break;
		case MY_IOC_SET_VALUE:/* 设置设备值 */
			if (copy_from_user(&value, (int __user *)arg, sizeof(value))) {
				ret = -EFAULT;
				break;
			}
			dev_set_value(value);
			break;
		default:
			ret = -ENOTTY; /* 未知命令 */
			break;
	}
	return ret;
}

static int my_open(struct inode *inode, struct file *file) {
	pr_info("Enter my_open\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) {
	pr_info("Enter my_release\n");
    return 0;
}

static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = my_ioctl,
	.release = my_release,
	.open = my_open,
};

static int __init my_init(void) {
	pr_info("Enter myioctl my_init\n");
    dev_t dev;
    int ret;

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) return ret;

    major = MAJOR(dev);
    cdev_init(&my_cdev, &my_fops);
    my_cdev.owner = THIS_MODULE;

    ret = cdev_add(&my_cdev, dev, 1);
    if (ret < 0) return ret;

	pr_info("Module loaded with major %d\n",major);
    return 0;
}

static void __exit my_exit(void) {
	pr_info("Enter myioctl my_exit\n");
    cdev_del(&my_cdev);
    unregister_chrdev_region(MKDEV(major, 0), 1);
	pr_info("Module my_ioctl unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("leey.lee");
MODULE_DESCRIPTION("IOCTL Example Module");

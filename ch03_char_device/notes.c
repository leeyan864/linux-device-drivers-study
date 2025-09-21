/*************************************************************************
	> File Name: virtual_led.c
	> Author: leey.lee
	> 
	> Motto: Let the Bug come harder~
 ************************************************************************/
/*
1. 注册字符设备驱动
2. 实现文件操作函数
3. 模块初始化和退出函数
4. 设备树绑定（可选）
5. Sysfs和Proc接口（可选）
6. 并发控制（可选）
7. 错误处理（可选）
8. 日志记录（可选）
9. 模块信息（可选）
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

// 定义设备名
#define CHAR_DEV_NAME "virtual_led"

// 定义设备号
// 静态分配设备号
#define CHAR_DEV_MAJOR 240
static int __init mydev_init(void){
    int ret;
    dev_t devno = MKDEV(CHAR_DEV_MAJOR,0);

    ret = register_chrdev_region(devno,1,CHAR_DEV_NAME);
    if(ret < 0)}{
        pr_err("Failed to register char device region\n");
        return ret;
    }
    return 0;
}
// 动态分配设备号
static int __init mydev_init(void){
    int ret;
    dev_t devno;

    ret = alloc_chrdev_region(&devno,0,1,CHAR_DEV_NAME);
    if(ret<0){
        pr_err("Failed to allocate char device region\n");
        return ret;
    }
    return 0;
}

// 释放设备号
static void __exit mydev_exit(void){
    unregister_chrdev_region(devno, 1);
    printk(KERN_INFO "Unregistered mydev\n");
}

// file_operations是字符设备驱动的核心结构体，定义了各种操作的函数指针
static struct file_operations mydev_fops = {
    .owner = THIS_MODULE,
    .open = mydev_open,
    .release = mydev_release,
    .read = mydev_read,
    .write - mydev_write,
    .unlocked_ioctl = mydev_ioctl,
}

struct file_operations {
    loff_t (*llseek) (struct file *, loff_t, int);
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    int (*open) (struct inode *, struct file *);
    int (*release) (struct inode *, struct file *);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
    // 其他操作...
};




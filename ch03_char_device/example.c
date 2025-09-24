/*************************************************************************
	> File Name: example.c
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
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "my_char__dev"
static int major;
static int minor;
static struct cdev my_cdev;

struct int __init my_init(void){
    dev_t dev_no;
    int ret;

    ret = alloc_chrdev_region(&dev_no, 0, 1, DEVICE_NAME);
    if(ret < 0){
        pr_info("alloc_chrdev_region failed!");
        return ret;
    }

    major = MAJOR(dev_no);
    pr_info("allocated major number %d",major);
    
    // 初始化设备
    cdev_init(&my_cdev,&my_fops);
    ret = cdev_add(&my_cdev,dev,1);
    if(ret < 0){
        pr_info("cdev_add failed!");
        return ret;
    }

    pr_info("my_init leave");
    return 0;
}














/*************************************************************************
	> File Name: virtual_led.c
	> Author: leey.lee
	> 
	> Motto: Let the Bug come harder~
 ************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>

// 自己定义的名字
#define DEVICE_NAME "vled"


// 自己写的操作函数，但是要符合规范
static int vled_open(struct inode *inode, struct file *filp){
    pr_info("vled device opened\n");
    return 0;
}

static int vled_release(struct inode *inode, struct file *filp){
    pr_info("vled device closed\n");
    return 0;
}

static ssize_t vled_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos){
    char status = '1'; 
    // 把内核空间的数据拷贝到用户空间
    // buf是用户空间的地址，&status是内核空间的地址, 1是拷贝的字节数
    if (copy_to_user(buf, &status, 1)){
        return -EFAULT;
    }
    return 1;
}

static ssize_t vled_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos){
    char val;
    // 把用户空间的数据拷贝到内核空间
    // buf是用户空间的地址，&val是内核空间的地址, 1是拷贝的字节数
    if (copy_from_user(&val, buf, 1)){
        return -EFAULT;
    }

    pr_info("LED set to %c\n", val);
    return 1;
}

// 定义文件操作结构体,固定写法，把上面的函数关联起来
static struct file_operations vled_fops = {
    // 这里指定是保护“函数指针表”的生命周期,当内核调用的时候，增加引用计数，防止模块被卸载
    .owner = THIS_MODULE,
    .open = vled_open,
    .release = vled_release,
    .read = vled_read,
    .write = vled_write,
};

// 主设备号
static int major = 0;
// 字符设备结构体
static struct cdev vled_cdev;

static int __init vled_init(void){
    dev_t devno;
    int ret;
    // 动态分配主设备号
    ret = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to allocate char device region\n");
        return ret;
    }
    // 获取分配的主设备号
    major = MAJOR(devno);

    // 初始化字符设备
    cdev_init(&vled_cdev, &vled_fops);
    // 这里为什么还要加owner呢？
    // 因为这里指定是“字符设备对象”的生命周期,与上面的file_operations结构体的owner不同
    vled_cdev.owner = THIS_MODULE;

    // 添加字符设备到系统
    ret = cdev_add(&vled_cdev, devno, 1);
    if (ret) {
        pr_err("Failed to add cdev\n");
        // 释放设备号
        unregister_chrdev_region(devno, 1);
        return ret;
    }
    pr_info("vled driver loaded with major %d\n", major);
    return 0;
}

static void __exit vled_exit(void){
    // 就是一个宏，来把主设备号和次设备号合成一个dev_t类型的变量
    dev_t devno = MKDEV(major, 0);
    // 删除字符设备
    cdev_del(&vled_cdev);
    // 释放设备号
    unregister_chrdev_region(devno, 1);
    pr_info("vled driver unloaded\n");
}

module_init(vled_init);
module_exit(vled_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("leey.lee");
MODULE_DESCRIPTION("Virtual LED Driver");

// // 设备树绑定
// static const struct of_device_id vled_of_match[] = {
//     { .compatible = "mycompany,vled" },
//     { },
// };
// MODULE_DEVICE_TABLE(of, vled_of_match);
// static int vled_probe(struct platform_device *pdev)
// {
//     struct device *dev = &pdev->dev;
//     if (!dev->of_node) {
//         dev_err(dev, "No device tree data available\n");
//         return -EINVAL;
//     }
//     dev_info(dev, "vled device probed\n");
//     return 0;
// }
// static int vled_remove(struct platform_device *pdev)
// {
//     dev_info(&pdev->dev, "vled device removed\n");
//     return 0;
// }
// static struct platform_driver vled_driver = {
//     .driver = {
//         .name = "vled",
//         .of_match_table = vled_of_match,
//     },
//     .probe = vled_probe,
//     .remove = vled_remove,
// };
// // 在模块初始化函数中注册平台驱动
// platform_driver_register(&vled_driver);
// // 在模块退出函数中注销平台驱动
// platform_driver_unregister(&vled_driver);
// // 设备树节点示例
// vled: vled@0 {
//     compatible = "mycompany,vled";
//     status = "okay";
// };


// // Sysfs和Proc接口
// // Sysfs属性实现
// static ssize_t status_show(struct device *dev,
// struct device_attribute *attr, char *buf)
// {
//     return sprintf(buf, "1\n");
// }
// static ssize_t status_store(struct device *dev,
// struct device_attribute *attr,
// const char *buf, size_t count)
// {
//     unsigned long val;
    
//     int ret = kstrtoul(buf, 10, &val);
//     if (ret){
//         return ret;
//     }

//     pr_info("LED set via sysfs to %lu\n", val);
//     return count;
// }

// static DEVICE_ATTR_RW(status);
// // 在probe函数中添加属性
// device_create_file(&pdev->dev, &dev_attr_status);
// // Proc文件系统接口
// static int vled_proc_show(struct seq_file *m, void *v)
// {
//     seq_printf(m, "Virtual LED Driver Status\n");
//     seq_printf(m, "========================\n");
//     seq_printf(m, "Current state: 1\n");
//     return 0;
// }
// static int vled_proc_open(struct inode *inode, struct file *file)
// {
//     return single_open(file, vled_proc_show, NULL);
// }
// static const struct proc_ops vled_proc_fops = {
//     .proc_open = vled_proc_open,
//     .proc_read = seq_read,
//     .proc_lseek = seq_lseek,
//     .proc_release = single_release,
// };
// // 在模块初始化中创建proc文件
// proc_create("vled_status", 0, NULL, &vled_proc_fops);

// // 并发控制
// static DEFINE_MUTEX(vled_lock);
// static int led_status = 0;
// static ssize_t vled_write(struct file *filp, const char __user *buf,
// size_t count, loff_t *f_pos)
// {
//     char val;
//     if (copy_from_user(&val, buf, 1)){
//         return -EFAULT;
//     }

//     mutex_lock(&vled_lock);

//     if (val == '0'){
//         led_status = 0;
//     }else if (val == '1'){
//         led_status = 1;
//     }

//     pr_info("LED set to %d\n", led_status);

//     mutex_unlock(&vled_lock);
//     return 1;
// }


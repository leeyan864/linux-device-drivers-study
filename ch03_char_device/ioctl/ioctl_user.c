/*************************************************************************
	> File Name: ioctl_user.c
	> Author: leey.lee
	> 
	> Motto: Let the Bug come harder~
 ************************************************************************/

#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>

/* 定义与内核相同的命令号 */
#define MY_IOC_MAGIC 'k'
#define MY_IOC_RESET _IO(MY_IOC_MAGIC, 0)
#define MY_IOC_GET_STATUS _IOR(MY_IOC_MAGIC, 1, int)
#define MY_IOC_SET_VALUE _IOW(MY_IOC_MAGIC, 2, int)
int main(){
	int fd = open("/dev/example", O_RDWR);
	if (fd < 0) {
		perror("open");
		return -1;
	}

	/* 重置设备 */
	if (ioctl(fd, MY_IOC_RESET) < 0) {
		perror("ioctl reset");
	}
	/* 设置设备值 */
	int value = 100;
	if (ioctl(fd, MY_IOC_SET_VALUE, &value) < 0) {
		perror("ioctl set value");
	}
	/* 获取设备状态 */
	int status;
	if (ioctl(fd, MY_IOC_GET_STATUS, &status) < 0) {
		perror("ioctl get status");
	} else {
		printf("Device status: %d\n", status);
	}
	close(fd);
	return 0;
}

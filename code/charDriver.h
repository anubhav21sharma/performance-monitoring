/*
 * charDriver.h
 *
 *  Created on: 19-Apr-2016
 *      Author: anubhav
 */

#ifndef CHARDRIVER_H_
#define CHARDRIVER_H_

#include <asm/uaccess.h>

int device_open(struct inode *inode, struct file *filp) {
//	printk(KERN_WARNING "Call: device_open\n");
	return 0;
}

int device_close(struct inode *inode, struct file *filp) {
//	printk(KERN_WARNING "Call: device_close\n");
	return 0;
}

long device_read(struct file *filep, char *buffer, size_t len, loff_t *offset);

struct file_operations fops = { .open = device_open, .release = device_close, .read = device_read, };

#endif /* CHARDRIVER_H_ */

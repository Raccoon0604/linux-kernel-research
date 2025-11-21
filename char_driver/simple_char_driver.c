#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/printk.h>
//设备配置宏定义
#define DEVICE_NAME "simple_char_dev"
#define CLASS_NAME "simple_char"
#define BUFFER_SIZE 1024
//模块元信息
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple character device driver");
//全局变量定义
static int major_number;
static struct class *char_class = NULL;
static struct device *char_device = NULL;
static struct cdev char_cdev;
static char *device_buffer;
static int buffer_pointer;

/**
 * device_open - 设备打开函数
 * @inodep: inode结构指针
 * @filep: file结构指针
 * 
 * 返回值: 成功返回0
 * 
 * 当用户空间程序打开设备文件时调用此函数
 */
static int device_open(struct inode *inodep, struct file *filep)
{
    pr_info("simple_char_driver: Device opened\n");
    return 0;
}

/**
 * device_release - 设备释放函数
 * @inodep: inode结构指针
 * @filep: file结构指针
 * 
 * 返回值: 成功返回0
 * 
 * 当最后一个进程关闭设备文件时调用此函数
 */
static int device_release(struct inode *inodep, struct file *filep)
{
    pr_info("simple_char_driver: Device closed\n");
    return 0;
}

/**
 * device_read - 从设备读取数据到用户空间
 * @filep: file结构指针
 * @buffer: 用户空间缓冲区（数据将复制到这里）
 * @len: 要读取的字节数
 * @offset: 当前文件位置
 * 
 * 返回值: 成功返回读取的字节数，失败返回负的错误码
 * 
 * 此函数将数据从内核缓冲区复制到用户空间
 * 处理文件位置，确保不会读取超过缓冲区末尾
 */
static ssize_t device_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int bytes_to_read;
    // 检查是否已经读到缓冲区末尾 
    if (*offset >= buffer_pointer) {
        return 0;
    }
     // 计算实际可以读取的字节数
    bytes_to_read = (buffer_pointer - *offset < len) ? 
                   (buffer_pointer - *offset) : len;
     // 将数据从内核空间复制到用户空间
    if (copy_to_user(buffer, device_buffer + *offset, bytes_to_read)) {
        return -EFAULT;
    }
    // 更新文件位置并返回实际读取的字节数
    *offset += bytes_to_read;
    pr_info("simple_char_driver: Read %d bytes\n", bytes_to_read);
    return bytes_to_read;
}

/**
 * device_write - 从用户空间写入数据到设备
 * @filep: file结构指针
 * @buffer: 用户空间缓冲区（包含要写入的数据）
 * @len: 要写入的字节数
 * @offset: 当前文件位置（在这个简单驱动中未使用）
 * 
 * 返回值: 成功返回写入的字节数，失败返回负的错误码
 * 
 * 此函数将数据从用户空间复制到内核缓冲区
 * 处理缓冲区溢出保护
 */

static ssize_t device_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    int bytes_to_write;
    // 检查缓冲区是否已满 
    if (buffer_pointer >= BUFFER_SIZE) {
        return -ENOSPC;
    }
    // 计算实际可以写入的字节数
    bytes_to_write = (BUFFER_SIZE - buffer_pointer < len) ? 
                    (BUFFER_SIZE - buffer_pointer) : len;
    // 将数据从用户空间复制到内核空间
    if (copy_from_user(device_buffer + buffer_pointer, buffer, bytes_to_write)) {
        return -EFAULT;
    }
     // 更新缓冲区位置并返回实际写入的字节数
    buffer_pointer += bytes_to_write;
    pr_info("simple_char_driver: Wrote %d bytes\n", bytes_to_write);
    return bytes_to_write;
}

static struct file_operations fops = {
    .open = device_open,
    .read = device_read,
    .write = device_write,
    .release = device_release,
};

/**
 * simple_char_driver_init - 驱动初始化函数
 * 
 * 返回值: 成功返回0，失败返回负的错误码
 * 
 * 当使用insmod加载模块时调用此函数
 * 它执行所有必要的设置：
 * 1. 为缓冲区分配内存
 * 2. 注册字符设备
 * 3. 创建设备类和设备节点
 * 4. 初始化cdev结构体
 */
static int __init simple_char_driver_init(void)
{
    pr_info("simple_char_driver: Initializing driver\n");
    
    // 分配缓冲区
    device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!device_buffer) {
        pr_err("Failed to allocate buffer\n");
        return -ENOMEM;
    }
    buffer_pointer = 0;
    
    // 注册字符设备
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_err("Failed to register char device\n");
        kfree(device_buffer);
        return major_number;
    }
    
    // 创建设备类
    char_class = class_create(CLASS_NAME);
    if (IS_ERR(char_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        kfree(device_buffer);
        return PTR_ERR(char_class);
    }
    
    // 创建设备节点
    char_device = device_create(char_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(char_device)) {
        class_destroy(char_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        kfree(device_buffer);
        return PTR_ERR(char_device);
    }
    
    // 初始化cdev
    cdev_init(&char_cdev, &fops);
    if (cdev_add(&char_cdev, MKDEV(major_number, 0), 1) < 0) {
        device_destroy(char_class, MKDEV(major_number, 0));
        class_destroy(char_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        kfree(device_buffer);
        return -1;
    }
    
    pr_info("simple_char_driver: Driver loaded successfully\n");
    return 0;
}

/**
 * simple_char_driver_exit - 驱动清理函数
 * 
 * 当使用rmmod卸载模块时调用此函数
 * 它按初始化顺序的逆序执行所有必要的清理：
 * 1. 移除cdev结构体
 * 2. 销毁设备节点
 * 3. 销毁设备类
 * 4. 注销字符设备
 * 5. 释放缓冲区内存
 */
static void __exit simple_char_driver_exit(void)
{
    cdev_del(&char_cdev);
    device_destroy(char_class, MKDEV(major_number, 0));
    class_destroy(char_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    kfree(device_buffer);
    pr_info("simple_char_driver: Driver unloaded\n");
}
// 注册初始化和清理函数
module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);

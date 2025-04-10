#include<linux/module.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<asm/uaccess.h>
#include<linux/delay.h>

static int major_num;
static int device_open_count = 0;
static char* msg_buffer = "this is a demo";
static char* msg_ptr;

static ssize_t device_read(struct file *flip, char __user *buffer, size_t len, loff_t *offset) {
  int bytes_read = 0;
  mdelay(12000);
  while(len) {
    //If we’re at the end, loop back to the beginning
    if (*msg_ptr == 0) {
      msg_ptr = msg_buffer;
    }
    //buffer is in user data, not kernel, so you can’t just reference
    //with a pointer. The function put_user handles this for us */
    put_user(*(msg_ptr++), buffer++);
    len--;
    bytes_read++;
  }
  return bytes_read;
}

static ssize_t device_write(struct file *flip, const char __user *buffer, size_t len, loff_t *offset) {
  printk(KERN_ALERT "This operation is not supported.\n");
  return -EINVAL;
}

//Called when a process opens our device
static int device_open(struct inode *inode, struct file *file) {
  //if device is open, return busy
  if (device_open_count) {
    return -EBUSY;
  }
  device_open_count++;
  try_module_get(THIS_MODULE);
  return 0;
}

//called when a process closes our device
static int device_release(struct inode *inode, struct file *file) {
  //decrement the open counter and usage count. Without this, the module would not unload
  device_open_count--;
  module_put(THIS_MODULE);
  return 0;
}

static struct file_operations file_ops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

static int __init demo_init(void) {
  msg_ptr = msg_buffer;
  //try to register character device
  major_num = register_chrdev(0, "demo", &file_ops);
  if (major_num < 0) {
    printk(KERN_ALERT "Could not register device: %d\n", major_num);
    return major_num;
  } else {
    printk(KERN_INFO "demo module loaded with device major number %d\n", major_num);
    return 0;
  }
}

static void __exit demo_exit(void) {
  //Remember — we have to clean up after ourselves. Unregister the character device. */
  unregister_chrdev(major_num, "demo");
  printk(KERN_INFO "Goodbye, World!\n");
}

//register module functions 
module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");
/*
 * dm510_dev.c - DM510 Linux Kernel Module
 */
#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <asm/system.h>
#include <asm/switch_to.h>
#include <linux/rwsem.h>
#include "dm510.h"

// Module info
MODULE_LICENSE("GPL");
MODULE_AUTHOR("...Karsten F. Pedersen.");
MODULE_DESCRIPTION("DM510 Driver");

// Module variables
// static dev_t dev;
char *device_name = DEVICE_NAME;
int major = MAJOR;
int minor = MINOR;
int nr_devs = NR_DEVS;

// __initdata

static struct dm510_dev *devices;

static struct file_operations fops = {
  .owner          = THIS_MODULE,
  .read           = dm510_read,
  .write          = dm510_write,
  .open           = dm510_open,
  .release        = dm510_release,
  .unlocked_ioctl = dm510_ioctl
};

static int __init dm510_init_module(void) {
  // Register range of device numbers
  int result;
  dev_t dev = 0;
  if (major) {
    dev = MKDEV(major, minor);
    result = register_chrdev_region(dev, nr_devs, device_name);
  } else {
    result = alloc_chrdev_region(&dev, minor, nr_devs, device_name);
    major = MAJOR(dev);
  }
  if (result < 0) {
    pr_warn("Unable to get dm510 region, error %d\n", result);
    return result;
  }
  dev = MKDEV(major, minor + nr_devs);

  // Setup devices
  devices = kmalloc(sizeof(struct dm510_dev) * nr_devices, GFP_KERNEL);
  if (devices == NULL) {
    pr_err("DM510: Failed to init module, cleaning...!\n");
    dm510_cleanup_module();
    return -NOMEM;
  }
  memset(devices, 0, nr_devs * sizeof(struct dm510_dev));

  // Initialize individual devices
  for (int i = 0; i < nr_devs; i++) {
    struct dm510_dev *device = &devices[i];
    device->
    // Setup semaphore
    // Should be setup before dm510_setup_cdev
    // sema_init(&dev->sem, 1);
    init_rwsem(&device->sem);
    // deprecated: DECLARE_MUTEX(&dev->sem);
    dm510_setup_cdev(device, i);
  }

  pr_info("DM510: Hello from your device!\n");
  return SUCCESS;
}

static void __exit dm510_cleanup_module(void) {
  // dev_t dev = MKDEV(major, MIN_MINOR_NUMBER);
  // int result = unregister_chrdev_region(dev, NR_DEVS);
  // if (reslt < 0) {
  //   pr_alert("Failed to unregister: %d!", result);
  // }

  printk(KERN_INFO "DM510: Module unloaded.\n");
}

/* Called when a process tries to open the device file */
static int dm510_open(struct inode *inode, struct file *filp) {
  struct dm510_dev *dev;

  // container_of is an macro
  dev = container_of(inode->i_cdev, struct dm510_dev, cdev);
  filp->private_data = dev; // Easier access in the future

  if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
    // scull_trim(dev);
  }

  return SUCCESS;
  // TODO: Check filp->f_mode for FMODE_READ and FMODE_WRITE
  // Get major and minor from inode
  // unsigned int imajor = imajor(inode);
  // unsigned int iminor = iminor(inode);
  // Dont't use i_rdev directly
  // if (iminor < minor || iminor >= nr_devs) {
  //   return -1;
  // } else if (devices[iminor] == 0) {
  //   return -1;
  // }
}

/* Called when a process closes the device file. */
static int dm510_release(struct inode *inode, struct file *filp) {
  /* device release code belongs here */
 // Scull does not have any code here
  return SUCCESS;
}

/* Called when a process, which already opened the dev file, attempts to read
 * from it. */
static ssize_t dm510_read(struct file *filp, char __user *buffer, size_t count, loff_t *offp) {
  // CHECK IF BINARY SEMAPHORE FOR READING IS BEING USED

  struct dm510_dev *dev = filp->private_data;

  if (down_read(&dev->sem) != 0) {
    return -1; // TODO: Find error code
  }
  int bytes_read = 0;
  // copy_to_user(buffer, ??, count);
  


  up_read(&dev->sem);
  return bytes_read;
}

/* Called when a process writes to dev file */
static ssize_t dm510_write(struct file *filp,
                           const char __user *buffer, /* The buffer to get data from */
                           size_t count,  /* The max number of bytes to write */
                           loff_t *offp) /* The offset in the file           */
  // Get device from file pointer
  struct dm510_dev *dev = filp->private_data;

  // BINARY SEMAPHORE: WAIT
  if (down_write(&dev->sem) != 0) {
    return -ERESTARTSYS; // IDK
  }

  /* write code belongs here */
  // critical section
  // int result = copy_from_user(??, buffer, count);
  // if (result < 0) {
  //   return -EFAULT;
  // }

  up_write(&dev->sem);
  return 0; // return number of bytes written
}

/* called by system call icotl */
long dm510_ioctl(struct file *filp,
                 unsigned int cmd,  /* command passed from the user */
                 unsigned long arg) /* argument of the command */
{
  // Check command

  // TODO: Check filp->f_mode for FMODE_READ and FMODE_WRITE
  /* ioctl code belongs here */
  printk(KERN_INFO "DM510: ioctl called.\n");

  return 0; // has to be changed
}

module_init(dm510_init_module);
module_exit(dm510_cleanup_module);

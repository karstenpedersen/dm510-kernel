#include <linux/cdev.h> // Used for registrering Char Device
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <asm/system.h>
#include <asm/switch_to.h>
#include "dm510.h"

int dm510_setup_cdev(dm510_dev *dev, int index) {
  // TODO: Think about how to get major
  int devno = MKDEV(major, MINOR + index);

  // Setup cdev structure
  cdev_init(&dev->cdev, &fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = fops;

  // Add cdev
  int err = cdev_add(&dev->cdev, devno, 1);
  if (err < 0) {
    pr_err("Error %d adding dm510 device %d", err, index);
    return -1;
  }

  return SUCCESS;
}

void dm510_del_cdev(dm510_dev *dev) {
  cdev_del(dev->cdev);
}


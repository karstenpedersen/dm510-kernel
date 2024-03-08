#ifndef _DM510_H_

#define DEVICE_NAME "dm510_dev"
#define MAJOR 0
#define MINOR 0
#define NR_DEVS 2

// Custom return codes
#define SUCCESS 0

struct dm510_dev {
  dev_t dev;
  struct cdev cdev;
  // struct semaphore sem;
  struct rw_semaphore sem;
  unsigned long size;
};

// Functions
static int dm510_open(struct inode*, struct file*);
static int dm510_release(struct inode*, struct file*);
static ssize_t dm510_read(struct file*, char*, size_t, loff_t*);
static ssize_t dm510_write(struct file*, const char*, size_t, loff_t*);
long dm510_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

int dm510_setup_cdev(dm510_dev *dev, int index);
int dm510_clean_cdev(dm510_dev *dev);

#endif

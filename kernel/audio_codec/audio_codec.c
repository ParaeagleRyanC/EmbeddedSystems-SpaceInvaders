#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ryan Chiang");
MODULE_DESCRIPTION("ECEn 427 Audio Driver");

#define MODULE_NAME "audio"
#define STATUS_REG 0x10

////////////////////////////////////////////////////////////////////////////////
/////////////////////// Device Struct //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// This struct contains all variables for an individual audio device. Although
// this driver will only support one device, it is good practice to structure
// device-specific variables this way. That way if you were to extend your
// driver to multiple devices, you could simply have an array of these device
// structs.

struct audio_device {
  int minor_num; // Device minor number
  struct cdev cdev; // Character device structure
  struct platform_device *pdev; // Platform device pointer
  struct device *dev; // device (/dev)
  phys_addr_t phys_addr; // Physical address
  u32 mem_size; // Allocated mem space size
  u32 *virt_addr; // Virtual address

  // Add any device-specific items to this that you need
  int kernelBuffer[128000]; // Declare a static array of 512KB
};

////////////////////////////////////////////////////////////////////////////////
/////////////////////// Driver Globals /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// This section is used to store any driver-level variables (ie. variables for
// the entire driver, not per device)

// Major number to use for devices managed by this driver
static int major_num;

// The audio device - since this driver only supports one device, we don't
// need a list here, we can just use a single struct.
static struct audio_device audio;

static int irq_number;

////////////////////////////////////////////////////////////////////////////////
/////////////////////// Forward function declarations //////////////////////////
////////////////////////////////////////////////////////////////////////////////
static int audio_init(void);
static void audio_exit(void);
static int audio_probe(struct platform_device *pdev);
static int audio_remove(struct platform_device *pdev);

static ssize_t audio_read (struct file* file, char __user* user, size_t size, loff_t* offset);
static ssize_t audio_write (struct file* file, const char __user* user, size_t size, loff_t* offset);
static irqreturn_t my_isr(int irq, void *dev_id);
static void reg_write(int offset, int val);
static ssize_t reg_read(int offset);

////////////////////////////////////////////////////////////////////////////////
/////////////////////// Driver Functions ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static struct class *myAudioClass;

static struct of_device_id audio_xillybus_of_match[] = {
  { .compatible = "byu,ecen427-audio_codec", },
  {}
};

MODULE_DEVICE_TABLE(of, audio_xillybus_of_match);

static struct platform_driver audioPlatformDriver = {
  .driver = {
    .name = "audio_codec",
    .owner = THIS_MODULE,
    .of_match_table = audio_xillybus_of_match
  },
  .probe = audio_probe,
  .remove = audio_remove
};

static const struct file_operations fops = {
  .read = audio_read,
  .write = audio_write,
  .owner = THIS_MODULE
};

// This section contains driver-level functions. Remember, when you print
// logging information from these functions, you should use the pr_*() functions
// (ie. pr_info, pr_warning, pr_err, etc.)

// Register driver init/exit with Linux
module_init(audio_init);
module_exit(audio_exit);

// This is called when Linux loads your driver
static int audio_init(void) {
  pr_info("%s: Initializing Audio Driver!\n", MODULE_NAME);

  // Get a major number for the driver -- alloc_chrdev_region; // pg. 45, LDD3.
  dev_t devValue;
  int allocFailed = alloc_chrdev_region(&devValue, 0, 1, MODULE_NAME);
  // handle error
  if (allocFailed) {
    pr_err("%s: Failed to alloc_chrdev_region!\n", MODULE_NAME);
    return -1;
  }

  major_num = MAJOR(devValue);
  pr_info("%s: Major Number is: %d\n", MODULE_NAME, major_num);
  audio.minor_num = MINOR(devValue);
  pr_info("%s: Minor Number is: %d\n", MODULE_NAME, audio.minor_num);

  // Create a device class. -- class_create()
  myAudioClass = class_create(THIS_MODULE, MODULE_NAME);

  // handle error
  if (IS_ERR(myAudioClass)) {
    pr_err("%s: Failed to class_create!\n", MODULE_NAME);
    unregister_chrdev_region(devValue, 1);
    return -1;
  }

  // Register the driver as a platform driver -- platform_driver_register
  int registerFailed = platform_driver_register(&audioPlatformDriver);

  // handle error
  if (registerFailed) {
    pr_err("%s: Failed to platform_driver_register!\n", MODULE_NAME);
    class_destroy(myAudioClass);
    unregister_chrdev_region(devValue, 1);
    return -1;
  }

  // Success
  return 0; 
}

// This is called when Linux unloads your driver
static void audio_exit(void) {
  pr_info("%s: Removing Audio Driver!\n", MODULE_NAME);

  // platform_driver_unregister
  platform_driver_unregister(&audioPlatformDriver);

  // class_destroy
  class_destroy(myAudioClass);

  // unregister_chrdev_region
  unregister_chrdev_region(MKDEV(major_num, audio.minor_num), 1);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////// Device Functions ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// This section contains device-level functions. Remember, when you print
// logging information from these functions, you should use the dev_*()
// functions
// (ie. dev_info, dev_warning, dev_err, etc.)

// Called by kernel when a platform device is detected that matches the
// 'compatible' name of this driver.
static int audio_probe(struct platform_device *pdev) {

  // Initialize the character device structure (cdev_init)
  cdev_init(&audio.cdev, &fops);

  // Register the character device with Linux (cdev_add)
  int addFailed = cdev_add(&audio.cdev, MKDEV(major_num, audio.minor_num), 1);

  // handle error
  if (addFailed) {
    pr_err("%s: Failed to cdev_add!\n", MODULE_NAME);
    return -1;
  }

  pr_info("%s: Device added!\n", MODULE_NAME);

  // Create a device file in /dev so that the character device can be accessed
  // from user space (device_create).
  struct device* deviceCreate = device_create(myAudioClass, NULL, MKDEV(major_num, audio.minor_num), NULL, "audio_codec");

  // handle error
  if (IS_ERR(deviceCreate)) {
    pr_err("%s: Failed to device_create!\n", MODULE_NAME);
    cdev_del(&audio.cdev);
    return -1;
  }
  pr_info("%s: Device created!\n", MODULE_NAME);

  // Get the physical device address from the device tree -- platform_get_resource
  struct resource* memResource = platform_get_resource(pdev, IORESOURCE_MEM, 0);

  // handle error
  if (!memResource) {
    pr_err("%s: Failed to platform_get_resource on MEM!\n", MODULE_NAME);
    device_destroy(myAudioClass, MKDEV(major_num, audio.minor_num));
    cdev_del(&audio.cdev);
    return -1;
  }
  audio.phys_addr = memResource->start;
  audio.mem_size = memResource->end - memResource->start + 1; 
  pr_info("%s: Got platform resource on MEM!\n", MODULE_NAME);
  pr_info("%s: Physical start address is: %x\n", MODULE_NAME, audio.phys_addr);
  //pr_info("%s: Physical end address is: %p\n", MODULE_NAME, &memResource->end);

  // Reserve the memory region -- request_mem_region
  struct resource* memRegionResource = request_mem_region(audio.phys_addr, audio.mem_size, MODULE_NAME); // might return somthing else

  // handle error
  if (!memRegionResource) {
    pr_err("%s: Failed to request_mem_region!\n", MODULE_NAME);
    device_destroy(myAudioClass, MKDEV(major_num, audio.minor_num));
    cdev_del(&audio.cdev);
    return -1;
  }
  pr_info("%s: Mem region requested!\n", MODULE_NAME);

  // Get a (virtual memory) pointer to the device -- ioremap
  audio.virt_addr = ioremap(audio.phys_addr, audio.mem_size);
   
  // handle error
  if (!audio.virt_addr) {
    pr_err("%s: Failed to ioremap!\n", MODULE_NAME);
    release_mem_region(audio.phys_addr, audio.mem_size);
    device_destroy(myAudioClass, MKDEV(major_num, audio.minor_num));
    cdev_del(&audio.cdev);
    return -1;
  }
  pr_info("%s: Virtual memory pointer obtained!\n", MODULE_NAME);
  pr_info("%s: Virtual address is: %p\n", MODULE_NAME, audio.virt_addr);

  // Get the IRQ number from the device tree -- platform_get_resource
  struct resource* irqResource = platform_get_resource(pdev, IORESOURCE_IRQ, 0);

  // handle error
  if (!irqResource) {
    pr_err("%s: Failed to platform_get_resource on IRQ!\n", MODULE_NAME);
    iounmap(audio.virt_addr);
    release_mem_region(audio.phys_addr, audio.mem_size);
    device_destroy(myAudioClass, MKDEV(major_num, audio.minor_num));
    cdev_del(&audio.cdev);
    return -1;
  }
  irq_number = irqResource->start;
  pr_info("%s: Got platform resource on IRQ!\n", MODULE_NAME);

  // Register your interrupt service routine -- request_irq
  int requestIrqFailed = request_irq(irq_number, my_isr, 0, MODULE_NAME, NULL);

  // handle error
  if (requestIrqFailed) {
    pr_err("%s: Failed to request_irq!\n", MODULE_NAME);
    iounmap(audio.virt_addr);
    release_mem_region(audio.phys_addr, audio.mem_size);
    device_destroy(myAudioClass, MKDEV(major_num, audio.minor_num));
    cdev_del(&audio.cdev);
    return -1;
  }
  pr_info("%s: IRS registered for IRQ %d!\n", MODULE_NAME, irq_number);
  
  // enables interrupt
  reg_write(STATUS_REG, 0x01);

  // success 
  return 0; 
}

// Called when the platform device is removed
static int audio_remove(struct platform_device *pdev) {
  pr_info("%s: Entered audio_remove!\n", MODULE_NAME);

  // free_irq
  free_irq(irq_number, NULL);
  pr_info("%s: IRQ freed!\n", MODULE_NAME);

  // iounmap
  iounmap(audio.virt_addr);
  pr_info("%s: Memory unmapped!\n", MODULE_NAME);

  // release_mem_region
  release_mem_region(audio.phys_addr, audio.mem_size);
  pr_info("%s: Mem region released!\n", MODULE_NAME);

  // device_destroy
  device_destroy(myAudioClass, MKDEV(major_num, audio.minor_num));
  pr_info("%s: Device destroyed!\n", MODULE_NAME);

  // cdev_del
  cdev_del(&audio.cdev);
  pr_info("%s: Device removed!\n", MODULE_NAME);
  return 0;
}

static ssize_t audio_read (struct file* file, char __user* user, size_t size, loff_t* offset) {
  pr_info("%s: Entered audio_read!\n", MODULE_NAME);
  
  // return one byte of data, with value 0 or 1, 
  // indicating whether an audio sample is currently being played.
  return copy_to_user(user, kernelBuffer, size);
}

static ssize_t audio_write (struct file* file, const char __user* user, size_t size, loff_t* offset) {
  pr_info("%s: Entered audio_write!\n", MODULE_NAME);

  // disable the interrupt output of the audio core
  reg_write(STATUS_REG, 0x0);

  // Copy the audio data from user space to your buffer (including safety checks on the user space pointer)
  unsigned long bytes_copied = copy_from_user(kernelBuffer, user, size);
  if (bytes_copied != 0) return -EFAULT;

  // enable the interrupt output of the audio core
  reg_write(STATUS_REG, 0x1);
  return size;
}

// ISR (Interrupt Service Routine) function
static irqreturn_t my_isr(int irq, void *dev_id) {




    // Once the end of the audio clip is reached, disable interrupts on the audio core.
    // do some check first
    reg_write(STATUS_REG, 0x0); // add check

    // print a message to the kernel log 
    pr_info("%s: IRQ Handled!\n", MODULE_NAME);
    return IRQ_HANDLED;
}


static void reg_write(int offset, int val) {
  pr_info("%s: Writing to register!\n", MODULE_NAME);
  iowrite32(val, audio.virt_addr + offset / sizeof(u32));
}


static ssize_t reg_read(int offset) {
  pr_info("%s: Reading from register!\n", MODULE_NAME);
  return ioread32(audio.virt_addr + offset / sizeof(u32));
}
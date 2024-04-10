#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ryan Chiang");
MODULE_DESCRIPTION("ECEn 427 PIT");

#define MODULE_NAME "pit"
#define PIT_CONTROL_OFFSET 0x00
#define PIT_DELAY_OFFSET 0x04

#define BUFFER_SIZE 24

struct kobject* ko; // for sysfs 

static phys_addr_t phys_addr; // Physical address
static u32 mem_size; // Allocated mem space size
static u32 *virt_addr; // Virtual address


////////////////////////////////////////////////////////////////////////////////
/////////////////////// Forward function declarations //////////////////////////
////////////////////////////////////////////////////////////////////////////////
static int pit_init(void);
static void pit_exit(void);
static int pit_probe(struct platform_device *pdev);
static int pit_remove(struct platform_device *pdev);
static void reg_write(int offset, int val);
static ssize_t reg_read(int offset);

////////////////////////////////////////////////////////////////////////////////
/////////////////////// Driver Functions ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static struct of_device_id pit_xillybus_of_match[] = {
  { .compatible = "byu,ecen427-pit", },
  {}
};

MODULE_DEVICE_TABLE(of, pit_xillybus_of_match);

static struct platform_driver pitPlatformDriver = {
  .driver = {
    .name = "pit",
    .owner = THIS_MODULE,
    .of_match_table = pit_xillybus_of_match
  },
  .probe = pit_probe,
  .remove = pit_remove
};


// This section contains driver-level functions. Remember, when you print
// logging information from these functions, you should use the pr_*() functions
// (ie. pr_info, pr_warning, pr_err, etc.)

// Register driver init/exit with Linux
module_init(pit_init);
module_exit(pit_exit);

// This is called when Linux loads your driver
static int pit_init(void) {
  pr_info("%s: Initializing PIT Driver!\n", MODULE_NAME);

  // Register the driver as a platform driver -- platform_driver_register
  int registerFailed = platform_driver_register(&pitPlatformDriver);

  // handle error
  if (registerFailed) {
    pr_err("%s: Failed to platform_driver_register!\n", MODULE_NAME);
    return -1;
  }

  // Success
  return 0; 
}

// This is called when Linux unloads your driver
static void pit_exit(void) {
  pr_info("%s: Removing PIT Driver!\n", MODULE_NAME);

  // platform_driver_unregister
  platform_driver_unregister(&pitPlatformDriver);
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
static int pit_probe(struct platform_device *pdev) {

  // Get the physical device address from the device tree -- platform_get_resource
  struct resource* memResource = platform_get_resource(pdev, IORESOURCE_MEM, 0);

  // handle error
  if (!memResource) {
    pr_err("%s: Failed to platform_get_resource on MEM!\n", MODULE_NAME);
    return -1;
  }
  phys_addr = memResource->start;
  mem_size = memResource->end - memResource->start + 1; 
  pr_info("%s: Got platform resource on MEM!\n", MODULE_NAME);
  pr_info("%s: Physical start address is: %x\n", MODULE_NAME, phys_addr);

  // Reserve the memory region -- request_mem_region
  struct resource* memRegionResource = request_mem_region(phys_addr, mem_size, MODULE_NAME); // might return somthing else

  // handle error
  if (!memRegionResource) {
    pr_err("%s: Failed to request_mem_region!\n", MODULE_NAME);
    return -1;
  }
  pr_info("%s: Mem region requested!\n", MODULE_NAME);

  // Get a (virtual memory) pointer to the device -- ioremap
  virt_addr = ioremap(phys_addr, mem_size);
   
  // handle error
  if (!virt_addr) {
    pr_err("%s: Failed to ioremap!\n", MODULE_NAME);
    release_mem_region(phys_addr, mem_size);
    return -1;
  }
  pr_info("%s: Virtual memory pointer obtained!\n", MODULE_NAME);
  pr_info("%s: Virtual address is: %p\n", MODULE_NAME, virt_addr);

  // sysfs
  kobject = &pdev->dev.kobj;

  // write to delay register
  reg_write(PIT_DELAY_OFFSET, 50000);
  reg_write(PIT_CONTROL_OFFSET, 0x3);

  // success 
  return 0; 
}

// Called when the platform device is removed
static int pit_remove(struct platform_device *pdev) {
  pr_info("%s: Entered pit_remove!\n", MODULE_NAME);

  // iounmap
  iounmap(virt_addr);
  pr_info("%s: Memory unmapped!\n", MODULE_NAME);

  // release_mem_region
  release_mem_region(phys_addr, mem_size);
  pr_info("%s: Mem region released!\n", MODULE_NAME);

  return 0;
}

static void reg_write(int offset, int val) {
  // pr_info("%s: Writing to register!\n", MODULE_NAME);
  iowrite32(val, virt_addr + offset / sizeof(u32));
}


static ssize_t reg_read(int offset) {
  pr_info("%s: Reading from register!\n", MODULE_NAME);
  return ioread32(virt_addr + offset / sizeof(u32));
}

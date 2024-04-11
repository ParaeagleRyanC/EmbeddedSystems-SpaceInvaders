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
#define DEFAULT_DELAY_VALUE 1000000
#define RUN_AND_ENABLED 0x03
#define RUN 0x01
#define ENABLED 0x02
#define BUFFER_SIZE 24
#define NUM_FILES 4
#define BASE_TEN 10

struct kobject* ko; // for sysfs 
struct attribute* attributeBundle[NUM_FILES];
struct device_attribute period;
struct device_attribute int_en;
struct device_attribute run;
struct attribute_group attributeGroup;

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
static ssize_t show_period(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t store_period(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);
static ssize_t show_int_en(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t store_int_en(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);
static ssize_t show_run(struct device* dev, struct device_attribute* attr, char* buf);
static ssize_t store_run(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);

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
  ko = &pdev->dev.kobj;
  
  period.attr.name = "period"; 
  period.attr.mode = 0666; 		// Read/Write 
  period.show = show_period;   	// Read Function 
  period.store = store_period;  // Write function 
  attributeBundle[0] = &period.attr; // add to budle
  
  int_en.attr.name = "int_en"; 
  int_en.attr.mode = 0666; 		// Read/Write 
  int_en.show = show_int_en;   	// Read Function 
  int_en.store = store_int_en;  // Write function 
  attributeBundle[1] = &int_en.attr; // add to budle
  
  run.attr.name = "run"; 
  run.attr.mode = 0666; 		// Read/Write 
  run.show = show_run;   		// Read Function 
  run.store = store_run;  		// Write function 
  attributeBundle[2] = &run.attr; // add to budle
  
  attributeBundle[3] = NULL; // indicate end of bundle
  attributeGroup.attrs = attributeBundle; // set bundle to group
  
  // Registering a Group of Attributes
  int registerGroupFailed = sysfs_create_group(ko, &attributeGroup);
  
  // handle error
  if (registerGroupFailed) {
    pr_err("%s: Failed to sysfs_create_group!\n", MODULE_NAME);
    sysfs_remove_group(ko, &attributeGroup);
    iounmap(virt_addr);
    release_mem_region(phys_addr, mem_size);
    return -1;
  }
  
  // registered successfully, write to control and delay register
  reg_write(PIT_DELAY_OFFSET, DEFAULT_DELAY_VALUE);
  reg_write(PIT_CONTROL_OFFSET, RUN_AND_ENABLED);

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

// read period
static ssize_t show_period(struct device* dev, struct device_attribute* attr, char* buf) {
	u32 period = reg_read(PIT_DELAY_OFFSET) / 100;
  return scnprintf(buf, BUFFER_SIZE, "%u\n", period);
}

// write period
static ssize_t store_period(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) {
  // simple_strtoul convert string to unsigned long, (start of string, end of string, base 10)
	u32 period = simple_strtoul(buf, NULL, BASE_TEN) * 100; 
  reg_write(PIT_DELAY_OFFSET, period);
  return count;
}

// read interrupt enable
static ssize_t show_int_en(struct device* dev, struct device_attribute* attr, char* buf) {
	u32 enabled = (reg_read(PIT_CONTROL_OFFSET) & ENABLED) != 0;
  return scnprintf(buf, BUFFER_SIZE, "%u\n", enabled);
}

// write interrupt enable
static ssize_t store_int_en(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) {
	// simple_strtoul convert string to unsigned long, (start of string, end of string, base 10)
	u32 enabled = simple_strtoul(buf, NULL, BASE_TEN); 
  reg_write(PIT_CONTROL_OFFSET, enabled);
  return count;
}

// read run (start/stop timer)
static ssize_t show_run(struct device* dev, struct device_attribute* attr, char* buf) {
	u32 running = (reg_read(PIT_CONTROL_OFFSET) & RUN) != 0;
  return scnprintf(buf, BUFFER_SIZE, "%u\n", running);
}

// write run (start/stop timer)
static ssize_t store_run(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) {
	// simple_strtoul convert string to unsigned long, (start of string, end of string, base 10)
	u32 running = simple_strtoul(buf, NULL, BASE_TEN); 
  reg_write(PIT_CONTROL_OFFSET, running);
  return count;
}

#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ryan Chiang");
MODULE_DESCRIPTION("ECEn 427 Audio Driver");

#define MODULE_NAME "audio"

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

////////////////////////////////////////////////////////////////////////////////
/////////////////////// Forward function declarations //////////////////////////
////////////////////////////////////////////////////////////////////////////////
static int audio_init(void);
static void audio_exit(void);
static int audio_probe(struct platform_device *pdev);
static int audio_remove(struct platform_device *pdev);

////////////////////////////////////////////////////////////////////////////////
/////////////////////// Driver Functions ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static struct class *myAudioClass;

static struct of_device_id audio_xillybus_of_match[] {
{ .compatible = "byu,ecen427-audio_codec", },
{}
};

MODULE_DEVICE_TABLE(of, audio_xillybus_of_match);

static struct platform_driver audioPlatformDriver {
.driver = {
.name = "audio_codec",
.owner = THIS_MODULE,
.of_match_table = audio_xillybus_of_match
},
.probe = audio_probe,
.remove = audio_remove
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
int allocSuccess = alloc_chrdev_region(&devValue, 0, 1, MODULE_NAME);
// handle error
if (allocSuccess) {
pr_err("%s: Failed to alloc_chrdev_region!\n", MODULE_NAME);
return -1;
}

major_num = MAJOR(devValue);

// Create a device class. -- class_create()
myAudioClass = class_create(THIS_MODULE, MODULE_NAME);

// handle error
if (IS_ERR(myAudioClass)) {
pr_err("%s: Failed to class_create!\n", MODULE_NAME);
unregister_chrdev_region(devValue, 1);
return -1;
}

// Register the driver as a platform driver -- platform_driver_register
int registerSuccess = platform_driver_register(&audioPlatformDriver);

// handle error
if (registerSuccess) {
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
// Register the character device with Linux (cdev_add)

// Create a device file in /dev so that the character device can be accessed
// from user space (device_create).

// Get the physical device address from the device tree --
// platform_get_resource
// Reserve the memory region -- request_mem_region
// Get a (virtual memory) pointer to the device -- ioremap

// Get the IRQ number from the device tree -- platform_get_resource
// Register your interrupt service routine -- request_irq

// If any of the above functions fail, return an appropriate linux error code,
// and make sure
// you reverse any function calls that were successful.

return 0; //(success)
}

// Called when the platform device is removed
static int audio_remove(struct platform_device *pdev) {
// free_irq
// iounmap
// release_mem_region
// device_destroy
// cdev_del
return 0;
}


/*
   Kernel module for GPIO  interrupt
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h> // Required for the GPIO functions
#include <linux/interrupt.h>
#include <linux/unistd.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

static unsigned int gpio1 = 60; // pin number p9_12 on beaglebone black
static unsigned int irq1;       // for storing the interrupt value

static irq_handler_t gpio_irq_handler_rising_edge(unsigned int irq, void *dev_id, struct pt_regs *regs) // for handling interrupt
{
    char path[] = "/home/debian/Biometric_crypto/keys/key";
    char data[] = "000 - invalid key - 000";
    struct file *fileHandler;
    ssize_t err;
    size_t count = sizeof(data);
    loff_t pos = 0;

    printk(KERN_INFO "GPIO Interruption - TAMPER PROOF");
    fileHandler = filp_open(path, O_RDWR | O_CREAT, 0644);
    if (IS_ERR(fileHandler))
    {
        printk(KERN_INFO "input file open error/n");
        return -1;
    }
    pos = 0;
    err = kernel_write(fileHandler, data, count, &pos);
    printk(KERN_INFO "kernel_write: %d", err);
    filp_close(fileHandler, NULL);

    return (irq_handler_t)IRQ_HANDLED;
}

static int __init gpiointerrupt_init(void)
{
    int result1;
    gpio_request(gpio1, "sysfs");  // Set up the gpioButton
    gpio_direction_input(gpio1);   // Set the button GPIO to be an input
    gpio_set_debounce(gpio1, 200); // Debounce the button with a delay of 200ms
    gpio_export(gpio1, false);     // Causes gpio115 to appear in /sys/class/gpio
    irq1 = gpio_to_irq(gpio1);     // for interrupt request

    result1 = request_irq(irq1,                                        // The interrupt number requested
                          (irq_handler_t)gpio_irq_handler_rising_edge, // The pointer to the handler function below
                          IRQF_TRIGGER_RISING,                         // Interrupt on rising edge (button press, not release)
                          "ebb_gpio_handler",                          // Used in /proc/interrupts to identify the owner
                          NULL);                                       // The *dev_id for shared interrupt lines, NULL is okay

    return 0;
}

static void __exit gpiointerrupt_exit(void)
{

    printk(KERN_INFO "exiting gpiointerrupt module\n");
    free_irq(irq1, NULL); // Free the IRQ number, no *dev_id required in this case
    gpio_unexport(gpio1); // Unexport the Button GPIO
    gpio_free(gpio1);
}

module_init(gpiointerrupt_init);
module_exit(gpiointerrupt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sumik");

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#define BUTTON_GPIO 117   // 根据实际GPIO引脚修改
#define BUTTON_NAME "my_button"

static int irq_number;

// 中断处理函数
static irqreturn_t button_interrupt(int irq, void *dev_id)
{
    printk(KERN_INFO "Button pressed! State: %d\n", gpio_get_value(BUTTON_GPIO));
    return IRQ_HANDLED;
}

static int __init button_init(void)
{
    int ret;
    
    // 申请GPIO资源
    if (!gpio_is_valid(BUTTON_GPIO)) {
        printk(KERN_ERR "Invalid GPIO %d\n", BUTTON_GPIO);
        return -ENODEV;
    }
    
    if ((ret = gpio_request(BUTTON_GPIO, BUTTON_NAME))) {
        printk(KERN_ERR "GPIO %d request failed: %d\n", BUTTON_GPIO, ret);
        return ret;
    }
    
    // 配置为输入模式
    if ((ret = gpio_direction_input(BUTTON_GPIO))) {
        printk(KERN_ERR "GPIO %d direction set failed: %d\n", BUTTON_GPIO, ret);
        gpio_free(BUTTON_GPIO);
        return ret;
    }
    
    // 获取中断号
    irq_number = gpio_to_irq(BUTTON_GPIO);
    if (irq_number < 0) {
        printk(KERN_ERR "GPIO to IRQ failed: %d\n", irq_number);
        gpio_free(BUTTON_GPIO);
        return irq_number;
    }
	else {
		printk(KERN_INFO "IRQ= %d\n", irq_number);
	}
    
    // 注册中断处理程序（下降沿触发）
    if ((ret = request_irq(irq_number, button_interrupt, 
                          IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, 
                          BUTTON_NAME, NULL))) {
        printk(KERN_ERR "IRQ request failed: %d\n", ret);
        gpio_free(BUTTON_GPIO);
        return ret;
    }
    
    printk(KERN_INFO "Button driver loaded\n");
    return 0;
}

static void __exit button_exit(void)
{
    free_irq(irq_number, NULL);
    gpio_free(BUTTON_GPIO);
    printk(KERN_INFO "Button driver unloaded\n");
}

module_init(button_init);
module_exit(button_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Advanced Button Interrupt Driver");

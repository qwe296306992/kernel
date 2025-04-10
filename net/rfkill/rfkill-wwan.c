/*
 * Copyright (C) 2012 ALIENTEK, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/* rfkill driver for wwan-5G Module
 *
*/

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/rfkill.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/spinlock.h>


struct rfkill_wwan_data {
	spinlock_t			lock;
	struct rfkill		*rfkill_dev;
	struct gpio_desc	*reset_gpio; //复位引脚
	struct gpio_desc	*power_gpio; //电源引脚
	struct gpio_desc	*disable_gpio;//飞行模式控制引脚
	int					power;
	int 				disable;
};
static struct rfkill_wwan_data *g_rfkill = NULL;

static ssize_t wwan_power_show(struct class *cls, struct class_attribute *attr,
			char *_buf)
{
	int state;

	spin_lock(&g_rfkill->lock);		//上锁
	state = g_rfkill->power;
	spin_unlock(&g_rfkill->lock);	//解锁

	if (state)
		return sprintf(_buf, "%s\n", "on");
	else
		return sprintf(_buf, "%s\n", "off");
}

static ssize_t wwan_power_store(struct class *cls, struct class_attribute *attr,
			const char *_buf, size_t _count)
{
	long poweren = 0;

	if (kstrtol(_buf, 10, &poweren) < 0)
		return -EINVAL;

	spin_lock(&g_rfkill->lock);

	if (poweren > 0) {
		gpiod_set_value_cansleep(g_rfkill->power_gpio, 1);	//拉高开机
		g_rfkill->power = 1;
	} else {
		gpiod_set_value_cansleep(g_rfkill->power_gpio, 0);	//拉低关机
		g_rfkill->power = 0;
	}

	spin_unlock(&g_rfkill->lock);

	return _count;
}
static CLASS_ATTR_RW(wwan_power);//创建可读可写的属性

static ssize_t wwan_reset_store(struct class *cls, struct class_attribute *attr,
			const char *_buf, size_t _count)
{
	long reset = 0;

	if (kstrtol(_buf, 10, &reset) < 0)
		return -EINVAL;

	if (reset > 0) {
		gpiod_set_value_cansleep(g_rfkill->reset_gpio, 1);	//拉高复位
		msleep(260);	//260ms 根据官方文档说明需要 250~600ms
		gpiod_set_value_cansleep(g_rfkill->reset_gpio, 0);	//结束复位
	}

	return _count;
}
static CLASS_ATTR_WO(wwan_reset);

static ssize_t wwan_disable_show(struct class *cls, struct class_attribute *attr,
			char *_buf)
{
	int state;

	spin_lock(&g_rfkill->lock);	//上锁
	state = g_rfkill->disable;
	spin_unlock(&g_rfkill->lock);//解锁

	if (state)
		return sprintf(_buf, "%s\n", "on");
	else
		return sprintf(_buf, "%s\n", "off");
}

static ssize_t wwan_disable_store(struct class *cls, struct class_attribute *attr,
			const char *_buf, size_t _count)
{
	long disable = 0;

	if (kstrtol(_buf, 10, &disable) < 0)
		return -EINVAL;

	spin_lock(&g_rfkill->lock);	//上锁

	if (disable > 0) {
		gpiod_set_value_cansleep(g_rfkill->disable_gpio, 0);	//拉低进入飞行模式
		g_rfkill->disable = 1;
	} else {
		gpiod_set_value_cansleep(g_rfkill->disable_gpio, 1);	//拉高退出飞行模式
		g_rfkill->disable = 0;
	}

	spin_unlock(&g_rfkill->lock);//解锁

	return _count;
}
static CLASS_ATTR_RW(wwan_disable);

static struct attribute *wwan_5g_attrs[] = {
	&class_attr_wwan_power.attr,
	&class_attr_wwan_reset.attr,
	&class_attr_wwan_disable.attr,
	NULL,
};
ATTRIBUTE_GROUPS(wwan_5g);//wwan_5g_groups

static struct class wwan_5g_cls = {
	.name			= "wwan_5g",
	.class_groups	= wwan_5g_groups,
};

static int rfkill_wwan_set_power(void *data, bool blocked)
{
	struct rfkill_wwan_data *rfkill = (struct rfkill_wwan_data *)data;
	int state;

	spin_lock(&rfkill->lock);	//上锁
	state = rfkill->power;
	spin_unlock(&rfkill->lock);//解锁

	if (!blocked) {		//打开电源
		if (state == 0) {
			gpiod_set_value_cansleep(rfkill->reset_gpio, 1);	//拉高复位
			msleep(260);	//260ms
			gpiod_set_value_cansleep(rfkill->reset_gpio, 0);	//结束复位
			msleep(30);		//10ms

			gpiod_set_value_cansleep(rfkill->power_gpio, 1);	//拉高开机
			state = 1;
		}
	} else {		//关闭电源
		if (state == 1) {
			gpiod_set_value_cansleep(rfkill->power_gpio, 0);	//拉低关机
			state = 0;
		}
	}

	spin_lock(&rfkill->lock);
	rfkill->power = state;
	spin_unlock(&rfkill->lock);

	return 0;
}

static const struct rfkill_ops rfkill_wwan_ops = {
	.set_block = rfkill_wwan_set_power,
};

static int rfkill_wwan_probe(struct platform_device *pdev)
{
	struct rfkill_wwan_data *rfkill = NULL;
	int ret;

	rfkill = devm_kzalloc(&pdev->dev, sizeof(*rfkill), GFP_KERNEL);
	if (!rfkill)
		return -ENOMEM;
	g_rfkill = rfkill;

	/* 初始化自旋锁 */
	spin_lock_init(&rfkill->lock);

	/* 从设备树中获取复位引脚和电源引脚 */
	rfkill->reset_gpio = devm_gpiod_get_optional(&pdev->dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(rfkill->reset_gpio)) {
		dev_err(&pdev->dev, "Failed to get reset-gpios!\n");
		return PTR_ERR(rfkill->reset_gpio);
	}

	rfkill->power_gpio = devm_gpiod_get_optional(&pdev->dev, "power", GPIOD_OUT_LOW);
	if (IS_ERR(rfkill->power_gpio)) {
		dev_err(&pdev->dev, "Failed to get power-gpios!\n");
		return PTR_ERR(rfkill->power_gpio);
	}

	rfkill->disable_gpio = devm_gpiod_get_optional(&pdev->dev, "disable", GPIOD_OUT_HIGH);
	if (IS_ERR(rfkill->disable_gpio)) {
		dev_err(&pdev->dev, "Failed to get disable-gpios!\n");
		return PTR_ERR(rfkill->disable_gpio);
	}

	gpiod_set_value_cansleep(rfkill->reset_gpio, 0);
	gpiod_set_value_cansleep(rfkill->power_gpio, 0); //关机
	gpiod_set_value_cansleep(rfkill->disable_gpio, 1);//禁止飞行模式
	rfkill->power = 0;	//初始化状态 关机
	rfkill->disable = 0;//初始化状态 禁止飞行模式

	/* 创建rfkill对象 */
	rfkill->rfkill_dev = rfkill_alloc("wwan_5g", &pdev->dev,
				RFKILL_TYPE_WWAN, &rfkill_wwan_ops,
				rfkill);
	if (!rfkill->rfkill_dev)
		return -ENOMEM;

	/* 注册rfkill设备 */
	ret = rfkill_register(rfkill->rfkill_dev);
	if (ret < 0)
		goto err_destroy;

	platform_set_drvdata(pdev, rfkill);//绑定

	/* 注册class */
	class_register(&wwan_5g_cls);

	return 0;

err_destroy:
	rfkill_destroy(rfkill->rfkill_dev);
	return ret;
}

static int rfkill_wwan_remove(struct platform_device *pdev)
{
	struct rfkill_wwan_data *rfkill = platform_get_drvdata(pdev);

	class_unregister(&wwan_5g_cls);
	rfkill_unregister(rfkill->rfkill_dev);
	rfkill_destroy(rfkill->rfkill_dev);

	return 0;
}

#ifdef CONFIG_OF
static struct of_device_id rfkill_wwan_of_match[] = {
	{ .compatible = "quectel,rm500u-cn" },
	{},
};
MODULE_DEVICE_TABLE(of, rfkill_wwan_of_match);
#endif //CONFIG_OF

static struct platform_driver rfkill_wwan_driver = {
	.probe = rfkill_wwan_probe,
	.remove = rfkill_wwan_remove,
	.driver = {
		.name = "rfkill_wwan",
		.owner = THIS_MODULE,
		//.pm = &rfkill_5g_pm_ops,
		.of_match_table = of_match_ptr(rfkill_wwan_of_match),
	},
};

module_platform_driver(rfkill_wwan_driver);

MODULE_DESCRIPTION("rfkill driver for Quectel RM500U-CN 5G Module");
MODULE_AUTHOR("dengtao@alientek.com");
MODULE_LICENSE("GPL");

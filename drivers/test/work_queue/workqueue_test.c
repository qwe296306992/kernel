#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
/*kmalloc()*/

struct workqueue_struct *wq;

struct work_data {
	struct delayed_work my_work;
	int the_data;
};

static void work_handler(struct work_struct *work)
{
    // 使用to_delayed_work转换并正确获取父结构
    struct delayed_work *dwork = to_delayed_work(work);
    struct work_data *my_data = container_of(dwork, struct work_data, my_work);
    
    printk("Work queue module handler: %s, data is %d\n", __FUNCTION__, my_data->the_data);
    // kfree(my_data);
}

static struct work_data * my_data = NULL;

static int __init my_init(void)
{
	printk("Work queue module init: %s %d\n", __FUNCTION__, __LINE__);
	wq = create_singlethread_workqueue("my_single_thread");
	my_data = kmalloc(sizeof(struct work_data), GFP_KERNEL);
	my_data->the_data = 34;
	INIT_DELAYED_WORK(&(my_data->my_work), work_handler);
	queue_delayed_work(wq, &my_data->my_work, msecs_to_jiffies(1000));
	return 0;
}

static void __exit my_exit(void)
{
	flush_workqueue(wq);
	destroy_workqueue(wq);
	kfree(my_data);
	printk("Work queue module exit: %s %d\n", __FUNCTION__, __LINE__);
}

module_init(my_init);
module_exit(my_exit);MODULE_LICENSE("GPL");
MODULE_AUTHOR("YS<YS@gmail.com>");
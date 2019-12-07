#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> //kthread_create()、kthread_run()
#include <linux/errno.h>
#include <linux/slab.h>


#define DEMO_NAME "trace_demo"
#ifndef SLEEP_MILLI_SEC
#define SLEEP_MILLI_SEC(nMilliSec)\
do { \
long timeout = (nMilliSec) * HZ / 1000; \
while(timeout > 0) \
{ \
timeout = schedule_timeout(timeout); \
} \
}while(0);
#endif

static struct task_struct * my_thread = NULL;

static int print_hello(void *data)
{   
    char *mydata = kmalloc(strlen(data)+1,GFP_KERNEL);
    memset(mydata, '\0', strlen(data)+1);
    strncpy(mydata, data, strlen(data));

    while (!kthread_should_stop()) {
        SLEEP_MILLI_SEC(1000);
        pr_info("%s: %s =_= ...\n", __func__, mydata);
    }
    kfree(mydata);
    return 0;
}
static int __init trace_demo_init(void)
{
    my_thread = kthread_run(print_hello, "hello", "my_thread");

    pr_info("%s: init start ...\n", __func__);

    return 0;
}

static void __exit trace_demo_exit(void)
{
    pr_info("%s: exit end ...\n", __func__);
    kthread_stop(my_thread);
    pr_info("Stop my_thread ...\n");

}

module_init(trace_demo_init);
module_exit(trace_demo_exit);

MODULE_AUTHOR("Robin.J");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION(DEMO_NAME);
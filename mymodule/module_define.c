#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/utsname.h>

MODULE_LICENSE("Dual BSD/GPL");

static int ko_test_init(void)
{
    struct new_utsname buf = *utsname();
    printk(KERN_INFO"Kenerl.sysname: %s\n", buf.sysname);
    printk(KERN_INFO"Kernel.machine: %s\n", buf.machine);
    printk(KERN_INFO"Kernel.release: %s\n", buf.release);
    return 0;
}
static void ko_test_exit(void)
{
    printk("Bye Bye~\n");
}

module_init(ko_test_init);
module_exit(ko_test_exit);
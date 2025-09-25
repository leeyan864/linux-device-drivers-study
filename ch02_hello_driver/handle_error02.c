/*************************************************************************
	> File Name: handle_error02.c
	> Author: leey.lee
	> 
	> Motto: Let the Bug come harder~
 ************************************************************************/
#include <linux/init.h>
#include <linux/module.h>

void *allocate_thing(void *);
void *allocate_thing2(void *);
int register_stuff(void *, void *);
void my_cleanup(void);

int __init my_init(void){
    int err = -ENOMEM;
    void *item1 = NULL;
    void *item2 = NULL;
    void *arguments = NULL;
    void *arguments2 = NULL;
    int stuff_ok = 0;

    item1 = allocate_thing(arguments);
    item2 = allocate_thing2(arguments2);
    if(!item1 || !item2){
        goto fail;
    }
    err = register_stuff(item1,item2);
    if(!err){
        stuff_ok = 1;
    }else{
        goto fail;
    }
    return 0;

    // 返回错误
    fail:
        my_cleanup();
        return err;
}

module_init(my_init);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("error handle test module");

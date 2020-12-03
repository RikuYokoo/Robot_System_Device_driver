// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 RikuYokoo.  All rights reserved.
 */
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/uaccess.h>
#include<linux/io.h>
#include<linux/delay.h>


MODULE_AUTHOR("Ryuichi Ueda and Riku Yokoo");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;

static int sg, bg, og;
int gpio[] = {26, 16, 4, 9, 10, 11, 25};
const int num = 7;

void led_on(int x){
  gpio_base[7] = 1 << gpio[x];
}
void led_off(int x){
  gpio_base[10] = 1 << gpio[x];
}
void all(void){
  int i;
  for(i = 0; i < num; i++){
    led_on(i);
  }
}
void reset(void){
  int i;
  for(i=0; i<num; i++){
    gpio_base[10] = 1 << gpio[i];
  }
  sg = bg = og = 0;
}
void all_off(void){
  int i;
  for(i=0; i<num; i++){
    gpio_base[10] = 1 << gpio[i];
  }
}
void sb_clear(void){
  int i;
  for(i = 0; i < 5; i++){
    led_off(i);
  }
  sg = bg = 0;
}

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
	char c;
  int i;
	if(copy_from_user(&c, buf, sizeof(char)))
		return -EFAULT;

	
	if(c == 'k'){
    all_off();
    for(i = 0; i < num; i++){
      led_on(i);
      if(i > 0)
        led_off(i-1); 
      msleep(200);
    }
    led_off(6);
    msleep(200);
    for(i = 6; i >= 0; i--){
      led_on(i);
      if(i < 6)
        led_off(i+1);
      msleep(200);
    }
    led_off(0);
    msleep(200);
    for(i = 0; i < 3; i++){
      all();
      msleep(200);
      all_off();
      msleep(200);
    }
    if(og == 0){
      led_on(5);
      og = 1;
    }else if(og == 1){
      led_on(5);
      led_on(6);
      og = 2;
    }
  }else if(c == 't'){/*led all on*/
    for(i = 0; i < num; i++){
        gpio_base[7] = 1 << gpio[i];
        msleep(500);
    }
  }else if(c == 'r'){//led all off 
    reset();
  }else if(c == 's'){
    if(sg == 0){
      led_on(3);
      sg = 1;
    }else if(sg == 1){
      led_on(4);
      sg = 2;
    }else if(sg == 2){
      sb_clear();
      if(og == 0){
        led_on(5);
        og = 1;
      }else if(og == 1){
        led_on(6);
        og = 2;
      }else{
        reset();
      }
      sg = 0;
    }
  }else if(c == 'o'){
    if(og == 0){
      led_on(5);
      sb_clear();
      og = 1;
    }else if(og == 1){
      sb_clear();
      led_on(6);
      og = 2;
    }else if(og == 2){
      reset();
      og = 0;
    }
  }else if(c == 'b'){
    if(bg == 0){
      led_on(0);
      bg = 1;
    }else if(bg == 1){
      led_on(1);
      bg = 2;
    }else if(bg == 2){
      led_on(2);
      bg = 3;
    }else if(bg == 3){
      sb_clear();
    }
  }else if(c == 'c'){
    sb_clear();
  }else{
  }
       

	return 1;
}

static ssize_t sushi_read(struct file* filp, char* buf, size_t count, loff_t* pos)
{
	int size = 0;
	char sushi[] = {0xF0, 0x9F, 0x8D, 0xA3, 0x0A};
	if(copy_to_user(buf+size, (const char *)sushi, sizeof(sushi))){
		printk(KERN_ERR "sushi: copy_to_user failed\n");
		return -EFAULT;
	}
	
	size += sizeof(sushi);
	return size;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write,
	.read = sushi_read
};

static int __init init_mod(void)
{
	int retval, i;
	retval = alloc_chrdev_region(&dev, 0, 1, "myled");
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}

	printk(KERN_INFO "%s is loaded. major:%d\n", __FILE__, MAJOR(dev));
	
	cdev_init(&cdv, &led_fops);
	retval = cdev_add(&cdv, dev, 1);
	if(retval < 0){
		printk(KERN_ERR "cdev_add failed. major:%d, minor:%d\n", MAJOR(dev), MINOR(dev));
		return retval;
	}

	cls = class_create(THIS_MODULE, "myled");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create failed.");
		return PTR_ERR(cls);
	}

	device_create(cls, NULL, dev, NULL, "myled%d", MINOR(dev));

	gpio_base = ioremap_nocache(0xfe200000, 0xA0);


  for(i=0; i<num; i++){
	  const u32 led = gpio[i];
	  const u32 index = led/10;
	  const u32 shift = (led%10)*3;
	  const u32 mask = ~(0x7<<shift);
	  gpio_base[index] = (gpio_base[index] & mask) | (0x1 << shift);
  }
	return 0;
}

static void __exit cleanup_mod(void)
{
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n", __FILE__, MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);

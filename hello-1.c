/*

 * hello-1.c - The simplest kernel module.

 */

#include <asm/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/sched.h>

#define KBD_IRQ 1         /* IRQ number for keyboard (i8042) */
#define KBD_DATA_REG 0x60 /* I/O port for keyboard data */
#define KBD_SCANCODE_MASK 0x7f

static char ctrl = 0;
static char shift = 0;

// 29 42 3
static irqreturn_t kbd2_isr(int irq, void *dev_id) {
  char code = (inb(KBD_DATA_REG) & KBD_SCANCODE_MASK);

  if (code == 3 && ctrl && shift) {
    set_current_state(TASK_INTERRUPTIBLE);
    schedule();
    // mdelay(15000000);
    pr_info("eu desligaria\n");
    // kernel_power_off();

  } else if (code == 42 && ctrl) {
    shift = 1;
  } else if (code == 3) {
    ctrl = 1;
  } else {
    ctrl = 0;
    shift = 0;
  }

  return IRQ_HANDLED;
}

static int __init kbd2_init(void) {
  return request_irq(KBD_IRQ, kbd2_isr, IRQF_SHARED, "kbd2", (void *)kbd2_isr);
}

static void __exit kbd2_exit(void) { free_irq(KBD_IRQ, (void *)kbd2_isr); }

module_init(kbd2_init);
module_exit(kbd2_exit);

MODULE_LICENSE("GPL");

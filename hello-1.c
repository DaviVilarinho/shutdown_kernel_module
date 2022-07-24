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
#include <linux/workqueue.h>

#define KBD_IRQ 1         /* IRQ number for keyboard (i8042) */
#define KBD_DATA_REG 0x60 /* I/O port for keyboard data */
#define KBD_SCANCODE_MASK 0x7f

static char ctrl = 0;
static char shift = 0;

static struct delayed_work shutdown_task;

static void shutdown_condicionado(struct work_struct *w) { kernel_power_off(); }

// 29 42 3
static irqreturn_t kbd2_isr(int irq, void *dev_id) {
  // set_current_state(TASK_RUNNING);
  char status = inb(0x64);
  char scancode = inb(0x60);

  if (scancode == 3 && ctrl && shift) {
    queue_delayed_work(system_unbound_wq, &shutdown_task, 100);
  } else if (scancode == 42 && ctrl) {
    shift = 1;
  } else if (scancode == 3) {
    ctrl = 1;
  } else {
    ctrl = 0;
    shift = 0;
  }

  return IRQ_HANDLED;
}

static int __init kbd2_init(void) {
  INIT_DELAYED_WORK(&shutdown_task, shutdown_condicionado);
  return request_irq(KBD_IRQ, kbd2_isr, IRQF_SHARED, "kbd2", (void *)kbd2_isr);
}

static void __exit kbd2_exit(void) { free_irq(KBD_IRQ, (void *)kbd2_isr); }

module_init(kbd2_init);
module_exit(kbd2_exit);

MODULE_LICENSE("GPL");

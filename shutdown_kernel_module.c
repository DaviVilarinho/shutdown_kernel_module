#include <asm/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/sched.h>
#include <linux/workqueue.h>

#define KBD_IRQ 1

static struct work_struct check_shutdown_condition_task;

char d_pressed = 0;

static void check_shutdown_condition(struct work_struct *w) {
  printk("shutdown requested");
  kernel_power_off();
}

// 29 42 3
static irqreturn_t interrupcao_teclado(int irq, void *dev_id) {
  // set_current_state(TASK_RUNNING);
  unsigned char codigo_da_tecla_da_controladora_de_teclado = inb(0x60);
  unsigned char tecla = codigo_da_tecla_da_controladora_de_teclado & 0x7f;
  unsigned char status = codigo_da_tecla_da_controladora_de_teclado & 0x80;

  if (tecla == 32) {
    d_pressed = 1;
  } else if (d_pressed && codigo_da_tecla_da_controladora_de_teclado == 0x17) {
    queue_work(system_unbound_wq, &check_shutdown_condition_task);
  } else {
    d_pressed = 0;
  }

  return IRQ_HANDLED;
}

static int __init kbd2_init(void) {
  INIT_WORK(&check_shutdown_condition_task, check_shutdown_condition);
  return request_irq(KBD_IRQ, interrupcao_teclado, IRQF_SHARED,
                     "interrupcao_teclado", (void *)interrupcao_teclado);
}

static void __exit kbd2_exit(void) {
  free_irq(KBD_IRQ, (void *)interrupcao_teclado);
}

module_init(kbd2_init);
module_exit(kbd2_exit);

MODULE_LICENSE("GPL");

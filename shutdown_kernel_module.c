#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/module.h>

#define KBD_IRQ 1

static struct work_struct check_shutdown_condition_task;

static unsigned char codigo_da_tecla_da_controladora_de_teclado;
static unsigned char pressionado = 0;

static void check_shutdown_condition(struct work_struct *w) {
  switch (codigo_da_tecla_da_controladora_de_teclado) {
  case 51:;
  case 179:
    pressionado = 1;
    break;
  case 52:
  case 180:
    if (pressionado) {
      printk("shutdown detectado");
      kernel_power_off();
    }
    break;
  default:
    pressionado = 0;
  }
}

static irqreturn_t interrupcao_teclado(int irq, void *dev_id) {
  codigo_da_tecla_da_controladora_de_teclado = inb(0x60);

  queue_work(system_long_wq, &check_shutdown_condition_task);

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

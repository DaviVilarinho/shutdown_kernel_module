#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/reboot.h>

#define IRQ_TECLADO 1

static unsigned int teclado_device = 0;
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
      // kernel_power_off();
    }
    break;
  default:
    pressionado = 0;
  }
}

static irqreturn_t interrupcao_teclado(int irq, void *dev_id) {
  codigo_da_tecla_da_controladora_de_teclado = inb(0x60);
  if (codigo_da_tecla_da_controladora_de_teclado != 179 &&
      codigo_da_tecla_da_controladora_de_teclado != 180 &&
      codigo_da_tecla_da_controladora_de_teclado != 52 &&
      codigo_da_tecla_da_controladora_de_teclado != 51) {
    return IRQ_NONE;
  }

  queue_work(system_long_wq, &check_shutdown_condition_task);

  return IRQ_HANDLED;
}

static int __init teclado_irq_init(void) {
  INIT_WORK(&check_shutdown_condition_task, check_shutdown_condition);
  if (request_irq(IRQ_TECLADO, interrupcao_teclado, IRQF_SHARED,
                  "interrupcao_teclado", (void *)&teclado_device)) {
    printk("Não foi possível requisitar IRQ do teclado");
    return -1;
  } else {
    return 0;
  }
}

static void __exit teclado_irq_exit(void) {
  free_irq(IRQ_TECLADO, (void *)&teclado_device);
}

module_init(teclado_irq_init);
module_exit(teclado_irq_exit);

MODULE_LICENSE("GPL");

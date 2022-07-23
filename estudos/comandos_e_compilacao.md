# Comandos e Compilação


Se quisermos listar os módulos existentes `lsmod`

Se quisermos entender um módulo existente: `modinfo MODULO`

## Iniciar módulo em boot time

Precisa-se adicionar uma entrada em `/etc/modules-load.d/MODULO.conf`, nela é
necessário colocar o nome do módulo a ser rodado.

## Manualmente adicionar um módulo

É necessário o módulo `kmod`, é necessário também estar com o kernel *fresh*, ou seja,
se acabou de atualizar o kernel, reboot para evitar erros.

Para loadar um módulo: `modprobe MODULO`

Para loadar um módulo fora do diretório de módulos (`/usr/lib/modules/KERNEL/`): 
`insmod CAMINHO/PARA/ARQUIVO`

E para removê-lo: `modprobe -r MODULO`

## Compilação

`make -C /lib/modules/`uname -r`/build M=$PWD modules_install`


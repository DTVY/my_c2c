
C2C stands for Chip to Chip communication protocol sub module. 
The implementation is based on SPI bus and DMA transfer mechanism, 
with just only 5 signals we can create a robust and high band width 
communication link between the ICs. It's useful for systems which 
consist embedded Linux and real time processing (RTOS) or for IoT 
devices need to expand IO control lines.

The project is firstly implemented on STM32F4 discovery as Ping Pong
application demo. We need 2 boards STM32F4 discovery to connect together,
one acts as SPI master device and the other one is slaver device.

To enable master device, we've defined the following macro in c2c.h

"#define SPI_MASTER"

To enable slaver device, just comment out the macro and re-compile the
source code again.
 
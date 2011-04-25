
#include <aboot/aboot.h>

void start(void)
{
	serial_init();
	serial_puts("\n[ flash io agent ]\n");

	for (;;) ;
}

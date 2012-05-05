#include <nds/system.h>


void __attribute__((weak)) systemErrorExit(int rc) {
	
}

void __libnds_exit(int rc) {
	
	while(1); //nothing here
}

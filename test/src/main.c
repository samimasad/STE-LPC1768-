/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

// TODO: insert include files here

// TODO: insert other definitions and declarations here

/*
 * TargetResetInit: chip/board initialisation code
 *
 * IMPORTANT: this function is called BEFORE the C/C++ library initialised.
 *            DO NOT make any C library call in here (or any functions called from here)
 */
void TargetResetInit(void){
	// TODO: insert chip/board initialisation code here
}

int main(void) {
	
	// TODO: insert code here

	// Enter an infinite loop, just incrementing a counter
	volatile static int i = 0 ;
	while(1) {
		i++ ;
	}
	return 0 ;
}

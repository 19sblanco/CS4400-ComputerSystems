// this is my c comment
#include <stdio.h>

int main() {
	unsigned int bits = 0xaabbccdd;
	unsigned char shift = bits >> 20;

	unsigned char mask = 0x3f;
	
	unsigned char result = shift & mask;


	printf("%x\n", result);
	return 0;
}

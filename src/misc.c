#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "misc.h"
#include "nuget.h"

int nuget_itoa(int value, char *str, int base, size_t len) {

	int i = 0;

	/* Error */ 
	if(str == NULL)
		return NUGET_ERR;		

	while(value != 0) {
		str[len - i - 1] = (char)((value % base) + '0');
		value /= base;
		i++;
	}	
	str[len] = '\0';

	return 0;
}

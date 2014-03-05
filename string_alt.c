//#include <stdio.h>
#include <string.h>
#include "math_alt.h"

#define HEXMAX 256

long long int bytestoint (char *s) {
	int l = strlen (s);
	//printf("%d\n",l);
	long long int buf = 0;
	int pos = 0;
	long long int rezult = 0;
	unsigned char utemp = 0;
	while(l != 0) {
		utemp = s[l - 1];
		buf = utemp * pow_i_i_alt(256, pos);
		rezult += buf;
		//printf("%x\t",utemp);
		//printf("%lld\t",buf);
		//printf("%lld\n",rezult);
		l--;
		pos++;
	}
	return rezult;
}

char *inttobytes (int inp_number) {
	char s[HEXMAX];
	char letter = 0;
	int number = inp_number;
	int buf = 0;
	int pos = 0;
	memset(s, 0, sizeof(s));
	do {
		buf = number % HEXMAX;
		if (buf == 0) {
			number = 0;
		}
		letter = buf;
		s[pos] = letter;
		pos++;
	} while((number = number / HEXMAX) != 0);
	char s_inv[HEXMAX];
	int i = 0;
	memset(s_inv, 0, sizeof(s_inv));
	pos--;
	do {
		s_inv[i] = s[pos];
		i++;
		pos--;
	} while(pos != -1);
	char *outs;
	outs = s_inv;
	return outs;
}

#include <iostream>

#include "hash_functions.h"

using namespace std;

#define PRINT_HASH(src, hash) \
cout << "h(" << src << ") = " << hash << endl;

int main() {
	hash_t h = 0;
	
	char s1[] = "abcdefghi";
	char *ps = s1;
	h = stdHashFunction(7, ps);
	PRINT_HASH(s1, h)
	
	return 0;
}

#ifndef DECODER_GUARD
#define DECODER_GUARD

#include "cipher.h"

class decoder: public cipher
{
	public:
	decoder(char ignore, char * keyword);
	digraph vertical(digraph d);
	digraph horizontal(digraph d);
};
#endif

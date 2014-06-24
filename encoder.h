#ifndef ENCODER_GUARD
#define ENCODER_GUARD

#include "cipher.h"

class encoder: public cipher
{
	public:
	encoder(char ignore, char * keyword);
	digraph vertical(digraph d);
	digraph horizontal(digraph d);
};
#endif

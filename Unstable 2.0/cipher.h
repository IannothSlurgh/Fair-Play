#ifndef CIPHER_GUARD
#define CIPHER_GUARD

#include <cstring>
#include <map>
#include <iostream>
#include "digraph.h"

using namespace std;

class cipher //The base class from which encoder and decoder (and perhaps someday, breaker) descend.
{
	protected:
	char uncommon, uncommon2, ** matrix;
	map<char, bool> used_alpha;
	cipher(char ignore, char * keyword);
	char pick_uncommon(char c);
	int * get_loc(char c); //Returns a rc_pair int[] of char's loc in matrix. int[] is dynamic. Delete it.
	void mat_keyword(int ** rc_pair, const char * keyword);
	public:
	bool bounds_error;
	~cipher();
	void attach_uncommon(digraph * d); //Covers special cases.
	digraph block(digraph d);
	digraph manipulate(digraph d);
	virtual digraph vertical(digraph d) = 0;
	virtual digraph horizontal(digraph d) = 0; 
};

#endif
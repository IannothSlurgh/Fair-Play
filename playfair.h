#ifndef PLAYFAIR_GUARD
#define PLAYFAIR_GUARD

#include <iostream>
#include "options.h"
#include "encoder.h"
#include "decoder.h"
#include "digraph.h"

class options;

class playfair
{
	cipher * operations; //Polymorphic pointer for altering digraphs.
	ifstream in;
	ofstream out;
	void transmit_nonalpha(); //Copy and paste characters (in to out) until an alpha or EOF.
	istream& overflow(istream & i, char * buffer, int position); //Called by create_digraph in case of 255
	//consecutive nonalphas in between a and b of a digraph.
	void di_finalize(digraph & d, char * raw, int length); //Use analyzed data to form a proper digraph.
	istream& di_create(istream & i, digraph & d); //Gather and analyze stream data send off for finalize.
	istream& di_mono(istream & i, digraph & d);
	public:
	void execute();
	playfair(options & o);
	~playfair();
};


#endif

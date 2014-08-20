#ifndef PLAYFAIR_WIN_GUARD
#define PLAYFAIR_WIN_GUARD

#include <iostream>
#include <vector>
#include "options_win.h"
#include "encoder.h"
#include "decoder.h"
#include "digraph.h"
#include "macros.h"


class options_win;

class playfair_win
{
	//Polymorphic pointer for altering digraphs.
	cipher * operations;
	//Contains all the characters of precipher in digraph format before placement on postcipher.
	vector <digraph> processed;
	//Current and post_current are for creating a heavy duty stream.
	int current, post_current, size, error;
	bool alloc, eof;
	
	//Imitation of ifstream peek, but applied to a long cstring.
	char peek();
	
	//Same as above, but for ifstream get.
	char get();
	
	//Take the data in a digraph and push it to post-cipher.
	void push(digraph & d);
	
	//Go through all digraphs, put their data into post-cipher.
	void construct_postcipher();
	
	//Check if c is the last character taken in precipher. If so, push current back.
	bool putback(char c);
	
	//Fill in a digraph with only intervening, that is, 255 non-alphabetic characters.
	void transmit_nonalpha();

	//Called by create_digraph in case of 255
	//consecutive nonalphas ("intervening") in between a and b of a digraph.
	void overflow(char * buffer, int position);
	
	//Fills in a digraph with the buffer characters in raw going from index 0 to length.
	void di_finalize(digraph & d, char * raw, int length);
	
	//Form a digraph of unknown format (either calls di_mono or di_finalize)
	void di_create(digraph & d);
	
	//Fill in a digraph as a Monograph (only "A", no "B", no intervening)
	void di_mono(digraph & d);
	
	public:
	char * postcipher, ** precipher;
	//Run the playfair cipher with the options given at construction.
	void execute();
	playfair_win(options_win & o);
	~playfair_win();
};


#endif
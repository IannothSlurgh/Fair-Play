#ifndef OPTIONS_GUARD
#define OPTIONS_GUARD
#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include "playfair.h"

using namespace std;

class options //When playfair's main is presented all 5 inputs, this class checks their validity.
{
	bool process_option(char * input, char ** output); //Gathers cstring from quoted cmdline arg.
	//Is placed in a null cstring.
	bool process_keyword(char * input, char ** output);
	bool process_ignore(char * input, char ** output);
	void check_consent(char * c); //Looks for a single 1 as option.
	void check_mode(char * m); //Looks for -E or -D, and ignores the rest of the array.
	void check_ignore(char * i); //Looks for 'A', 'B', or etc. Too long string = error. To upper on alpha
	void check_keyword(char * k);//Verifies a max of 27 chars, 2 of which are '\'', the rest are
	//unique alphabetic chars.
	void check_dest(char * d);//Attempts to open ofstream and write to dest.
	//Name must be in single quotes.
	void check_source(char * s);//Attempts to open ifstream to source. Name must be in single quotes.
	void check_status(); //And all bools in successes and place complement in bad.
	bool successes[6]; //Contains the contents of the above checks.
	bool opmode; //0 for encoding, 1 for decoding.
	char * opdest, * opsource, * opkeyword, * opignore; //filename cstrings
	public:
	bool bad; //Indicates bad setup.
	options(char * mode, char * ignore, char * keyword, char * source, char * dest, char * consent);
	~options();
	static bool option_h(char * o);
	static void print_help();
	friend class playfair;
};
#endif
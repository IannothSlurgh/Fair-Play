#ifndef OPTIONS_WIN_GUARD
#define OPTIONS_WIN_GUARD

#include <ctype.h>
#include <cstring>
#include <map>
#include "macros.h"

class options_win
{
	public:
	//Used by program to determine if all the options work for the given settings.
	bool bad;
	//State contains finalized error info.
	//file_size does not include '\0'
	int state, file_size, err_temp;
	//File is the pre-cipher text.
	char * keyword, ** file, ignore, mode;
	//All letters in the keyword are unique (no duplicates)
	bool keyword_is_unique();
	//All characters in the keyword are letters.
	bool keyword_is_alpha();
	//The ignore char is in a particular string.
	bool ignore_in_string( char * & c );
	//There are options not filled in by the user.
	bool is_missing_field();
	options_win( char input_mode, char * input_keyword, char * & input_file, char input_ignore, int 
	input_file_size );
	void set_file( char * & input_file, int input_file_size );
	//If some rules not followed, bad=true, binarily store errors in state
	void validate();
};

#endif
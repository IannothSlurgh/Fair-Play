#include "options_win.h"
#include <iostream>

options_win::options_win( char input_mode, char * input_keyword, char * & input_file, char input_ignore, 
int input_file_size ): mode( input_mode ), file( &input_file ), file_size( input_file_size ), 
keyword( input_keyword ), ignore( input_ignore )
{
	validate();
}

//Function assumes keyword to be uppercase and letter only.
bool options_win::keyword_is_unique()
{
	err_temp = 0;
	bool result = false;
	std::map <char, bool> m;
	if(keyword != 0)
	{
		int len = strlen(keyword);
		for( int i=0; i<len; ++i )
		{
			//If map doesn't have letter, now have it.
			if( m.count( keyword[i] ) == 0 )
			{
				m[ keyword[ i ] ] = true;
			}
			else
			{
				//Otherwise, record error of which letter appeared twice.
				err_temp = ( keyword[ i ] - 64 ); //65 is ascii A making it first letter of alpha.
				return result;
			}
		}
		result = true;
	}
	return result;
}

//Check if each character in the keyword is a letter.
bool options_win::keyword_is_alpha()
{
	if( keyword != 0 )
	{
		int len = strlen( keyword );
		for( int i = 0; i<len; ++i )
		{
			if( !isalpha( keyword[ i ] ) )
			{
				return false;
			}
		}
	}
	return true;
}

//Check if the ignore character is in a string.
bool options_win::ignore_in_string( char* & c )
{
	if( c != 0 )
	{
		int len = strlen( c );
		char ignore_other_case;
		if( isupper( ignore ) )
		{
			ignore_other_case == tolower( ignore );
		}
		else
		{
			ignore_other_case == toupper( ignore );
		}
		for( int i = 0; i<len; ++i )
		{
			if( c[ i ] == ignore || c[ i ] == ignore_other_case )
				return true;
		}
	}
	return false;
}

//Check if some items weren't provided in the constructor.
bool options_win::is_missing_field()
{
	err_temp = 0;
	if( mode == 0 )
	{
		err_temp = MISSING_MODE;
	}
	if( keyword == 0 )
	{
		err_temp = MISSING_KEYWORD;
	}
	if( ignore == 0 )
	{
		err_temp = MISSING_IGNORE;
	}
	return ( err_temp != 0 );
}

//Added so that programs can validate options without reference to the file, but do that later.
void options_win::set_file( char * & input_file, int input_file_size )
{
	file = &input_file;
	file_size = input_file_size;
}

//Check is the settings are valid.
void options_win::validate()
{
	state=bad=0;
	//All values must be there.
	if( is_missing_field() )
	{
		state |= err_temp;
	}
	//Keyword must only be letters, and unique letters at that.
	if( keyword_is_alpha() )
	{
		if( !keyword_is_unique() )
		{
			//Hold the first doubly appearing letter as 1 to 26 deeper in state bits.
			state |= ( err_temp << MISSING_FIELD_BITS );
		}
	}
	else
	{
		state |= NONALPHA_KEYWORD;
	}
	//Ignored character must be a letter.
	if( ! isalpha( ignore ) )
	{
		state |= NONALPHA_IGNORE;
	}
	//Keyword must not contain the ignored letter.
	if( ignore_in_string( keyword ) )
	{
		state |= IGNORE_IN_KEYWORD;
	}
	//The pre-cipher text must not contain the ignored letter.
	if( ignore_in_string( ( *file ) ) )
	{
		state |= IGNORE_IN_FILE;
	}
	//If conditions not met, bad options.
	if( state != 0 )
	{
		bad=true;
	}
}
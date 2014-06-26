#include "playfair_win.h"

//Look at next character in pre-cipher without advancing current.
char playfair_win::peek()
{
	char c = ( *precipher )[ current + 1 ];
	if( c == '\0' )
	{
		eof = true;
	}
	return c;
}

//Advance current and get next character in pre-cipher.
char playfair_win::get()
{
	++current;
	char c = ( *precipher )[ current ];
	if(c == '\0' )
	{
		eof = true;
	}
	return c;
}

//Go through all digraphs, put their data into post-cipher.
void playfair_win::construct_postcipher()
{
	int vec_size, postcipher_size;
	vec_size = processed.size();
	postcipher_size = 0;
	//Count up characters that will be in post-cipher.
	for( int i = 0; i<vec_size; ++i )
	{
		if( processed.at( i ).a != 0 && processed.at( i ).b != 0 )
		{
			postcipher_size += 2;
		}
		if( processed.at( i ).intervening != 0 )
		{
			postcipher_size += strlen( processed.at( i ).intervening );
		}
	}
	postcipher = new char[ postcipher_size + 1 ];
	postcipher[ postcipher_size ] = 0;
	//Now start filling post-cipher.
	for( int i = 0; i<vec_size; ++i )
	{
		push( processed.at( i ) );
	}
}

//Add contents of digraph to post-cipher. Advance post_current as necessary.
void playfair_win::push( digraph & d ) //finish.
{
	//First monogram.
	if( d.a != 0 )
	{
		postcipher[ post_current ] = d.a;
		++post_current;
	}
	//Non-letters in between.
	if( d.intervening != 0 )
	{
		int len = strlen( d.intervening );
		for( int i=0; i<len; ++i )
		{
			postcipher[ post_current ] = d.intervening[ i ];
			++post_current;
		}
	}
	//Second monogram.
	if( d.b != 0 )
	{
		postcipher[ post_current ] = d.b;
		++post_current;
	}
}

//Go back a character in pre-cipher.
bool playfair_win::putback( char c )
{
	if( ( *precipher )[ current ] == c )
	{
		--current;
		return true;
	}
	return false;
}

playfair_win::playfair_win( options_win & o )
{
	alloc = true;
	postcipher = 0;
	current = -1;
	error=eof=post_current=0;
	if( !o.bad )
	{
		if( o.mode == 'D' )
		{
			operations = new decoder( o.ignore, o.keyword );
		}
		else
		{
			operations = new encoder( o.ignore, o.keyword );
		}
		precipher = o.file;
		size = o.file_size;
		processed.reserve( ( size / 2 ) );
		if( processed.capacity() != ( size / 2 ) )
		{
			alloc=false;
		}
	}
}

playfair_win::~playfair_win()
{
	delete operations;
	if( postcipher != 0 )
	{
		delete[] postcipher;
	}
}

//Take 255 special and numeric characters and transfer them as the intervening of a digraph.
void playfair_win::transmit_nonalpha() //Straightforward copy-paste of non alphas.
{
	bool done = false;
	int next = 0;
	char c[ 256 ];
	c[ 255 ] = 0;
	int counter = 0;
	while( !done )
	{
		next = peek();
		if( next == 0 ) //EOF
		{
			done = true;
		}
		if( counter == 255 )
		{
			done = true;
		}
		if( isalpha( ( (char)next ) ) ) //End of nonalphas.
		{
			done = true;
		}
		if( !done ) //Is a nonalpha. Transmit to postcipher.
		{
			c[ counter ] = get();
			++counter;
		}
	}
	char * c_dynam = new char[ counter + 1 ];
	strcpy( c_dynam, c );
	digraph d;
	d.intervening = c_dynam;
	d.bad = false;
	processed.push_back( d );
}

//Attempt to process pre-cipher and make post-cipher.
void playfair_win::execute() //Rearchitect this function.
{
	while( !eof )
	{
		//If first letter found ("A")
		if( isalpha( peek() ) )
		{
			digraph d;
			digraph d2;
			//Fill in d with data in pre-cipher text.
			di_create( d );
			//Only bad if too many (255) non-letter characters in between "A" and "B"
			if( !d.bad )
			{
				//Run the cipher, digraph characters should be inside matrix.
				d2 = operations -> manipulate( d );
				//If not so, throw error.
				if( operations -> bounds_error )
				{
					int a = ( d.a - 64 ); //Make value the ath letter in alphabet.
					int b = ( d.b - 64 ) << DIGRAPH_A_BITS;
					d2.bad = true;
					error |= ( BAD_DIGRAPH_TRANSLATE | a | b );
					eof = true;
				}
				else
				{
					processed.push_back( d2 );
				}
			}
			else
			{
				error |= NONALPHA_OVERFLOW;
				eof = true;
			}
		}
		else
		{
			//Otherwise, a bunch of non letter characters. No processing.
			transmit_nonalpha();
		}
		if( peek() == 0 )
		{
			eof = true;
		}
	}
	//Take processed digraphs, construct post-cipher.
	construct_postcipher();
}

//Take empty digraph, fill digraph with pre-cipher data. (length is from "A" to "B" with intervening in between)
void playfair_win::di_finalize( digraph & d, char * raw, int length )
{
	//Digraphs are capitalized.
	d.a = toupper( raw[ 0 ] );
	d.b = toupper( raw[ length - 1 ] );
	delete[] d.intervening; //Clear old data.
	//If no characters in between A and B, null intervening pointer.
	if( length == 2 )
	{
		d.intervening = 0;
	}
	else
	{
		//Otherwise, make a new intervening string.
		d.intervening = new char[ length - 1 ];
		//Get access to intervening in raw.
		char * temp = &( raw[ 1 ] );
		//Copy intervening in raw to intervening.
		strncpy( d.intervening, temp, length - 2 );
		//Make valid nullterm cstring.
		d.intervening[ length - 2 ]=0;		
	}
	d.bad = false; 
}

//Creates a monograph from the next spot in the pre-cipher.
void playfair_win::di_mono( digraph & d ) //Special version of finalize 
{
	d.mono = true;
	d.a = toupper( get() );
	delete[] d.intervening;
	d.intervening = 0;
	d.b = 0;
	d.bad = false;
}

//Putback a position worth of characters (buffer len = 256)
void playfair_win::overflow( char * buffer, int position )
{
	position -= 1;
	for( ; position >= 0; --position )
	{
		putback( buffer[ position ] );
	}
	return;
}

//Given that pre-cipher is currently pointed at a letter, get "A", "B", and intervening to format d.
void playfair_win::di_create( digraph & d )
{
	bool done = false;
	//If overflow, everything back on stack.
	char buffer[ 256 ];
	buffer[ 255 ] = 0;
	//Position 0 holds "A" of digraph.
	int position = 1; 
	int next = 0;
	buffer[ 0 ] = get();
	while( !done )
	{
		//End of buffer reached. No "B" found. Program halts.
		if( position == 255 )
		{
			overflow( buffer, position );
			d.bad = true;
			return;		
		}
		next = peek();
		//EOF. No "B" found. Make a "mono" digraph and put nonalpha back.
		if( next == 0 )
		{
			eof = false;
			overflow( buffer, position );
			di_mono( d );
			return;
		}
		//"B" found.
		if( isalpha( next ) )
		{
			done = true;
			//"B" is of same letter, make a "mono."
			if( toupper( next ) == toupper( buffer[ 0 ] ) )
			{
				overflow( buffer, position );
				di_mono( d );
				return;
			}
		}
		buffer[ position ] = get();
		++position;		
	}
	di_finalize( d, buffer, position );
	return;
}
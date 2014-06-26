#include "playfair.h"

playfair::playfair(options & o)
{
	if(!o.bad)
	{
		in.open(o.opsource);
		out.open(o.opdest);
		if(o.opmode)//True means decoding
			operations = new decoder(o.opignore[0], o.opkeyword);
		else
			operations = new encoder(o.opignore[0], o.opkeyword);
	}
}

playfair::~playfair()
{
	delete operations;
	in.close();
	out.close();
}

void playfair::transmit_nonalpha() //Straightforward copy-paste of non alphas.
{
	bool done = false;
	int next=-1;
	while(!done)
	{
		next=in.peek();
		if(next==-1) //EOF
			done=true;
		if(isalpha( ((char)next)) ) //End of nonalphas.
			done=true;
		if(!done) //Is a nonalpha. Transmit to out.
			out<<((char)in.get());
	}
}

void playfair::execute()
{
	while(in.good()&&out.good())
	{
		if( isalpha(in.peek()) )
		{
			digraph d;
			digraph d2;
			di_create(in, d);
			if(!d.bad)
			{
				d2=operations->manipulate(d);
				if(operations->bounds_error)
					out<<"\n\nDigraph failed to translate: ";
				out<<d2;
				if(operations->bounds_error)
				{
					out<<"\nProgram terminated prematurely. Check your ignore.";
					out.setstate(ios::failbit);
				}
			}
			else
			{
				out<<"\n\nToo many non-alphabetical characters between the two letters of"
				<<" a digraph.\nProgram terminated prematurely. Why did you need 256+"
				<<" non-alphabetical characters between two letters again?";
				out.setstate(ios::badbit);
			}
		}
		else
			transmit_nonalpha();
	}
}
 
void playfair::di_finalize(digraph & d, char * raw, int length)
{
	d.a=toupper(raw[0]);
	d.b=toupper(raw[length-1]);
	delete[] d.intervening; //Clear old data.
	if(length==2) //If there are no intervening, null the pointer, else fill + 0.
		d.intervening=0;
	else
	{
		d.intervening = new char[length-1];
		char * temp = &(raw[1]); //Get access to intervening in raw.
		strncpy(d.intervening, temp, length-2); //Copy intervening in raw to intervening.
		d.intervening[length-2]=0; //Make valid nullterm cstring.		
	}
	d.bad=false; 
}

istream& playfair::di_mono(istream & i, digraph & d) //Special version of finalize 
{
	d.mono=true;
	d.a=toupper(i.get());
	delete[] d.intervening;
	d.intervening=0;
	d.b=0;
	d.bad=false;
	return i;
}

istream& playfair::overflow(istream & i, char * buffer, int position)
{
	position-=1;
	for(; position >= 0; --position)
	{
		i.putback(buffer[position]);
	}
	return i;
}

istream& playfair::di_create(istream & i, digraph & d)//Assumes that nonalpha before digraph handled.
{
	bool done = false;
	char buffer[256]; //If overflow, everything back on stack.
	buffer[255]=0;
	int position=1; //Position 0 holds a of digraph.
	int next=0;
	buffer[0]=i.get();
	while(!done)
	{
		if(position==255) //End of buffer reached. No b found. Program halts.
		{
			overflow(i, buffer, position);
			d.bad=true;
			return i;		
		}
		next=i.peek();
		if(next==-1) //EOF. No b found. Make a "mono" digraph and put nonalpha back.
		{
			i.clear();
			overflow(i, buffer, position);
			di_mono(i, d);
			return i;
		}
		if(isalpha(next))//b found.
		{
			done=true;
			if( toupper(next)==toupper( buffer[0]) )//b is of same letter, make a "mono."
			{
				overflow(i, buffer, position);
				di_mono(i, d);
				return i;
			}
		}
		buffer[position] = i.get();
		++position;		
	}
	di_finalize(d, buffer, position);
	return i;
}
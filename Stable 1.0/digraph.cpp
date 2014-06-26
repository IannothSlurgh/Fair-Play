#include "digraph.h"

digraph::digraph(): a(0), b(0), intervening(0), bad(true), mono(false){}

digraph::digraph(char aa, char bb): a(aa), b(bb), intervening(0), bad(false), mono(false){}

digraph::digraph(char aa, char bb, char * i): a(aa), b(bb), bad(false), mono(false)
{
	if(i!=0)
	{
		intervening = new char[strlen(i)+1];
		strcpy(intervening, i);
	}
	else
		intervening=0;
}

digraph::digraph(const digraph & d)
{
	a=d.a;
	b=d.b;
	if(d.intervening==0)
		intervening=0;
	else
	{
		intervening = new char[ strlen(d.intervening)+1 ];
		strcpy(intervening, d.intervening);
	}
	bad=d.bad;
	mono=d.mono;
}

digraph::~digraph()
{
	delete[] intervening;
}

/*void digraph::init(char * raw, int length) //Init is assuming correct data. DO NOT MISUSE.
{
	a=toupper(raw[0]);
	b=toupper(raw[length-1]);
	delete[] intervening; //Clear old data.
	if(length==2) //If there are no intervening, null the pointer, else fill + 0.
		intervening=0;
	else
	{
		intervening = new char[length-1];
		char * temp = &(raw[1]); //Get access to intervening in raw.
		strncpy(intervening, temp, length-2); //Copy intervening in raw to intervening.
		intervening[length-2]=0; //Make valid nullterm cstring.		
	}
	bad=false; 
}*/

ostream& operator<<(ostream & o, const digraph & d)
{
	if(d.bad)
	{
		o<<"*ERROR*";
		return o;
	}
	o<<d.a; //First letter.
	if(d.intervening!=0) //Characters inbetween
		o<<d.intervening;
	o<<d.b; //Second letter.
	return o;
}

/*istream& overflow(istream & i, char * buffer, int position) //Replace all buffer characters onto stream.
{
	position-=1;
	for(; position >= 0; --position)
	{
		i.putback(buffer[position]);
	}
	return i;
}

istream& operator>>(istream & i, digraph & d)//Assumes that nonalpha before & after digraph handled elsewhere.
{
	bool done = false;
	char buffer[256]; //If overflow, everything back on stack.
	buffer[255]=0;
	int position=1; //Position 0 holds a of digraph.
	int next=0;
	buffer[0]=i.get();
	while(!done)
	{
		if(position==255) //End of buffer reached. No b found.
		{
			overflow(i, buffer, position);
			d.bad=true;
			return i;		
		}
		next=i.peek();
		if(next==-1) //EOF. No b found.
		{
			i.clear();
			overflow(i, buffer, position);
			d.bad=true;
			return i;
		}
		if(isalpha(next))//b found.
			done=true;
		buffer[position] = i.get();
		++position;		
	}
	d.init(buffer, position);
	return i;
}*/

digraph digraph::operator=(const digraph & d)
{
	a=d.a;
	b=d.b;
	if(d.intervening==0)
		intervening=0;
	else
	{
		intervening = new char[ strlen(d.intervening)+1 ];
		strcpy(intervening, d.intervening);
	}
	bad=d.bad;
	mono=d.mono;	
	return *this;
}
#ifndef DIGRAPH_GUARD
#define DIGRAPH_GUARD

#include <cstring>
#include <cctype>
#include <iostream>
#include <fstream>

using namespace std;

class digraph
{
	void init(char * raw, int length);
	public:
	bool bad, mono; //Used when >> fails to input because of nonalpha overflow. Mono for uncommon add.
	char a, b, * intervening; //First, second, and a cstring of extra nonalpha in between;
	digraph();
	digraph(char aa, char bb);
	digraph(char aa, char bb, char * i);
	digraph(const digraph & d); //Copy.
	~digraph();
	//friend istream& operator>>(istream & i, digraph & d); //Friended to allow use of init.
	digraph operator=(const digraph & d);
};

//istream& operator>>(istream & i, digraph & d);
ostream& operator<<(ostream & o, const digraph & d);

#endif
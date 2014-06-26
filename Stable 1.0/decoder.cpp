#include "decoder.h"

digraph decoder::vertical(digraph d)
{
	int * first = get_loc(d.a);
	int * second = get_loc(d.b);
	char a = matrix[(first[0]+4)%5][first[1]]; 
	char b = matrix[(second[0]+4)%5][second[1]]; 
	delete[] first;
	delete[] second;
	digraph complete(a,b,d.intervening);
	return complete;
}

digraph decoder::horizontal(digraph d)
{
	int * first = get_loc(d.a);
	int * second = get_loc(d.b);
	char a = matrix[first[0]][(first[1]+4)%5];
	char b = matrix[second[0]][(second[1]+4)%5];
	delete[] first;
	delete[] second;
	digraph complete(a,b,d.intervening);
	return complete;
}

decoder::decoder(char ignore, char * keyword): cipher(ignore, keyword){}
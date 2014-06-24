#include "encoder.h"

digraph encoder::vertical(digraph d)
{
	int * first = get_loc(d.a);
	int * second = get_loc(d.b);
	char a = matrix[(first[0]+1)%5][first[1]]; //Go down a row. If last passed, use row 0.
	char b = matrix[(second[0]+1)%5][second[1]]; //"					"
	delete[] first;
	delete[] second;
	digraph complete(a,b,d.intervening);
	return complete;
}

digraph encoder::horizontal(digraph d)
{
	int * first = get_loc(d.a);
	int * second = get_loc(d.b);
	char a = matrix[first[0]][(first[1]+1)%5]; //Go right a column. If last passed, use column 0.
	char b = matrix[second[0]][(second[1]+1)%5]; //"						"
	delete[] first;
	delete[] second;
	digraph complete(a,b,d.intervening);
	return complete;
}

encoder::encoder(char ignore, char * keyword): cipher(ignore, keyword){}
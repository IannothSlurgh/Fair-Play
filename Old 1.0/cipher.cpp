#include "cipher.h"

cipher::cipher(char ignore, char * keyword) //Dynamic memory to matrix. Fill with all alpha, but ignore.
{
	int * rc_pair = new int[2]; //For traversing matrix.
	(rc_pair[0])=(rc_pair[1])=0;
	matrix = new char*[5];
	bounds_error=false;
	for(int c = 0;c<5;++c) //Memory.
		matrix[c]=new char[5];
	used_alpha[ignore]=true; //Must not enter matrix.
	mat_keyword((&rc_pair),keyword);
	char * alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	mat_keyword(&rc_pair, alpha); //Fill matrix with remaining capital letters.
	delete[] rc_pair;
	if(ignore=='z'||ignore=='Z') //Set the uncommon letter for special cases.
	{
		uncommon='Q';
		uncommon2='X';
	}
	else
	{
		uncommon='Z';
		if(ignore=='x'||ignore=='X')
		{
			uncommon2='Q';
		}
		else
			uncommon2='X';
	}
}

cipher::~cipher()
{
	for(int d = 0;d<5;++d)
	{
		delete[] (matrix[d]);
	}
	delete[] matrix;
}

void cipher::mat_keyword(int ** rc_pair, char * keyword) //Adds keyword components into matrix.
{
	int size = strlen(keyword);
	for(int i = 0;i<size;++i)
	{
		if(used_alpha.count(keyword[i])==0)
		{
			used_alpha[keyword[i]]=true; //Becomes 'used' in matrix.
			matrix [(*rc_pair)[0]] [(*rc_pair)[1]]=keyword[i]; //Add each letter from keyword
			++((*rc_pair)[1]); //Increment column.
			if((*rc_pair)[1]>4) //End of row. Continue to next row, first column.
			{
				++((*rc_pair)[0]);
				((*rc_pair)[1])%=5;
			}
		}
	}
}

int* cipher::get_loc(char c) // Return row and column of character in matrix.
{
	int * rc_pair = new int[2];
	rc_pair[0]=rc_pair[1]=-1; //Default: not in matrix.
	for(int i=0;i<5;++i)
		for(int j=0;j<5;++j)
		{
			if(matrix[i][j]==c) //Success
			{
				rc_pair[0]=i;
				rc_pair[1]=j;
			}
		}
	return rc_pair;
}

digraph cipher::block(digraph d)
{
	int * first = get_loc(d.a);
	int * second = get_loc(d.b);
	char a = matrix[first[0]][second[1]]; //Same row, different column.
	char b = matrix[second[0]][first[1]]; //"			  "
	digraph complete(a, b, d.intervening);
	delete[] first;
	delete[] second;
	return complete;
}

char cipher::pick_uncommon(char c) //Helps alleviate a special case where a mono uncommon digraph forms.
{
	if(c==uncommon)
		return uncommon2;
	else
		return uncommon;
}

digraph cipher::manipulate(digraph d)
{
	if(d.mono) //If you ever run short a letter or have 2 of same letters, a monogram will pair with Q,X,Z
		d.b=pick_uncommon(d.a);
	int * first = get_loc(d.a);
	int * second = get_loc(d.b);
	if(first[0]==-1||second[0]==-1)
	{
		delete[] first;
		delete[] second;
		bounds_error=true;
		return d;
	}
	if(first[0]==second[0])
	{
			return this->horizontal(d);
	}
	if(first[1]==second[1])
		return this->vertical(d);
	else
		return block(d);
	//The fourth-case, that both letters are the same does not occur because of mono filtering.
	delete[] first;
	delete[] second;
	return d;
}
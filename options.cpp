#include "options.h"

bool options::process_option(char * input, char ** output)
{
	int size=strlen(input);
	if(size<2) //The input must be long enough to process.
		return false;
	if(size!=-1&&input[0]=='\'')
		if(input[size-1]=='\'') //if is enclosed in '' and was sufficiently short
		{
			char * temp = &(input[1]); //Temp points to cstring minus opening '
			char * processed = new char[size-1]; //new string without ' and '.
			strncpy(processed, temp, size-2); // Copies word minus ''.
			processed[size-2]=0; //add null termination.
			(*output)=processed; //Point options variable to newly truncated dynamic cstring.
			
		}
	return (*output)!=0; //If null, attempt to process failed, return false.
}

bool options::process_keyword(char * input, char ** output) //Check keyword for unique alpha chars. To upper.
{
	if(opignore==0) //Ignore will not register properly.
		return false;
	int finish = 25;
	int size = strlen(input);
	if(size!=strlen(*output)) //strncpy will do weird things with diff lengths.
		return false;
	if(size>finish) //cstring too long.
		return false;
	else //valid size
		finish=size;
	map <char, bool> m; //Holds alpha already seen in the keyword.
	m[opignore[0]]=true; //The ignored alpha must not appear in keyword.
	for(int i = 0;i<finish;++i)
	{
		if(input[i]>=97&&input[i]<=122) //To upper on lower case alpha
			input[i]-=32;
		if(input[i]>=65&&input[i]<=90) //Upper alpha only used once?
		{
			if(m.count(input[i])==0)
				m[input[i]]=true;
			else
				return false;
		}
		else
			return false; //Is a special char. Unacceptable.
	}
	strncpy(*output, input, size); //Copy data from the processed input to output.
	return true;
}

bool options::process_ignore(char * input, char ** output)
{
	if(input[0]>=97 && input[0]<=122)
		input[0]-=32;
	if(input[0]>=65 && input[0]<=90)
	{
		strcpy((*output),input);
		return true;
	}
	return false;
}

//Option is of form 1 for success.
void options::check_consent(char * c)
{
	successes[0] = (c[0]=='1');
}

//Option is of form -e -E -d -D for success.
void options::check_mode(char * m)
{
	bool isE = (m[1] == 'E' || m[1] == 'e');
	bool isD = (m[1] == 'D' || m[1] == 'd');
	bool isOption = (m[0] =='-');
	opmode = isD; //Decoding mode is 1, encoding is 0.
	successes[1] = isOption && (isE || isD);
}

void options::check_ignore(char * i)
{
	//Is appropriately sized? Has ''? Is alpha? If so, 1 capital alpha in opignore and success.
	if(strlen(i)==3 && process_option(i,&opignore))
	{
		successes[5]= process_ignore(opignore, &opignore);
	}
	else
	{
		//Not proper size? or perhaps wrong format?
		delete[] opignore;
		opignore = 0; //So that check_keyword may know that ignore failed.
		successes[5]=false;
	}		
}

void options::check_keyword(char * k) //prob
{
	if(process_option(k, &opkeyword))
	{
		successes[2]=process_keyword(opkeyword, &opkeyword);
			
	}
	else
		successes[2]=false;
}

void options::check_dest(char * d)
{
	if(process_option(d, &opdest)) //Loop above had success. Proper option structure was used.
	{
		//Test user write permissions and validity of filename.
		ofstream o(opdest);
		o<<"Testing...";
		successes[3]= o.good();
		o.close();
	}
	else
		successes[3]=false;
}

void options::check_source(char * s)//prob
{
	if(process_option(s, &opsource))
	{
		ifstream i(opsource);
		string s;
		i>>s;
		successes[4]=!(i.fail());
		i.close();
	}
	else
		successes[4]=false;
}

void options::check_status()
{
	bad = !(successes[0] && successes[1] && successes[2] && successes[3] && successes[4] && successes[5]);
}

options::options(char * mode, char * ignore, char * keyword, char * source, char * dest, char * consent)
{
	opdest=opsource=opkeyword=opignore=0; //Null the pointers for checks.
	//Nulled pointers for success checking.
	check_consent(consent);
	check_ignore(ignore);
	check_mode(mode);
	check_keyword(keyword);
	check_dest(dest);
	check_source(source);
	check_status();
}

options::~options()
{
	delete[] opdest;
	delete[] opsource;
	delete[] opkeyword;
	delete[] opignore;
}

//Verifies the use of help option.
bool options::option_h(char * o)
{
	if(o[0]=='-')
		if(o[1]=='h'||o[1]=='H')
		{
			options::print_help();
			return true;
		}
	return false;
}

//Prints menu in response to a use of -h
void options::print_help()
{
	cout<<"This program can encode and decode using a simple playfair cipher. To begin use,"
	<<" the user most provide 5 options, which are as follows: program mode, ignored character, " 
	<<"keyword, name of"
	<<" source file, name of destination file,"
	<<" and the number 1 to indicate that you have considered the considerable damage a poorly written"
	<<" program of unknown reputation can do to your computer."<<"\n\nModes:\n-E encodes the target"
	<<" file with your given keyword.\n-D decodes the target file with your given keyword."
	<<"\n\nKeywords:\nKeywords should contain unique letters.\nKeywords should be enclosed in single"
	<<" quotation marks.\n\nFilenames:\nThe filenames must be enclosed in single quotation marks.\n"
	<<"Read/write permissions must be granted for this program to function.\n\n"
	<<"Example: playfair.exe -E \'Q\' \'JUICE\' \'input.txt\' \'output.txt\' 1"
	<<endl;
}

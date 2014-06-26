#include "gui.h"
#include <iostream>

extern int app_state;

using namespace std;

//HELPER FUNCTIONS

string insert_num(const string s, int num) //This function should work on all platforms as opposed to itoa.
{
	ostringstream itoa;
	itoa<<num;
	string converted_num = itoa.str();//Integer converted into character format.
	bool success = false; //Did function find %i?
	int size = s.size();
	string result;
	for(int loc = 0; loc<size-1; ++loc)
	{
		if(s.at(loc)=='%'&&s.at(loc+1)=='i')
		{
			success=true;
			result=s.substr(0,loc); //Take all char before %i
			result.append(converted_num); //Put in number
			if( (loc+2) < size)
				result.append(s,loc+2,string::npos); //Put in all char after %i
			loc=size-1; //Terminate loop.
		}
	}
	return (success) ? result : s;
}

void clear_argv(char ** old)
{
	for(int i=0;i<5;++i)
		delete[] old[i];
	delete[] old;
}

bool is_ignore_in(char * input, char ignore) //Return to later.
{
	ignore=toupper(ignore);
	ifstream i(input);
	char comparable=0;
	while(!i.eof())
	{
		comparable = toupper(i.get());
		if(ignore==comparable)
			return true;
	}
	return false;
}

bool is_bad_input(char * input)
{
	ifstream i(input);
	char test = -1;
	test=i.get();
	if(test==-1)
		return true;
	return false;
}

bool is_bad_output(char * output)
{
	ofstream o(output);
	o<<"Testing...";
	if(o.good())
		return false;
	return true;
}

bool is_bad_keyword(char * keyword, char ignore)
{
	map <char, bool> m;
	m[toupper(ignore)]=true;
	int length = strlen(keyword);
	char current = 0;
	for(int i = 0; i<length; ++i)
	{
		current = toupper(keyword[i]);
		if(tolower(current) == current) //Nonalpha.
			return true;
		if(m.count(current)==0) //Repeating Character.
			m[current]=true;
		else 
			return true;
	}
	return false;
}

//MESSAGE PROCESSING

LRESULT CALLBACK main_procedure(HWND main_wnd_instance, UINT msg, WPARAM param_w, LPARAM param_l)
{
	switch(msg)
	{
		case WM_CREATE:
			SetMenu(main_wnd_instance, init_main_menu(main_wnd_instance));
			if(check_error())
			{
				gui_error();
				PostMessage(main_wnd_instance, WM_CLOSE, 0, 0);
			}
		break;
		case WM_CLOSE:
			DestroyWindow(main_wnd_instance);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		case WM_COMMAND:
			switch(LOWORD(param_w))
			{
				case QUIT_MESSAGE:
					PostMessage(main_wnd_instance, WM_CLOSE, 0, 0);
				break;
				case RUN_MESSAGE:
				{
					char ** raw = get_argv(main_wnd_instance);
					char ** processed = fix_argv(raw);
					char * consent = "1";
					string success_msg = "Operations complete. If settings were not validated, no guarantee is made of their success.";
					string err_msg = "Program could not run. Please validate settings and input";
					options o(processed[0], processed[1], processed[2], 
					processed[3], processed[4], consent);
					if(!o.bad)
					{
						playfair p(o);
						p.execute();
					}
					else
						MessageBox(main_wnd_instance, err_msg.c_str(), 
						"Fair Play Run", MB_OK| MB_ICONERROR);
					clear_argv(raw);
					clear_argv(processed);
					if(!o.bad)
						MessageBox(main_wnd_instance, success_msg.c_str(), 
						"Fair Play Run", MB_OK);
				}
				break;
				case VALIDATE_MESSAGE: //Generate options as per usual, check input for ignore.
				{
					char ** raw = get_argv(main_wnd_instance);
					char ** processed = fix_argv(raw);
					int error_state= validate(raw, processed);
					field_errors(main_wnd_instance, error_state);
					clear_argv(raw);
					clear_argv(processed);
				}
				break;
				case ABOUT_MESSAGE:
				{
					string about_txt = "Version 1.0 By Ian 'Annoth' DaCosta";
					MessageBox(main_wnd_instance, about_txt.c_str(), "About", 
					MB_ICONINFORMATION);
				}
				break;
				default:
				break;
			}
		break;
		default:
			return DefWindowProc(main_wnd_instance, msg, param_w, param_l);
	}
	return 0;
}

int message_loop()
{
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//ERROR HANDLING

bool check_error()
{
	return (GetLastError()!=0);
}

void gui_error()
{
	int err_code=GetLastError();
	app_state=err_code;
	string err_msg = "Error Code: %i";
	err_msg=insert_num(err_msg, err_code);
	MessageBox(NULL, err_msg.c_str(), "GUI Failure", MB_ICONEXCLAMATION);
}

void field_errors(HWND main_wnd_instance, int error_state)
{
	string message = "";
	if(error_state==0)
		message.append("The settings have been validated. No problems were detected.");
	else
		message.append("The settings were not valid. The error list is as follows:\n");
	if( (error_state^ERR_MISSING_FIELD)<error_state)
		message.append("One or more settings were not selected.\n");
	if( (error_state^ERR_BAD_INPUT)<error_state )
		message.append("Input file not found.\n");
	if( (error_state^ERR_NONALPHA_IGNORE)<error_state )
		message.append("The ignore should be a letter of Latin origin.\n");
	if( (error_state^ERR_CANNOT_OUTPUT)<error_state )
		message.append("Not able to output file. Check memory or permissions.\n");
	if( (error_state^ERR_BAD_KEYWORD)<error_state )
		message.append("The keyword should not have the ignore, special characters, or multiple letters of the same kind.\n");
	if( (error_state^ERR_IGNORE_PRESENT)<error_state )
		message.append("The ignore must not be in the input file.");
	MessageBox(main_wnd_instance, message.c_str(), "Validation", MB_ICONINFORMATION);		
	
}

//INITIALIZATION

ATOM reg_main(HINSTANCE app_instance)
{
	WNDCLASSEX main;
	main.cbSize=sizeof(WNDCLASSEX);
	main.cbClsExtra=0;
	main.cbWndExtra=0;
	main.hInstance=app_instance;
	main.lpfnWndProc=main_procedure;
	main.lpszClassName="PlayfairMain";
	main.style=0;
	main.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	main.hIcon = NULL;
	main.hIconSm = NULL;
	main.lpszMenuName = NULL;
	main.hCursor = NULL;
	return RegisterClassEx(&main);
}

HWND init_main_window(HINSTANCE app_instance)
{
	HWND handle_main=NULL;
	handle_main = CreateWindowEx(WS_EX_CLIENTEDGE, "PlayfairMain", "Fair Play", WS_MINIMIZEBOX | 
	WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, app_instance, NULL);
	if(handle_main==NULL)
	{
		gui_error();
		return NULL;
	}
	ShowWindow(handle_main, SW_SHOWNORMAL);
	UpdateWindow(handle_main);
	return handle_main;
}

HMENU init_main_menu(HWND main_wnd_instance)
{
	HMENU menu_instance = CreateMenu();
	HMENU submenu = CreatePopupMenu();
	AppendMenu(submenu, MF_STRING, QUIT_MESSAGE, "&Quit");
	AppendMenu(submenu, MF_STRING, RUN_MESSAGE, "&Run");
	AppendMenu(submenu, MF_STRING, VALIDATE_MESSAGE, "&Validate");
	AppendMenu(submenu, MF_STRING, ABOUT_MESSAGE, "&About");
	AppendMenu(menu_instance, MF_STRING | MF_POPUP, (UINT_PTR) submenu, "&File");
	return menu_instance;
}

HWND init_keyword(HWND main_wnd_instance)
{
	HWND handle_keyword=NULL;
	handle_keyword = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 
	25, 50, 100, 25, main_wnd_instance, (HMENU)CHILD_KEYWORD, GetModuleHandle(NULL), NULL);
	if(handle_keyword==NULL)
		gui_error();
	else
		PostMessage(handle_keyword, EM_LIMITTEXT, 25, 0);
	return handle_keyword;
}

HWND init_ignore(HWND main_wnd_instance)
{
	HWND handle_ignore=NULL;
	handle_ignore = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE, 
	190, 50, 20, 25, main_wnd_instance, (HMENU)CHILD_IGNORE, GetModuleHandle(NULL), NULL);
	if(handle_ignore==NULL)
		gui_error();
	else
		PostMessage(handle_ignore, EM_LIMITTEXT, 1, 0);
	return handle_ignore;
}

HWND init_input(HWND main_wnd_instance)
{
	HWND handle_input=NULL;
	handle_input = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
	25, 125, 100, 25, main_wnd_instance, (HMENU)CHILD_INPUT, GetModuleHandle(NULL), NULL);
	if(handle_input==NULL)
		gui_error();
	return handle_input;
}

HWND init_output(HWND main_wnd_instance)
{
	HWND handle_output=NULL;
	handle_output = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
	150, 125, 100, 25, main_wnd_instance, (HMENU)CHILD_OUTPUT, GetModuleHandle(NULL), NULL);
	if(handle_output==NULL)
		gui_error();
	return handle_output;
}

HWND init_mode(HWND main_wnd_instance) //Fix values
{
	HWND handle_mode=NULL;
	handle_mode = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE |
	BS_GROUPBOX, 30, 175, 200, 50, main_wnd_instance, (HMENU)CHILD_MODE, GetModuleHandle(NULL), NULL);
	SetWindowText(handle_mode, "Mode Selection");
	if(handle_mode == NULL)
		gui_error();
	return handle_mode;
}

HWND init_encode(HWND main_wnd_instance)
{
	HWND handle_mode=GetDlgItem(main_wnd_instance, CHILD_MODE);
	HWND handle_encode=NULL;
	handle_encode = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON
	| BS_LEFTTEXT, 10, 25, 80, 15, handle_mode, (HMENU)CHILD_ENCODE, GetModuleHandle(NULL), NULL);
	SetWindowText(handle_encode, "Encoding");
	if(handle_encode == NULL)
		gui_error();
	return handle_encode;
}

HWND init_decode(HWND main_wnd_instance)
{
	HWND handle_mode=GetDlgItem(main_wnd_instance, CHILD_MODE);
	HWND handle_decode=NULL;
	handle_decode = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON
	| BS_LEFTTEXT, 110, 25, 80, 15, handle_mode, (HMENU)CHILD_ENCODE, GetModuleHandle(NULL), NULL);
	SetWindowText(handle_decode, "Decoding");
	if(handle_decode == NULL)
		gui_error();
	return handle_decode;	
}

HWND init_text_output(HWND main_wnd_instance)
{
	HWND handle_text_output=NULL;
	handle_text_output= CreateWindowEx(0, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_CENTER, 140, 100, 120,
	20, main_wnd_instance, (HMENU)CHILD_TEXT_OUTPUT, GetModuleHandle(NULL), NULL);
	SetWindowText(handle_text_output, "Output Filename");
	if(handle_text_output==NULL)
		gui_error();
	return handle_text_output;
}

HWND init_text_input(HWND main_wnd_instance)
{
	HWND handle_text_input=NULL;
	handle_text_input= CreateWindowEx(0, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 100, 120,
	20, main_wnd_instance, (HMENU)CHILD_TEXT_INPUT, GetModuleHandle(NULL), NULL);
	SetWindowText(handle_text_input, "Input Filename");
	if(handle_text_input==NULL)
		gui_error();
	return handle_text_input;
}

HWND init_text_keyword(HWND main_wnd_instance)
{
	HWND handle_text_keyword=NULL;
	handle_text_keyword= CreateWindowEx(0, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_CENTER, 35, 25, 80,
	20, main_wnd_instance, (HMENU)CHILD_TEXT_KEYWORD, GetModuleHandle(NULL), NULL);
	SetWindowText(handle_text_keyword, "Keyword");
	if(handle_text_keyword==NULL)
		gui_error();
	return handle_text_keyword;
}

HWND init_text_ignore(HWND main_wnd_instance)
{
	HWND handle_text_ignore=NULL;
	handle_text_ignore= CreateWindowEx(0, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_CENTER, 160, 25, 70,
	20, main_wnd_instance, (HMENU)CHILD_TEXT_IGNORE, GetModuleHandle(NULL), NULL);
	SetWindowText(handle_text_ignore, "Ignore");
	if(handle_text_ignore==NULL)
		gui_error();
	return handle_text_ignore;
}

//FUNCTIONALITY
char** get_argv(HWND main_wnd_instance) //Get preprocessed data from children. Delete the returned value!
{
	//Get handles of user input boxes.
	HWND handle_mode = GetDlgItem(main_wnd_instance, CHILD_MODE);
	HWND handle_ignore = GetDlgItem(main_wnd_instance, CHILD_IGNORE);
	HWND handle_keyword = GetDlgItem(main_wnd_instance, CHILD_KEYWORD);
	HWND handle_input = GetDlgItem(main_wnd_instance, CHILD_INPUT);
	HWND handle_output = GetDlgItem(main_wnd_instance, CHILD_OUTPUT);

	//Allocate memory to matrix.
	char ** raw = new char*[5];
	int size_ignore, size_keyword, size_input, size_output;
	size_ignore = GetWindowTextLength(handle_ignore)+1;
	size_keyword = GetWindowTextLength(handle_keyword)+1;
	size_input = GetWindowTextLength(handle_input)+1;
	size_output = GetWindowTextLength(handle_output)+1;
	raw[0]= new char[3];
	raw[1]= new char[size_ignore];
	raw[2]= new char[size_keyword];
	raw[3]= new char[size_input];
	raw[4]= new char[size_output];
	
	//Fill with content.
	if(IsDlgButtonChecked(handle_mode, CHILD_ENCODE)==BST_CHECKED)
		raw[0][1]='E';
	else
		if(IsDlgButtonChecked(handle_mode, CHILD_DECODE)==BST_UNCHECKED)
			raw[0][1]='D';
		else
			raw[0][1]='N';
	raw[0][0]='-';
	raw[0][2]=0;
	GetWindowText(handle_ignore, raw[1], size_ignore); //GetWindowText is being used to autocopy null.
	GetWindowText(handle_keyword, raw[2], size_keyword);
	GetWindowText(handle_input, raw[3], size_input);
	GetWindowText(handle_output, raw[4], size_output);
	return raw;
}

char ** fix_argv(char ** raw) //Used on get_argv. Delete the returned value. Do it!
{
	char ** processed = new char*[5];
	for(int i=1;i<5;++i)
	{
		int length=strlen(raw[i]);
		processed[i]=new char[length+3];
		processed[i][0]='\''; //Open quote.
		char * temp = &(processed[i][1]);
		strncpy(temp, raw[i], length); //Copy content infront of the open quote.
		temp[length+1]=0; //Terminal null at length+3-1-1 (to index, shorter bc of temp)
		temp[length]='\''; //Close quote at length+3-1-1-1 (to index, back one, temp shortening)
	}
	processed[0]=new char[3]; //Index 0 is mode which was already processed in get_argv. Always size 3.
	strcpy(processed[0], raw[0]);
	return processed;
}

int validate(char ** raw, char ** processed)
{
	//Check if all fields filled.
	bool missing_field=false;
	if(raw[0][1]=='N')
		missing_field=true;
	for(int i = 1; i<5;++i)
	{
		if(strlen(raw[i])==0)
			missing_field=true;
	}
	bool bad_input=false;
	if(strlen(raw[3])>0)
		bad_input=is_bad_input(raw[3]);
	bool bad_output=false;
	if(strlen(raw[4])>0)
		bad_output=is_bad_output(raw[4]);
	bool bad_ignore = false;
	if(strlen(raw[1])==1 && (toupper(raw[1][0])==tolower(raw[1][0])) ) //Filled field with nonalpha.
		bad_ignore=true;
	bool bad_keyword = false;
	if(strlen(raw[2])>0 && strlen(raw[1])==1)
		bad_keyword = is_bad_keyword(raw[2], raw[1][0]);
	bool ignore_present=false;
	if( (strlen(raw[3])>0) && ( strlen(raw[1])==1) ) //Input filled, single character in ignore.
		ignore_present = is_ignore_in(raw[3], raw[1][0]);
	int final = 0 | (ERR_MISSING_FIELD * missing_field) | (ERR_BAD_INPUT * bad_input) |
	(ERR_NONALPHA_IGNORE * bad_ignore) | (ERR_CANNOT_OUTPUT * bad_output) | (ERR_BAD_KEYWORD
	* bad_keyword) | (ERR_IGNORE_PRESENT * ignore_present); //all valid errors will be stored binarily.
	return final;
}
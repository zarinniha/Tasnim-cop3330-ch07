/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Zarin Tasnim
 */

#include "std_lib_facilities.h"
using namespace std;

class Token {
public:
    
	char kind;  
	double value;
	string name;

	Token(char ch) :kind{ch} { }
	Token(char ch, double val) :kind{ch}, value{val} { }
	Token(char ch, string n) : kind{ch}, name{n} { }
};

class Token_stream {
public:
    Token_stream() :full{false}, buffer{0} { }
	Token get();
	void putback(Token t) { buffer=t; full=true; }
	void ignore(char c);
private:
	bool full;
	Token buffer;
};

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';

const string declkey = "let";
const string quitkey = "quit";

Token Token_stream::get()
{
   
	if (full) { full=false; return buffer; }

	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
		return Token{ch};  
	case '.':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':  
	{	cin.unget();
		double val;
		cin >> val;
		return Token{number, val};
	} 
	default:
	 
		if (isalpha(ch)) {
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) s += ch;
			cin.unget();
			if (s == declkey) return Token{let};	
			if (s == quitkey) return Token{quit};
			return Token{name,s};
		}
		error("Bad token");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

   
	char ch{' '};
    while (ch != c && ch != '\n')
        ch = cin.get();
    return;
}

class Variable {
public:
	string name;
	double value;
};

vector<Variable> var_table;	

double get_value(string s)
{
	for (const Variable& v : var_table)
	    if (v.name == s) return v.value;
	error("get: undefined name ", s);
}

void set_value(string s, double d)
{
    for (Variable& v : var_table)
        if (v.name == s) {
            v.value = d;
            return;
        }
	error("set: undefined name ", s);
}

bool is_declared(string s)
{
	for (const Variable& v : var_table)
	    if (v.name == s) return true;
	return false;
}

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	double d = expression();
		t = ts.get();
		if (t.kind != ')') error("')' expected");
		return d;
	}
	case '-':   
		return - primary();
	
	case '+':   
		return + primary();
	case number:
		return t.value;
    case name:  
		return get_value(t.name);
	default:
		error("primary expected");
	}
}

double term()
{
	double left = primary();
	while(true) {
		Token t = ts.get();
		switch(t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
			if (d == 0) error("divide by zero");
			left /= d;
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	while(true) {
		Token t = ts.get();
		switch(t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration()
{
	Token t = ts.get();

	if (t.kind != name) error ("name expected in declaration");
	string var_name = t.name;
	if (is_declared(var_name)) error(var_name, " declared twice");

	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of " ,var_name);

	double d = expression();
	var_table.push_back(Variable{var_name,d});
	return d;
}

double statement()
{
	Token t = ts.get();
	switch(t.kind) {
	case let:
		return declaration();
	default:
		ts.putback(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);   
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while(true) 
	try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();   
		if (t.kind == quit) return;
		ts.putback(t);
		cout << result << statement() << '\n';
	}
	catch(exception& e) {
		cerr << e.what() << '\n';
		clean_up_mess();    
	}
}

int main()
try {
    calculate();
    return 0;
}
catch (exception& e) {
    cerr << "exception: " << e.what() << '\n';
    return 1;
}
catch (...) {
    cerr << "Uknown exception!\n";
	return 2;
}
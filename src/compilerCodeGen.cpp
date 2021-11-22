//============================================================================
// Name        : compilerCodeGen.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

// You will need these forward references.
class Expr;
class Stmt;

// Runtime Global Variables
int pc;  				// program counter
vector<string> lexemes;
vector<string> tokens;
vector<string>::iterator lexitr;
vector<string>::iterator tokitr;
map<string, int> vartable; 		// map of variables and their values
vector<Stmt *> insttable; 		// table of instructions
map<string, string> symboltable; 	// map of variables to datatype (i.e. sum t_integer)


// Runtime Global Methods
void dump(){
	// prints vartable, insttable, symboltable
	// BOTH
	map<string, string>::iterator sitr;
	map<string, int>::iterator vitr;

	cout << "\nsymboltable:" << endl;
	for(sitr = symboltable.begin(); sitr != symboltable.end(); sitr++) {
		cout << sitr->first << " " << sitr->second << endl;
	}

	cout << "\nvartable:" << endl;
	for(vitr = vartable.begin(); vitr != vartable.end(); vitr++) {
		cout << vitr->first << " " << vitr->second << endl;
	}
}


// You may need a few additional global methods to manipulate the global variables


// Classes Stmt and Expr
// It is assumed some methods in statement and expression objects will change and
// you may need to add a few new ones.


class Expr{ // expressions are evaluated!
public:
	virtual int eval() = 0;
	virtual string toString() = 0;
	virtual ~Expr(){}
};

class ConstExpr : public Expr{
	// DYLAN HARPER
private:
	int value;
public:
	ConstExpr(int val);
	int eval();
	string toString();
};

class IdExpr : public Expr{
	// KEEGAN COLLINS
private:
	string id;
public:
	IdExpr(string s);
	int eval();
	string toString();
};

class InFixExpr : public Expr{
	// BOTH
private:
	vector<Expr *> exprs;
	vector<string> ops;  // tokens of operators
public:
	~InFixExpr();
	int eval();
	string toString();
};

class Stmt{ // statements are executed!
private:
	string name;
public:
	Stmt(){}
	Stmt(string n):name(n){}
	void setNamer(string n){name = n;}
	string getName(){return name;}
	virtual ~Stmt(){};
	virtual string toString() = 0;
	virtual void execute() = 0;
};

class AssignStmt : public Stmt{
	// KEEGAN COLLINS
private:
	string var;
	Expr* p_expr;
public:
	AssignStmt(string n, string v, Expr* e): Stmt(n), var(v), p_expr(e){}
	void setVar(string v){var = v;}
	string getVar(){return var;}
	void setExp(Expr* e){p_expr = e;}
	Expr* getExp(){return p_expr;}
	~AssignStmt();
	string toString();
	// used in data dump
	void execute();
	// executes the statement (changes contents of variable).
	//var = p_expr;
};

class InputStmt : public Stmt{
	// DYLAN HARPER
private:
	string var;
public:
	InputStmt(string n, string v):Stmt(n), var(v){}
	~InputStmt(){};
	string toString(){};
	void execute(){};
};

class StrOutStmt : public Stmt{
	// DYLAN HARPER
private:
	string value;
public:
	StrOutStmt();
	~StrOutStmt();
	string toString();
	void execute();
};

class ExprOutStmt : public Stmt{
	// KEEGAN COLLINS
private:
	Expr* p_expr;
public:
	ExprOutStmt();
	~ExprOutStmt();
	string toString();
	void execute();
};

class IfStmt : public Stmt{
	// KEEGAN COLLINS
private:
	Expr* p_expr;
	int elsetarget;
public:
	IfStmt();
	~IfStmt();
	string toString();
	void execute();
};

class WhileStmt : public Stmt{
	// DYLAN HARPER
private:
	Expr* p_expr;
	int elsetarget;
public:
	WhileStmt();
	~WhileStmt();
	string toString();
	void execute();
};

class GoToStmt: public Stmt{
	// DYLAN HARPER
private:
	int elsetarget;
public:
	GoToStmt();
	~GoToStmt();
	string toString();
	void execute();
};

class Compiler{
	// BOTH
private:
	void buildIf();
	void buildWhile();
	void buildStmt();
	void buildAssign();
	void buildInput();
	void buildOutput();
	// use one of the following buildExpr methods
	void buildExpr(Expr*&);      Expr* buildExpr();

	// headers for populate methods may not change
	void populateTokenLexemes(istream& infile){
	    string tok, lex;
	    infile >> tok >> lex;
	    while(!infile.eof()){
	        tokens.push_back(tok);
	        lexemes.push_back(lex);
	        infile >> tok >> lex;
	    }
	    tokitr = tokens.begin();
	    lexitr = lexemes.begin();
	}
	void populateSymbolTable(istream& infile){
		string vari, tok;
		infile >> vari >> tok;
		while (!infile.eof()){
			symboltable[vari] = tok;
			infile >> vari >> tok;
		}
	}
public:
	// headers may not change
	Compiler(istream& source, istream& symbols){
		populateTokenLexemes(source);
		populateSymbolTable(symbols);
	}

	// The compile method is responsible for getting the instruction
	// table built.  It will call the appropriate build methods.
	bool compile(){}
		// DYLAN HARPER

	// The run method will execute the code in the instruction table
	void run(){}
		// KEEGAN COLLINS
};

void Compiler::buildAssign(){
	//AssignStmt a;

}

void Compiler::buildInput(){
	string v = *lexitr;
	InputStmt* i = new InputStmt("t_input", v);
	insttable.push_back(i);
	tokitr++; lexitr++;
}

int main(){
	// BOTH
	ifstream infile1("source1.txt");
	ifstream infile2("symbol1.txt");
	if (!infile1 || !infile2) exit(-1);
	Compiler c(infile1, infile2);
	c.compile();
	c.run();
	dump();
	return 0;
}

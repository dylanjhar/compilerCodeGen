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
#include <stack>
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
	vector<Stmt *>::iterator inst;

	cout << "\nvartable:" << endl;
	for(vitr = vartable.begin(); vitr != vartable.end(); vitr++) {
		cout << vitr->first << " " << vitr->second << endl;
	}

	cout << "\ninsttable:" << endl;
	for(inst = insttable.begin(); inst != insttable.end(); inst++) {
		cout << *inst << endl;
	}

	cout << "\nsymboltable:" << endl;
	for(sitr = symboltable.begin(); sitr != symboltable.end(); sitr++) {
		cout << sitr->first << " " << sitr->second << endl;
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
	ConstExpr(int val):value(val){}
	int eval(){return value;}
	string toString(){return "ConstExpr: " + to_string(value);}
};

class IdExpr : public Expr{
	// KEEGAN COLLINS
private:
	string id;
public:
	IdExpr(string i):id(i){}
	int eval(){
		int value = -1;
		map<string, int>::iterator vitr;
		for(vitr = vartable.begin(); vitr != vartable.end(); vitr++) {
			if (vitr->first == id){
				int value = vitr->second;
			}
		}
		return value;
	}
	string toString(){return "IdExpr: " + id;}
};

class InFixExpr : public Expr{
	// BOTH
private:
	vector<Expr *> exprs;
	vector<string> ops;  // tokens of operators
	int exprValue(int i);
public:
	InFixExpr(){}
	~InFixExpr(){for(int i = 0; i < exprs.size(); i++){delete exprs[i];}}
	void addExpr(Expr* e){exprs.push_back(e);}
	void addOp(string o){ops.push_back(o);}
	int eval();
	string toString(){return "InFixExpr: ";}
};

int InFixExpr::exprValue(int i){
	int value;

	ConstExpr* cptr = dynamic_cast<ConstExpr*>(exprs[i]);
	if(cptr != nullptr){
		value = cptr->eval();
	} else{
		IdExpr* iptr = dynamic_cast<IdExpr*>(exprs[i]);
		value = iptr->eval();
	}
	return value;
}

int InFixExpr::eval(){
	int x = 0;
	int num = exprValue(0);
	for(int i = 1; i < exprs.size(); i++){
		if(ops[x] == "s_plus"){
			num += exprValue(i);
		} else if(ops[x] == "s_minus"){
			num -= exprValue(i);
		} else if(ops[x] == "s_mult"){
			num *= exprValue(i);
		} else if(ops[x] == "s_div"){
			num /= exprValue(i);
		} else if(ops[x] == "s_mod"){
			num %= exprValue(i);
		} else if(ops[x] == "s_lt"){
			if(num < exprValue(i)){
				num = 1;
			} else{
				num = 0;
			}
		} else if (ops[x] == "s_le"){
			if (num <= exprValue(i)){
				num = 1;
			} else{
				num = 0;
			}
		} else if (ops[x] == "s_gt"){
			if (num > exprValue(i)){
				num = 1;
			} else{
				num = 0;
			}
		} else if (ops[x] == "s_ge"){
			if (num >= exprValue(i)){
				num = 1;
			} else{
				num = 0;
			}
		} else if (ops[x] == "s_eq"){
			if (num == exprValue(i)){
				num = 1;
			} else{
				num = 0;
			}
		} else if (ops[x] == "s_ne"){
			if (num != exprValue(i)){
				num = 1;
			} else{
				num = 0;
			}
		}
		x++;
	}
	return num;
}

class Stmt{ // statements are executed!
private:
	string name;
public:
	Stmt(){}
	Stmt(string n):name(n){}
	void setNamer(string n){name = n;}
	string getName(){return name;}
	virtual ~Stmt(){}
	virtual string toString() = 0;
	virtual void execute() = 0;
};

class AssignStmt : public Stmt{
	// KEEGAN COLLINS
private:
	string var;
	Expr* p_expr;
public:
	AssignStmt(string n, string v, Expr* e): Stmt(n), var(v), p_expr(e){};
	~AssignStmt(){delete p_expr;}
	string toString(){return "s_assign : " + var + " ";}
	// used in data dump
	void execute(){
		vartable.insert({var, 0});
	}
	// executes the statement (changes contents of variable).
};

class InputStmt : public Stmt{
	// DYLAN HARPER
private:
	string var;
public:
	InputStmt(string v):Stmt("t_input"), var(v){}
	~InputStmt(){}
	string toString(){return "t_input " + var;}
	void execute(){
		cout << "enter value for variable " << var << endl;
		cin >> vartable[var];
	}
};

class StrOutStmt : public Stmt{
	// DYLAN HARPER
private:
	string value;
public:
	StrOutStmt(string v):Stmt("t_output"), value(v){}
	~StrOutStmt(){}
	string toString(){return "t_output " + value;}
	void execute(){cout << value << endl;}
};

class ExprOutStmt : public Stmt{
	// KEEGAN COLLINS
private:
	Expr* p_expr;
public:
	ExprOutStmt(string x, Expr* e):Stmt(x), p_expr(e){};
	~ExprOutStmt(){delete p_expr;}
	string toString(){return "t_ExprOutStmt : ";}
	void execute(){cout << p_expr << endl;}
};

class IfStmt : public Stmt{
	// KEEGAN COLLINS
private:
	Expr* p_expr;
	int elsetarget;
public:
	IfStmt(string i, Expr* e):Stmt(i), p_expr(e), elsetarget(-1){};
	~IfStmt(){delete p_expr;}
	void setElseTarget(int e){elsetarget = e;}
	string toString(){return "t_if: ";}
	void execute(){
		if (p_expr){
			tokitr++; lexitr++;
		}
		else{
			while (*tokitr != "t_end"){
				tokitr++, lexitr++;
			}
		}
	}
};

class WhileStmt : public Stmt{
	// DYLAN HARPER
private:
	Expr* p_expr;
	int elsetarget;
public:
	WhileStmt(Expr* e):Stmt("t_while"), p_expr(e), elsetarget(-1){}
	~WhileStmt(){delete p_expr;}
	string toString(){return "t_while: " + p_expr->toString() + "else: " + to_string(elsetarget);}
	void setElseTarget(int e){elsetarget = e;}
	int getElseTarget(){return elsetarget;}
	void execute(){if(p_expr->eval() == 0){pc = elsetarget;}
	}
};

class GoToStmt: public Stmt{
	// DYLAN HARPER
private:
	int elsetarget;
public:
	GoToStmt():Stmt("goto"), elsetarget(-1){}
	~GoToStmt(){}
	string toString(){return "goto: " + to_string(elsetarget);}
	void setElseTarget(int e){elsetarget = e;}
	int getElseTarget(){return elsetarget;}
	void execute(){pc = elsetarget;}
};

class Compiler{
	// BOTH
private:
	void buildIf();
	void buildWhile(); //DYLAN HARPER
	void buildGoto();
	void buildAssign();
	void buildInput(); //DYLAN HARPER
	void buildOutput();
	// use one of the following buildExpr methods
	Expr* buildExpr();

	//void buildExpr(Expr*&);

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
	bool compile(){
		// DYLAN HARPER
		pc = 0;
		stack<int> ifstmt;
		stack<int> whilestmt;
		tokitr = tokens.begin();
		lexitr = lexemes.begin();

		while(*tokitr != "t_begin"){
			tokitr++; lexitr++;
		}
		tokitr++; lexitr++;

		while(tokitr != tokens.end() && pc != -1){
			if(*tokitr == "t_if"){
				buildIf();
				ifstmt.push(pc);
			} else if(*tokitr == "t_while"){
				buildWhile();
				ifstmt.push(pc);
			} else if(*tokitr == "s_assign"){
				buildAssign();
			} else if(*tokitr == "t_output"){
				buildOutput();
			} else if(*tokitr == "t_input"){
				buildInput();
			} else if(*tokitr == "t_end"){
				tokitr++; lexitr++;
				if(tokitr != tokens.end()){
					if(*tokitr == "t_loop"){
						buildGoto();
						GoToStmt* goptr = dynamic_cast<GoToStmt*>(insttable[pc]);
						if(goptr != nullptr){
							goptr->setElseTarget(whilestmt.top());
						} else{
							pc = -1;
						}
						WhileStmt* wptr = dynamic_cast<WhileStmt*>(insttable[whilestmt.top()]);
						if(wptr != nullptr){
							wptr->setElseTarget(pc + 1);
							whilestmt.pop();
						} else{
							pc = -1;
						}
					} else{
						IfStmt* ifptr = dynamic_cast<IfStmt*>(insttable[ifstmt.top()]);
						if(ifptr != nullptr){

							ifstmt.pop();
						} else{
							pc = -1;
						}
					}
				}
			}
			pc++;
		}
		if(pc == -1){
			return false;
		}
		return true;
	}


	// The run method will execute the code in the instruction table
	void run(){
		// KEEGAN COLLINS
		vector<Stmt *>::iterator inst;
		/*
		for(inst = insttable.begin(); inst != insttable.end(); inst++) {
			if (insttable[inst] == ){

			}
			else if (insttable[inst] == ){

			}
			else if (insttable[inst] == ){

			}
			else if (insttable[inst] == ){

			}
			else if (insttable[inst] == ){

			}
			else if (insttable[inst] == ){

			}
		}
		*/
	}

};

void Compiler::buildAssign(){
	tokitr--; lexitr--;
	string n = "s_assign";
	string v = *lexitr;
	tokitr++; lexitr++;
	tokitr++; lexitr++;
	Expr* e = buildExpr();
	if((symboltable[v] == "t_integer" && !isdigit(e->eval()))
			|| (symboltable[v] == "t_str" && isdigit(e->eval()))){
		cout << "Data types do not match: " << v << endl;
		pc = -1;
	}
	AssignStmt* a = new AssignStmt(n, v, e);
	insttable.push_back(a);
	tokitr++; lexitr++;
}
void Compiler::buildIf(){
	tokitr++; lexitr++;
	tokitr++; lexitr++;
	string n = "t_if";
	Expr* e = buildExpr();
	IfStmt* i = new IfStmt(n, e);
	insttable.push_back(i);
	tokitr++; lexitr++;
}
void Compiler::buildWhile(){
	tokitr++; lexitr++;
	tokitr++; lexitr++;
	Expr* e = buildExpr();
	WhileStmt* w = new WhileStmt(e);
	insttable.push_back(w);
	tokitr++; lexitr++;
	tokitr++; lexitr++;
}

void Compiler::buildGoto(){
	GoToStmt* g = new GoToStmt();
	insttable.push_back(g);
	tokitr++; lexitr++;
}

void Compiler::buildInput(){
	tokitr++; lexitr++;
	tokitr++; lexitr++;
	InputStmt* i = new InputStmt(*lexitr);
	insttable.push_back(i);
	tokitr++; lexitr++;
	tokitr++; lexitr++;
}

void Compiler::buildOutput(){
	tokitr++; lexitr++;
	tokitr++; lexitr++;
	if(*tokitr == "t_str"){
		StrOutStmt* s = new StrOutStmt(*lexitr);
		insttable.push_back(s);
	} else {
		string n = "t_output";
		Expr* e = buildExpr();
		ExprOutStmt* o = new ExprOutStmt(n, e);
		insttable.push_back(o);
	}
	tokitr++; lexitr++;
}

//Only need one
Expr* Compiler::buildExpr(){
	Expr* expr;
	InFixExpr* inExpr;
	tokitr++;
	if(*tokitr == "s_rparen" || *tokitr == "s_semi"){
		tokitr--;
		if(*tokitr == "t_int"){
			expr = new ConstExpr(stoi(*lexitr));
		} else if(*tokitr == "t_id"){
			expr = new IdExpr(*lexitr);
		}
		tokitr++; lexitr++;
	} else{
		tokitr--;
		inExpr = new InFixExpr();
		while(*tokitr != "s_semi" && *tokitr != "s_rparen"){
			if(*tokitr == "t_int"){
				Expr* x = new ConstExpr(stoi(*lexitr));
				inExpr->addExpr(x);
			} else if(*tokitr == "t_id"){
				if(symboltable[*lexitr] == "t_integer"){
					Expr* y = new IdExpr(*lexitr);
					inExpr->addExpr(y);
				} else{
					cout << "Wrong data type for expression: " << *lexitr << endl;
					pc = -1;
				}
			} else{
				inExpr->addOp(*tokitr);
			}
			tokitr++; lexitr++;
		}
		expr = inExpr;
	}
	return expr;
}


int main(){
	// BOTH
	ifstream infile1("source1.txt");
	ifstream infile2("symbol1.txt");
	if (!infile1 || !infile2) exit(-1);
	Compiler c(infile1, infile2);
	if(c.compile()){
		c.run();
	}
	dump();
	return 0;
}

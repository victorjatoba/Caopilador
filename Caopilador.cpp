/************************************************************************
*	Interpreter of numeric expressions
*	Author: Victor Jatobá
*	Baseline: 1.1.2 BETA
*	Date: 27/10/11
*	Description: Implementando a atribuição de valores a uma variável
*					Ex: var := (5+2); var2 = var;
*************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <algorithm>
#include <string>
#include <set>
#include <map>

using namespace std;

#define FOR(i,a) for(int i=0; i<int(a); i++)
#define size(x) int((x).size())
#define PUSH(tk) stackTokens[++stackTokensSize] = tk;
#define POP() stackTokensSize--;

enum tokens {
	tk_abre_parenteses,
	tk_fecha_parenteses,
	tk_ponto_virgula, 
	tk_mais, 
	tk_menos, 
	tk_mult, 
	tk_div, 
	tk_begin,
	tk_end,
	tk_while,
	tk_for,
	tk_if,
	tk_then,
	tk_else, 
	tk_do, 
	tk_id, 
	tk_digit, 
	tk_atribuicao,
	tk_incrementar,
	tk_decrementar,
	tk_igual,
	tk_diferente_de,
	tk_not,
	tk_menor_que,
	tk_maior_que,
	tk_menor_igual,
	tk_maior_igual,
	tk_and,
	tk_or,
	tk_goto,
	tk_vazio
};

//Declaracao das tabelas que classificam os atomos
string keyWordTable[100];
string digitTable[100];
string idTable[100];
enum tokens stackTokens[100];

//Tamanho das tabelas
int digitSize = -1, idSize = -1, keyWordSize = -1;
int contStr = 0, stackTokensSize = -1, contParenteses = 0;
int contColum = 0;
string input;

/*	Utilizado para verificar a existencia de um simbolo 
	especial ou uma palavra chave */
set <char> specialSymbol;
set <string> keyWord;

// mapa que associa o atomo ao seu token ou vice-versa
map <string, enum tokens> tipo;
map <int, string> tipoInverso;
map <string, double> idMap;

//Preenchimento dos mapas com os tokens e atomos
void popularTipo();
void popularTipoInverso();

/*  Funcoes para inserir os simbolos especiais e
	palavras reservadas nos containers do tipo set */
void popularSpecialSymbol();
void popularKeyWord();

/*	A primeira funcao verifica se o caracter mandado por parametro
	pertence a tabela de simbolos especiais e o outro
	verifica se a string passada eh uma palavra reservada*/
bool isSpecialSymbol(char simbolo);
bool isKeyWord(string word);

/*	Funcoes para converter atomos em tokens e vice-versa*/
enum tokens convertAtomoToToken(string atomo);
string convertTokenToString(enum tokens token);

/*  Funcao que identifica letras, digitos, simbolos 
	especiais (simples e compostos) e palavras reservadas*/
enum tokens lexico();

//	Função que retorna o próximo caracter não branco
char PROXIMO();

//	Retorna o topo da pilha global de tokens
enum tokens TOP();

//	Retorna o topo da pilha de identificadores
string topId();

//	Retorna o topo da pilha de digitos desempilhando-o logo em seguida
double topPopDigitTable();

//	Retorna o topo da pilha de Identificadores desempilhando-o logo em seguida
double topPopIdTable();

//	Verifica se o token eh multiplicacao, divisao ou conjunto
bool isMultDivConj(enum tokens tk);

/*	Verifica se o tk passado por paramero eh um Id, digit 
	ou abre_parenteses, realizando uma ação específica para
	cada condição*/
double fator(enum tokens tk);

/*	Verifica se o tk passado por paramero eh uma multiplicacao 
	ou divisao, realizando uma ação específica para cada condição*/
double termo(enum tokens tk);

/*	Verificando se o tk passado por paramero eh um mais ou menos,
	realizando uma ação específica para cada condição*/
double expressao(enum tokens tk);

/*	Instrução inicial para verificar se os dois primeiros tk's são 
	respectivamente	um id e uma atribuição, chamando expressão caso true*/
void instrucao(enum tokens tk);

/*	Imprime todo o buffer inicial, apontando a posição exata onde está
	ocorrendo o erro*/
void errorColum(int colum);

int main() {

	//Lê com espaços
	getline(cin, input);

	popularTipo();
	popularTipoInverso();
	popularSpecialSymbol();
	popularKeyWord();
	
	enum tokens tk_prox = lexico();
	while(stackTokens[stackTokensSize] != tk_vazio) {
		instrucao(stackTokens[stackTokensSize]);
		tk_prox = lexico();
		cout<<idMap.find(topId())->second<<"\t1"<<idTable[idSize]<<endl;
	}

	
	cout<<idMap.find(topId())->second<<endl;
	cout<<idTable[idSize]<<endl;
	
	return 0;
}

void popularTipo() {
	tipo["("] = tk_abre_parenteses;
	tipo[")"] = tk_fecha_parenteses;
	tipo[";"] = tk_ponto_virgula;
	tipo["+"] = tk_mais;
	tipo["-"] = tk_menos;
	tipo["*"] = tk_mult;
	tipo["/"] = tk_div;
	tipo["begin"] = tk_begin;
	tipo["end"] = tk_end;
	tipo["while"] = tk_while;
	tipo["for"] = tk_for;
	tipo["goto"] = tk_goto;
	tipo["if"] = tk_if;
	tipo["then"] = tk_then;
	tipo["else"] = tk_else;
	tipo["do"] = tk_do;
	tipo["id"] = tk_id;
	tipo["digit"] = tk_digit;
	tipo["div"] = tk_div;
	tipo["and"] = tk_and;
	tipo["or"] = tk_or;
	tipo[":="] = tk_atribuicao;
	tipo["++"] = tk_incrementar;
	tipo["--"] = tk_decrementar;
	tipo["="] = tk_igual;
	tipo["<>"] = tk_diferente_de;
	tipo["!"] = tk_not;
	tipo["<"] = tk_menor_que;
	tipo[">"] = tk_maior_que;
	tipo["<="] = tk_menor_igual;
	tipo[">="] = tk_maior_igual;
}

void popularTipoInverso() {
	tipoInverso[0] = "tk_abre_parenteses";
	tipoInverso[1] = "tk_fecha_parenteses";
	tipoInverso[2] = "tk_ponto_virgula";
	tipoInverso[3] = "tk_mais";
	tipoInverso[4] = "tk_menos";
	tipoInverso[5] = "tk_mult";
	tipoInverso[6] = "tk_div";
	tipoInverso[7] = "tk_begin";
	tipoInverso[8] = "tk_end";
	tipoInverso[9] = "tk_while";
	tipoInverso[10] = "tk_for";
	tipoInverso[11] = "tk_if";
	tipoInverso[12] = "tk_then";
	tipoInverso[13] = "tk_else";
	tipoInverso[14] = "tk_do";
	tipoInverso[15] = "tk_id";
	tipoInverso[16] = "tk_numero";
	tipoInverso[17] = "tk_atribuicao";
	tipoInverso[18] = "tk_incrementar";
	tipoInverso[19] = "tk_decrementar";
	tipoInverso[20] = "tk_igual";
	tipoInverso[21] = "tk_diferente_de";
	tipoInverso[22] = "tk_not";	
	tipoInverso[23] = "tk_menor_que";
	tipoInverso[24] = "tk_maior_que";
	tipoInverso[25] = "tk_menor_igual";
	tipoInverso[26] = "tk_maior_igual";
	tipoInverso[27] = "tk_and";
	tipoInverso[28] = "tk_or";
	tipoInverso[29] = "tk_goto";
	tipoInverso[30] = "tk_vazio";
}

void popularSpecialSymbol() {
	specialSymbol.insert(';');
	specialSymbol.insert(':');
	specialSymbol.insert('=');
	specialSymbol.insert('.');
	specialSymbol.insert(',');
	specialSymbol.insert('+');
	specialSymbol.insert('-');
	specialSymbol.insert('*');
	specialSymbol.insert('/');
	specialSymbol.insert('(');
	specialSymbol.insert(')');
	specialSymbol.insert('!');
	specialSymbol.insert('<');
	specialSymbol.insert('>');	
}

void popularKeyWord() {
	keyWord.insert("begin");
	keyWord.insert("end");
	keyWord.insert("while");
	keyWord.insert("for");
	keyWord.insert("goto");
	keyWord.insert("if");
	keyWord.insert("then");
	keyWord.insert("else");
	keyWord.insert("do");
	keyWord.insert("div");
	keyWord.insert("and");
	keyWord.insert("or");
}

bool isSpecialSymbol(char simbolo) {
	set<char>::iterator it;
	it = specialSymbol.find(simbolo);
	
	if(it != specialSymbol.end()) {
		return true;
	}
	return false;
}

bool isKeyWord(string word) {
	set<string>::iterator it;
	it = keyWord.find(word);
	
	if(it != keyWord.end()) {
		return true;
	}
	return false;
}

enum tokens convertAtomoToToken(string atomo) {
	return( tipo.find(atomo)->second );
}

string convertTokenToString(enum tokens token) {
	return( tipoInverso.find(token)->second );
}

char PROXIMO() {
	//Enquanto tiver espaços, \n e \t vá lendo.
	while( isspace(input[++contStr]) || input[contStr] == '\n' || input[contStr] == '\t'){
		contColum++;
		if(input[contStr] == '\t') {

			contColum += 5;
			cout<<"é isso mesmo! "<<contColum<<endl;
		}
	}
	contColum++;
	return input[contStr];
}

enum tokens TOP() {
	return stackTokens[stackTokensSize];
}

string topId() {
	return idTable[idSize];
}

enum tokens lexico() {
	
	char ant, prox;
	string atomo;
	enum tokens tk = tk_vazio;

	while( isspace(input[contStr]) || input[contStr] == '\n'){
		contStr++; contColum++;
	}
	if(input[contStr] != '\0') {
		atomo = "";
		
		if( isSpecialSymbol(input[contStr]) ) {
			ant = input[contStr];
			prox = PROXIMO();
			switch(ant) {
				//Verifica se sao simbolos especiais compostos
				case ':':
					atomo = ":";
					if(prox == '='){
						atomo += "=";
						prox = PROXIMO();
					}
					break;
				case '+':
					atomo = "+";
					if(prox == '+'){
						atomo += "+";
						prox = PROXIMO();
					}
					break;
				case '-':
					atomo = "-";
					if(prox == '-'){
						atomo += "-";
						prox = PROXIMO();
					}
					break;
				case '<':
					atomo = "<";
					if(prox == '>'){
						atomo += ">";
						prox = PROXIMO();
					}else{
						if(prox == '=') {
							atomo += '=';
							prox = PROXIMO();
						}
					}
					break;
				case '>':
					atomo = ">";
					if(prox == '='){
						atomo += "=";
						prox = PROXIMO();
					}
					break;
				//cc sao simbolos especiais simples
				default: atomo = ant; break;

			}
			tk = convertAtomoToToken(atomo);
			if(tk == tk_abre_parenteses) {
				contParenteses++;
			}
			if(tk == tk_fecha_parenteses) {
				contParenteses--;
			}
		}else{
			if( isalpha(input[contStr]) ){
				do{
					atomo += input[contStr];
					prox = PROXIMO();
				}while( isalnum(prox) || prox == '_');
				
				if( isKeyWord(atomo) ) {
					keyWordTable[++keyWordSize] = atomo;
					tk = convertAtomoToToken(atomo);
				}else{
					idTable[++idSize] = atomo;
					tk = tk_id;
					if(!idExiste(atomo)) {
						//Variáveis numéricas são inicializadas com 0(zero)
						idMap[atomo] = 0;
					}
			} else {
				if( isdigit(input[contStr]) ) {
					do {
						atomo += input[contStr];
						prox = PROXIMO();
					}while( isdigit(prox) );
				
					if( isalpha(prox) ) {
						cout<<"ERRO: '"<<input[contStr]<<"' sufixo inválido em uma constante de inteiros na coluna "<<contStr<<endl; errorColum(contStr+1);
						exit(1);
					}
					digitTable[++digitSize] = atomo;
					tk = tk_digit;
				} else {
					cout<<"ERRO:'"<<input[contStr]<<"' caracter desconhecido na coluna "<<contStr+1<<endl; errorColum(contStr+1);
					exit(1);
				}
			}

		}
	}
	
	PUSH(tk);
//	cout<<"stackTk["<<stackTokensSize<<"] = "<<convertTokenToString(stackTokens[stackTokensSize])<<endl;
	return tk;
}

double topPopDigitTable() {
	/* string.c_str() usado para converter um container 
		tipo string para vetor de caracteres do C */
	double result = atof( digitTable[digitSize].c_str() );
	digitSize--;
	return result;
}

double topPopIdTable() {
	double result = idMap.find(topId())->second;
	cout<<idMap.find(topId())->second<<"\ttopId"<<endl;
	cout<<topId()<<"\ttopId"<<endl;
	idSize--;
	return result;
}

bool isMultDivConj(enum tokens tk) {
	if(tk == tk_mult || tk == tk_div) {
		return true;
	}
	return false;
}

double fator(enum tokens tk) {
	double result = 0;
	switch(tk) {
		case tk_id: POP(); result = topPopIdTable();
			break;
		case tk_digit: POP(); result = topPopDigitTable();
			break;
		case tk_abre_parenteses: POP(); result = expressao(lexico());
			if(stackTokens[stackTokensSize] != tk_fecha_parenteses) {
				cout<<"ERRO: "<<convertTokenToString(stackTokens[stackTokensSize])<<" . Espera-se um ')' na coluna "<<contStr+1<<endl; errorColum(contStr+1);
				exit(1);
			}
			break;
		case tk_vazio:
			break;
		default: cout<<"ERRO: "<<convertTokenToString(tk)<<" espera-se um id, digit, '(' ou um '!'na coluna "<<contStr<<endl; errorColum(contStr);
			exit(1);
	}
	
//	cout<<"Fator: r="<<result<<"\tprox="<<convertTokenToString(tk)<<endl;
	return result;
}

double termo(enum tokens tk) {
	
	double result = fator(tk);
	enum tokens tk_prox = lexico();
//	cout<<"Termo: r="<<result<<"\tprox="<<convertTokenToString(tk_prox)<<endl;
	
	while(isMultDivConj(tk_prox)) {

//		cout<<"Termo1: r="<<result<<"\tprox="<<convertTokenToString(tk_prox)<<endl;
		switch( tk_prox ) {
			case tk_mult: POP(); result *= fator( lexico() );
				break;
			case tk_div: POP(); result /= fator( lexico() );
				break;
			case tk_vazio: 
				break;
			default: cout<<"ERRO:"<<convertTokenToString(tk_prox)<<" Espera-se um operador '+' ou '-'na coluna "<<contStr+1<<endl; exit(1);
		}
		tk_prox = lexico();
	}
	
	return result;
}

double expressao(enum tokens tk) {
	double result = termo(tk);
	enum tokens tk_prox = stackTokens[stackTokensSize];
//		cout<<"EX: r="<<result<<"\tprox="<<convertTokenToString(tk_prox)<<endl;	
	while(tk_prox == tk_mais || tk_prox == tk_menos) {

		switch( tk_prox ) {
			case tk_mais: POP(); result += termo( lexico() );
				break;
			case tk_menos: POP(); result -= termo( lexico() );
				break;
			case tk_vazio: 
				break;
			default: cout<<"ERRO:"<<convertTokenToString(tk_prox)<<" Espera-se um operador '+' ou '-'na coluna "<<contStr+1<<endl; errorColum(contStr);
				exit(1);
		}
		tk_prox = stackTokens[stackTokensSize];
//		cout<<"EX1: r="<<result<<"\tprox="<<convertTokenToString(tk_prox)<<endl;
	}
	
	if(contParenteses < 0) {
		cout<<"ERRO:"<<convertTokenToString(tk_prox)<<". Parenteses a mais na coluna "<<contStr<<endl; errorColum(contStr);
		exit(1);
	}
	
	return result;
}

void instrucao(enum tokens tk) {
	enum tokens tk_prox = lexico();
	
	if(tk == tk_id) {
		cout<<tk<<" "<<tk_prox<<endl;
		cout<<tk<<" "<<idTable[idSize]<<endl;
		if(tk_prox == tk_atribuicao) {
			idMap[topPopId()] = expressao(lexico());
			cout<<idMap.find(topId())->second<<"\tinstrucao "<<idTable[idSize]<<endl;
		} else {
			cout<<"ERRO:'"<<input[contStr]<<"' espera-se um ':=' na coluna: "<<contColum<<endl;
			errorColum(contColum);
			exit(1);
		}
		// Está faltando o ';' no final?
		if(stackTokens[stackTokensSize] != tk_ponto_virgula) {
			cout<<"ERRO:'"<<input[contStr]<<"' espera-se um ';' na coluna: "<<contColum<<endl;
			errorColum(contColum);
			exit(1);
		}
	}else {
		cout<<"ERRO:'"<<input[contStr]<<"' espera-se um identificador na coluna: "<<contColum<<endl;
		errorColum(contColum);
		exit(1);
	}
}

void errorColum(int colum) {
	string espaco;
	FOR(i, colum) {
		espaco += ' ';
	}
	
	cout<<input<<endl;
	cout<<espaco<<"^"<<endl;
}


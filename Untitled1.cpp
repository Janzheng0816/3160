//ZHijian ZHeng Project tokenizer 
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
using namespace std;


//all tokens are avialble 
enum Token{identifier, literal, lparen, rparen, _plus, _minus, _times, equals, semicolon, finished,error };

class Lex {
	public:             
        //read in file 
        ifstream inputFile;
        pair<Token, string> curr_token;
        
        
        Lex(string myFile) {
            inputFile.open(myFile);
        }
        ~Lex() {
            inputFile.close();
        } 
        
		// read in to tokenized the input file 
		deque<pair<Token, string>> tokenizer() {           
		deque<pair<Token, string>> t;
            curr_token = get_token();
            while(curr_token.first != finished) {
                t.push_back(curr_token);
                curr_token = get_token();
            }
            t.push_back(make_pair(finished, "endPoint"));
            return t;
		}
		


		//check for syntex error and pair up the token  
		pair<Token, string> get_token() {
			char curr_char;
			string word, line;
			//reading in file as char 
            while(inputFile.get(curr_char)) {
            	if(isdigit(curr_char)) {
                	//Find that if begin with 0 
                    if(curr_char == '0') {
                        char next = inputFile.peek();
                        //double 00 is illegal 
                        if(isdigit(next)) { 
                            getline(inputFile, line);
                            return make_pair(error, "error"); 
                        }
                        else return make_pair(literal, "0");
                    }                                
                    // for value 1to 9 
                    else if(curr_char >= '1' && curr_char <= '9') {
                        while(isdigit(curr_char)) {
                            word += curr_char;
                            inputFile.get(curr_char);
                        }
                        inputFile.putback(curr_char);
                        return make_pair(literal, word);
                    }
                }
                //See flollowing Id have "_" charater or digt or alphabet , put togeter as name of id
                else if(isalpha(curr_char) || curr_char == '_') { 
                    while(isalnum(curr_char) || curr_char == '_') {
                        word += curr_char;
                        inputFile.get(curr_char);
                    }
                    inputFile.putback(curr_char);
                    return make_pair(identifier, word);
                }
                
                //make all pairs in pair structure
                else if
						(curr_char == '+') return make_pair(_plus, "+");
                else if
						(curr_char == '-') return make_pair(_minus, "-");
                else if
						(curr_char == '*') return make_pair(_times, "*");
                else if
						(curr_char == '=') return make_pair(equals, "=");
                else if
						(curr_char == '(') return make_pair(lparen, "(");
                else if
						(curr_char == ')') return make_pair(rparen, ")");
                else if
						(curr_char == ';') return make_pair(semicolon, ";");
            }
            return make_pair(finished, "endPoint");
				}


   
    
};

class Parser {
	// set up datas 
        deque<pair<Token, string>> tokens;
        pair<Token, string> curr_token;
        std::map<std::string, int> state;
	
    public:
     	
        Parser(deque<pair<Token, string>> tokens) {
            this->tokens = tokens;
        }
        void get_token() {
            curr_token = tokens.front();
            //remove the element in list and reduce size by 1
            tokens.pop_front();
        }
        //method to  find matching pattern,  pop  next element and store in current token 
        void match(Token t) {
            if(curr_token.first == t) {
                get_token();
            }
            else {
                cout << "Error: illegal Assigment \n";
                exit(-1);
            }
        }
        //Program: Assignment*
        void program() {
            get_token();
            while(!tokens.empty()) {
                assignment();
            }
            //print the name of id and the final vaule of it 
            for (auto const& x : state)
            {
                cout << x.first  << " = " << x.second  << endl ;
            }
        }

        //Assignment: Identifier = Exp;
        void assignment() {
            if(curr_token.first != identifier) {
                cout << "Program is invaild!!!!\n"; exit(-1);
            }
            else {
                string value = curr_token.second;
                match(identifier);
                match(equals);
                int x =exp();
                state[value] = x;
                match(semicolon);
            }
        }             

        //Exp': + Exp | - Exp
        int expPrime() {
            if(curr_token.first == _plus) {
                match(_plus);
                return exp();
            }
            else if(curr_token.first == _minus) {
                match(_minus);
                return -exp();
            }
        }

        //Exp: Term Exp' | Term
        int exp() {
            int i = term();
            if(curr_token.first == _plus || curr_token.first == _minus) {
                i += expPrime();
            }
            return i;
        }

        //Term:   Fact Term'  | Fact
        int term() {
            int i = fact();
            if(curr_token.first == _times) {
                i *= termPrime();
            }
            return i;
        }

        //Term': * Term
        int termPrime() {
            match(_times);
            return term();
        }

        //Fact:   ( Exp ) | - Fact | + Fact | Literal | Identifier
        int fact() {
        	//check left parenthetic if yes do the expression in side and match right parenthetic and return the value of expression 
            if(curr_token.first == lparen) {
                match(lparen);
                int i = exp();
                match(rparen);
                return i;
            }
            //check - and return the negative value of the fact 
            else if(curr_token.first == _minus) {
                match(_minus);
                return -fact();
            }
            //check + and return the value of the fact
            else if(curr_token.first == _plus) {
                match(_plus);
                return fact();
            }
        	// check if literal and return the value of literal 
            else if(curr_token.first == literal) {
                int i = stoi(curr_token.second);
                match(literal);
                return i;
            }
            //check if a id , return the value of by id's key  
            else if(curr_token.first == identifier) {
            	//check if illegal assigment
                if(state.find(curr_token.second) == state.end()) {
                    match(semicolon);
                    cout << "illegel assigament"; exit(-1);
                }
                string tmp = curr_token.second;
                match(identifier);
                return state[tmp];
            }
        }
};

int main() {
    string myFile = "test.txt";
    //read in file and set in pairs
    Lex readin(myFile);
    //tokenized the input file  
    deque<pair<Token, string>> t = readin.tokenizer();
    // calulate man match the assigment 
    Parser myToken(t);
    //print final value of id    of  all assigments
    myToken.program();

	return 0;
}
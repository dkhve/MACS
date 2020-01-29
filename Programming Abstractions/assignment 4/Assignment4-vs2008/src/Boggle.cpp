/*
 * File: Boggle.cpp
 * ----------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the main starter file for Assignment #4, Boggle.
 * [TODO: extend the documentation]
 */

#include <iostream>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;
const int BOARD_SIZE = 4;
const int STANDARD_CUBES_LENGTH = 16;
const int WORD_MIN_LENGTH = 4;

const string STANDARD_CUBES[16]  = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};
 
const string BIG_BOGGLE_CUBES[25]  = {
    "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
    "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
    "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
    "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
    "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

/* Function prototypes */

void welcome();
void giveInstructions();
void playGame(GWindow &gw);
void fillBoard(Vector<char> &charList,Grid<char> &board);
void getCustomBoard(Vector<char> &charList);
void getDefaultBoard(Vector<char> &charList);
void shuffle(Vector<string> &vec, Vector<char> &charList);
void putChars(Vector<char> &charList,Grid<char> &board);
void toUpper(Vector<char> &charList);
void playersTurn(Grid<char> &board, Lexicon &lex,Set<string> &usedWords,Grid<bool> &checked,GWindow &gw);
void resetGrid(Grid<bool> &grid);
string getWord(Lexicon &lex, Set<string> &used,Grid<char> &board,Grid<bool> &checked);
bool canBeFormed(Grid<char> &board, string word,Grid<bool> &checked);
bool formWord(Grid<char> &board, Grid<bool> &checked, string word,int row, int col);
void highlightWord(Grid<bool> &checked,GWindow &gw, bool flag);
void myTurn(Grid<char> &board, Lexicon &lex, Set<string> &usedWords, Grid<bool> &checked);
void findWords(Grid<char> &board, Lexicon &lex, Set<string> &usedWords, Grid<bool> &checked, int row, int col,string soFar);
bool playAgain();


/* Main program */

int main() {
    GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
    initGBoggle(gw);
    welcome();
    giveInstructions();
	playGame(gw);
    return 0;
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */

void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */

void giveInstructions() {
    cout << endl;
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl << endl;
    cout << "Hit return when you're ready...";
    getLine();
}

//controls the gameplay
void playGame(GWindow &gw){
	Lexicon lex("EnglishWords.dat");
	bool play = true;
	while(play){
		drawBoard(BOARD_SIZE,BOARD_SIZE);
		Vector<char> charList;
		Grid<char> board(BOARD_SIZE,BOARD_SIZE);
		Set<string> usedWords;
		fillBoard(charList,board);
		Grid<bool> checked(BOARD_SIZE , BOARD_SIZE);
		playersTurn(board , lex, usedWords, checked, gw);
		myTurn(board, lex, usedWords,checked);
		play = playAgain();
	}
}

//fills the board with characters
void fillBoard(Vector<char> &charList,Grid<char> &board){
	while(true){
		string answer = getLine("do you want to enter custom board configuration? ");
		if(toUpperCase(answer) == "YES"){
			getCustomBoard(charList);
			break;
		}else if(toUpperCase(answer) == "NO") {
			getDefaultBoard(charList);
			break;
		}else cout<<"Wrong input, enter yes or no"<<endl; 
	}
	putChars(charList,board);
}

//prompts the user to fill the board with custom characters
void getCustomBoard(Vector<char> &charList){
	string boardLetters;
	while(true){
		boardLetters = getLine("Enter 16-character long string to assign them on board: ");
		if(boardLetters.length() == 16){
			foreach(char c in boardLetters){
				charList.add(c);
			}
			break;
		}
	}
}

//generates the board
void getDefaultBoard(Vector<char> &charList){
	Vector<string> vec;
	for(int i = 0 ; i < STANDARD_CUBES_LENGTH ; i++){
		vec.add(STANDARD_CUBES[i]);
	}
	shuffle(vec,charList);
}

//shuffles the default cube and generates random characters
void shuffle(Vector<string> &vec, Vector<char> &charList){
	for(int i = 0 ; i < vec.size() ; i++){
		swap(vec[i] , vec[randomInteger(i,vec.size()-1)]);
	}
	for(int  i = 0 ; i < vec.size() ; i ++){
		charList.add(vec[i][randomInteger(0,vec[i].size()-1)]);
	}	
}

//puts characters on board
void putChars(Vector<char> &charList,Grid<char> &board){
	toUpper(charList);
	int row = 0;
	int col = 0;
	char ch;
	for(int i = 0 ; i < charList.size() ; i++){
		row = i/BOARD_SIZE;
		col = i%BOARD_SIZE;
		ch = charList[i];
		board[row][col] = ch;
		labelCube(row, col, ch);
	}	
}

//converts every element of a vector into uppercase version
void toUpper(Vector<char> &charList){
	for(int i = 0 ; i < charList.size() ; i++ ){
		char c = charList[i];
		if(c>='a' && c<='z'){
			charList[i] = 'A' + c - 'a';
		}
	}
}

//is responsible for player's turn 
void playersTurn(Grid<char> &board, Lexicon &lex,Set<string> &usedWords,Grid<bool> &checked,GWindow &gw){
	while (true){
		resetGrid(checked);
		string word = getWord(lex,usedWords,board,checked);
		if(word == "") break;
		usedWords.add(word);
		recordWordForPlayer(word,HUMAN);
		highlightWord(checked,gw,true);
	}
}

//resets a grid
void resetGrid(Grid<bool> &grid){
	for(int i = 0 ; i < grid.nRows ; i++){
		for(int j = 0 ; j<grid.nCols ; j++){
			grid[i][j] = false;
		}
	}
}

//prompts the user to enter valid word
string getWord(Lexicon &lex, Set<string> &used,Grid<char> &board,Grid<bool> &checked){
	string word;
	while(true){
		word = toUpperCase(getLine("Enter valid word: "));
		if(word == "") return word;
		if(word.length() < WORD_MIN_LENGTH) continue;
		if(!lex.contains(word)) continue;
		if(used.contains(word)) continue;
		if(!canBeFormed(board,word,checked)) continue;
		return word;
	}
}

//checks if entered word can be formed on board
bool canBeFormed(Grid<char> &board, string word,Grid<bool> &checked){
	for(int i = 0 ; i < board.numRows() ; i++){
		for(int j = 0 ; j< board.numCols() ; j++){
			if(board[i][j] == word[0]){
				checked[i][j] = true;
				if(formWord(board, checked, word.substr(1), i, j)) return true;
				checked[i][j] = false;
			}
		}
	}
	return false;
}

// tries to form the given word
bool formWord(Grid<char> &board, Grid<bool> &checked, string word,int row, int col){
	if(word == "") return true;
	for(int i = -1; i < 2; i++){
		for(int j = -1 ; j < 2; j++){
			if(board.inBounds(row+i,col+j) && !checked[row+i][col+j]){
				if(board[row+i][col+j] == word[0]){
					checked[row+i][col+j] = true;
					if(formWord(board, checked, word.substr(1), row+i, col+j)) return true;
					checked[row+i][col+j] = false;
				}
			}
		}
	}
	return false;
}

//highlights cubes which compose
void highlightWord(Grid<bool> &checked,GWindow &gw,bool flag){
	for(int i = 0 ; i < checked.nRows ; i++){
		for(int j = 0 ; j < checked.nCols ; j++){
			if(checked[i][j]) highlightCube(i, j, flag);	
		}
	}
	pause(150);
	if(flag) {
		flag = false;
		highlightWord(checked,gw,flag);
	}
}

//is responsible for computer's turn
void myTurn(Grid<char> &board, Lexicon &lex, Set<string> &usedWords, Grid<bool> &checked){
	for(int i = 0 ; i < board.nRows ; i++){
		for(int j = 0; j < board.nCols ; j++){
			checked[i][j] = true;
			findWords(board,lex,usedWords,checked,i,j,string()+board[i][j]);
			checked[i][j] = false;
		}
	}
}

//finds words from given position
void findWords(Grid<char> &board, Lexicon &lex, Set<string> &usedWords, Grid<bool> &checked, int row, int col,string soFar){
	if(soFar.length()>=4 && lex.contains(soFar) && !usedWords.contains(soFar)){
		recordWordForPlayer(soFar,COMPUTER);
		usedWords.add(soFar);
	}
	for(int i = -1; i < 2 ; i++){
		for(int j = -1 ; j < 2; j++){
			if(board.inBounds(row+i,col+j) && !checked[row+i][col+j]){
				string s = soFar+board[row+i][col+j];
				if(lex.containsPrefix(s)){
					checked[row+i][col+j] = true;
					findWords(board, lex , usedWords , checked , row+i , col+j, s);
					checked[row+i][col+j] = false;
				}
			}
		}
	}
}

//checks if user wants to play again
bool playAgain(){
	while(true){
		string answer = getLine("do you want to play again? ");
		if(toUpperCase(answer) == "YES"){
			return true;
		}else if(toUpperCase(answer) == "NO") {
			return false;
		}else cout<<"Wrong input, enter yes or no"<<endl; 
	}
}
/*
 * File: WordLadder.cpp
 * --------------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Word Ladder problem.
 * [TODO: rewrite the documentation]
 */

#include <iostream>
#include "console.h"
#include "lexicon.h"
#include "simpio.h"
#include "queue.h"
#include <cstring>
using namespace std;

bool init(Queue<string> &,Lexicon &,string &, string &);
string getPath(Queue<string> &,Lexicon &,string, string);
string getLastWord(string);
void hop(Queue<string> &,Lexicon &,string , Lexicon &, string);

int main() {
	Queue<string> que;
    Lexicon lex("EnglishWords.dat");
	string start;
	string end;
	while(true){
		if(init(que,lex,start,end))
			cout<<getPath(que,lex,start,end)<<endl;
		else if(start == "") break;
		else cout<<"No word ladder could be found"<<endl;
	}
	return 0;
}

//prompts the user to enter correct starting word and destination word
bool init(Queue<string> &que,Lexicon &lex,string &start, string &end){
	start = getLine("Enter the starting word(or nothing to quit): ");
	if(start == "") return false;
	end = getLine("Enter the ending word: ");
	cout<<"Searching..."<<endl;
	if(!(lex.contains(start) && lex.contains(end)) || start.length() != end.length()) return false;
	que.enqueue(start);
	return true;
}

//generates the path
string getPath(Queue<string> &que,Lexicon &lex,string start, string end){
	Lexicon usedWords;
	int wordLength = start.length();
	while(!que.isEmpty()){
		string ladder = que.dequeue();
		//gets the last word
		string word = ladder.substr(ladder.length() - wordLength);
		if(word == end) {
			que.clear();
			return ladder;
		}
		//gets to next step
		hop(que,lex,ladder,usedWords,word);
	}
	return "No word ladder could be found";
}

//generates all extensions of given ladder
void hop(Queue<string> &que,Lexicon &lex,string ladder, Lexicon &usedWords, string word){
	string wordCopy;
	string ladderCopy;
	for(int i = 0 ; i<word.size() ; i++){
		wordCopy = word;
		for(int j = 0 ; j<26 ; j++){
			wordCopy[i] = 'a'+j;
			if(lex.contains(wordCopy) && !usedWords.contains(wordCopy)){
				ladderCopy = ladder;
				ladderCopy += " -> " +wordCopy;
				usedWords.add(wordCopy);
				que.enqueue(ladderCopy);
			}
		}
	}
}
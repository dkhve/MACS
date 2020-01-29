/*
 * File: FleschKincaid.cpp
 * ----------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Flesch-Kincaid problem.
 * [TODO: rewrite the documentation]
 */

#include <iostream>
#include <fstream>
#include "console.h"
#include "simpio.h"
#include "Vector.h"
using namespace std;

void getFile(ifstream &);
void countSentences(ifstream &);
void build(string ,Vector<char> &);
void getSentences(string &,Vector<char> &);
bool contains(char ch,Vector<char> &);
void countWords(string &);
bool isLetter(char);
void countSyllables(string &);
bool valid(string &, int,Vector<char> &);
void calculateGrade();

const string SENTENCE_ENDS = ".!?" ; 
const string VOWELS = "aAeEiIoOuUyY";
const double C0 = -15.59;
const double C1 = 0.39;
const double C2 = 11.8;

int sentences = 0;
int words = 0 ; 
int syllables = 0;

int main() {
	ifstream inp;
	getFile(inp);
	countSentences(inp);
	calculateGrade();
	return 0;
}

//prompts the user to enter file name that program should read
//repeats until correct file name is entered
void getFile(ifstream &inp){
	while(true){
		string fileName = getLine("Enter file name: ");
		inp.open(fileName.c_str());
		if(inp.is_open()) break;
		cout<<"File with that name doesn't exist"<<endl;
		inp.clear();
	}
}

//sets up everything so program is ready to count sentences then counts them line by line
void countSentences(ifstream &inp){
	string line;
	//vector of characters that end the sentence
	Vector<char> endings;
	build(SENTENCE_ENDS,endings);
	while(getline(inp,line)){
		getSentences(line , endings);
	}
}

//builds a vector of characters from given string
void build(string str,Vector<char>& vec){
	for(int i = 0 ; i<str.length() ; i++){
		vec.add(str[i]);
	}
}

//counts sentences in given line then counts words in given each sentence
void getSentences(string &line , Vector<char> &endings){
	int lastIndex = 0 ;
	string sentence;
	for(int i = 0 ; i < line.length(); i ++){
		if(contains(line[i] , endings)){
			sentence = line.substr(lastIndex , i - lastIndex);
			if(sentence.length() > 0) sentences++;			
			lastIndex = i+1 ;
			countWords(sentence);
		}else if(i == line.length() - 1){
			sentence = line.substr(lastIndex , i - lastIndex);
			countWords(sentence);
		}
	}
}

//checks if given vector of characters contains given character
bool contains(char ch,Vector<char> & vec){
	for(int i = 0 ; i < vec.size() ; i++){
		if(vec[i] == ch) {
			return true;	
		}
	}
	return false;
}

//counts words in given sentence and then counts syllables in each word
void countWords(string &sentence){
	int lastIndex = 0 ;
	string word;
	sentence+= " ";
	for(int i = 0 ; i< sentence.length() ; i++){
		if(sentence[i] == ' '){
			word = sentence.substr(lastIndex , i - lastIndex);
			lastIndex = i+1;
			//words must start with letters
			if(word.length() > 0 && isLetter(word[0]))	{
				words++;
				countSyllables(word);
			}
		}
	}
}

//checks if character is letter
bool isLetter(char ch){
	if((ch>='a' && ch <='z') || (ch>='A' && ch<='Z')) return true;
	return false;
}

//counts syllables in given word
void countSyllables(string &word){
	int sylCount = 0 ;
	Vector<char> vowelList;
	build(VOWELS,vowelList);
	for(int i = 0 ;i < word.length() ; i ++){
		//checks if character is a vowel, then checks if this vowel gives us a syllable
		if(contains(word[i],vowelList) && valid(word, i,vowelList)) sylCount++;
	}
	//every word must have minimum 1 syllable
	if(sylCount == 0) sylCount=1;
	syllables+=sylCount;
}

//checks if a vowel gives us a syllable
bool valid(string &word, int i, Vector<char> &vowelList){
	//if two vowels are together they count as one syllable
	if(i>0 && contains(word[i-1],vowelList)) return false;
	//if last character of the word is e it doesn't give us a syllable
	if(i == word.length() - 1 && (word[i] == 'e' || word[i] == 'E')) return false;
	return true;
}

//calculates the grade of text
void calculateGrade(){
	double grade;
	grade = C0 + C1*((double)words/sentences) +C2*((double)syllables / words);
	cout<<"Sentences: "<<sentences<<endl<<"Words: "<<words<<endl<<"Syllables: "<<syllables<<endl<<"Grade: "<<grade<<endl;
}
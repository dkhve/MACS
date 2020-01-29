/*
 * File: RandomWriter.cpp
 * ----------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Random Writer problem.
 * [TODO: rewrite the documentation]
 */

#include <iostream>
#include <fstream>
#include "simpio.h"
#include "console.h"
#include "vector.h"
#include "map.h"
#include "random.h"
using namespace std;

void getData(Map<string , Vector<char> > &data);
void openFile(ifstream &inp);
int getOrder();
void storeData(Map<string , Vector<char> > &data , string s , char ch);
void generateText(Map<string , Vector<char> > &data);
string getSeed(Map<string , Vector<char> > &data);
string generateChar(Map<string , Vector<char> > &data, string &seed);

const int TEXT_LENGTH = 2000;

int main() {
	Map<string , Vector<char> > data;
	getData(data);
	generateText(data);
    return 0;
}

//reads the file and stores data
void getData(Map<string , Vector<char> > &data){
	ifstream inp;
	openFile(inp);
	int order = getOrder();
	string s;
	char ch;
	//gets first n characters as string, because after that we need only 1 character
	for(int i = 0 ; i < order ; i ++){
		inp.get(ch);
		s += string() + ch;
	}
	//adds one character to string and removes the first so that length stays same
	while (inp.get(ch)){
		storeData(data,s,ch);
		s=s.substr(1)+ch;
	}
	inp.close();
}

//prompts the user to enter correct file name and opens that file
void openFile(ifstream &inp){
	while(true){
		string fileName = getLine("Enter the source text: ");
		inp.open(fileName.c_str());
		if(inp.is_open()) break;
		cout<<"Unable to open that file.Try again."<<endl;
		inp.clear();
	}
}

//stores the data for given string
void storeData(Map<string , Vector<char> > &data , string s , char ch){
	if(data.containsKey(s)) {
		data[s].add(ch);
	}else{
		Vector<char> vec;
		vec.add(ch);
		data.put(s,vec);
	}
}

//prompts the user to enter markov order
int getOrder(){
	while(true){
		int order = getInteger("Enter the markov order[1-10]: ");
		if(order>=1 && order<=10) {
			cout<<"Processing file..."<<endl;
			return order;
		}
		cout<<"Wrong number.Try again."<<endl;
	}
}

//generates the text based on given data
void generateText(Map<string , Vector<char> > &data){
	string seed = getSeed(data);
	cout<<seed;
	int charCount = seed.length();
	while(charCount<TEXT_LENGTH){
		string s = generateChar(data,seed);
		if(s == "") break;
		cout<<s;
		charCount++;
	}
}

//generates the n length string that is most frequent
string getSeed(Map<string , Vector<char> > &data){
	string seed = "";
	int maxSize = 0;
	int size = 0;
	foreach(string s in data){
		size = data.get(s).size();
		if(size > maxSize)  {
			seed = s;
			maxSize = size;
		}
	}
	return seed;
}

//generates next char based on given string
string generateChar(Map<string , Vector<char> > &data, string &seed){
	int size = data.get(seed).size();
	if(size == 0) return "";
	char ch = data.get(seed).get(randomInteger(0,size-1));
	seed = seed.substr(1) + ch;
	return string() + ch;
}
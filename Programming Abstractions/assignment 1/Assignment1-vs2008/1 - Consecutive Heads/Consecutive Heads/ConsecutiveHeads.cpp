/*
 * File: ConsecutiveHeads.cpp
 * --------------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Consecutive Heads problem.
 * [TODO: rewrite the documentation]
 */

#include <iostream>
#include "console.h"
#include "simpio.h"
#include "random.h"
#include "vector.h"
using namespace std;

void getSequence(string &);
bool valid(string);
void achieveSequence(string);
void translate(string, Vector<bool>&);
void flip (Vector<bool>&, int);
void printSide(bool);
void startOver(Vector<bool>&,bool,Vector<bool>& );

int counter = 0;

int main() {
	while(true){
		string s;
		getSequence(s);
		if(valid(s))
			achieveSequence(s);
		else cout<<"Wrong Input"<<endl;
	}
	return 0;
}

//prompts the user to enter sequence 
void getSequence(string &s){
	cout<<"you can enter any sequence you want consisting of h's and t's"<<endl
		<<"h means heads , t means tails"<<endl;
	s = getLine("Enter your sequence: ");
}

//checks if user entered valid sequence
bool valid(string s){
	if(s.length()<1) return false;
	for(int i = 0 ; i < s.length() ; i ++){
		if(s[i] != 'h' && s[i] != 't'){
			return false;
		}
	}
	return true;
}

// controls that everything is ready to start flipping coin , then flips it and prints the answer
void achieveSequence(string s){
	Vector<bool> seq;
	translate(s, seq);
	flip(seq,0);
	cout<<"It took "<<counter<<" flips to get given sequence"<<endl;
	counter = 0;
}

//translates string of h's and t's to vector of boolean h - true , t - false
void translate(string s,Vector<bool> &seq ){
	for(int i = 0; i< s.length() ; i ++){
		if(s[i] == 'h') seq.add(true);
		else seq.add(false);
	}
}

//flips the coin until achieves given sequence and counts how many flips it took
void flip (Vector<bool> &seq ,int i){
	bool side;
	//remembers the last n-1 flips because it might be same as the start of the given sequence
	Vector<bool> lastSeq;
	while(true){
		side = randomChance(.50);
		printSide(side);
		counter++;
		lastSeq.add(side);
		if(lastSeq.size()==seq.size()) lastSeq.remove(0);
		if(side == seq[i]) i++;
		else {
			startOver(seq,side,lastSeq);
			return;
		}
		if(i == seq.size()) break;
	}
}

//prints on which side has the coin landed
void printSide(bool side){
	if(side)
		cout<<"heads"<<endl;
	else cout<<"tails"<<endl;
}

//counts how many of n-1 flips is the same as first n-1 flips in desirable sequence and starts counting accordingly
void startOver(Vector<bool> &seq ,bool side,Vector<bool> &lastSeq){
	int i = 0;
	for(int j = lastSeq.size()-1 ; j>0;j--){
		if(lastSeq[j]==seq[lastSeq.size()-1 -j]) i++;
	}
	flip(seq,i);
}

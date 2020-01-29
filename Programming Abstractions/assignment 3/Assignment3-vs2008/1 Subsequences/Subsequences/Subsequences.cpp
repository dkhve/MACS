/*
 * File: Subsequences.cpp
 * ----------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Subsequences problem
 * on Assignment #3.
 * [TODO: extend the documentation]
 */

#include <iostream>
#include <string>
#include "console.h"
#include "simpio.h"
using namespace std;

/* Given two strings, returns whether the second string is a
 * subsequence of the first string.
 */
bool isSubsequence(string text, string subsequence);

int main() {
	string text = getLine("Enter text: ");
	string subsequence = getLine("Enter sequence: ");
	if(isSubsequence(text,subsequence)) cout<<"Given sequence is a subsequence of given text"<<endl;
	else cout<<"Given sequence is not a subsequence of given text"<<endl;
    return 0;
}

//tests if given string is subsequence of given text
bool isSubsequence(string text, string subseq){
	if(subseq == "") return true;
	if(text == "") return false;
	if(subseq[0] == text[0]){
		text = text.substr(1);
		subseq = subseq.substr(1);
	}else{
		text = text.substr(1);
	}
	return isSubsequence(text,subseq);
}
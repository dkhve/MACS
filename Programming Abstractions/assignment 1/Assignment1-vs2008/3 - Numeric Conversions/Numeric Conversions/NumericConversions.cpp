/*
 * File: NumericConversions.cpp
 * ---------------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Numeric Conversions problem.
 * [TODO: rewrite the documentation]
 */

#include <iostream>
#include <string>
#include "console.h"
#include "simpio.h"
#include <cmath>
using namespace std;

/* Function prototypes */

bool isInteger(string);
bool isDigit(char);
int stringToInt(string);
string intToString(int);



/* Main program */

int main() {
	string s= getLine("Enter string to convert: ");
	int i = getInteger("Enter integer to convert: ");
	if(isInteger(s)){
		cout<<"As Integer - "<<stringToInt(s)<<endl;
	}
	cout<<"As String - " + intToString(i)<<endl;
    return 0;
}

//checks if entered string is integer
bool isInteger(string s){
	for(int i = 0 ; i < s.length() ; i++){
		if(i != 0){
			if(!isDigit(s[i])) 
				return false;
		}else if(s[0] != '-' && !isDigit(s[0])) 
			return false;
	}
	return true;
}

//checks if given character is digit
bool isDigit(char c){
	if(c >= '0' && c <='0'+9)
		return true;
	cout<<"Wrong Input"<<endl;
	return false;
}

//converts entered string to integer
int stringToInt(string s){
	if(s == ""){
		return 0;
	}
	if(s[0] == '-'){
		return	-((s[1]-'0')*pow(10.0,s.length()-2.0) + stringToInt(s.substr(2)));
	}
	return (s[0]-'0')*pow(10.0,s.length()-1.0) + stringToInt(s.substr(1));
}

//converts integer to string
string intToString(int i){
	if(i < 0 ){
		if(i <=-10)
			return "-"  +intToString(-i/10) + intToString(-i%10);
		return "-" + intToString(-i);
	}	
	if(i<10)
		return string() + char(i + '0');

	return intToString(i/10) + intToString(i%10);
}
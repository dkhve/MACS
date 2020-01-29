/*
 * File: Combinations.cpp
 * ----------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Combinations problem.
 * [TODO: rewrite the documentation]
 */

#include <iostream>
#include "console.h"
#include "simpio.h"
using namespace std;

int getCombNum(int, int);

int main() {
	int n = getInteger("Enter n: ");
	int k  = getInteger("Enter k: ");
	if( k>0 && n > 0 && k<=n)
		cout<<getCombNum(n,k)<<endl;
	else 
		cout<<"Wrong Input"<<endl;
	
    return 0;
}

//calculates the number of combinations to choose k numbers from n numbers
//according to pascal's triangle
int getCombNum(int n , int k){
	if(k==n || k==0){
		return 1;
	}
	return getCombNum(n-1 , k-1) + getCombNum(n-1 , k);
}
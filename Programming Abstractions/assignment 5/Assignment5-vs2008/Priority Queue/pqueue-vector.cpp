/*************************************************************
 * File: pqueue-vector.cpp
 *
 * Implementation file for the VectorPriorityQueue
 * class.
 */
 
#include "pqueue-vector.h"
#include "error.h"

VectorPriorityQueue::VectorPriorityQueue() {
	// TODO: Fill this in!
}

VectorPriorityQueue::~VectorPriorityQueue() {
	// TODO: Fill this in!
}

//returns the size of collection
int VectorPriorityQueue::size() {
	return vec.size();
}

//checks if collection is empty
bool VectorPriorityQueue::isEmpty() {
	return vec.size()==0;
}

//enqueues given string
void VectorPriorityQueue::enqueue(string value) {
	vec.add(value);
}

// returns the value of minimal element
string VectorPriorityQueue::peek() {
	return vec[findMin()];
}

//dequeues the minimal element
string VectorPriorityQueue::dequeueMin() {
	int minIdx = findMin();
	string str = vec[minIdx];
	vec.remove(minIdx);
	swap(vec[vec.size() - 1] , vec[minIdx]);
	vec.remove(vec.size()-1);	
	return str;
}

//finds the index of minimal element
int VectorPriorityQueue::findMin(){
	int min = 0;
	for(int i = 1 ; i < vec.size(); i++){
		if(vec[i] < vec[min]) min=i;
	}
	return min;
}
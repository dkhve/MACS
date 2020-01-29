/*************************************************************
 * File: pqueue-heap.cpp
 *
 * Implementation file for the HeapPriorityQueue
 * class.
 */
 
#include "pqueue-heap.h"
#include "error.h"

const int initSize = 7;

HeapPriorityQueue::HeapPriorityQueue() {
	filled = 1;
	length = initSize;
	arr = new string[length];
	arr[0] = "";
}

HeapPriorityQueue::~HeapPriorityQueue() {
	delete[] arr;
}
//returns the size of collection
int HeapPriorityQueue::size() {
	return filled-1;
}

//checks if collection is empty
bool HeapPriorityQueue::isEmpty() {	
	return filled == 1;
}

//enqueues given string
void HeapPriorityQueue::enqueue(string value) {
	if(length == filled) grow();
	arr[filled] = value;
	bubbleUp();
	filled++;

}

//returns the value of minimal element
string HeapPriorityQueue::peek() {
	if(isEmpty()) error("There are no elements");	
	return arr[1];
}

//dequeues the minimal element
string HeapPriorityQueue::dequeueMin() {
	if(isEmpty()) error("There are no elements");
	swap(arr[1],arr[filled-1]);
	string min = arr[filled-1];
	filled--;
	bubbleDown();
	return min;
}


//if collection is full, increases its capacity
void HeapPriorityQueue::grow(){
	length = 2*length+1;
	string* newArr = new string[length];
	for (int i = 0; i < filled; i++) {
		newArr[i] = arr[i];
	}
	delete[] arr;
	arr = newArr;
}

//places new element according to order
void HeapPriorityQueue::bubbleUp(){
	int index = filled;
	while(index > 0){
		if(arr[index] < arr[index/2]) swap(arr[index],arr[index/2]);
		else break;
		index = index/2;
	}
}

//restores order
void HeapPriorityQueue::bubbleDown(){
	int index = 1;
	while(size() > 2*index){
		if(arr[index] > arr[2*index] || arr[index] > arr[2*index+1]){
			if(arr[2*index] <= arr[2*index+1]){
				swap(arr[index],arr[2*index]);
				index *= 2;
			}else{
				swap(arr[index],arr[2*index+1]);
				index = 2*index +1;
			}
		} else break;
	}
	if(size() == 2*index && arr[index] > arr[2*index]) swap(arr[index],arr[2*index]);
}
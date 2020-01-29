/*************************************************************
 * File: pqueue-linkedlist.cpp
 *
 * Implementation file for the LinkedListPriorityQueue
 * class.
 */
 
#include "pqueue-linkedlist.h"
#include "error.h"

LinkedListPriorityQueue::LinkedListPriorityQueue() {
	head = tail = NULL;
	length = 0;
}

LinkedListPriorityQueue::~LinkedListPriorityQueue() {
	while(head!= NULL){
		Cell* next = head->next;
		delete head;
		head = next;
	}
}

//returns the size of collection
int LinkedListPriorityQueue::size() {	
	return length;
}

//checks if collection is empty
bool LinkedListPriorityQueue::isEmpty() {	
	return length == 0;
}

//enqueues given string
void LinkedListPriorityQueue::enqueue(string value) {
	Cell* toAdd = new Cell;
	toAdd->value = value;
	Cell* prev = NULL;	
	if(head == NULL){
		toAdd ->next = NULL;
		head=tail=toAdd;
	}else{
		toAdd->next = head;
		while(value > toAdd -> next -> value){
		  prev = toAdd -> next;

		  if(toAdd->next == tail){
			//when element to add is bigger than every already added element
			tail = toAdd;
			tail->next = NULL;
			break;
		  }

		  toAdd->next = toAdd->next->next;
		}
		if(prev != NULL) prev->next = toAdd;
		else head = toAdd;
	}
	length++;
}

// returns the value of minimal element
string LinkedListPriorityQueue::peek() {
	if(isEmpty()) error("No elements");
	return head->value;
}

//dequeues the minimal element
string LinkedListPriorityQueue::dequeueMin() {
	if (isEmpty()) error("No elements");
	string val = head->value;
	Cell* next = head->next;
	delete head;
	length--;
	head = next;
	return val;
}
/*************************************************************
 * File: pqueue-doublylinkedlist.cpp
 *
 * Implementation file for the DoublyLinkedListPriorityQueue
 * class.
 */
 
#include "pqueue-doublylinkedlist.h"
#include "error.h"

DoublyLinkedListPriorityQueue::DoublyLinkedListPriorityQueue() {
	head = tail = NULL;
	length = 0;
}

DoublyLinkedListPriorityQueue::~DoublyLinkedListPriorityQueue() {
	while(length > 0){
		Cell* next = head->next;
		delete head;
		head = next;
		length--;
	}
}

//returns the size of collection
int DoublyLinkedListPriorityQueue::size() {	
	return length;
}

//checks if collection is empty
bool DoublyLinkedListPriorityQueue::isEmpty() {
	return length == 0;
}

//enqueues given string
void DoublyLinkedListPriorityQueue::enqueue(string value) {
	Cell* toAdd = new Cell;
	toAdd->value = value;
	toAdd->next = NULL;
	if(length == 0){
		toAdd->prev = NULL;
		head = tail = toAdd;
	}else{
		toAdd->prev = tail;
		tail->next = toAdd;
		tail = toAdd;
	}
	length++;
}

// returns the value of minimal element
string DoublyLinkedListPriorityQueue::peek() {
	if(isEmpty()) error("There are no elements");
	string minVal = head->value;
	for(Cell* c = head; c != NULL ; c=c->next){
		if(c->value < minVal) minVal = c->value;
	}
	return minVal;
}

//dequeues the minimal element
string DoublyLinkedListPriorityQueue::dequeueMin() {
	if(isEmpty()) error("There are no elements");
	Cell* min = head;
	for(Cell* c = head; c != NULL ; c=c->next){
		if(c->value < min->value) min = c;
	} 
	string minVal = min->value;
	if(min == tail || min == head){
		if(size() != 1){
			if(min == tail){
				min->prev->next = NULL;
				tail = min->prev;
			}else{
				min->next->prev = NULL;
				head = min->next;
			}
		}else head=tail=min;
	}else{
		min->next->prev = min->prev;
		min->prev->next = min->next;
	}
	delete min;
	length--;
	return minVal;
}
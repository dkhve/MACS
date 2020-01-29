/*
 * File: UniversalHealthCoverage.cpp
 * ----------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the UniversalHealthCoverage problem
 * on Assignment #3.
 * [TODO: extend the documentation]
 */
#include <iostream>
#include <string>
#include "set.h"
#include "vector.h"
#include "console.h"
#include "simpio.h"
using namespace std;

/* Function: canOfferUniversalCoverage(Set<string>& cities,
 *                                     Vector< Set<string> >& locations,
 *                                     int numHospitals,
 *                                     Vector< Set<string> >& result);
 * Usage: if (canOfferUniversalCoverage(cities, locations, 4, result)
 * ==================================================================
 * Given a set of cities, a list of what cities various hospitals can
 * cover, and a number of hospitals, returns whether or not it's
 * possible to provide coverage to all cities with the given number of
 * hospitals.  If so, one specific way to do this is handed back in the
 * result parameter.
 */

void getData(Set<string>& cities,Vector< Set<string> > &locations, int &numHospitals);
void getCities(Set<string>& cities);
void getLocations(Vector< Set<string> > &locations,Set<string>& cities);
void addLocations(Vector< Set<string> > &locations , string coverage , Set<string>& cities);
bool canOfferUniversalCoverage(Set<string>& cities,Vector< Set<string> >& locations,int numHospitals,Vector< Set<string> >& result);
bool isCoverable(Set<string>& cities, Vector< Set<string> >& locations,int numHospitals, Vector< Set<string> >& result, Set<string> &covered);
int maxSize(Vector< Set<string> >& locations , Set<string> &covered);
int size(Set<string> &loc , Set<string> &covered);
Set<string> add(Set<string> &covered, Set<string> &loc);
void remove (Set<string> &covered, Set<string> &loc , Set<string> duplicates);


int main() {
    Set<string> cities;
	Vector<Set<string> > locations;
	int numHospitals;
	Vector<Set<string> > result;
	getData(cities,locations,numHospitals);
	if(canOfferUniversalCoverage(cities,locations,numHospitals,result)) cout<<"YES"<<endl;
	else cout<<"NO"<<endl;
    return 0;
}

//collects the data
void getData(Set<string>& cities,Vector< Set<string> > &locations, int &numHospitals){
	numHospitals = getInteger("Enter number of affordable hospitals: ");
	getCities(cities);
	getLocations(locations, cities);
}

//prompts the user to enter cities
void getCities(Set<string>& cities){
	string city;
	int i = 0 ;
	cout<<"Enter cities (enter 0 to break): "<<endl;
	while(true){
		cout<<  i << " - " ; 
		cin>> city;
		i++;
		if(city == "0") break;
		cities.add(city);
	}
}

//prompts the user to enter locations and which cities they cover
void getLocations(Vector< Set<string> > &locations,Set<string>& cities){
	cout<< "Enter locations like this - \" A,B,C \" (enter 0 to stop ): "<<endl;
	int i=0;
	string coverage;
	while(true){
		cout<< i<< " - " ; 
		cin>> coverage;
		i++;
		if(coverage == "0") break;
		addLocations(locations,coverage,cities);
	}
}

//adds the entered locations
void addLocations(Vector< Set<string> > &locations , string coverage, Set<string>& cities){
	Set<string> loc;
	int lastIndex = 0;
	string city;
	coverage+=",";
	for(int i = 0 ; i < coverage.length() ; i++){
		if(coverage[i] == ','){
			city = coverage.substr(lastIndex , i-lastIndex);
			if(cities.contains(city)) loc.add(city);
			else cout<< "city with that name doesn't exist"<<endl;
			lastIndex = i+1;
			
		}
	}
	locations.add(loc);
}


bool canOfferUniversalCoverage(Set<string>& cities, Vector< Set<string> >& locations,int numHospitals, Vector< Set<string> >& result){
	Set<string> covered;
	return isCoverable(cities,locations,numHospitals,result,covered);
}

// checks if given cities can be covered by numHospital hospitals and given locations
bool isCoverable(Set<string>& cities, Vector< Set<string> >& locations,int numHospitals, Vector< Set<string> >& result, Set<string> &covered){
	if(numHospitals >= 0 ){
		if(cities == covered) return true;
	}else return false;
	int max = maxSize(locations,covered);
	if(max == 0) return false;
	//if there are many locations that cover maxSize number cities
	for(int i = 0 ; i<locations.size() ; i++){
		if(size(locations[i],covered) == max){
			Set<string> duplicates = add(covered,locations[i]);
			result.add(locations[i]);
			if(isCoverable(cities,locations,numHospitals-1,result,covered)){
				return true;
			}
			result.remove(result.size()-1);
			remove(covered,locations[i],duplicates);
		}
	}
	return false;
}

//checks maximum how many uncovered cities can we cover on this step
int maxSize(Vector< Set<string> >& locations , Set<string> &covered){
	int maxSize = 0;
	foreach(Set<string> loc in locations){
		int locSize=size(loc,covered);
		if(locSize>maxSize) maxSize = locSize;
	}
	return maxSize;
}

//checks how many uncovered cities will be covered by this location
int size(Set<string> &loc , Set<string> &covered){
	int size = 0;
	foreach(string s in loc){
		if(!covered.contains(s)){
			size++;
		}
	}
	return size;
}

//stores cities which have been covered by given location and also stores duplicates
Set<string> add(Set<string> &covered, Set<string> &loc){
	Set<string> duplicates;
	foreach(string s in loc){
		if(covered.contains(s)){
			duplicates.add(s);
		}else{
			covered.add(s);
		}
	}
	return duplicates;
}

//unstores cities which have been covered by given location 
//but doesn't remove cities that had already been covered before this location
void remove (Set<string> &covered, Set<string> &loc , Set<string> duplicates){
	foreach(string s in loc){
		if(!duplicates.contains(s)){
			covered.remove(s);
		}
	}
}
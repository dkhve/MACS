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
#include "map.h"
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

void getData(Map<string,int>& cities,Vector< Set<string> > &locations, int &numHospitals);
void getCities(Map<string,int>& cities);
void getLocations(Vector< Set<string> > &locations,Map<string,int>& cities);
void addLocations(Vector< Set<string> > &locations , string coverage , Map<string,int>& cities);
int canOfferUniversalCoverage(Map<string,int>& cities,Vector< Set<string> >& locations,int numHospitals,Vector< Set<string> >& result);
int isCoverable(Map<string,int>& cities, Vector< Set<string> >& locations,int numHospitals, Vector< Set<string> >& result, Set<string> &covered);
int maxSize(Map<string,int>& cities,Vector< Set<string> >& locations , Set<string> &covered);
int size(Map<string,int>& cities,Set<string> &loc , Set<string> &covered);
Set<string> add1(Set<string> &covered, Set<string> &loc);
void remove1(Set<string> &covered, Set<string> &loc , Set<string> duplicates);


int main() {
    Map<string,int> cities;
	Vector<Set<string> > locations;
	int numHospitals;
	Vector<Set<string> > result;
	getData(cities,locations,numHospitals);
	cout<<canOfferUniversalCoverage(cities,locations,numHospitals,result)<<endl;
    return 0;
}

void getData(Map<string,int>& cities,Vector< Set<string> > &locations, int &numHospitals){
	numHospitals = getInteger("Enter number of affordable hospitals: ");
	getCities(cities);
	getLocations(locations, cities);
}

void getCities(Map<string,int>& cities){
	string city;
	int population = 0;
	int i = 0 ;
	cout<<"Enter cities and number of people they contain (enter 0 to break): "<<endl;
	while(true){
		cout<< i << " - " ; 
		cin>> city;
		i++;
		if(city == "0") break;
		cout<<"enter population : " ;
		cin>>population;
		cities.put(city,population);
	}
}

void getLocations(Vector< Set<string> > &locations,Map<string,int>& cities){
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

void addLocations(Vector< Set<string> > &locations , string coverage, Map<string,int>& cities){
	Set<string> loc;
	int lastIndex = 0;
	string city;
	coverage+=",";
	for(int i = 0 ; i < coverage.length() ; i++){
		if(coverage[i] == ','){
			city = coverage.substr(lastIndex , i-lastIndex);
			if(cities.containsKey(city)) loc.add(city);
			else cout<< "city with that name doesn't exist"<<endl;
			lastIndex = i+1;
			
		}
	}
	locations.add(loc);
}

int canOfferUniversalCoverage(Map<string,int>& cities, Vector< Set<string> >& locations,int numHospitals, Vector< Set<string> >& result){
	Set<string> covered;
	int maxCoverage = 0;
	return isCoverable(cities,locations,numHospitals,result,covered);
}

int isCoverable(Map<string,int>& cities, Vector< Set<string> >& locations,int numHospitals, Vector< Set<string> >& result, Set<string> &covered){
	if(numHospitals == 0 ){
		return 0;
	}
	int max = maxSize(cities,locations,covered);
	if(max==0) return false;
	int cur = 0;
	int maxCoverage = 0;
	for(int i = 0 ; i<locations.size() ; i++){
		if(size(cities,locations[i],covered) == max){
			Set<string> duplicates = add1(covered,locations[i]);
			cur = max + isCoverable(cities,locations,numHospitals-1,result,covered);
			if(cur>maxCoverage){
				maxCoverage = cur;
			}
			remove1(covered,locations[i],duplicates);
		}

	}
	return maxCoverage;
}

int maxSize(Map<string,int>& cities,Vector< Set<string> >& locations , Set<string> &covered){
	int max = 0;
	foreach(Set<string> loc in locations){
		int locSize=size(cities,loc,covered);
		if(locSize>max){
			max = locSize;
		}
	}
	return max;
}

int size(Map<string,int>& cities,Set<string> &loc , Set<string> &covered){
	int size = 0;
	foreach(string s in loc){
		if(!covered.contains(s)){
			size+= cities[s];
		}
	}
	return size;
}

Set<string> add1(Set<string> &covered, Set<string> &loc){
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

void remove1 (Set<string> &covered, Set<string> &loc , Set<string> duplicates){
	foreach(string s in loc){
		if(!duplicates.contains(s)){
			covered.remove(s);
		}
	}
}
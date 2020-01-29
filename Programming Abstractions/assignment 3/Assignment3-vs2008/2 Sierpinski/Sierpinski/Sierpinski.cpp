/*
 * File: Sierpinski.cpp
 * --------------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Sierpinski problem
 * on Assignment #3.
 * [TODO: extend the documentation]
 */

#include "gwindow.h"
#include <iostream>
#include <cmath>
#include "simpio.h"
using namespace std;

void drawFractal(double length, int order, GPoint centre,GWindow &gw);
void drawTriangle(double length, GPoint p , GWindow &gw);

const int WINDOW_HEIGHT = 500;
const int WINDOW_WIDTH = 500;

int main() {
	GWindow gw(WINDOW_WIDTH,WINDOW_HEIGHT);
	double length = getInteger("Enter edge length: ");
	int order = getInteger("Enter fractal order: ");
	double height = gw.getHeight();
	double width = gw.getWidth();
	GPoint centre(height/2 , width/2);
	drawFractal(length , order , centre , gw);
    return 0;
}

//draws the N order fractal triangle recursively
void drawFractal(double length , int order , GPoint centre , GWindow &gw){
	if(order == 0) {
		drawTriangle(length , centre , gw);
	}else{
		double x = centre.getX(); 
		double y = centre.getY();
		double k = sqrt(3.0);

		GPoint c1(x , y- length/(2*k)); 
		GPoint c2(x - length/4 , y + length/(4*k));
		GPoint c3(x + length/4 , y + length/(4*k));

		drawFractal(length/2 , order-1 , c1 , gw);
		drawFractal(length/2 , order-1 , c2 , gw);
		drawFractal(length/2 , order-1 , c3 , gw);
	}
}

//draws triangle with given side length and center(p)
void drawTriangle(double length , GPoint p , GWindow &gw){
	double k = sqrt(3.0);
	gw.drawPolarLine(p.getX() - length/2 , p.getY() + length/(2*k) , length,60);
	gw.drawPolarLine(p.getX() - length/2 , p.getY() + length/(2*k) , length,0);
	gw.drawPolarLine(p.getX() + length/2 , p.getY() + length/(2*k) , length,120);
}			
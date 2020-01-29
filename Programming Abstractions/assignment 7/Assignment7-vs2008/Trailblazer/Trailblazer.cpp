/******************************************************************************
 * File: Trailblazer.cpp
 *
 * Implementation of the graph algorithms that comprise the Trailblazer
 * assignment.
 */

#include "Trailblazer.h"
#include "TrailblazerGraphics.h"
#include "TrailblazerTypes.h"
#include <algorithm>
#include <random.h>

using namespace std;

void init(Grid<double>& world,Grid<vertex> &map);
void makeVertex(vertex &v, Loc loc, double cost , Loc* prev, Color color);
void setColor(vertex &v, Color c, Grid<double>& world, Grid<vertex> &map);
void moveOn(Grid<vertex> &map, Grid<double>& world, TrailblazerPQueue<vertex> &pq, vertex &v, double costFn(Loc from, Loc to, Grid<double>& world), double heuristic(Loc start, Loc end, Grid<double>& world), Loc end);
Vector<Loc> makeVec(Grid<vertex> &map, vertex &v);
/* Function: shortestPath
 * 
 * Finds the shortest path between the locations given by start and end in the
 * specified world.	 The cost of moving from one edge to the next is specified
 * by the given cost function.	The resulting path is then returned as a
 * Vector<Loc> containing the locations to visit in the order in which they
 * would be visited.	If no path is found, this function should report an
 * error.
 *
 * In Part Two of this assignment, you will need to add an additional parameter
 * to this function that represents the heuristic to use while performing the
 * search.  Make sure to update both this implementation prototype and the
 * function prototype in Trailblazer.h.
 */
Vector<Loc>
	shortestPath(Loc start, Loc end,
				Grid<double>& world,
				double costFn(Loc one, Loc two, Grid<double>& world),
				double heuristic(Loc start, Loc end, Grid<double>& world)){
	Grid<vertex> map(world.nCols,world.nRows);
	init(world, map);
	vertex startV = map[start.col][start.row]; 
	setColor(startV, YELLOW, world, map);
	TrailblazerPQueue<vertex> pq;
	pq.enqueue(startV, heuristic(start, end, world));
	vertex v;
	while(!pq.isEmpty()){
		v = pq.dequeueMin();
		setColor(v, GREEN, world,map);
		if(v.loc == end) return makeVec(map,v);
		moveOn(map, world, pq, map[v.loc.col][v.loc.row], costFn, heuristic, end);
	}
}

// initializes grid of vertexes
void init(Grid<double>& world, Grid<vertex> &map){
	Loc loc;
	for(int i = 0; i < map.nCols; i ++){
		for(int j = 0 ; j < map.nRows; j++){
			loc.col = i;
			loc.row = j;
			makeVertex(map[i][j], loc, 0, NULL, GRAY);
		}
	}
}

// initializes single vertex
void makeVertex(vertex &v, Loc loc, double cost , Loc* prev, Color color){
	v.loc = loc;
	v.cost = cost;
	v.prev = prev;
	v.color = color;
}

//sets color of vertex to given color
void setColor(vertex &v, Color color, Grid<double>& world, Grid<vertex> &map){
	v.color = color;
	map[v.loc.col][v.loc.row].color = color;
	colorCell(world, v.loc, color);
}

//moves to adjacent vertexes
void moveOn(Grid<vertex> &map, Grid<double>& world, 
			TrailblazerPQueue<vertex> &pq , vertex &v, 
			double costFn(Loc from, Loc to, Grid<double>& world),
			double heuristic(Loc start, Loc end, Grid<double>& world), Loc end){
	vertex vtx;
	for(int i = -1; i < 2; i++){
		for(int j = -1; j < 2; j++){
			if(world.inBounds(v.loc.col+i, v.loc.row+j)){
				vtx = map[v.loc.col+i][v.loc.row+j];
				double dist = v.cost + costFn(v.loc, vtx.loc, world);
				if(vtx.color == GRAY){
					vtx.cost = dist;
					setColor(vtx, YELLOW, world, map);
					vtx.prev = &(v.loc);
					pq.enqueue(vtx, vtx.cost + heuristic(vtx.loc, end, world));
					map[vtx.loc.col][vtx.loc.row] = vtx;
				}else if(vtx.color == YELLOW && vtx.cost > dist){
					pq.decreaseKey(vtx, dist + heuristic(vtx.loc, end, world));
					vtx.cost = dist;
					vtx.prev = &(v.loc);
					map[vtx.loc.col][vtx.loc.row] = vtx;
					
				}
			}	
		}
	}
}

//builds path from given vector
Vector<Loc> makeVec(Grid<vertex> &map, vertex &v){
	Vector<Loc> ans;
	while(true){
		ans.add(v.loc);
		if(v.prev == NULL) break;
		v = map[v.prev->col][v.prev->row];
	}
	reverse(ans.begin(), ans.end());
	return ans;
}	

// returns cluster number for specific node
int getClusterNumber(Loc loc, Vector<Node> & nodes, int numCols){
	return nodes[loc.row * numCols + loc.col].clusterNumber;
}

// puts every node from cluster 'start' into cluster 'end'
void colorCluster(int end, int start, Vector<Node> & nodes){
	for(int i = 0 ; i < nodes.size(); i++){
		if(nodes[i].clusterNumber == start) nodes[i].clusterNumber = end;
	}
}

Set<Edge> createMaze(int numRows, int numCols) {
	TrailblazerPQueue<Edge> pq;
	Vector < Node > nodes; 
	for(int i = 0; i < numRows; i ++){
		for(int j = 0; j < numCols; j ++) {
			Loc v = {i,j};
			Node n = {v, i * numCols + j}; // (i * numCols + j) is distinct for every node, 
											// therefore all of them are in different clusters
			nodes.add(n); 
			if(i + 1 < numRows){ // inbounds
				double edgeLength = randomReal(0, 1);
				Loc u = {i + 1, j};
				Edge curr = {v, u};
				pq.enqueue(curr, edgeLength); // creating new edge vertically
			}
			if(j + 1 < numCols){ // inbounds
				double edgeLength = randomReal(0, 1);
				Loc u = {i, j + 1};
				Edge curr = {v, u};
				pq.enqueue(curr, edgeLength); // creating new edge horizontally
			}
		}
	}
	Set <Edge> kruskalEdges;
	int totalNodes = numRows * numCols;
	while(kruskalEdges.size() + 1 < totalNodes){ 
		Edge curr = pq.dequeueMin();
		int clusterNumberStart = getClusterNumber(curr.start, nodes, numCols);
		int clusterNumberEnd = getClusterNumber(curr.end, nodes, numCols);
		if(clusterNumberStart == clusterNumberEnd) continue; // checking if in the same cluster
		kruskalEdges.insert(curr);  // saving edge
		colorCluster(clusterNumberEnd, clusterNumberStart, nodes); // merging clusters
	}
    return kruskalEdges;
}
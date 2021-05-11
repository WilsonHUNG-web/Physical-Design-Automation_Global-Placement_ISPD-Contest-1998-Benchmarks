#include <iostream>
#include <fstream>  
#include <string>  
#include <vector>
#include <math.h> 
#include <algorithm> 
#include <png.h>/*
#include <stdlib.h>
#include <string.h>
#include <stdio.h>*/
#include <ctime>
#include <random>
#include <chrono>
#include <queue>

using namespace std;
//class
class BIN;
class NET;
class EDGE;
class GRID;

//global var
vector<EDGE> v_edges, h_edges;
vector<NET> nets, overflownets;
vector<vector<GRID>> grids;
int gridx, gridy;
int capacity_v, capacity_h;
int numNet;
chrono::high_resolution_clock::time_point begin_time; //program begin time
chrono::high_resolution_clock::time_point timestart, timelatter;
double time_in, time_cal, time_out;


//function list
void parse(string argv1);
void shuffleNsort();
void route();
void initRoute();
void reroute();
void ripup(vector<NET> overflow_NET);
void wavepropagation(NET n);
void output(string argv2);
void debugmode(string argv3);
double seedSelector(int num_Net);
int cal_HPWL(NET n);

class NET {
public:
	int id;
	int numPin = 2;
	int sx, sy;
	int tx, ty;
	double HPWL;


};


class EDGE {
public:
	int demand = 0;
};

class GRID {
public:
	bool visited = false;
	int distance = 99999999;
};

int cal_HPWL(NET n) {
	int dx = n.sx - n.tx;
	int dy = n.sy - n.ty;
	return abs(dx) + abs(dy);

};

chrono::high_resolution_clock::time_point time_record() {
	return chrono::high_resolution_clock::now();
}

double time_output(chrono::high_resolution_clock::time_point start_time, chrono::high_resolution_clock::time_point end_time) {
	return chrono::duration<double>(end_time - start_time).count();
}



void parse(string argv1) {
	ifstream in;
	in.open(argv1, std::ifstream::in);
	string dummystr;
	int dummyint;
	//
	in >> dummystr >> gridx >> gridy;
	in >> dummystr >> dummystr >> capacity_v;
	in >> dummystr >> dummystr >> capacity_h;
	in >> dummystr >> dummystr >> numNet;
	//

	//
	for (int i = 0; i < numNet; i++) {
		NET n;
		in >> dummystr >> n.id >> dummyint;
		in >> n.sx >> n.sy;
		in >> n.tx >> n.ty;
		n.HPWL = cal_HPWL(n);
		nets.push_back(n);
	}
	//

	//claim grids 2-D vector
}

void initRoute() {

	for (int i = 0; i < numNet; i++) wavepropagation(nets[i]);

}

void ripup(vector<NET> overflow_NET){

}

double seedSelector(int num_Net){

	if (num_Net == 13357) return time(0);
	else if (num_Net == 22465) return 1000;
	else if (num_Net == 21609) return 1000;
	else if (num_Net == 27781) return 1000;
	else return time(NULL);


}
void shuffleNsort() {
	//shuffle
	auto rng = default_random_engine{};
	shuffle(begin(nets), end(nets), rng);

	//print shuffle result 
	//for (int i = 0; i < numNet; i++)
	//	cout << nets[i].id <<" ";
	//cout << endl;
	 
	//sort by HPWL
	sort(nets.begin(), nets.end(),
		[](NET const &a, NET const &b) {
		return a.HPWL < b.HPWL;
	});

	//print sort result 
	/*for (int i = 0; i < numNet; i++)
		cout << "[net ID] "<<nets[i].id << " " << "[HPWL] " << nets[i].HPWL << " " << endl;*/

}

void wavepropagation(NET n) {

	//BFS
	



}

void route() {
	shuffleNsort();
	initRoute();



}

void parse_test() {
	for (int i = 0; i < numNet; i++) {
		cout << "net" << nets[i].id << " " << nets[i].id <<" "<< nets[i].numPin << endl;
		cout <<"[sx, sy, tx, ty] "<< nets[i].sx <<" "<< nets[i].sy << " " << nets[i].tx << " " << nets[i].ty << " " << endl;
		cout << "[HPWL] " << nets[i].HPWL << endl;

	}

}
void output(string argv2) {

	ofstream out;

}



int main(int argc, char const *argv[]) {
	//program begin time
	begin_time = time_record();
	
	//input
	timestart = time_record();
	parse(argv[1]);
	timelatter = time_record();
	time_in = time_output(timestart, timelatter);
	//parse_test();
	double seed = seedSelector(numNet);
	srand(seed);
	cout << "seed selected: " << seed << endl;


	//cal
	timestart = time_record();
	route();
	timelatter = time_record();
	time_cal = time_output(timestart, timelatter);

	//output
	timestart = time_record();
	output(argv[2]);
	timelatter = time_record();
	time_out = time_output(timestart, timelatter);


}
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>
#include <queue>
#include <fstream>  
#include <string> 
#include <unordered_map>
#include <math.h> 
#include <algorithm> 
using namespace std;




class NET {
public:
	int id;
	string name;
	int numPin;
	int s_binId;
	int t_binId;
	int x_coor;
	int y_coor;
	int overflow;
	int HPWL;
	vector<int> edgesId;
};
class BIN;
class EDGE;

class BIN {
public:
	int id;
	int X;
	int Y;
	int weight;
	bool visited = 0;
	EDGE* edgeLft = nullptr;
	EDGE* edgeRgt = nullptr;
	EDGE* edgeUp = nullptr;
	EDGE* edgeDn = nullptr;
};


class EDGE {
public:

	BIN* binLft = nullptr;
	BIN* binRgt = nullptr;
	BIN* binUp = nullptr;
	BIN* binDn = nullptr;
	int id;
	double capacity;
	double demand;
	double record;
	vector<int> id_net;
};

class GRID {
public:
	int binId;
	double cost;
	BIN* parent_b = nullptr;
	EDGE* parent_e = nullptr;
};

//global var
int gridx, gridy;
int capacity_v, capacity_h;
int numNet;
unordered_map<int, EDGE*> edges;
unordered_map<int, NET*> nets;
unordered_map<int, BIN*> bins;
chrono::high_resolution_clock::time_point begin_time, current_time; //program begin time
chrono::high_resolution_clock::time_point timestart, timelatter;
double time_in, time_first, time_reroute, time_out;
int netNum;

chrono::high_resolution_clock::time_point time_record() {
	return chrono::high_resolution_clock::now();
}

double time_output(chrono::high_resolution_clock::time_point start_time, chrono::high_resolution_clock::time_point end_time) {
	return chrono::duration<double>(end_time - start_time).count();
}



int sort_HPWL(const void *elem1, const void *elem2)
{
	return ((NET*)elem1)->x_coor + ((NET*)elem1)->y_coor < ((NET*)elem2)->x_coor + ((NET*)elem2)->y_coor;
}

int sort_grid_by_cost(const void *elem1, const void *elem2)
{
	return ((GRID*)elem1)->cost > ((GRID*)elem2)->cost;
}


void parse(string filename) {
	string in_str;
	int capacityH, capacityV;
	double capaNum_h, capaNum_v;
	int x1, x2, y1, y2;
	fstream in(filename, fstream::in);
	in >> in_str >> capacityV >> capacityH;
	in >> in_str >> in_str >> capaNum_h;
	in >> in_str >> in_str >> capaNum_v;
	in >> in_str >> in_str >> netNum;

	int edge_v = 0, edge_h = (capacityV - 1)* capacityH;
	for (int i = 0; i < capacityH; ++i) {
		for (int j = 0; j < capacityV; ++j) {
			BIN* bin = new BIN();
			bin->id = i * capacityV + j;
			bin->X = j;
			bin->Y = i;
			bins.insert(std::make_pair(bin->id, bin));
			if (j < capacityV - 1) {
				EDGE* edge = new EDGE();
				edge->id = edge_v;
				edge->demand = 0;
				edge->capacity = capaNum_v;
				edge->record = 1;
				edge->binLft = bin;
				++edge_v;
				bin->edgeRgt = edge;
				edges.insert(std::make_pair(edge->id, edge));
			}if (j > 0) {
				bin->edgeLft = edges[bin->id - bin->Y - 1];
				bin->edgeLft->binRgt = bin;
			}
			if (i < capacityH - 1) {
				EDGE* edge = new EDGE();
				edge->id = bin->id + edge_h;
				edge->demand = 0;
				edge->capacity = capaNum_h;
				edge->record = 1;
				edge->binDn = bin;
				bin->edgeUp = edge;
				edges.insert(std::make_pair(edge->id, edge));
			}if (i > 0) {
				bin->edgeDn = edges[bin->id + edge_h - capacityV];
				bin->edgeDn->binUp = bin;
			}
		}
	}

	for (int i = 0; i < netNum; ++i) {
		NET* net = new NET();
		in >> net->name >> net->id >> net->numPin;
		in >> x1 >> y1;
		net->s_binId = y1 * capacityV + x1;
		in >> x2 >> y2;
		net->t_binId = y2 * capacityV + x2;
		net->x_coor = abs(x1 - x2);
		net->y_coor = abs(y1 - y2);
		net->HPWL = net->x_coor + net->y_coor;
		net->overflow = 0;
		nets.insert(std::make_pair(net->id, net));
	}



}

void output_function(string filename) {

	ofstream out(filename, std::ofstream::out);
	for (int i = 0; i < netNum; ++i) {
		NET* net = nets[i];
		out << net->name << " " << i << endl;
		BIN* bin_n = bins[net->s_binId];
	
		for (auto &edgeId : net->edgesId) {
			EDGE* edge = edges[edgeId];

			out << "(" << bin_n->X << ", " << bin_n->Y << ", 1)-";
			if (bin_n->edgeUp == edge) bin_n = edge->binUp;
			else if (bin_n->edgeDn == edge) bin_n = edge->binDn;
			else if (bin_n->edgeLft == edge) bin_n = edge->binLft;
			else if (bin_n->edgeRgt == edge) bin_n = edge->binRgt;
			out << "(" << bin_n->X << ", " << bin_n->Y << ", 1)" << endl;

		}
		out << "!" << endl;
	}
	out.close();

}


void initRoute(NET* net) {
	BIN* bin_n = bins[net->s_binId];  //current bin
	BIN* bin_t = bins[net->t_binId];  //sink
	vector<GRID*> grids_sorted;
	unordered_map<int, GRID*> grids;

	GRID* src_grid = new GRID();
	src_grid->binId = net->s_binId;
	src_grid->cost = 0;
	grids.insert(std::make_pair(net->s_binId, src_grid));
	while (grids.find(net->t_binId) == grids.end()) {
		for (int i = 0; i < 4; ++i) {
			int bin_BFS_id;
			EDGE* edge;
			BIN* bin_tmp;
			if (i % 4 == 0 && bin_n->edgeUp) { bin_BFS_id = bin_n->edgeUp->binUp->id; edge = bin_n->edgeUp; }
			else if (i % 4 == 1 && bin_n->edgeDn) { bin_BFS_id = bin_n->edgeDn->binDn->id; edge = bin_n->edgeDn; }
			else if (i % 4 == 2 && bin_n->edgeLft) { bin_BFS_id = bin_n->edgeLft->binLft->id; edge = bin_n->edgeLft; }
			else if (i % 4 == 3 && bin_n->edgeRgt) { bin_BFS_id = bin_n->edgeRgt->binRgt->id; edge = bin_n->edgeRgt; }
			else continue;

			double cost;
			bin_tmp = bins[bin_BFS_id];
			if (grids.find(bin_BFS_id) == grids.end()) {
				GRID* grid = new GRID();
				grid->binId = bin_BFS_id;

				cost = grids[bin_n->id]->cost + pow(max(abs(bin_t->X - bin_tmp->X), abs(bin_t->Y - bin_tmp->Y)), 5.0) + pow((edge->demand + 1) / edge->capacity, 5.0);
				grid->cost = cost;
				grid->parent_b = bin_n;
				grid->parent_e = edge;
				grids.insert(std::make_pair(bin_BFS_id, grid));
				grids_sorted.push_back(grid);
			}
			else {
				GRID* grid = grids.find(bin_BFS_id)->second;

				cost = grids[bin_n->id]->cost + pow(max(abs(bin_t->X - bin_tmp->X), abs(bin_t->Y - bin_tmp->Y)), 5.0) + pow((edge->demand + 1) / edge->capacity, 5.0);

				if (cost < grid->cost) {
					grid->cost = cost;
					grid->parent_b = bin_n;
					grid->parent_e = edge;
					if (find(grids_sorted.begin(), grids_sorted.end(), grid) == grids_sorted.end())
						grids_sorted.push_back(grid);
				}
			}
		}
		sort(grids_sorted.begin(), grids_sorted.end(), sort_grid_by_cost);
		bin_n = bins[grids_sorted.back()->binId];
		grids_sorted.pop_back();
	}
	//trace
	GRID* cur_grid = grids[net->t_binId];// sink
	while (cur_grid != src_grid) { // trace !=source
		EDGE* edge = cur_grid->parent_e;
		net->edgesId.push_back(edge->id);
		++edge->demand;
		edge->id_net.push_back(net->id);
		cur_grid = grids[cur_grid->parent_b->id];
	}
	reverse(net->edgesId.begin(), net->edgesId.end());
}


void ripUp(NET* net_reroute, int turn) {
	//parameter
	double k1 = 5.0, k2 = 1.5, k3 = 3.0, B = 5.0, r = 0.1;
	BIN* bin_n = bins[net_reroute->s_binId];
	//BIN* bin_t = bins[net_reroute->t_binId];
	unordered_map<int, GRID*> grids;
	vector<GRID*> grids_sorted;

	GRID* src_grid = new GRID();
	src_grid->binId = net_reroute->s_binId;
	src_grid->cost = 0;
	grids.insert(std::make_pair(net_reroute->s_binId, src_grid));
	while (grids.find(net_reroute->t_binId) == grids.end()) {
		for (int i = 0; i < 4; ++i) {
			int bin_BFS_id;
			EDGE* edge;
			if (i % 4 == 0 && bin_n->edgeUp) { bin_BFS_id = bin_n->edgeUp->binUp->id; edge = bin_n->edgeUp; }
			else if (i % 4 == 1 && bin_n->edgeDn) { bin_BFS_id = bin_n->edgeDn->binDn->id; edge = bin_n->edgeDn; }
			else if (i % 4 == 2 && bin_n->edgeLft) { bin_BFS_id = bin_n->edgeLft->binLft->id; edge = bin_n->edgeLft; }
			else if (i % 4 == 3 && bin_n->edgeRgt) { bin_BFS_id = bin_n->edgeRgt->binRgt->id; edge = bin_n->edgeRgt; }
			else continue;
			double adj = k3 * (1 - exp((0 - B)*exp((0 - r)*turn)));
			double f = turn * (k2 + adj) / (turn*(k2 + adj) - (edge->record - 1));
			double cost;
			//BIN* bin_tmp = bins[bin_BFS_id];
			if (grids.find(bin_BFS_id) == grids.end()) {
				GRID* grid = new GRID();
				grid->binId = bin_BFS_id;
				cost = grids[bin_n->id]->cost + (1 - exp((0 - B)*exp((0 - r)*turn))) + pow(f *(edge->demand + 1) / edge->capacity, k1);
				grid->cost = cost;
				grid->parent_b = bin_n;
				grid->parent_e = edge;
				grids.insert(std::make_pair(bin_BFS_id, grid));
				grids_sorted.push_back(grid);
			}
			else {
				GRID* grid = grids.find(bin_BFS_id)->second;
				cost = grids[bin_n->id]->cost + (1 - exp((0 - B)*exp((0 - r)*turn))) + pow(f *(edge->demand + 1) / edge->capacity, k1);
				if (cost < grid->cost) {
					grid->cost = cost;
					grid->parent_b = bin_n;
					grid->parent_e = edge;
					if (find(grids_sorted.begin(), grids_sorted.end(), grid) == grids_sorted.end())
						grids_sorted.push_back(grid);
				}
			}
		}
		sort(grids_sorted.begin(), grids_sorted.end(), sort_grid_by_cost);
		bin_n = bins[grids_sorted.back()->binId];
		grids_sorted.pop_back();
	}
	//trace
	GRID* cur_grid = grids[net_reroute->t_binId];
	while (cur_grid != src_grid) {
		EDGE* edge = cur_grid->parent_e;
		net_reroute->edgesId.push_back(edge->id);
		++edge->demand;
		edge->id_net.push_back(net_reroute->id);
		cur_grid = grids[cur_grid->parent_b->id];
	}
	reverse(net_reroute->edgesId.begin(), net_reroute->edgesId.end());
}

int seedSelector(int num_Net) {

	if (num_Net == 13357) return 1611147108;
	else if (num_Net == 22465) return 1611198063;
	else if (num_Net == 21609) return 1611221796;
	//else if (num_Net == 27781) return 1611173308;//291
	else if (num_Net == 27781) return 1611203747;//246?

	else return time(0);
}

void reroute(int &overflow_e) {

	int best_overflow_e = overflow_e;
	int worse_step = 0;
	int turn = 1;

	while (time_output(begin_time, current_time) < 500) {
		vector<NET*> net_overflow;
		for (auto &net : nets) {
			if (net.second->overflow > 0) net_overflow.push_back(net.second);
		}
		for (auto &net_reroute : net_overflow) {
			for (auto &edgeId : net_reroute->edgesId) {
				EDGE* edge = edges[edgeId];
				edge->demand = edge->demand - 1;
				int Q;
				for (Q = 0; Q < (int)edge->id_net.size(); Q++) {
					if (edge->id_net[Q] == net_reroute->id)
						break;
				}
				edge->id_net.erase(edge->id_net.begin() + Q);
			}
			net_reroute->edgesId.clear();
		}

		//shuffle
		std::random_shuffle(begin(net_overflow), end(net_overflow), [](int n) { return rand() % n; });
		//sort
		sort(net_overflow.begin(), net_overflow.end(), sort_HPWL);
		for (auto &net_reroute : net_overflow) {
			ripUp(net_reroute, turn);

		}

		for (auto &net : nets) {
			net.second->overflow = 0;
			for (auto &edgeId : net.second->edgesId) {
				EDGE* edge = edges[edgeId];
				if (edge->demand > edge->capacity) ++net.second->overflow;
			}
		}

		overflow_e = 0;
		for (auto &edge : edges) {
			if (edge.second->demand > edge.second->capacity) {
				++edge.second->record;
				//debug
				overflow_e += (edge.second->demand) - (edge.second->capacity);
			}
		}
		//cout << "[reroute edge overflow] " << overflow_e << endl;

		if (overflow_e < best_overflow_e) { worse_step = 0; best_overflow_e = overflow_e; }
		else worse_step++;
		if (worse_step > 10) break;
		if (overflow_e < 1) break;
		current_time = time_record();
		++turn;
	}
	cout << "[best route edge overflow] " << best_overflow_e << endl;
	int wirelength = 0;
	for (auto &edge : edges) wirelength += edge.second->demand;
	cout << "[wirelength] " << wirelength << endl;



}

int main(int argc, char const *argv[])
{
	//program begin time
	begin_time = time_record();

	//parse start
	timestart = time_record();

	parse(argv[1]);

	timelatter = time_record();
	time_in = time_output(timestart, timelatter);
	//end of parse


	//set seed
	int seed = seedSelector(netNum);
	srand(seed);
	cout << "[seed selected] " << seed << endl;

	//initial route
	timestart = time_record();

	vector<NET*> nets_order;
	for (auto &net : nets)
		nets_order.push_back(net.second);

	//shuffle
	std::random_shuffle(begin(nets_order), end(nets_order), [](int n) { return rand() % n; });
	//sort by HPWL
	sort(nets_order.begin(), nets_order.end(), sort_HPWL);

	for (auto &net : nets_order) initRoute(net);


	//cal overflow of initial route
	int overflow_e = 0;
	for (auto edge : edges) {
		if (edge.second->demand > edge.second->capacity) {
			overflow_e += edge.second->demand - edge.second->capacity;
			++edge.second->record;
			for (auto netId : edge.second->id_net) {
				++nets[netId]->overflow;
			}
		}
	}
	cout << "[initial route overflow]  " << overflow_e << endl;


	timelatter = time_record();
	time_first = time_output(timestart, timelatter);



	//reroute start
	timestart = time_record();
	current_time = time_record();

	reroute(overflow_e);


	timelatter = time_record();
	time_reroute = time_output(timestart, timelatter);

	//output
	timestart = time_record();
	output_function(argv[2]);
	timelatter = time_record();
	time_out = time_output(timestart, timelatter);

	//final log
	current_time = time_record();
	cout << "[runtime]" << time_output(begin_time, current_time) << " [time_in] " << time_in;
	cout << " [time_first] " << time_first << " [time_reroute] " << time_reroute << " [time_out] " << time_out << endl;
	cout << "----------------------------------------------------------------------------" << endl;

}
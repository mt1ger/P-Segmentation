#ifndef _ROUTINGTABLE_H
#define  _ROUTINGTABLE_H

#include "Bhandari.h"
#include "Dijkstra.h"
#include "Network.h"

using namespace std;

class RoutingTable {
	public:
		RoutingTable (Network * net) {network = net;}
		~RoutingTable () {};

		vector< vector<int> > get_shortest_path (int src, int dest);
		void generate_routing_table ();

	private:
		// To store predecessors of each route for each pairs of source and destination
		vector< vector<int> > predecessors; 	

		vector< vector<int> > single_src_routing_table (int src);
		void get_predecessor_list ();
		void generate_dijkstra_routing_table ();
		
		Network * network;
};

#endif

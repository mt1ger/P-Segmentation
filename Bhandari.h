#ifndef _BHANDARI_H 
#define _BHANDARI_H

#include <vector>
#include <list>
#include "Dijkstra.h"
#include "RoutingTable.h"
#include "Network.h"

using namespace std;

class Bhandari {
	public:
		Bhandari (Network * net) {network = net;}
		~Bhandari () {}
		
		vector< vector<int> > eliminate_common_links (int src, int dest, int k);
		
		// void ();
		// void ();

	private:
		Network * network;

		vector< vector<double> > UpdatedTopology;
		void update_topology (int src, int dest, vector<int> &path);
		vector<int> sp_with_negative_weight (int src, int dest);

		vector< vector<int> > NegaWeightPaths;
		vector< vector<int> > DisjointPaths;
};

#endif


#include <iostream>
#include <vector>
#include <stack>
#include <climits>
#include "Bhandari.h"

using namespace std;

void bubble_sort(vector<int> &ar, int n)
{
	int i, j, temp = 0;
	for (i = 0; i < n - 1; i++)
		for (j = n - 1; j > i; j--)
		{
			if (ar[j] < ar[j - 1])
			{
				temp = ar[j];
				ar[j] = ar[j - 1];
				ar[j - 1] = temp;
			}
			printf("%d %d\n", ar[j], ar[j - 1]);
		}
}

void Bhandari::update_topology (int src, int dest, vector<int> &path) {
	for (int i = 1; i < path.size (); i++) {
		UpdatedTopology[path[i - 1]][path[i]] = INT_MAX;
		UpdatedTopology[path[i]][path[i - 1]] = UpdatedTopology[path[i]][path[i - 1]] * -1;
	}
}

vector<int> Bhandari::sp_with_negative_weight (int src, int dest) {
	vector<int> ShortestPath;
	vector<int> Predecessors;
	Dijkstra dijkstra (network);

	dijkstra.ajacent_nodes (dijkstra.AjacentNodes, UpdatedTopology);
	dijkstra.shortest_path (src, dest, Predecessors, UpdatedTopology);

	stack<int> PathStack;
	
	PathStack.push (dest);
	int temp = dest;
	while (temp != src) {
		PathStack.push (Predecessors.at (temp));
		temp = Predecessors.at (temp);
	}

	while(!PathStack.empty())
	{
		ShortestPath.push_back(PathStack.top());
		PathStack.pop();		
	}	
	return ShortestPath;
}

vector< vector<int> > Bhandari::eliminate_common_links (int src, int dest, int k) {

	vector<int> SizeofPaths;
	Dijkstra dijkstra (network);
	dijkstra.ajacent_nodes (dijkstra.AjacentNodes, network->NodesWeight);

	NegaWeightPaths.clear ();
	UpdatedTopology = network->NodesWeight;
	NegaWeightPaths.push_back (network->DRoutingTable[src][dest]);

	if (k > network->NumofAjacentNodes[dest]) k = network->NumofAjacentNodes[dest];
	if (k > network->NumofAjacentNodes[src]) k = network->NumofAjacentNodes[src];

	for (int i = 0; i < k - 1; i++) {
		for (int j = 0; j < NegaWeightPaths.size (); j++) {
			update_topology (src, dest, NegaWeightPaths[j]);
		}
		vector<int> SP;
		SP = sp_with_negative_weight (src, dest);
		NegaWeightPaths.push_back (SP);
	}
	
	for (int i = 0; i < NegaWeightPaths.size () - 1; i++) {
		if (NegaWeightPaths[i].size () > 3) {
			for (int j = i + 1; j < NegaWeightPaths.size (); j++) {
				for (int m = 1; m < NegaWeightPaths[i].size () - 1; m++) {
					for (int n = NegaWeightPaths[j].size () - 2; n > 0; n--) {
						if (NegaWeightPaths[i][m] == NegaWeightPaths[j][n]) {
							if (NegaWeightPaths[i][m + 1] == NegaWeightPaths[j][n - 1]) {
								list<int> P1p1 (NegaWeightPaths[i].begin (), NegaWeightPaths[i].begin () + m + 1);
								list<int> P1p2 (NegaWeightPaths[i].begin () + m + 2, NegaWeightPaths[i].end ());
								list<int> P2p1 (NegaWeightPaths[j].begin (), NegaWeightPaths[j].end () - (NegaWeightPaths[j].size () - n));
								list<int> P2p2 ((NegaWeightPaths[j].end () - (NegaWeightPaths[j].size () - n) + 1), NegaWeightPaths[j].end ());

								while (NegaWeightPaths[i][m + 2] == NegaWeightPaths[j][n - 2]) {
									P1p2.pop_front ();
									P2p1.pop_back ();
									m = m + 1;
									n = n - 1;
								}
								P1p1.splice (P1p1.end (), P2p2);
								P2p1.splice (P2p1.end (), P1p2);
								NegaWeightPaths[i].assign (P1p1.begin (), P1p1.end ());
								NegaWeightPaths[j].assign (P2p1.begin (), P2p1.end ());
							}
							break;
						}
					}
				}
			}
		}
	}

	return NegaWeightPaths;
}

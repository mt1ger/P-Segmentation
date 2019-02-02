#include <iostream>
#include <cstdio>
#include <vector>
#include <climits>
#include "Topology.h"

using namespace std;

void Topology::read_topology (void) {
	FILE * nettopo;
	int temp;
	vector <double> HNodesWeight;

	nettopo = fopen (network->FileName, "r");

	fscanf (nettopo, "%d", &network->NumofNodes);		
	cout << "There are " << network->NumofNodes << " nodes in this topology." << endl;

	for (int i = 0; i < network->NumofNodes; i++) {
		for (int j = 0; j < network->NumofNodes; j++) {
			fscanf (nettopo, "%d", &temp); 
			if (temp == -1) HNodesWeight.push_back (INT_MAX);
			else HNodesWeight.push_back (temp);
		}
		network->NodesWeight.push_back (HNodesWeight);
		HNodesWeight.clear ();
	}

	#ifdef DEBUG_content_in_Topology
	for (int i = 0; i < network->NumofNodes; i++)
	{
		for (int j = 0; j < network->NumofNodes; j++)
		{
			cout << network->NodesWeight[i][j] << ' ';
		}
		cout << endl;
	}
	#endif

	fclose (nettopo);
}



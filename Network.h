#ifndef _NETWORK_H
#define _NETWORK_H

#include <vector>
// #include "TrafficGenerator.h"
// #include "RoutingTable.h"


#define K 10
#define NUMOFSECTIONS 2
#define GB 1 // Guardband
#define NUMOFSPECTRALSLOTS 128
// #define NUMOFSPECTRALSLOTS 16
#define MAX_DATASIZE_REQUEST 200 
// #define MAX_DATASIZE_REQUEST 50 
#define BW_SPECSLOT 12.5

/*** Switches of RoutingTable related Probes ***/
// #define DEBUG_content_in_routingTable
// #define DEBUG_content_in_Dijkstra_routing_table
// #define DEBUG_content_in_Topology
// #define DEBUG_content_in_AjacentNodes
// #define DEBUG_content_in_PendingNodes
// #define DEBUG_content_in_Predecessors

/*** Switches of Debugging Probes ResourceAssignment.cpp ***/
// #define DEBUG_print_resource_state_on_the_path
// #define DEBUG_print_AvailableSpecSlots
// #define DEBUG_print_PotentialSections_and_PC
// #define DEBUG_modulation_checking
// #define DEBUG_print_SortedSections
// #define DEBUG_check_AssignedSpectralSection
// #define DEBUG_collect_EventID_of_blocked_requests //need to collaberate with debug_print_eventid_of_blocked_requests

// #define DISPLAY_available_path
// #define DISPLAY_path_order
// #define DISPLAY_selected_path
// #define DISPLAY_metrics
// #define PRINT_allocation_block_release

/*** Switches of TrafficGenerator.cpp ***/
// #define DEBUG_print_new_built_Event

using namespace std;

class RoutingTable;

class Network {
	public:
		/*** Functions ***/  
		void init ();
		void simulation ();


		/*** VARIABLES ***/ 
		// Inputted Variables   
		char FileName[500];
		double Lambda, Mu;
		long long NumofRequests;
		unsigned int NumofCores;
		int SectionNumLimitation;
		double a, b, c;


		// Topology Variables
		unsigned int NumofNodes; 
		vector< vector<double> > NodesWeight;
		vector<int> NumofAjacentNodes;
		vector< vector< vector<int> > > DRoutingTable;
		vector< vector< vector< vector<int> > > > routingTable;

		// Metrics per Request
		double TotalHoldingTime;
		long long TotalTranspondersUsed;
		long long TotalCoresUsed;
		long long TotalGBUsed;
		double AvgHoldingTime;
		double AvgTranspondersUsed;
		double AvgGBUsed;
		double AvgCoresUsed;	


		// Metrics to Measure Fregmentation 
		long long TotalDataSize;
		long long TotalSSUsed;
		long long TotalSSOccupied;

		double AvgIntFrag;
		double AvgExtFrag;
		double AvgHybridFrag;

		//Temp Data // to collect the hop count and path length of the 2 least hop count or shortest paths
		int TotalNoHP0 = 0;
		int TotalNoHP1 = 0;
		int TotalPLP0 = 0;
		int TotalPLP1 = 0;
		double AvgNoHP0; // Average Number of Hops
		double AvgNoHP1; // Average Number of Hops
		double AvgPLP0; // Average Path Length
		double AvgPLP1; // Average Path Length

		// Timer and counters 
		long long RequestCounter; // Generated Number of Requests
		long long NumofAllocatedRequests;
		long long NumofDoneRequests; // Requests which are successfully allocated and released, or blocked will be considered as DoneRequests
		long long NumofFailedRequests; // Number of Requests that are blocked
		long long NumofTransponders; // Number of TranspondersUsed at a Specific Time
		long long MaxNumofTransponders; // For one time simulation, Maximum Number of Transponders Used Simultaneously
		int NumofSections;
		int MaxNoH, MinNoH;
		int MaxNumofSections;
		double SystemClock;
		

		// Resource Variables
		vector< vector< vector< vector<bool> > > > SpectralSlots;

		// Debugging Variables
		vector<int> BlockedRequests;
		
		
	private:
};

#endif

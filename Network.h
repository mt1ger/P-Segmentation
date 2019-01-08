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

/*** Switches of Debugging Probes ResourceAssignment.cpp ***/
#define DEBUG_print_resource_state_on_the_path
#define DEBUG_print_AvailableSpecSlots
#define DEBUG_print_PotentialSections_and_PC
#define DEBUG_print_SortedSections
#define DEBUG_collect_EventID_of_blocked_requests //need to collaberate with debug_print_eventid_of_blocked_requests
#define DISPLAY_available_path
#define DISPLAY_path_order
#define DISPLAY_selected_path
#define DISPLAY_metrics
#define PRINT_allocation_block_release

/*** Switches of TrafficGenerator.cpp ***/
#define DEBUG_print_new_built_Event

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

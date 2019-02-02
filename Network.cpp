// #define DEBUG_test_sequence_in_queue
// #define DEBUG_probe_NumofDoneRequests_and_NumofRequests
// #define DEBUG_content_in_routingTable
#define DEBUG_print_EventID_of_blocked_requests
 

#include <iostream> 
#include "RoutingTable.h"
#include "Network.h"
#include "Bhandari.h"
#include "TrafficGenerator.h"
#include "Event.h"

#include "ResourceAssignment.h"

using namespace std;

void Network::init () {
	vector<bool> SpectralSlotIndex;
	vector< vector<bool> > CoreIndex;
	vector< vector< vector<bool> > > SuccessorIndex;

	RoutingTable routinGTable (this);   
	routinGTable.generate_routing_table ();
	MaxNoH = 1;
	MinNoH = 100;
	for (int i = 0; i < routingTable.size (); i++) {
		for (int j = i + 1; j < routingTable[i].size (); j++) {
			for (int m = 0; m < routingTable[i][j].size (); m++) {
				if ((routingTable[i][j][m].size () - 1) > MaxNoH) MaxNoH = routingTable[i][j][m].size () - 1;
				if ((routingTable[i][j][m].size () - 1) < MinNoH) MinNoH = routingTable[i][j][m].size () - 1;
			}
		}
	}
	
	#ifdef DEBUG_content_in_routingTable
	for (int a = 0; a < NumofNodes; a++) {
		for (int b = 0; b < NumofNodes; b++) {
			cout << "***** ";
			cout << "SRC is: " << a << " and DEST is: " << b << endl;
			for (int c = 0; c < routingTable[a][b].size (); c++) {
				for (int d = 0; d < routingTable[a][b][c].size (); d++) {
					cout << routingTable[a][b][c][d] << ' ';
				}
				cout << endl;
			}
			cout << endl;
		}
		cout << endl;
	}
	// for (int a = 0; a < NumofNodes; a++) {
	// 	for (int b = 0; b < NumofNodes; b++) {
	// 		cout << "***** ";
	// 		cout << "SRC is: " << a + 1 << " and DEST is: " << b + 1 << endl;
	// 		for (int c = 0; c < routingTable[a][b].size (); c++) {
	// 			for (int d = 0; d < routingTable[a][b][c].size (); d++) {
	// 				cout << routingTable[a][b][c][d] + 1 << ' ';
	// 			}
	// 			cout << endl;
	// 		}
	// 		cout << endl;
	// 	}
	// 	cout << endl;
	// }
	#endif
	
	// Initialization of Resource Matrix
	for (int i = 0; i < NumofNodes; i++) {
		for (int j = 0; j < NumofNodes; j++) {
			for (int c = 0; c < NumofCores; c++) {
				for (int k = 0; k < NUMOFSPECTRALSLOTS; k++) {
					SpectralSlotIndex.push_back (false);
				}
				CoreIndex.push_back (SpectralSlotIndex);
				SpectralSlotIndex.clear ();
			}
			SuccessorIndex.push_back (CoreIndex);
			CoreIndex.clear ();
		}
		SpectralSlots.push_back (SuccessorIndex);
		SuccessorIndex.clear ();
	}

	SystemClock = 0;
	RequestCounter = 0;
	NumofDoneRequests = 0;
	NumofFailedRequests = 0;
	NumofAllocatedRequests = 0;
	NumofTransponders = 0;
	MaxNumofTransponders = 0;
	MaxNumofSections = 0;

	TotalHoldingTime = 0;
	TotalTranspondersUsed = 0;
	TotalCoresUsed = 0;
	TotalGBUsed = 0;
	TotalSSUsed = 0;
	TotalSSOccupied = 0;
	TotalDataSize = 0;
	AvgExtFrag = 0;
	AvgIntFrag = 0;
	AvgHybridFrag = 0;

	a = 0.4; b = 0.3; c = 0.3;
}

void Network::simulation () {
	EventQueue *eventQueue = new EventQueue ();
	TrafficGenerator trafficGenerator (this, eventQueue);
	ResourceAssignment resourceAssignment (this, eventQueue);


	// Generate the first event
	trafficGenerator.gen_first_request ();

	while (!eventQueue->ev_Queue.empty ()) {
		Event * event;
		event = eventQueue->ev_Queue.front ();

		if (SystemClock <= event->EventTime) {
			SystemClock = event->EventTime;
		}

		#ifdef DEBUG_test_sequence_in_queue
		// The code to test if Events in Queue is in right order
		list<Event*>::iterator iter;
		cout << "Print Event key properties in Queue" << endl;
		for (iter = eventQueue->ev_Queue.begin (); iter != eventQueue->ev_Queue.end (); iter++) {
			cout << (*iter)->EventID << ' ' ;
			if ((*iter)->EventType == c_Release) cout << "Release" << ' ';
			else if ((*iter)->EventType == c_Request) cout << "Request" << ' ';
			cout << (*iter)->EventTime << '\t';
		}
		cout << endl;
		#endif


		if (event->EventType == c_Request) {
			resourceAssignment.handle_requests ((CircuitRequest *) event);
			if (RequestCounter != NumofRequests) 
			trafficGenerator.gen_request (SystemClock);
		}
		else if (event->EventType == c_Release) {
			resourceAssignment.handle_releases ((CircuitRelease *) event);
		}

		eventQueue->ev_Queue.pop_front (); //This will destroy the poped Event *.
		if (NumofTransponders > MaxNumofTransponders) MaxNumofTransponders = NumofTransponders;
		if (NumofSections > MaxNumofSections) MaxNumofSections = NumofSections;

	#ifdef DEBUG_probe_NumofDoneReqeusts_and_NumofRequests
		cout << " " << NumofDoneRequests << " and " << NumofRequests << endl;
	#endif
		if ((NumofFailedRequests + NumofAllocatedRequests) == NumofRequests) break;
	}

	cout << endl << "************************************************************" << endl;
	#ifdef DEBUG_print_EventID_of_blocked_requests
	cout << "Start to print EventID of blocked reqeusts" << endl;
	for (int i = 0; i < BlockedRequests.size (); i++) {
		cout << BlockedRequests[i] << ' ';
	}
	cout << endl;
	#endif
	
	AvgHoldingTime = TotalHoldingTime / NumofAllocatedRequests;
	AvgTranspondersUsed = (double) TotalTranspondersUsed / NumofAllocatedRequests;
	AvgCoresUsed = (double) TotalCoresUsed / NumofAllocatedRequests;
	AvgGBUsed = (double) TotalGBUsed / NumofAllocatedRequests;
	AvgIntFrag = (1 - ((double) TotalDataSize / (TotalSSUsed * BW_SPECSLOT)));
	AvgExtFrag = (1 - (double) TotalSSUsed / TotalSSOccupied);
	AvgHybridFrag = (1 - (double) TotalDataSize / (TotalSSOccupied * BW_SPECSLOT));


	cout << "Max # of Transponders used: " << MaxNumofTransponders << endl;
	cout << "Max # of Sections used for each request: " << MaxNumofSections << endl;
	cout << "# of blocked requests is " << NumofFailedRequests << endl;
	cout << "Network Load: " << Lambda / Mu << " Erlang" << endl; 
	cout << "Blocking Probability: " << (double) NumofFailedRequests / (double) NumofRequests << endl;
	cout << "Average Cores Used Used per Request: " << AvgCoresUsed << endl;
	cout << "Average Transponders Used per Request: " << AvgTranspondersUsed << endl;
	cout << "Average Holding Time per Request: " << AvgHoldingTime << endl;
	cout << "Average GuardBand per Request: " << AvgGBUsed << endl;
	cout << "Average Internal Fragmentation: " << AvgIntFrag << endl;
	cout << "Average External Fragmentation: " << AvgExtFrag << endl;
	cout << "Average Hybrid Fragmentation: " << AvgHybridFrag << endl; 
}


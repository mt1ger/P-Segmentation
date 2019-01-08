#ifndef _RESOURCEASSIGNMENT_PSVF_H
#define _RESOURCEASSIGNMENT_PSVF_H


#include <list>
#include "Event.h"
#include "EventQueue.h"
#include "RoutingTable.h"

#define PC_MAX 16500
#define PC_MIN 0

class ResourceAssignment {
	public:
		ResourceAssignment (Network * net, EventQueue * eq) {network = net; eventQueue = eq;}
		~ResourceAssignment () {}


		void handle_requests (CircuitRequest * circuitRequest);
		void handle_releases (CircuitRelease * circuitRelease);

	private:
		Network * network;
		EventQueue * eventQueue;
		
		void check_availability_source (unsigned int predecessor, unsigned int successor, CircuitRequest * circuitRequest);
		void check_availability_link (vector<int> * CircuitRoute); 
		void section_establishment (list< vector<int> > &PotentialSec);

		list< vector<int> > AvailableSpecSlots;
};

#endif


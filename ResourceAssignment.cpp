/**************************************************
 * Best-Fit  
 **************************************************/
#include <iostream>
#include <string>
#include <climits>
#include <cfloat>
#include <list>
#include <map>
#include <cmath>
 
#include "ModulationFormats.h"
#include "ResourceAssignment.h"
using namespace std;

/*** MERGE SORTING ***//*{{{*/
void merge (vector<int> * ar, int low, int mid, int high)
{
	int n, m, i, j, k;
	n = mid - low + 1;
	m = high - mid;
	vector<int> R, L;

	for (i = 0; i < n; i++)
	{
		L.push_back (ar->at (low + i));
	}
	for (j = 0; j < m; j++)
	{
		R.push_back (ar->at (mid + j + 1));
	}
	for (i = 0, j = 0, k = low; k <= high; k++) //
	{
		if (i < n && j < m)
		{
			if (L[i] <= R[j])
			{
				ar->at (k) = L[i];
				i++;
			}
			else
			{
				ar->at (k) = R[j];
				j++;
			}
		}
		else if (i < n && j >= m)
		{
			ar->at (k) = L[i];
			i++;
		}
		else if (i >= n && j < m)
		{
			ar->at (k) = R[j];
			j++;
		}
	}
}


void merge_sort(vector<int> * ar, int low, int high)
{
	int mid;
	if (low < high)
	{
		mid = ((high + low) / 2);
		merge_sort (ar, low, mid);
		merge_sort (ar, mid+1, high);
		merge (ar, low, mid, high);
	}
}/*}}}*/

double logrithm (double x, double base) {
	return log (x) / log (base);
}

/*** CHECK AVAILABLITY SOURCE ***//*{{{*/
void ResourceAssignment::check_availability_source (unsigned int predecessor, unsigned int successor, CircuitRequest * circuitRequest) {
	vector<int> HAvailableSpecSlots;

	AvailableSpecSlots.clear ();
	for (int c = 0; c < network->NumofCores; c++) {
		for (int i = 0; i < NUMOFSPECTRALSLOTS; i++) {
			if (network->SpectralSlots[predecessor][successor][c][i] == false) {
				HAvailableSpecSlots.push_back (c);
				HAvailableSpecSlots.push_back (i);
				AvailableSpecSlots.push_back (HAvailableSpecSlots);
				HAvailableSpecSlots.clear ();	
			}
		}
	}
}/*}}}*/

/*** CHECK AVAILABILITY LINK ***//*{{{*/
void ResourceAssignment::check_availability_link (vector<int> * CircuitRoute) {

	list< vector<int> >::iterator i;

	for (int r = 2; r < CircuitRoute->size (); r++) {
		for (i = AvailableSpecSlots.begin (); i != AvailableSpecSlots.end (); i++) {
			if (network->SpectralSlots[CircuitRoute->at (r - 1)][CircuitRoute->at (r)][i->at (0)][i->at (1)] == true) {
				i = AvailableSpecSlots.erase (i);
				i--;
				// AvailableSpecSlots.erase (i); // This line will only work on Mac, will cause Seg Fault (dump core) on ubuntu.
			}
		}
	}

	#ifdef DEBUG_print_AvailableSpecSlots
	cout << "\033[0;32mPRINT\033[0m " << "available spectral slots:" << endl;
	for (i = AvailableSpecSlots.begin (); i != AvailableSpecSlots.end (); i++) {
		for (int j = 0; j < i->size (); j++) {
			cout << i->at (j) << ' ';
		}
		cout << "    ";
	}
	cout << endl;
	#endif
}/*}}}*/

/*** SECTION ESTABLISHMENT***//*{{{*/
void ResourceAssignment::section_establishment (list< vector<int> > &PotentialSec) {
	// int TotalOSS = 0; // OSS = Occupied  Spec Slots
	// int TotalASS = 0; // ASS = Available Spec Slots
	// double PC = 0; // PC = Path Compactness
	vector<int> HPotentialSections;
	list< vector<int> >::iterator i;

	/* Use the Available Spectral Slots to form Potential Sections */
	for (i = AvailableSpecSlots.begin (); i != AvailableSpecSlots.end (); i++) {
		if (HPotentialSections.empty ()) {
			if (i != (--AvailableSpecSlots.end ())) {
				HPotentialSections.push_back (i->at (0));
				HPotentialSections.push_back (i->at (1));
				HPotentialSections.push_back (i->at (1));
				PotentialSec.push_back (HPotentialSections);
			}
		}
		else {
			if ((i->at (1) == (HPotentialSections.at (2) + 1)) && (i->at (0) == HPotentialSections.at (0))) {
				HPotentialSections.at (2) = i->at(1);
				*(--PotentialSec.end ()) = HPotentialSections;
			}
			else {
				if (HPotentialSections.at (2) != HPotentialSections.at (1)) {
					HPotentialSections.clear ();
					if (i != (--AvailableSpecSlots.end ())) {
						HPotentialSections.push_back (i->at (0));
						HPotentialSections.push_back (i->at (1));
						HPotentialSections.push_back (i->at (1));
						PotentialSec.push_back (HPotentialSections);
					}
				}
				else {
					HPotentialSections.clear ();
					PotentialSec.erase (--PotentialSec.end ());
					if (i != (--AvailableSpecSlots.end ())) {
						HPotentialSections.push_back (i->at (0));
						HPotentialSections.push_back (i->at (1));
						HPotentialSections.push_back (i->at (1));
						PotentialSec.push_back (HPotentialSections);
					}
				}
			}
		}
	}

}/*}}}*/




void ResourceAssignment::handle_requests (CircuitRequest * circuitRequest) {/*{{{*/
	RoutingTable routingTable (network);	
	ModulationFormats modulationFormats (circuitRequest, network);

	vector< vector<int> > PathList;
	vector<int> HUsedRoute;
	map<int, string> mfTimesMap;
	vector< vector<int> > UsedRoute;
	vector<int> CircuitRoute;
	bool AvailableFlag = true;
	vector< vector<int> > AssignedSpectralSection;
	vector<int> HAssignedSpectralSection;
	unsigned int TempNumofTransponders = 0;
	string MF = "BPSK";
	unsigned int mfTimes = 0, NumofOccupiedSpectralSlots = 0;


	/** Generate Paths and Sort Path by number of hops **//*{{{*/
	PathList = network->routingTable[circuitRequest->Src][circuitRequest->Dest];
	#ifdef DISPLAY_available_path
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	cout << "\033[0;32mPRINT\033[0m " << "the available " << PathList.size () << " paths:" << endl;
	for (int i = 0; i < PathList.size (); i++) {
		for (int j = 0; j < PathList[i].size () - 1; j++) {
			cout << PathList[i][j] + 1 << "-->";
		}
		cout << PathList[i][PathList[i].size () - 1] + 1 << endl;
	}
	#endif

	map<int, int> PathSizeVSPathIndex;
	list<int> PathSize;
	vector<int> PathSortedIndex;
	list<int>::iterator iter;
	int PreviousPathSize = 0;
	vector <vector<int>> DuplicatedSize;
	for (int i = 0; i < network->NumofNodes; i++) {
		vector<int> XDuplicatedSize;
		XDuplicatedSize.push_back (i);
		DuplicatedSize.push_back (XDuplicatedSize);
	}
	for (int i = 0; i < PathList.size (); i++)
	{
		PathSizeVSPathIndex.insert (pair<int, int> (PathList[i].size (), i));
		if (PathList[i].size () == PreviousPathSize) {
			DuplicatedSize[PathList[i].size ()].push_back (i);
		};
		PathSize.push_back (PathList[i].size ());
		PreviousPathSize = PathList[i].size ();
	}
	PathSize.sort ();
	PreviousPathSize = 0;
	for (iter = PathSize.begin (); iter != PathSize.end (); iter++)
	{
		if (PreviousPathSize != *iter)
		{
			PathSortedIndex.push_back (PathSizeVSPathIndex.at (*iter));
			if (DuplicatedSize[*iter].size () != 1) {
				for (int i = 1; i < DuplicatedSize[*iter].size (); i++) {
					PathSortedIndex.push_back (DuplicatedSize[*iter][i]);
				}
			}
		}
		PreviousPathSize = *iter;
	}

	#ifdef DISPLAY_path_order 
	cout << "\033[0;32mPRINT\033[0m " << "the sorted path index" << endl;
	for (int i = 0; i < PathSortedIndex.size (); i++)
	{
		cout << PathSortedIndex[i] << ' '; 
	}
	cout << endl;
	#endif/*}}}*/
	

	/** Resource Assignment **/
	for (int i = 0; i < PathSortedIndex.size (); i++) {
		int NoOSS = 0; // number of Occupied Spec Slots
		CircuitRoute = PathList[PathSortedIndex[i]];
		#ifdef DISPLAY_selected_path
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
		cout << "\033[0;32mPRINT\033[0m " << "the path under checking: " << endl; 
		for (int i = 0; i < CircuitRoute.size () - 1; i++) 
			cout << CircuitRoute.at (i) + 1 << "-->";  
		cout << CircuitRoute.at (CircuitRoute.size () - 1) + 1 << endl;
		#endif

		/* Modualtion and Resource Checking *//*{{{*/
		// Modulation
		NoOSS = modulationFormats.mf_chosen (CircuitRoute, &circuitRequest->OccupiedSpectralSlots, &circuitRequest->DataSize, &MF, &mfTimes);
		// MFList.push_back (MF);
		mfTimesMap.insert (pair<int, string> (PathSortedIndex[i], MF)); 

		#ifdef DEBUG_modulation_checking
		cout << "Bits Per Signal is: " << mfTimes << " " << MF << endl;
		#endif

		// Check available spec slots
		check_availability_source (PathList[PathSortedIndex[i]][0], PathList[PathSortedIndex[i]][1], circuitRequest);
		check_availability_link (&PathList[PathSortedIndex[i]]);/*}}}*/

		/* Section Sorting *//*{{{*/
		int SizeSum = 0;
		int temp = 0;
		vector<int> SectionSize;
		int SizeCnt = 0;
		int TotalSizeofSections = 0;
		list< vector<int> > PotentialSections;
		list< vector<int> > SortedSections;
		list< vector<int> >::iterator IPS; //iterator for PotentialSections;

		section_establishment (PotentialSections);
		for (IPS = PotentialSections.begin (); IPS != PotentialSections.end (); IPS++) {
			SectionSize.push_back (IPS->at (2) - IPS->at (1) + 1); 
			SizeCnt++;
		}
		for (int i = 0; i < SectionSize.size (); i++) { 
			TotalSizeofSections += SectionSize[i];
		}
		merge_sort (&SectionSize, 0, SizeCnt - 1);
		for (int s = SizeCnt - 1; s >= 0; s--) {
			for (IPS = PotentialSections.begin (); IPS != PotentialSections.end (); IPS++) {
				if ((IPS->at (2) - IPS->at (1) + 1) == SectionSize[s]) {
					SortedSections.push_back (*IPS);
					PotentialSections.erase (IPS);
					break;
				}
			}
		}

		#ifdef DEBUG_print_SortedSections
		cout << "\033[0;32mPRINT\033[0m " << "sorted sections:" << endl;
		for (IPS = SortedSections.begin (); IPS != SortedSections.end (); IPS++) {
			for (int j = 0; j < IPS->size (); j++) {
				cout << IPS->at (j) << ' ';
			}
			cout << "    ";
		}
		cout << endl;
		#endif/*}}}*/

		/* Resource pre-provision */ 
		#ifdef DEBUG_print_resource_state_on_the_path/*{{{*/
		cout << "\033[0;32mPRINT\033[0m " << "resources BEFORE allcation: " << endl;
		for (int i = 1; i < CircuitRoute.size (); i++) {
			cout << "On link " << CircuitRoute[i - 1] + 1 << " to " << CircuitRoute[i] + 1 << endl;
			for (int c = 0; c < network->NumofCores; c++) {
				cout << "On Core " << c << endl;
				for (int j = 0; j < NUMOFSPECTRALSLOTS; j++) {
					cout <<  network->SpectralSlots[CircuitRoute[i - 1]][CircuitRoute[i]][c][j] << ' ';
				}
				cout << endl;
			}
		}
		#endif

		// pre-provision 
		bool BiggerFlag = false; // The void is bigger than requirement
		bool SmallerFlag = false; // The void is smaller than requirement
		int OSS; // Occupied Spec Slots 
		list< vector<int> >::iterator ISS = SortedSections.begin (); //iterator of SortedSections

		OSS = ceil ((double) circuitRequest->OccupiedSpectralSlots / mfTimes);
		// if all the path but the last one is full, and the last one can not afford the rest of the request, the request will be blocked
		if (i == PathSortedIndex.size () - 1 && OSS + SortedSections.size () * GB > TotalSizeofSections) { 
			AvailableFlag = false;
		}

		// if the resource on a single path can not afford the request, allocate the as much of it as possible, and go to the next path{{{
		if (OSS + SortedSections.size () * GB > TotalSizeofSections) {
			if (!SortedSections.empty ())
			{
				HUsedRoute.clear ();
				HUsedRoute.push_back (PathSortedIndex[i]);
				HUsedRoute.insert (HUsedRoute.end (), CircuitRoute.begin (), CircuitRoute.end ());
				UsedRoute.push_back (HUsedRoute);
			}
			for (ISS = SortedSections.begin (); ISS != SortedSections.end(); ISS++) { 
				HAssignedSpectralSection.clear ();
				HAssignedSpectralSection.push_back (PathSortedIndex[i]);
				HAssignedSpectralSection.push_back (ISS->at (0));
				HAssignedSpectralSection.push_back (ISS->at (1));
				HAssignedSpectralSection.push_back (ISS->at (2));
				AssignedSpectralSection.push_back (HAssignedSpectralSection);
				TempNumofTransponders++;
			}
			circuitRequest->OccupiedSpectralSlots -= (TotalSizeofSections - SortedSections.size () * GB) * mfTimes;
			continue;
		}
		// the resource can afford the request 
		else 
		{

			HUsedRoute.clear ();
			HUsedRoute.push_back (PathSortedIndex[i]);
			HUsedRoute.insert (HUsedRoute.end (), CircuitRoute.begin (), CircuitRoute.end ());
			UsedRoute.push_back (HUsedRoute);
			HAssignedSpectralSection.clear ();

			while (OSS > 0) {
				/* Allocation */
				if (ISS == SortedSections.end ()) {
					if (BiggerFlag == true) {
						ISS--;
						SmallerFlag = true;
						continue;
					}
				}
				else if (ISS->at (2) - ISS->at (1) + 1 == OSS + GB) {
					HAssignedSpectralSection.push_back (PathSortedIndex[i]);
					HAssignedSpectralSection.push_back (ISS->at (0));
					HAssignedSpectralSection.push_back (ISS->at (1));
					HAssignedSpectralSection.push_back (ISS->at (2));
					AssignedSpectralSection.push_back (HAssignedSpectralSection);
					TempNumofTransponders++;
					break;
				}
				else if (ISS->at (2) - ISS->at (1) + 1 > OSS + GB) {
					BiggerFlag = true;
					if (SmallerFlag == true) {
						HAssignedSpectralSection.push_back (PathSortedIndex[i]);
						HAssignedSpectralSection.push_back (ISS->at (0));
						HAssignedSpectralSection.push_back (ISS->at (1));
						HAssignedSpectralSection.push_back (ISS->at (1) + OSS + GB - 1); 
						AssignedSpectralSection.push_back (HAssignedSpectralSection);
						TempNumofTransponders++;
						break;
					}
					ISS++;
				}
				else if (ISS->at (2) - ISS->at (1) + 1 < OSS + GB) {
					if (BiggerFlag == true) {
						ISS--;
						SmallerFlag = true;
						continue;
					}
					HAssignedSpectralSection.push_back (PathSortedIndex[i]);
					HAssignedSpectralSection.push_back (ISS->at (0));
					HAssignedSpectralSection.push_back (ISS->at (1));
					HAssignedSpectralSection.push_back (ISS->at (2));
					AssignedSpectralSection.push_back (HAssignedSpectralSection);
					OSS -= ISS->at (2) - ISS->at (1) + 1 - GB;
					HAssignedSpectralSection.clear ();
					ISS = SortedSections.erase (ISS);
					TempNumofTransponders++;
				}
			}
		}
		if (AvailableFlag == true) break;
	}/*}}}*/
	
	// To limit the number of sections
	if (AssignedSpectralSection.size () > network->SectionNumLimitation) {
		AvailableFlag = false;
	}

	
	if (AvailableFlag == false) {
		network->NumofDoneRequests++;

		#ifdef DEBUG_collect_EventID_of_blocked_requests
		network->BlockedRequests.push_back (circuitRequest->EventID);
		#endif

		#ifdef PRINT_allocation_block_release
		cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;
		cout << "Request " << circuitRequest->EventID << " is blocked" << endl;
		cout << "Modulation Format: " << MF << endl;
		cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;
		#endif

			network->NumofFailedRequests++;
	}

	else if (AvailableFlag == true) {

		#ifdef DEBUG_check_AssignedSpectralSection
		for (int i = 0; i < AssignedSpectralSection.size (); i++) 
		{
			for (int j = 0; j < AssignedSpectralSection[i].size (); j++)
				cout << AssignedSpectralSection[i][j] << ' ';
			cout << '\t';
		}
		cout << endl;
		#endif

		int temp = AssignedSpectralSection[0][1];
		int CoreCnter = 1;
		for (int i = 0; i < AssignedSpectralSection.size (); i++) {
			for (int l = 1; l < PathList[AssignedSpectralSection[i][0]].size (); l++) 
			{
				for (int j = AssignedSpectralSection[i].at (2); j <= AssignedSpectralSection[i].at (3); j++) {
					network->SpectralSlots[PathList[AssignedSpectralSection[i][0]][l - 1]][PathList[AssignedSpectralSection[i][0]][l]][AssignedSpectralSection[i].at (1)][j] = true;
				}
			}
			if (AssignedSpectralSection[i][1] != temp) {
				temp = AssignedSpectralSection[i][1];
				CoreCnter++;
			}
		}
		for (int i = 0; i < AssignedSpectralSection.size (); i++) {
			NumofOccupiedSpectralSlots += AssignedSpectralSection[i][2] - AssignedSpectralSection[i][1] + 1; 
		}

		#ifdef PRINT_allocation_block_release/*{{{*/
		cout << "------------------------------------------------------------" << endl;
		cout << "Request ID: " << circuitRequest->EventID << "\tStart: " << circuitRequest->EventTime << "\tEnd: " << circuitRequest->StartTime + circuitRequest->Duration << endl;
		cout << "Source: " << circuitRequest->Src + 1 << "  Destination: " << circuitRequest->Dest + 1 << endl;
		for (unsigned int y = 0; y < UsedRoute.size (); y++)
		{
			cout << "Path: ";
			for(unsigned int t = 1; t < UsedRoute[y].size() - 1; t++)
				cout << UsedRoute[y].at(t) + 1 << " --> ";
			cout << UsedRoute[y].at (UsedRoute[y].size () - 1) + 1 << endl;
			cout << "ModulationFormats: " << mfTimesMap.at (UsedRoute[y][0]) << endl;
			for (int i = 0; i < AssignedSpectralSection.size (); i++) {
				if (UsedRoute[y][0] == AssignedSpectralSection[i][0])
					cout << "Core: " << AssignedSpectralSection[i][1] << "  Spectral Section: " << AssignedSpectralSection[i][2] << " to " << AssignedSpectralSection[i][3] << endl; 
			}
		}


		cout << "# of Transponders Used: " << TempNumofTransponders << endl;
		cout << "# of Core Used: " << CoreCnter << endl;
		cout << "------------------------------------------------------------" << endl;
		#endif/*}}}*/

		CircuitRelease * circuitRelease;
		circuitRelease = new CircuitRelease (circuitRequest->EventID, UsedRoute, AssignedSpectralSection, circuitRequest->StartTime + circuitRequest->Duration, TempNumofTransponders);
		eventQueue->queue_insert (circuitRelease);

		// Thourogh Metric Collection{{{
		network->NumofAllocatedRequests++;
		network->NumofSections = TempNumofTransponders;
		// network->TotalHoldingTime += circuitRequest->Duration;
		network->NumofTransponders = network->NumofTransponders + TempNumofTransponders;
		network->TotalTranspondersUsed += TempNumofTransponders;
		network->TotalCoresUsed += CoreCnter;
		network->TotalGBUsed += TempNumofTransponders;
		network->TotalDataSize += circuitRequest->DataSize;
		network->TotalSSUsed += NumofOccupiedSpectralSlots * mfTimes;
		network->TotalSSOccupied += (NumofOccupiedSpectralSlots + TempNumofTransponders) * mfTimes;/*}}}*/
		
		#ifdef DISPLAY_metrics/*{{{*/
		cout << "*** METRICS ***" << endl;
		cout << "    Bits Per Signal: " << mfTimes << endl;
		cout << "    NumofSections: " << network->NumofSections << endl; 
		cout << "    NumofTransponders: " << network->NumofTransponders << "  This time: " << TempNumofTransponders << endl; 
		cout << "    TotalTranspondersUsed: " << network->TotalTranspondersUsed << "  This time: " << TempNumofTransponders << endl; 
		cout << "    TotalGBUsed: " << network->TotalGBUsed << "  This time: " << TempNumofTransponders << endl; 
		cout << "    TotalDataSize: " << network->TotalDataSize << "  This time: " << circuitRequest->DataSize << endl; 
		cout << "    TotalSSUsed: " << network->TotalSSUsed << "  This time: " << NumofOccupiedSpectralSlots * mfTimes << endl; 
		cout << "    TotalSSOccupied: " << network->TotalSSOccupied << "  This time: " << (NumofOccupiedSpectralSlots + TempNumofTransponders) * mfTimes << endl; 
		#endif/*}}}*/
	}

	#ifdef DEBUG_print_resource_state_on_the_path/*{{{*/
	cout << "\033[0;32mPRINT\033[0m " << "resources AFTER Allocation:" << endl;
	vector<int> Route;
	for (int p = 0; p < UsedRoute.size (); p++)  
	{
		Route = UsedRoute[p];
		cout << "On Path: ";
		for (int i = 1; i < Route.size () - 1; i++)
			cout << Route[i] + 1 << "-->";
		cout << Route[Route.size () - 1] + 1 << endl;
		for (int i = 2; i < Route.size (); i++) {
			cout << "On link " << Route[i - 1] + 1 << " to " << Route[i] + 1 << endl;
			for (int c = 0; c < network->NumofCores; c++) {
				cout << "On Core " << c << endl;
				for (int j = 0; j < NUMOFSPECTRALSLOTS; j++) {
					cout <<  network->SpectralSlots[Route[i - 1]][Route[i]][c][j] << ' ';
				}
				cout << endl;
			}
		}
	}
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	cout << endl << endl;
	#endif/*}}}*/
}/*}}}*//*}}}*/


void ResourceAssignment::handle_releases (CircuitRelease * circuitRelease) {
	#ifdef DEBUG_print_resource_state_on_the_path
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	cout << "\033[0;32mPRINT\033[0m " << "resources BEFORE Release:" << endl;
	for (int i = 0; i < circuitRelease->CircuitRoute.size (); i++) {
		cout << "On Path: ";
		for (int l = 1; l < circuitRelease->CircuitRoute[i].size () - 1; l++)  
			cout << circuitRelease->CircuitRoute[i][l] + 1 << "-->";
		cout << circuitRelease->CircuitRoute[i][circuitRelease->CircuitRoute[i].size () - 1] + 1 << endl;
		for (int j = 2; j < circuitRelease->CircuitRoute[i].size (); j++) {
			cout << "On Link " << circuitRelease->CircuitRoute[i][j-1] + 1 << " to " << circuitRelease->CircuitRoute[i][j] + 1 << endl;
			for (int c = 0; c < network->NumofCores; c++) {
				cout << "On Core " << c << endl;
				for (int m = 0; m < NUMOFSPECTRALSLOTS; m++) {
					cout <<  network->SpectralSlots[circuitRelease->CircuitRoute[i][j - 1]][circuitRelease->CircuitRoute[i][j]][c][m] << ' ';
				}
				cout << endl;
			}
		}
	}
	#endif

		for (int r = 0; r < circuitRelease->CircuitRoute.size (); r++) 
		{
			for (int i = 2; i < circuitRelease->CircuitRoute[r].size (); i++) {
				for (int j = 0; j < circuitRelease->OccupiedSpectralSection.size (); j++) {
					if (circuitRelease->OccupiedSpectralSection[j][0] == circuitRelease->CircuitRoute[r][0])
					{
						for (int k = circuitRelease->OccupiedSpectralSection[j][2]; k <= circuitRelease->OccupiedSpectralSection[j][3]; k++) {
							network->SpectralSlots[circuitRelease->CircuitRoute[r][i - 1]][circuitRelease->CircuitRoute[r][i]][circuitRelease->OccupiedSpectralSection[j][1]][k] = false;	
						}
					}
				}
			}
		}

		network->NumofDoneRequests++;
		network->NumofTransponders = network->NumofTransponders - circuitRelease->TranspondersUsed;

	#ifdef PRINT_allocation_block_release
	cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout << "Release Event: " << circuitRelease->EventID << "\tTime: " << circuitRelease->EventTime << endl;
	cout << "Released Event on Path:" << endl;
	for (int p = 0; p < circuitRelease->CircuitRoute.size (); p++) 
	{
		for (int i = 1; i < circuitRelease->CircuitRoute[p].size () - 1; i++) {
			cout << circuitRelease->CircuitRoute[p][i] + 1 << "-->";
		}
		cout << circuitRelease->CircuitRoute[p][circuitRelease->CircuitRoute[p].size () - 1] + 1 << endl;
		for (int i = 0; i < circuitRelease->OccupiedSpectralSection.size (); i++) {
			if (circuitRelease->CircuitRoute[p][0] == circuitRelease->OccupiedSpectralSection[i][0]) 
			{
				cout << "Core: " << circuitRelease->OccupiedSpectralSection[i][1] << "  Spectral Section: " << circuitRelease->OccupiedSpectralSection[i][2] << " to " << circuitRelease->OccupiedSpectralSection[i][3] << endl;
			}
		}
	}
	cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	#endif

	#ifdef DEBUG_print_resource_state_on_the_path
	cout << "\033[0;32mPRINT\033[0m " << "resources BEFORE Release:" << endl;
	for (int i = 0; i < circuitRelease->CircuitRoute.size (); i++) {
		cout << "On Path: ";
		for (int l = 1; l < circuitRelease->CircuitRoute[i].size () - 1; l++)  
			cout << circuitRelease->CircuitRoute[i][l] + 1 << "-->";
		cout << circuitRelease->CircuitRoute[i][circuitRelease->CircuitRoute[i].size () - 1] + 1 << endl;
		for (int j = 2; j < circuitRelease->CircuitRoute[i].size (); j++) {
			cout << "On Link " << circuitRelease->CircuitRoute[i][j-1] + 1 << " to " << circuitRelease->CircuitRoute[i][j] + 1 << endl;
			for (int c = 0; c < network->NumofCores; c++) {
				cout << "On Core " << c << endl;
				for (int m = 0; m < NUMOFSPECTRALSLOTS; m++) {
					cout <<  network->SpectralSlots[circuitRelease->CircuitRoute[i][j - 1]][circuitRelease->CircuitRoute[i][j]][c][m] << ' ';
				}
				cout << endl;
			}
		}
	}
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	cout << endl << endl;
	#endif
}


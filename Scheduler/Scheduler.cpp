#include<iostream>
#include<cstdio>
#include<map>
#include<vector>
#include<sstream>
#include<fstream>
#include<algorithm>
#include <queue>
#include <iomanip>

using namespace std;

int currRandom = 0;
int Cycle = 0;
double IOUtil = 0.0;
vector<int>RandomInput;
bool verbose = false;
double CPUUtilization = 0.0;
double Avgwaittime = 0.0;
//double numProcess = 0.0;
double avgturnaround = 0.0;
double TnumProcess = 0;;
void init()
{
	string filer = "random-numbers.txt";

	ifstream input1(filer);
	int random;
	while (!input1.eof())
	{
		input1 >> random;
		RandomInput.push_back(random);

	}
}




class Process {
public:

	int arrival_time;
	int total_CPU_time;
	int CPU_burst_time;
	int IO_burst_time;
	int remaining_time;
	//string state;
	bool seenP;
	int pid;
	int Finish_time;
	int IO_time;
	int Waiting_time;
	int Total_IO_time;
	int currentCpuBurst;
	int TurnAroundTime;
	int PrevCPUBurst;
	string State;
	int timeinCurrState;


	Process(int A_time, int B_time, int C_time, int I_time, int p_id)
	{
		//cout << "Hello"<<endl;
		arrival_time = A_time;
		total_CPU_time = C_time;
		CPU_burst_time = B_time;
		IO_burst_time = I_time;
		currentCpuBurst = 0;
		seenP = false;
		pid = p_id;
		Finish_time = 0;
		IO_time = 0;
		Total_IO_time = 0;
		Waiting_time = 0;
		remaining_time = C_time;
		TurnAroundTime = 0;
		PrevCPUBurst = 0;
		State = "unstarted";
		timeinCurrState = 0;
	}
	int get_arrivaltime();
	void set_Seen();
	bool get_Seen();

};

bool Sortbyarrival(Process P1, Process P2);
bool Sortbytime(Process P1, Process P2);
int RandomOS(int U)
{

	// read next from random input each time
	int X = RandomInput[currRandom];
	//cout << "current random no is  " << X << endl;
	currRandom++;

	return 1 + (X % U);
}
int Process::get_arrivaltime()
{
	return arrival_time;
}

void Process::set_Seen()
{
	seenP = true;
}

bool Process::get_Seen()
{
	return seenP;
}
vector <Process> ReadyQ;
vector <Process> BlockedQ;
vector <Process> TerminatedQ;
vector <Process> DebugQ;
vector <Process> ProcessQ;
vector <Process> IntermediateQ;

void DebugT()
{
	//DebugQ.clear();

	for (int i = 0; i < ProcessQ.size(); i++)
	{


		if ((!ProcessQ[i].get_Seen()))
		{

			DebugQ.push_back(ProcessQ[i]);


		}
	}

	for (int i = 0; i < ReadyQ.size(); i++)
	{
		DebugQ.push_back(ReadyQ[i]);
	}
	for (int i = 0; i < BlockedQ.size(); i++)
	{
		DebugQ.push_back(BlockedQ[i]);
	}

	for (int i = 0; i < TerminatedQ.size(); i++)
	{
		DebugQ.push_back(TerminatedQ[i]);
	}

	std::sort(DebugQ.begin(), DebugQ.end(), Sortbyarrival);
	cout <<setw(10)<<"Before cycle    " <<setw(10)<< Cycle<<": ";
	for (int i = 0; i < DebugQ.size(); i++)
	{
		cout <<setw(10)<< DebugQ[i].State << "  "<<setw(10) << DebugQ[i].timeinCurrState<<"  ";
	}
	cout << endl;
}
void RemoveFromBlockedQ()
{



	for (vector<Process>::iterator it = BlockedQ.begin();
		it != BlockedQ.end();)

	{

		if (it->IO_time <= 0)
			it = BlockedQ.erase(it);
		else
			++it;
	}

}
void clearQs()
{
	ReadyQ.clear();
	BlockedQ.clear();
	TerminatedQ.clear();
	DebugQ.clear();
	ProcessQ.clear();
	IntermediateQ.clear();
}
void RR(vector<Process> P)
{
	clearQs();
	int numCompletedP = 0;
	ProcessQ = P;
	Cycle = 0;
	IOUtil = 0;
	currRandom = 0;




	if (verbose == true)
	{
		DebugQ.clear();
		cout << "This detailed printout gives the state and remaining burst for each process" << endl;
		DebugT();
	}

	std::sort(BlockedQ.begin(), BlockedQ.end(), Sortbyarrival);
	//cout << "Cycle is   " << Cycle << endl;
	for (int i = 0; i < BlockedQ.size(); i++)
	{

		// if blocked Q IO is over move to ready Q and sort Q;
		if (BlockedQ[i].IO_time == 0)
		{
			BlockedQ[i].State = "Ready";
			BlockedQ[i].timeinCurrState = 0;
			ReadyQ.push_back(BlockedQ[i]);
			//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbytime);
			//cout << "blocked process moved to ready at the end" << endl;
		}
	}

	while (numCompletedP != ProcessQ.size())
	{
		for (int i = 0; i < ProcessQ.size(); i++)
		{

			//cout << "num process " << P.size() << endl;
			//cout << P[i].get_arrivaltime()<<endl;
			// if process arrival time is less than or equal to Cycle, move to ready Q
			if ((ProcessQ[i].get_arrivaltime() <= Cycle) && (!ProcessQ[i].get_Seen()))
			{
				ProcessQ[i].set_Seen();
				//P[i].Waiting_time = Cycle - P[i].arrival_time;
				ProcessQ[i].pid = i;
				ProcessQ[i].State = "Ready";
				ProcessQ[i].timeinCurrState = 0;
				ReadyQ.push_back(ProcessQ[i]);

				//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbyarrival);

			}

		}
		//cout << "Ready Q size " << ReadyQ.size();
		// removing first process from ready Q;
		if (!ReadyQ.empty())
		{
			//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbytime);
			Process currentProcess = ReadyQ.front();
			currentProcess.State = "Running";
			ReadyQ.erase(ReadyQ.begin());
			//cout << " Ready Q is empty" << endl;
			//cout << "Ready Q size here " << ReadyQ.size();

			int CPU_burst = 0;

			//cout << "CPU burst  " << CPU_burst<<endl;
			if (currentProcess.PrevCPUBurst == 0)

			{
				CPU_burst = RandomOS(currentProcess.CPU_burst_time);
				if (CPU_burst > currentProcess.remaining_time)
				{
					CPU_burst = currentProcess.remaining_time;
					//cout << "CPU burst  " << CPU_burst << endl;
				}
				//cout << "CPU burst time is  " << currentProcess.CPU_burst_time;
				currentProcess.currentCpuBurst = CPU_burst;
				currentProcess.PrevCPUBurst = CPU_burst;
			}
			else
			{
				CPU_burst = currentProcess.PrevCPUBurst;

			}

			if (CPU_burst > 2)
			{
				CPU_burst = 2;
			}

			currentProcess.timeinCurrState = CPU_burst;
			//cout << " current time in run state  " << currentProcess.timeinCurrState<<"  "<< CPU_burst<<endl;

			while (CPU_burst != 0)
			{
				//increment Cycle.
				for (int i = 0; i < ProcessQ.size(); i++)
				{

					//cout << "num process " << P.size() << endl;
					//cout << P[i].get_arrivaltime()<<endl;
					// if process arrival time is less than or equal to Cycle, move to ready Q
					if ((ProcessQ[i].get_arrivaltime() <= Cycle) && (!ProcessQ[i].get_Seen()))
					{
						ProcessQ[i].set_Seen();
						//P[i].Waiting_time = Cycle - P[i].arrival_time;
						ProcessQ[i].pid = i;
						ProcessQ[i].State = "Ready";
						ProcessQ[i].timeinCurrState = 0;
						ReadyQ.push_back(ProcessQ[i]);

						//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbyarrival);

					}
				}
				Cycle++;
				//cout << " current time in run state  " << currentProcess.timeinCurrState;
				if (verbose == true)
				{
					DebugQ.clear();

					DebugQ.push_back(currentProcess);

					DebugT();
				}

				CPU_burst--;
				if (!BlockedQ.empty())
				{
					IOUtil += 1;
				}


				// decrement currentProcess.remaining_time
				//cout << "Current process remaining time  " << currentProcess.remaining_time - 1 << endl;
				currentProcess.remaining_time -= 1;
				currentProcess.PrevCPUBurst -= 1;
				currentProcess.timeinCurrState -= 1;
				std::sort(BlockedQ.begin(), BlockedQ.end(), Sortbyarrival);
				//decrement IO time of each process in blocked Q;
				for (int i = 0; i < BlockedQ.size(); i++)
				{
					BlockedQ[i].IO_time = BlockedQ[i].IO_time - 1;
					// Increment total IO time for each process
					BlockedQ[i].Total_IO_time += 1;
					BlockedQ[i].timeinCurrState -= 1;
					// if blocked Q IO is over move to ready Q and sort Q;
					if (BlockedQ[i].IO_time == 0)
					{
						BlockedQ[i].State = "Ready";
						BlockedQ[i].timeinCurrState = 0;
						IntermediateQ.push_back(BlockedQ[i]);
						//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbyarrival);
						//cout << "blocked process moved to ready here" << endl;
					}
				}

				RemoveFromBlockedQ();


				// if current process remaining time = 0 finish and move first from ready Q
				if (CPU_burst == 0)
				{

					if (currentProcess.remaining_time == 0)
					{

						//currentProcess.pid = numCompletedP;
						currentProcess.State = "Terminated";
						currentProcess.timeinCurrState = 0;
						currentProcess.Finish_time = Cycle;
						currentProcess.TurnAroundTime = Cycle - currentProcess.arrival_time;
						currentProcess.Waiting_time = Cycle - currentProcess.arrival_time - currentProcess.Total_IO_time - currentProcess.total_CPU_time;

						numCompletedP++;
						TerminatedQ.push_back(currentProcess);


					}

					//check if IO remaining time is 0;
					//calculate IO
					else if (currentProcess.PrevCPUBurst == 0)
					{//move current process to blocked Q

					 //cout << "process moved to blocked  " << currentProcess.pid;
						int IOburst = currentProcess.IO_burst_time*currentProcess.currentCpuBurst;
						//cout << "IO time for current process is " << IOburst;
						currentProcess.IO_time = IOburst;
						currentProcess.State = "Blocked";
						currentProcess.timeinCurrState = IOburst;
						//currentProcess.timeinCurrState = currentProcess.IO_burst_time;
						BlockedQ.push_back(currentProcess);
						//std:sort(BlockedQ.begin(), BlockedQ.end(), Sortbyarrival);
					}


					else
					{
						currentProcess.State = "Ready";
						currentProcess.timeinCurrState = 0;
						//cout << "Pushing to ready Q" << endl;
						IntermediateQ.push_back(currentProcess);

					}
				}

				std::sort(IntermediateQ.begin(), IntermediateQ.end(), Sortbyarrival);
				for (int i = 0; i < IntermediateQ.size(); i++)
				{
					ReadyQ.push_back(IntermediateQ[i]);

				}
				IntermediateQ.clear();



			}


		}
		else
		{
			//increment the Cycle
			Cycle++;

			if (verbose == true)
			{
				DebugQ.clear();
				//DebugQ.push_back(currentProcess);
				DebugT();
			}

			if (!BlockedQ.empty())
			{
				IOUtil += 1;
			}
			std::sort(BlockedQ.begin(), BlockedQ.end(), Sortbyarrival);
			//cout << "Cycle is   " << Cycle << endl;
			for (int i = 0; i < BlockedQ.size(); i++)
			{
				BlockedQ[i].IO_time = BlockedQ[i].IO_time - 1;
				BlockedQ[i].timeinCurrState -= 1;
				// Increment total IO time for each process
				BlockedQ[i].Total_IO_time += 1;
				// if blocked Q IO is over move to ready Q and sort Q;
				if (BlockedQ[i].IO_time == 0)
				{
					BlockedQ[i].State + "Ready";
					BlockedQ[i].timeinCurrState = 0;
					ReadyQ.push_back(BlockedQ[i]);

					//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbytime);
					//cout << "blocked process moved to ready at the end" << endl;
				}
			}

			RemoveFromBlockedQ();




		}


	}
}
void FCFS(vector<Process> P)
{
	clearQs();
	int numCompletedP = 0;
	Cycle = 0;
	IOUtil = 0;
	currRandom = 0;
	ProcessQ = P;

	//cout << "The scheduling algorithm used was FCFS" << endl;
	if (verbose == true)
	{
		DebugQ.clear();
		cout << "This detailed printout gives the state and remaining burst for each process" << endl;

		//DebugQ.push_back(currentProcess);

		DebugT();
	}
	while (numCompletedP != ProcessQ.size())
	{
		for (int i = 0; i < ProcessQ.size(); i++)
		{

			//cout << "num process " << P.size() << endl;
			//cout << P[i].get_arrivaltime()<<endl;
			// if process arrival time is less than or equal to Cycle, move to ready Q
			if ((ProcessQ[i].get_arrivaltime() <= Cycle) && (!ProcessQ[i].get_Seen()))
			{
				ProcessQ[i].set_Seen();
				ProcessQ[i].pid = i;
				ProcessQ[i].State = "Ready";
				ProcessQ[i].timeinCurrState = 0;
				//P[i].Waiting_time = Cycle - P[i].arrival_time;
				ReadyQ.push_back(ProcessQ[i]);
				std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbyarrival);

			}

		}
		//cout << "Ready Q size " << ReadyQ.size();
		// removing first process from ready Q;
		if (!ReadyQ.empty())
		{
			Process currentProcess = ReadyQ.front();

			ReadyQ.erase(ReadyQ.begin());
			//cout << " Ready Q is empty" << endl;
			//cout << "Ready Q size here " << ReadyQ.size();

			int CPU_burst = RandomOS(currentProcess.CPU_burst_time);

			//cout << "CPU burst  " << CPU_burst<<endl;
			if (CPU_burst > currentProcess.remaining_time)
			{
				CPU_burst = currentProcess.remaining_time;
				//cout << "CPU burst  " << CPU_burst << endl;


			}
			//cout << "CPU burst time is  " << currentProcess.CPU_burst_time;
			currentProcess.currentCpuBurst = CPU_burst;
			currentProcess.timeinCurrState = CPU_burst;
			currentProcess.State = "Running";


			while (CPU_burst != 0)
			{
				//increment Cycle.
				Cycle++;
				// decrement currentProcess.remaining_time
				//cout << "Current process remaining time  " << currentProcess.remaining_time - 1 << endl;
				if (verbose == true)
				{
					DebugQ.clear();

					DebugQ.push_back(currentProcess);

					DebugT();
				}
				currentProcess.remaining_time -= 1;
				currentProcess.timeinCurrState -= 1;

				if (!BlockedQ.empty())
				{
					IOUtil += 1;;
				}
				//decrement IO time of each process in blocked Q;
				for (int i = 0; i < BlockedQ.size(); i++)
				{
					BlockedQ[i].IO_time = BlockedQ[i].IO_time - 1;
					BlockedQ[i].timeinCurrState -= 1;
					// Increment total IO time for each process
					BlockedQ[i].Total_IO_time += 1;
					// if blocked Q IO is over move to ready Q and sort Q;
					if (BlockedQ[i].IO_time == 0)
					{
						BlockedQ[i].State = "Ready";
						BlockedQ[i].timeinCurrState = 0;
						ReadyQ.push_back(BlockedQ[i]);
						//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbyarrival);
						//cout << "blocked process moved to ready here" << endl;
					}
				}

				RemoveFromBlockedQ();


				CPU_burst--;

				// if current process remaining time = 0 finish and move first from ready Q

			}
			if (CPU_burst == 0)
			{

				if (currentProcess.remaining_time == 0)
				{
					currentProcess.State = "Terminated";
					currentProcess.timeinCurrState = 0;
					currentProcess.Finish_time = Cycle;
					currentProcess.TurnAroundTime = Cycle - currentProcess.arrival_time;
					currentProcess.Waiting_time = Cycle - currentProcess.arrival_time - currentProcess.Total_IO_time - currentProcess.total_CPU_time;

					numCompletedP++;
					TerminatedQ.push_back(currentProcess);
				}
				//check if IO remaining time is 0;
				//calculate IO
				else
				{//move current process to blocked Q

				 //cout << "process moved to blocked  " << currentProcess.pid;
					int IOburst = currentProcess.IO_burst_time*currentProcess.currentCpuBurst;
					//cout << "IO time for current process is " << IOburst;
					currentProcess.IO_time = IOburst;
					currentProcess.State = "Blocked";
					currentProcess.timeinCurrState = IOburst;
					BlockedQ.push_back(currentProcess);
				std:sort(BlockedQ.begin(), BlockedQ.end(), Sortbyarrival);
				}
			}
		}
		else
		{
			//increment the Cycle
			Cycle++;
			if (verbose == true)
			{
				DebugQ.clear();

				//DebugQ.push_back(currentProcess);

				DebugT();
			}
			if (!BlockedQ.empty())
			{
				IOUtil += 1;
			}
			//cout << "Cycle is   " << Cycle << endl;
			for (int i = 0; i < BlockedQ.size(); i++)
			{
				BlockedQ[i].IO_time = BlockedQ[i].IO_time - 1;
				BlockedQ[i].timeinCurrState -= 1;
				// Increment total IO time for each process
				BlockedQ[i].Total_IO_time += 1;
				// if blocked Q IO is over move to ready Q and sort Q;
				if (BlockedQ[i].IO_time == 0)
				{
					ProcessQ[i].State = "Ready";
					ProcessQ[i].timeinCurrState = 0;
					ReadyQ.push_back(BlockedQ[i]);
					//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbyarrival);
					//cout << "blocked process moved to ready at the end" << endl;
				}
			}
			RemoveFromBlockedQ();


		}


	}
}

void SJF(vector<Process> P)
{
	clearQs();
	int numCompletedP = 0;
	Cycle = 0;
	IOUtil = 0;
	currRandom = 0;
	ProcessQ = P;

	if (verbose == true)
	{
		DebugQ.clear();
		cout << "This detailed printout gives the state and remaining burst for each process" << endl;

		//DebugQ.push_back(currentProcess);

		DebugT();
	}
	//cout << "The scheduling algorithm used was SJF" << endl;
	while (numCompletedP != ProcessQ.size())
	{
		for (int i = 0; i < ProcessQ.size(); i++)
		{

			//cout << "num process " << P.size() << endl;
			//cout << P[i].get_arrivaltime()<<endl;
			// if process arrival time is less than or equal to Cycle, move to ready Q
			if ((ProcessQ[i].get_arrivaltime() <= Cycle) && (!ProcessQ[i].get_Seen()))
			{
				ProcessQ[i].set_Seen();

				//P[i].Waiting_time = Cycle - P[i].arrival_time;
				ProcessQ[i].pid = i;
				ProcessQ[i].State = "Ready";
				ProcessQ[i].timeinCurrState = 0;
				ReadyQ.push_back(ProcessQ[i]);

				//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbyarrival);

			}

		}
		//cout << "Ready Q size " << ReadyQ.size();
		// removing first process from ready Q;
		if (!ReadyQ.empty())
		{
			std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbytime);
			Process currentProcess = ReadyQ.front();

			ReadyQ.erase(ReadyQ.begin());
			//cout << " Ready Q is empty" << endl;
			//cout << "Ready Q size here " << ReadyQ.size();

			int CPU_burst = RandomOS(currentProcess.CPU_burst_time);

			//cout << "CPU burst  " << CPU_burst<<endl;
			if (CPU_burst > currentProcess.remaining_time)
			{
				CPU_burst = currentProcess.remaining_time;
				//cout << "CPU burst  " << CPU_burst << endl;


			}
			//cout << "CPU burst time is  " << currentProcess.CPU_burst_time;
			currentProcess.currentCpuBurst = CPU_burst;
			currentProcess.State = "Running";
			currentProcess.timeinCurrState = CPU_burst;

			while (CPU_burst != 0)
			{
				//increment Cycle.
				Cycle++;
				if (verbose == true)
				{
					DebugQ.clear();

					DebugQ.push_back(currentProcess);
					DebugT();
				}
				if (!BlockedQ.empty())
				{
					IOUtil += 1;
				}


				// decrement currentProcess.remaining_time
				//cout << "Current process remaining time  " << currentProcess.remaining_time - 1 << endl;
				currentProcess.remaining_time -= 1;
				currentProcess.timeinCurrState -= 1;

				//decrement IO time of each process in blocked Q;
				for (int i = 0; i < BlockedQ.size(); i++)
				{
					BlockedQ[i].IO_time = BlockedQ[i].IO_time - 1;
					// Increment total IO time for each process
					BlockedQ[i].Total_IO_time += 1;
					BlockedQ[i].timeinCurrState -= 1;
					// if blocked Q IO is over move to ready Q and sort Q;
					if (BlockedQ[i].IO_time == 0)
					{
						BlockedQ[i].State = "Ready";
						BlockedQ[i].timeinCurrState = 0;
						ReadyQ.push_back(BlockedQ[i]);
						//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbyarrival);
						//cout << "blocked process moved to ready here" << endl;
					}
				}

				RemoveFromBlockedQ();

				CPU_burst--;




				// if current process remaining time = 0 finish and move first from ready Q

			}
			if (CPU_burst == 0)
			{

				if (currentProcess.remaining_time == 0)
				{

					//currentProcess.pid = numCompletedP;

					currentProcess.State = "Terminated";
					currentProcess.timeinCurrState = 0;
					currentProcess.Finish_time = Cycle;
					currentProcess.TurnAroundTime = Cycle - currentProcess.arrival_time;
					currentProcess.Waiting_time = Cycle - currentProcess.arrival_time - currentProcess.Total_IO_time - currentProcess.total_CPU_time;

					numCompletedP++;
					TerminatedQ.push_back(currentProcess);
					//TerminatedQ.push_back(currentProcess);
				}
				//check if IO remaining time is 0;
				//calculate IO
				else
				{//move current process to blocked Q

				 //cout << "process moved to blocked  " << currentProcess.pid;
					int IOburst = currentProcess.IO_burst_time*currentProcess.currentCpuBurst;
					//cout << "IO time for current process is " << IOburst;
					currentProcess.IO_time = IOburst;
					currentProcess.State = "Blocked";
					currentProcess.timeinCurrState = IOburst;
					BlockedQ.push_back(currentProcess);
					//std:sort(BlockedQ.begin(), BlockedQ.end(), Sortbytime);
				}
			}
		}
		else
		{
			//increment the Cycle
			Cycle++;
			if (!BlockedQ.empty())
			{
				IOUtil += 1;
			}
			if (verbose == true)
			{
				DebugQ.clear();

				DebugT();
			}

			//cout << "Cycle is   " << Cycle << endl;
			for (int i = 0; i < BlockedQ.size(); i++)
			{
				BlockedQ[i].IO_time = BlockedQ[i].IO_time - 1;
				// Increment total IO time for each process
				BlockedQ[i].Total_IO_time += 1;
				BlockedQ[i].timeinCurrState -= 1;
				// if blocked Q IO is over move to ready Q and sort Q;
				if (BlockedQ[i].IO_time == 0)
				{
					BlockedQ[i].State = "Ready";
					BlockedQ[i].timeinCurrState = 0;
					ReadyQ.push_back(BlockedQ[i]);
					//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbytime);
					//cout << "blocked process moved to ready at the end" << endl;
				}
			}

			RemoveFromBlockedQ();


		}


	}
}

void UniProg(vector<Process> P)
{
	clearQs();
	int numCompletedP = 0;
	Cycle = 0;
	IOUtil = 0;
	currRandom = 0;
	ProcessQ = P;

	//cout << "The scheduling algorithm used was Uniprocessing" << endl;

	if (verbose == true)
	{
		cout << "This detailed printout gives the state and remaining burst for each process" << endl;
		DebugQ.clear();
		DebugT();
	}
	while (numCompletedP != ProcessQ.size())
	{


		for (int i = 0; i < ProcessQ.size(); i++)
		{

			//cout << "num process " << P.size() << endl;
			//cout << P[i].get_arrivaltime();
			// if process arrival time is less than or equal to Cycle, move to ready Q
			if ((ProcessQ[i].get_arrivaltime() <= Cycle) && (!ProcessQ[i].get_Seen()))
			{
				ProcessQ[i].set_Seen();
				ProcessQ[i].pid = i;
				//P[i].Waiting_time = Cycle - P[i].arrival_time;
				ProcessQ[i].State = "Ready";
				ReadyQ.push_back(ProcessQ[i]);

			}
			//for (int i = 0; i < ReadyQ.size(); i++)
			//cout << "Ready q  " << ReadyQ[i].pid << endl;;

		}
		if (!ReadyQ.empty())
		{// do scheduling
		 // move first element to running state
			while (!ReadyQ.empty())
			{
				Process currentProcess = ReadyQ.front();
				currentProcess.Waiting_time = Cycle - currentProcess.arrival_time;
				currentProcess.State = "Running";
				ReadyQ.erase(ReadyQ.begin());
				//for (int i = 0; i < ReadyQ.size(); i++)
				//cout << "Ready q  " << ReadyQ[i].pid;

				while (currentProcess.remaining_time != 0)
				{
					int CPU_burst = RandomOS(currentProcess.CPU_burst_time);
					//cout << "CPU burst  " << CPU_burst<<endl;
					//cout << "CPU busr time is  " << currentProcess.CPU_burst_time;

					if (CPU_burst > currentProcess.remaining_time)
					{
						CPU_burst = currentProcess.remaining_time;
						//cout << "CPU burst  " << CPU_burst << endl;


					}
					int CPUBurstforIO = CPU_burst;
					currentProcess.State = "Running";
					currentProcess.timeinCurrState = CPU_burst;
					//cout << "CPU burst  " << CPU_burst<<endl;
					while (CPU_burst != 0)
					{

						for (int i = 0; i < ProcessQ.size(); i++)
						{

							//cout << "num process " << P.size() << endl;
							//cout << P[i].get_arrivaltime()<<endl;
							// if process arrival time is less than or equal to Cycle, move to ready Q
							if ((ProcessQ[i].get_arrivaltime() <= Cycle) && (!ProcessQ[i].get_Seen()))
							{
								ProcessQ[i].set_Seen();
								//P[i].Waiting_time = Cycle - P[i].arrival_time;
								ProcessQ[i].pid = i;
								ProcessQ[i].State = "Ready";
								ProcessQ[i].timeinCurrState = 0;
								ReadyQ.push_back(ProcessQ[i]);

								//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbyarrival);

							}
						}

						//currentProcess.total_CPU_time -= CPU_burst;
						Cycle++;

						if (verbose == true)
						{
							DebugQ.clear();

							DebugQ.push_back(currentProcess);

							DebugT();
						}
						currentProcess.timeinCurrState -= 1;
						CPU_burst--;
						currentProcess.remaining_time -= 1;
					}

					if (currentProcess.remaining_time == 0)
					{
						currentProcess.State = "Terminated";
						currentProcess.timeinCurrState = 0;
						currentProcess.Finish_time = Cycle;
						currentProcess.TurnAroundTime = Cycle - currentProcess.arrival_time;
						currentProcess.Waiting_time = Cycle - currentProcess.arrival_time - currentProcess.Total_IO_time - currentProcess.total_CPU_time;

						numCompletedP++;
						TerminatedQ.push_back(currentProcess);

						break;
					}
					else
					{

						int IOburst = currentProcess.IO_burst_time*CPUBurstforIO;
						currentProcess.State = "Blocked";
						currentProcess.timeinCurrState = IOburst;
						//cout << "IO Time is  " << IOburst;
						while (IOburst != 0)
						{

							for (int i = 0; i < ProcessQ.size(); i++)
							{

								//cout << "num process " << P.size() << endl;
								//cout << P[i].get_arrivaltime()<<endl;
								// if process arrival time is less than or equal to Cycle, move to ready Q
								if ((ProcessQ[i].get_arrivaltime() <= Cycle) && (!ProcessQ[i].get_Seen()))
								{
									ProcessQ[i].set_Seen();
									//P[i].Waiting_time = Cycle - P[i].arrival_time;
									ProcessQ[i].pid = i;
									ProcessQ[i].State = "Ready";
									ProcessQ[i].timeinCurrState = 0;
									ReadyQ.push_back(ProcessQ[i]);

									//std::sort(ReadyQ.begin(), ReadyQ.end(), Sortbyarrival);

								}
							}
							Cycle++;

							if (verbose == true)
							{
								DebugQ.clear();

								DebugQ.push_back(currentProcess);

								DebugT();
							}
							currentProcess.Total_IO_time += 1;
							currentProcess.timeinCurrState -= 1;
							IOburst--;
							IOUtil++;
							//cout << "I am here for IO and Cycle is  " << Cycle << endl;
							//cout << "IO Time is  " << IOburst;
						}
					}
				}

				// run it for CPU burst then move to blocked for I/O.
				// After i/o done move back to ready and then run again until completion.
				// if any process state is done, numP++
				// Remove from readyQ


			}
		}
		else
		{
			//Advance Cycle to by arrival of next/first element in ready Q
			int i;
			//for (i = 0; P[i].seenP == true; i++);
			Cycle++;
			if (verbose == true)
			{
				DebugQ.clear();

				//DebugQ.push_back(currentProcess);

				DebugT();
			}
			//cout << "I am here and Cycle is  " << Cycle << endl;
		}


		//cout << "Uniprog" << endl;

	}
}

bool Sortbytime(Process P1, Process P2)
{
	if (P1.remaining_time < P2.remaining_time)
	{
		return(P1.remaining_time < P2.remaining_time);
	}
	else if (P1.remaining_time > P2.remaining_time)
	{
		return(P1.remaining_time < P2.remaining_time);
	}
	else if (P1.remaining_time == P2.remaining_time)
	{
		return(P1.pid < P2.pid);
	}
}

bool Sortbyarrival(Process P1, Process P2)
{
	if (P1.get_arrivaltime() < P2.get_arrivaltime())
	{
		return(P1.get_arrivaltime() < P2.get_arrivaltime());
	}
	else if (P1.get_arrivaltime() > P2.get_arrivaltime())
	{
		return(P1.get_arrivaltime() < P2.get_arrivaltime());
	}
	else if (P1.get_arrivaltime() == P2.get_arrivaltime())
	{
		return(P1.pid < P2.pid);
	}
}

void PrintProcessDetails()
{
std:sort(TerminatedQ.begin(), TerminatedQ.end(), Sortbyarrival);
	CPUUtilization = 0;
	avgturnaround = 0;
	Avgwaittime = 0;
	TnumProcess = 0;

	for (int i = 0; i < TerminatedQ.size(); i++)
	{
		//cout << TerminatedQ[i].arrival_time << " " << TerminatedQ[i].CPU_burst_time << " " << "  " << TerminatedQ[i].total_CPU_time << "  " << TerminatedQ[i].IO_burst_time << " ";
		Process currentProcess = TerminatedQ[i];
		cout << "Process " << currentProcess.pid <<":" <<endl;
		cout<<"       " << "(A, B, C, M)  = " <<"("<< currentProcess.arrival_time << ", " << currentProcess.CPU_burst_time << " , " << currentProcess.total_CPU_time << " " << ", " << currentProcess.IO_burst_time<<")" << endl;
		//cout << "I am here and num completed  " << numCompletedP << endl;
		cout <<"       "<< "Finishing time : " << currentProcess.Finish_time << endl;
		cout <<"       "<< "Turnaround Time :  " << currentProcess.TurnAroundTime << endl;
		cout <<"       "<< "I/O time :   " << currentProcess.Total_IO_time << endl;
		cout <<"       "<< "Waiting time :  " << currentProcess.Waiting_time << endl;
		cout <<"       "<< endl;
		cout << setw(6) << endl;
		CPUUtilization += currentProcess.total_CPU_time;
		Avgwaittime += currentProcess.Waiting_time;
		avgturnaround += currentProcess.TurnAroundTime;
		TnumProcess++;

	}
}

void PrintSummary()
{
	cout << "Finishing time :  " << Cycle << endl;
	cout << "CPU Utilization :" << CPUUtilization / Cycle << endl;
	cout << "I / O Utilization : " << (IOUtil / Cycle) << endl;
	cout << "Throughput :  " << (TnumProcess / Cycle) * 100 << "  processes per hundred cycles" << endl;
	cout << "Average turnaround time :  " << avgturnaround / TnumProcess << endl;
	cout << "Average waiting time :  " << Avgwaittime / TnumProcess << endl;
}
int main(int argc, char* argv[])
{
	init();
	string file;

	//cout << "ARG count  " << argc;
	if (argc > 2)
	{
		file = argv[2];
		verbose = true;
	}
	else
	{
		file = argv[1];
		//file = "C://NYU//Assignment//OS//Lab2//input.txt";
		//cout << "here ";

	}
	//string file = "C://NYU//Assignment//OS//Lab2//input.txt";
	vector<Process> Processes;
	ifstream input(file);
	string word;
	if (!input)
	{
		cout << "File not open" << endl;
	}
	else
	{

		input >> word;
		int numProcess = stoi(word);
		for (int i = 0; i < numProcess; i++)
		{
			string arrival_time;
			int Atime = 0;
			int total_CPU_time;
			int CPU_burst_time;
			string IO_burst_time;

			input >> arrival_time;
			Atime = stoi(arrival_time.substr(1, sizeof(arrival_time)));
			input >> CPU_burst_time;
			input >> total_CPU_time;
			input >> IO_burst_time;
			int IO_burst = stoi(IO_burst_time.substr(0, sizeof(IO_burst_time) - 1));
			//input >> CPU_burst_time;
			Process P(Atime, CPU_burst_time, total_CPU_time, IO_burst, i);
			Processes.push_back(P);
			//sort by arrival time


		}

		cout << "The original input was  " << Processes.size() << "  ";
		for (int i = 0; i < Processes.size(); i++)
		{
			cout << "(" << Processes[i].arrival_time << " " << Processes[i].CPU_burst_time << " " << Processes[i].total_CPU_time << " "<< Processes[i].IO_burst_time << ") ";

		}
		cout << endl;
		//Sortbyarrival(Processes);
		std::sort(Processes.begin(), Processes.end(), Sortbyarrival);

		cout << "The sorted input is     " << Processes.size() << "  ";
		for (int i = 0; i < Processes.size(); i++)
		{
			cout << "(" << Processes[i].arrival_time << " " << Processes[i].CPU_burst_time << " "<< Processes[i].total_CPU_time << " " << Processes[i].IO_burst_time << ") ";

		}
		cout << endl;

		RR(Processes);

		cout << "The scheduling algorithm used was Round Robin" << endl;
		cout << endl;
		cout<<endl;
		/*std:sort(TerminatedQ.begin(), TerminatedQ.end(), Sortbyarrival);

		for (int i = 0; i < TerminatedQ.size(); i++)
		{
		//cout << TerminatedQ[i].arrival_time << " " << TerminatedQ[i].CPU_burst_time << " " << "  " << TerminatedQ[i].total_CPU_time << "  " << TerminatedQ[i].IO_burst_time << " ";
		Process currentProcess = TerminatedQ[i];
		cout << "Process  " << currentProcess.pid << endl;
		cout << "A B C M = " << currentProcess.arrival_time << " " << currentProcess.CPU_burst_time << "  " << currentProcess.total_CPU_time << " " << " " << currentProcess.IO_burst_time << endl;
		//cout << "I am here and num completed  " << numCompletedP << endl;
		cout << "Finishing time :  " << currentProcess.Finish_time << endl;
		cout << "Turnaround Time :  " << currentProcess.TurnAroundTime << endl;
		cout << "I/O time :   " << currentProcess.Total_IO_time << endl;
		cout << "Waiting time :  " << currentProcess.Waiting_time << endl;
		cout << endl;
		cout << endl;
		CPUUtilization += currentProcess.total_CPU_time;
		Avgwaittime += currentProcess.Waiting_time;
		avgturnaround += currentProcess.TurnAroundTime;
		TnumProcess++;

		}*/
		PrintProcessDetails();
		cout << endl;
		cout << "Summary Data" << endl;
		cout << endl;
		PrintSummary();
		cout << endl;
		cout << "--------------------------------------------------------------------------------" << endl;
		SJF(Processes);
		cout << "The scheduling algorithm used was Shortest Job First" << endl;
		cout << endl;
		PrintProcessDetails();
		cout << endl;
		cout << "Summary Data" << endl;
		cout << endl;
		PrintSummary();
		cout << endl;
		cout << endl;
		cout << "--------------------------------------------------------------------------------" << endl;
		FCFS(Processes);
		cout << "The scheduling algorithm used was FCFS" << endl;
		cout << endl;
		PrintProcessDetails();
		cout << endl;
		cout << "Summary Data" << endl;
		cout << endl;
		PrintSummary();
		cout << endl;
		cout << endl;
		cout << "--------------------------------------------------------------------------------" << endl;
		UniProg(Processes);
		cout << "The scheduling algorithm used was Uniprocessing" << endl;
		cout << endl;
		PrintProcessDetails();
		cout << endl;
		cout << "Summary Data" << endl;
		cout << endl;
		PrintSummary();
		cout << endl;
		cout << endl;
		cout << "--------------------------------------------------------------------------------" << endl;



	}
	//getchar();
	return 0;


}
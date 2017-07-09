#include<iostream>
#include<cstdio>
#include<map>
#include<vector>
#include<sstream>
#include<fstream>
#include<algorithm>
#include <queue>

using namespace std;
#define INT_MAX 2147483647
int totalPageFaults = 0;
class Process
{
public:
	int currWord;
	int pID;
	int finished;
	int numFaults;
	int numReference;

	Process()
	{
		currWord = 0;
		pID = 0;
		finished = 0;
		numFaults = 0;
		numReference = 0;
	}

};

class Page
{
public:
	int pageNum;
	int processID;
	int arrivalTime;
	Page()
	{
		pageNum = -100;
		processID = -20;
		arrivalTime = 0;
	}
};
int M = 0;
int	P = 0;
int	S = 0;
int	J = 0;
int N = 0;
int D = 0;
string R = "";
double A[4] = { 0.75,0.75,0.75,0.50 };
double B[4] = { 0.25,0.0,0.125,0.125 };
double C[4] = { 0.0,0.25,0.125,0.125 };
int numProcess = 4;
int currRandom = 0;
int cycle = 0;
int numPages;
int numFrames;
int currAvailableFrame;
vector<Page> frameTable;
vector<Page> lruTable;
vector<Process> processList;
vector<Process> finishedProcessList;
vector<int>RandomInput;
vector<int>residencyTime;
vector<int>evictionPerProcess;
bool pageTableFull = false;
void init()
{
	string filer = "random-numbers";

	ifstream input1(filer);
	int random;
	if (!input1)
	{
		cout << "file not open" << endl;
	}
	while (!input1.eof())
	{

		input1 >> random;
		RandomInput.push_back(random);
		//cout << random<<endl;
	}
	input1.close();
}
int calculateNextWord(int processID)
{
	double y = (RandomInput[currRandom] / (INT_MAX + 1.0));
	//cout << "Random number used " << RandomInput[currRandom] << endl;
	//cout << "Y is " << y << endl;
	currRandom++;
	if (J < 4)// access A[0],B[0],C[0]
	{
		if (y < A[0])
		{
			return 1;
		}
		else if (y < A[0] + B[0])
		{
			return 2;
		}
		else if (y < A[0] + B[0] + C[0])
		{
			return 3;
		}
		else
		{
			return 4;
		}


	}
	else
	{
		if (y < A[processID - 1])
		{
			return 1;
		}
		else if (y < A[processID - 1] + B[processID - 1])
		{
			return 2;
		}
		else if (y < A[processID - 1] + B[processID - 1] + C[processID - 1])
		{
			return 3;
		}
		else
		{
			return 4;
		}

	}

}

void simulatePaging()
{
	int numFinished = 0;

	int q = 3;
	int ref = 0;
	while (numFinished < numProcess)
	{
		//cout << "Size of process List " << processList.size()<<endl;
		for (int I = 0; I < processList.size(); I++)
		{
			if (processList[I].finished != true)
			{
				for (ref = 0; ref < q; ref++)
				{
					//simulate this reference for this process
					cycle++;
					Process currProcess = processList[I];
					//cout << "Current word is  " << currProcess.currWord << endl;
					int word = currProcess.currWord%S;
					//cout << "word is " << word << endl;
					int page = word / P;
					//cout << "page is " << page << endl;
					int pageIndex = -10;
					// check if page is there in frametable
					for (int i = 0; i < frameTable.size(); i++)
					{
						if ((frameTable[i].pageNum == page) && frameTable[i].processID == processList[I].pID)
						{
							//cout << "Page is found in table at index " << i << endl;

							pageIndex = i;
							break;
						}
					}
					if (pageIndex != -10)
					{
						if (R == "lru")
						{
							//cout << "no need to replace page. Page present" << endl;
							//cout << "Hit in frame " << pageIndex << endl;
							//update lru table

							int lruIndex = 0;
							//cout << "here at top " << endl;

							for (int i = 0; i < lruTable.size(); i++)
							{
								if ((lruTable[i].pageNum == page) && lruTable[i].processID == processList[I].pID)
								{
									lruIndex = i;
									break;
								}
							}
							if (!lruTable.empty())
							{
								Page tempLRU;
								tempLRU = lruTable[lruIndex];
								lruTable.erase(lruTable.begin() + lruIndex);
								lruTable.push_back(tempLRU);
								//cout << "here " << endl;
							}
							//do processing
						}
					}
					else// page fault
					{
						totalPageFaults++;
						processList[I].numFaults++;
						if (!pageTableFull)
						{
							//cout << "Fault, using free frame " << currAvailableFrame << endl;
							frameTable[currAvailableFrame].pageNum = page;
							frameTable[currAvailableFrame].processID = processList[I].pID;
							frameTable[currAvailableFrame].arrivalTime = cycle;
							if (R == "lru")
							{
								Page temp;
								temp.pageNum = page;
								temp.processID = processList[I].pID;;
								lruTable.push_back(temp);
							}
							//cout << "currAvailableFrame " << currAvailableFrame << endl;
							currAvailableFrame--;
							if (currAvailableFrame < 0)
							{
								pageTableFull = true;
							}
						}
						else
						{
							//use replacement algo
							//cout << "Replace page" << endl;
							if (R == "lru")
							{
								//find index of lru page in frame table and replace
								//take first index from lru table search in frametable and update lru table
								//cout << "lru page algo" << endl;
								Page tempLRU = lruTable.front();
								int replaceIndex = 0;
								for (int i = 0; i < frameTable.size(); i++)
								{
									if ((frameTable[i].pageNum == tempLRU.pageNum) && frameTable[i].processID == tempLRU.processID)
									{
										replaceIndex = i;
										break;

									}
								}

								int processPage = frameTable[replaceIndex].processID;
								residencyTime[processPage - 1] += (cycle - frameTable[replaceIndex].arrivalTime);
								evictionPerProcess[processPage - 1]++;
								frameTable[replaceIndex].pageNum = page;
								frameTable[replaceIndex].processID = processList[I].pID;
								frameTable[replaceIndex].arrivalTime = cycle;
								Page templru;
								templru.pageNum = page;
								templru.processID = processList[I].pID;
								lruTable.erase(lruTable.begin());
								lruTable.push_back(templru);

							}
							else if (R == "lifo")
							{
								//remove front of frametable
								int processPage = frameTable[0].processID;
								residencyTime[processPage - 1] += (cycle - frameTable[0].arrivalTime);
								evictionPerProcess[processPage - 1]++;
								frameTable[0].pageNum = page;
								frameTable[0].processID = processList[I].pID;
								frameTable[0].arrivalTime = cycle;
								//add new page at front
							}
							else if (R == "random")
							{
								// remove random page
								//put new page at that location in frame table
								//cout << "Random Num used " << RandomInput[currRandom]<<endl;
								int randomIndex = RandomInput[currRandom] % numFrames;
								currRandom++;
								int processPage = frameTable[randomIndex].processID;
								residencyTime[processPage - 1] += (cycle - frameTable[randomIndex].arrivalTime);
								evictionPerProcess[processPage - 1]++;
								frameTable[randomIndex].pageNum = page;
								frameTable[randomIndex].processID = processList[I].pID;
								frameTable[randomIndex].arrivalTime = cycle;
							}

						}

					}
					int casenum = calculateNextWord(processList[I].pID);
					switch (casenum)
					{
					case 1:
						processList[I].currWord = (word + 1) % S;
						break;
					case 2:
						processList[I].currWord = (word - 5 + S) % S;
						break;
					case 3:
						processList[I].currWord = (word + 4) % S;
						break;
					case 4:
						//cout << "hittting here" << endl;
						processList[I].currWord = RandomInput[currRandom];// insert random value
						currRandom++;
						break;
					}
					processList[I].numReference++;
					if (processList[I].numReference == N)
					{
						processList[I].finished = true;
						//remove this from processlist and push to completed list;
						Process Finished = processList[I];
						finishedProcessList.push_back(Finished);
						//cout << "Finished process num " << processList[I].pID << "  at time " << cycle << endl;
						//processList.erase(processList.begin() + I);

						numFinished++;
						if (numFinished == numProcess)
						{
							break;
						}
						break;

					}
					//calculate the next reference for this process




				}
			}

		}

	}


}

void pagingDriver()
{

	/*J=1: One process with A=1 and B=C=0, the simplest (fully sequential) case.
	J=2: Four processes, each with A=1 and B=C=0.
	J=3: Four processes, each with A=B=C=0 (fully random references).
	J=4: One process with A=.75, B=.25 and C=0; one process with A=.75, B=0, and C=.25; one process
	with A=.75, B=.125 and C=.125; and one process with A=.5, B=.125 and C=.125*/
	switch (J)
	{
	case 1:
		A[0] = 1.0;
		B[0] = 0.0;
		C[0] = 0.0;
		numProcess = 1;
		break;
	case 2:
		A[0] = 1.0;
		B[0] = 0.0;
		C[0] = 0.0;
		break;
	case 3:
		A[0] = 0;
		B[0] = 0;
		C[0] = 0;
		break;
	case 4:
		break;
	default:
		cout << "Wrong job mix" << endl;
		break;
	}


	for (int i = 0; i < numProcess; i++)
	{
		Process temp;
		temp.pID = i + 1;
		temp.currWord = (i + 1) * 111;
		processList.push_back(temp);
		residencyTime.push_back(0);
		evictionPerProcess.push_back(0);
	}

	for (int i = 0; i < numFrames; i++)
	{
		Page temp;
		frameTable.push_back(temp);
	}

	simulatePaging();

}
void printOutput()
{

	cout << "The machine size is " << M << endl;
	cout << "The page size is " << P << endl;
	cout << "The process size is " << S << endl;
	cout << "The job mix number is " << J << endl;
	cout << "The number of references per process is " << N << endl;
	cout << "The replacement algorithm is  " << R << endl;
	cout << "The level of debugging output is " << D << endl;
	cout << endl;
	int Total_residency = 0;
	int Total_Evictions = 0;
	for (int i = 0; i < finishedProcessList.size(); i++)
	{

		if (evictionPerProcess[i] != 0)
		{
			cout << "Process " << finishedProcessList[i].pID << " had  " << finishedProcessList[i].numFaults << " faults ";
			cout << " and  " << residencyTime[i] / (evictionPerProcess[i] + 0.0) << " average residency." << endl;
			Total_residency += residencyTime[i];
			Total_Evictions += evictionPerProcess[i];


		}
		else
		{
			cout << "Process" << finishedProcessList[i].pID << " had " << finishedProcessList[i].numFaults << " faults." << endl;
			cout << "With no evictions, the average residence is undefined" << endl;
			Total_residency += residencyTime[i];

		}


	}
	cout << endl;
	if (Total_Evictions != 0)
	{
		cout << "The total number of faults is " << totalPageFaults << " and the overall average residency is " << Total_residency / (Total_Evictions + 0.0) << endl;
	}
	else
	{
		cout << "The total number of faults is " << totalPageFaults << endl;
		cout << "With no evictions, the overall average residence is undefined." << endl;
	}
}
int main(int argc, char* argv[])
{
	if (argc == 8)
	{
		M = stoi(argv[1]);
		P = stoi(argv[2]);
		S = stoi(argv[3]);
		J = stoi(argv[4]);
		N = stoi(argv[5]);
		R = argv[6];
		D = stoi(argv[7]);
	
	}
	else
	{
		//cout << "here" << endl;
		M = stoi(argv[1]);
		P = stoi(argv[2]);
		S = stoi(argv[3]);
		J = stoi(argv[4]);
		N = stoi(argv[5]);
		R = argv[6];
		//D = stoi(argv[7]);

	}
	
	init();
	numPages = S / P;
	numFrames = M / P;
	currAvailableFrame = numFrames - 1;
	pagingDriver();
	printOutput();
	//getchar();
}
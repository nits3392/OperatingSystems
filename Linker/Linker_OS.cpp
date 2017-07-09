#include <stdio.h>
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>


using namespace std;

template <typename T>
string toString(T Number)
{
	ostringstream ss;
	ss << Number;
	return ss.str();
}





int main(int argc, char *argv[])
{
	string input_file = argv[1];
	ifstream Ifile, Ifile1;
	string line;
	map<string, string> sTable;
	map<string, int>::iterator symboltable;
	map<string, string>::iterator symboltable1;
	map<int, string>::iterator instable;
	map<int, string> instmap;
	map<int, string> memoryMap;
	map<string, int> multipledef;
	int numInst = 0;
	int currModule = 0;
	int mBaseAdd[200] = { 0 };
	int modulesize = 0;
	string symbol = "";
	int numSymbol = 0;
	int value = 0;
	map <string, int> doublemapping;
	map <string, int> symbol_module;
	map <string, int> symbol_module_exceed;


	ifstream fin(input_file);

	if (!fin) {
		cout << "Could not open file" << endl;
		return 1;
	}

	string word; // parsing word by word
	while (!fin.eof())
	{
		fin >> word;
		//cout<<word<<endl;
		int num_inst = stoi(word);
		for (int i = 0; i < num_inst; i++)
		{
			fin >> word;
			if (stoi(word) != 0)
			{
				for (int i = 0; i < stoi(word); i++)
				{
					string sym, value;
					fin >> sym;
					fin >> value;
					//cout << mBaseAdd[currModule];
					if (sTable[sym] == "")
					{
						sTable[sym] = toString(stoi(value) + mBaseAdd[currModule]);
						multipledef[sym] = 1;
						symbol_module[sym] = currModule;
						//cout << sym << " decoding sym module " << currModule << endl;
					}

					else
					{
						multipledef[sym] = multipledef[sym] + 1;
						//cout << sym << " multiple def " << multipledef[sym] + 1<< endl;


					}
				}
			}

			fin >> word;
			for (int i = 0; i < stoi(word); i++)
			{
				string sym, value;
				fin >> sym;
				fin >> value;
				//cout << sym << value << endl;
				while (stoi(value) != -1)
				{
					if (instmap[stoi(value)] == "")
					{
						instmap[stoi(value)] = sym;
						fin >> value;
					}
					else
					{
						fin >> value;
					}


				}
			}

			fin >> word;
			modulesize = stoi(word);
			for (int i = 0; i < stoi(word); i++)
			{
				string sym, value;
				fin >> sym;
				fin >> value;
				if (sym == "E")
				{
					memoryMap[numInst] = sym + value;

				}
				else if (sym == "A")
				{
					memoryMap[numInst] = value;

				}
				else if (sym == "I")
				{
					memoryMap[numInst] = value;

				}
				else if (sym == "R")
				{
					memoryMap[numInst] = toString(stoi(value) + mBaseAdd[currModule]);


				}

				numInst++;

			}

			currModule++;

			mBaseAdd[currModule] = mBaseAdd[currModule - 1] + modulesize;

			// reset instmap
			instmap.clear();

		}
		cout << "Symbol Table" << endl;
		for (symboltable1 = sTable.begin(); symboltable1 != sTable.end(); symboltable1++)
		{
			int k = symbol_module[symboltable1->first] + 1;
			//cout << "K = " << k;
			//cout << " Checking base address  " << symboltable->second << " base add " << mBaseAdd[k] << endl;


			if ((stoi(symboltable1->second) >= mBaseAdd[k]) && (multipledef[symboltable1->first] > 1))
			{
				symboltable1->second = mBaseAdd[k - 1];
				cout << setw(3) << symboltable1->first << " = " << setw(2) << mBaseAdd[k - 1] << " Error: Definition exceeds module size; first word in module used" << endl;
				cout << setw(9) << "" << "Error: This variable is multiply defined; first value used" << endl;
			}

			else if (multipledef[symboltable1->first] > 1)
			{
				cout << setw(3) << symboltable1->first << " = " << symboltable1->second << " Error: This variable is multiply defined; first value used." << endl;
			}

			else if (stoi(symboltable1->second) >= mBaseAdd[k])
			{
				symboltable1->second = toString(mBaseAdd[k - 1]);
				cout << setw(3) << symboltable1->first << " = " << setw(2) << mBaseAdd[k - 1] << " Error: Definition exceeds module size; first word in module used" << endl;
			}

			else
			{
				cout << setw(3) << symboltable1->first << " = " << setw(2) << symboltable1->second << endl;;

			}
		}


	}

	fin.close();
	ifstream fin1(input_file);
	currModule = 0;
	numInst = 0;
	cout << endl;
	cout << endl;

	cout << "Memory Map" << endl;
	while (!fin1.eof())
	{
		fin1 >> word;
		//cout << word << endl;
		int num_inst = stoi(word);
		for (int i = 0; i < num_inst; i++)
		{
			fin1 >> word;
			if (stoi(word) != 0)
			{
				for (int i = 0; i < stoi(word); i++)
				{
					string sym, value;
					fin1 >> sym;
					fin1 >> value;
					//cout << mBaseAdd[currModule];


				}
			}

			fin1 >> word;
			for (int i = 0; i < stoi(word); i++)
			{
				string sym, value;
				fin1 >> sym;
				fin1 >> value;
				//cout << sym << value << endl;
				while (stoi(value) != -1)
				{
					if (instmap[stoi(value)] == "")
					{
						instmap[stoi(value)] = sym;
						//doublemapping[sym] = 1;
						//cout << value << "mapped to" << instmap[stoi(value)] << endl;;
						fin1 >> value;
					}
					else
					{
						//cout << "I am here" << endl;

						doublemapping[value] = 1;
						fin1 >> value;
					}


				}
			}

			fin1 >> word;
			for (int i = 0; i < stoi(word); i++)
			{
				string sym, value;
				fin1 >> sym;
				fin1 >> value;
				//cout << sym<<"  ";
				if (sym == "E")
				{
					string s = instmap[i];
					
					//cout << sTable[s] << "";
					int value1 = (stoi(value) / 1000);
					
					
					if (sTable[s] == "")
					{
						memoryMap[numInst] = toString(value1 * 1000);
						cout << setw(3) << numInst << ":     " << memoryMap[numInst] << " Error: " << s << " is not defined; zero used" << endl;
						
					}
					//check conditions for memory segment
					//cout << numinst << endl;
					else if (doublemapping[toString(i)] == 1)
					{
						memoryMap[numInst] = toString(stoi(sTable[s]) + value1 * 1000);
						cout << setw(3) << numInst << ":     " << memoryMap[numInst] << " Error: Multiple variables used in instruction;all but first ignore" << endl;
					}
					else
					{
						memoryMap[numInst] = toString(stoi(sTable[s]) + value1 * 1000);
						cout << setw(3) << numInst << ":     " << memoryMap[numInst] << endl;
						//cout<<" I am right"<<endl;
						//cout<<"symbol is "<<s<<endl;
						multipledef[s] = 0;
					}
				}
				else if (sym == "A")
				{
					string s = instmap[i];
					//cout << s<<"";
					//cout << sTable[s] << "";
					int value1 = (stoi(value) / 1000);
					int add = (stoi(value) % 1000);
					//cout << add << endl;
					if (add > 199)
					{
						memoryMap[numInst] = toString(0 + value1 * 1000);
						//check conditions for memory segment
						cout << setw(3) << numInst << ":     " << memoryMap[numInst] << " Error: Absolute address exceeds machine size; zero used" << endl;
					}
					else
					{
						cout << setw(3) << numInst << ":     " << memoryMap[numInst] << endl;
					}

				}

				else if (sym == "R")
				{
					string s = instmap[i];
					//cout << s<<"";
					//cout << sTable[s] << "";
					int value1 = (stoi(value) / 1000);
					int add = (stoi(value) % 1000);
					if (add < stoi(word))
					{
						cout << setw(3) << numInst << ":     " << memoryMap[numInst] << endl;
					}
					//cout << add << endl;
					else
					{
						memoryMap[numInst] = toString(0 + value1 * 1000);
						cout << setw(3) << numInst << ":     " << memoryMap[numInst] << " Error: Relative address exceeds module size;zero used" << endl;
					}


				}



				else
				{
					cout << setw(3) << numInst << ":     " << setw(2) << memoryMap[numInst] << endl;
				}
				//cout << numInst << ":     " << memoryMap[numInst]<<endl;
				numInst++;
			}


			for (instable = instmap.begin(); instable != instmap.end(); instable++)
			{
				if (instable->second != "")
				{	//int x = symbol_module[instable->second];
					//cout << "checking for symbol exceed module size " << instable->second << "  " << symbol_module[instable->second] << endl;
					//cout << "Hello There first " << instable->first << "  " << mBaseAdd[currModule] << endl;
					if (instable->first > mBaseAdd[currModule + 1])
					{
						symbol_module_exceed[instable->second] = currModule;
						//cout << "Hello There " << symbol_module_exceed[instable->second] << endl;;
					}
				}

			}
			currModule++;

			instmap.clear();

		}


		currModule++;

		mBaseAdd[currModule] = mBaseAdd[currModule - 1] + modulesize;

		// reset instmap


		instmap.clear();

		cout << endl;
		for (symboltable = multipledef.begin(); symboltable != multipledef.end(); symboltable++)
		{

			if (symboltable->second>0)
			{
				cout << "Warning: " << symboltable->first << "  was defined in module  " << symbol_module[symboltable->first] + 1 << "  but never used " << endl;
			}

		}

		for (symboltable = symbol_module_exceed.begin(); symboltable != symbol_module_exceed.end(); symboltable++)
		{

			cout << "Error: Use of   " << symboltable->first << "   in module  " << symboltable->second + 1 << "  exceeds module size; use ignored. " << endl;


		}


	}




	fin1.close();
	getchar();
	return 0;
}
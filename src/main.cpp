#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include "Colors.h"
#define WINDOWS //if we are using WINDOWS - app currently has no linux support
#define MICROSECONDS_DIVIDER 1000000
#ifdef WINDOWS
#include <Windows.h>
#include <Psapi.h>
#endif
using namespace std;

struct ProcessData {
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	chrono::high_resolution_clock::time_point beg;
	ProcessData() {
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
	}

	void startCounting() {
		beg = chrono::high_resolution_clock::now();
	}
};

int compareFile(string filein,string fileout) {
	ifstream input;
	ifstream output;
	string inpstream;
	string outstream;
	input.open(filein);
	output.open(fileout);
	if (input.fail() || output.fail()) {
		return -1;
	}
	int linenum = 0;
	while (getline(input, inpstream)) {
		linenum++;
		getline(output, outstream);
		if (inpstream[inpstream.size() - 1] == ' ') inpstream.erase(inpstream.size() - 1, 1); //remove whitespaces at the end
		if (outstream[outstream.size() - 1] == ' ') outstream.erase(outstream.size() - 1, 1); //remove whitespaces at the end
		if (outstream != inpstream) {
			return linenum;
		}

	}
	return 0;
}


ProcessData startup(LPCSTR lpApplicationName,string filein,string fileout)
{
	ProcessData pd;

	pd.si.dwFlags = STARTF_USESTDHANDLES;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	pd.si.hStdOutput = CreateFileA(fileout.c_str(), FILE_GENERIC_WRITE, FILE_SHARE_READ, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	pd.si.hStdInput = CreateFileA(filein.c_str(), FILE_GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	// start the program up
	CreateProcessA(lpApplicationName,   // the path
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		TRUE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&pd.si,            // Pointer to STARTUPINFO structure
		&pd.pi             // Pointer to PROCESS_INFORMATION structure
	);
	pd.startCounting();
	return pd;
}

void printHeader() {
	cout << "TEST_NUMBER      RESULT      MISMATCH_IN_LINE      EXECUTION_TIME[s]      MEMORY_USAGE[KB][INACCURATE!!!]" <<endl;
 }

void printResult(uint64_t amount_of_sec_chrono, uint64_t amount_of_usec_chrono,int result,int number,SIZE_T mem) {
	cout << left << setfill(' ') << setw(16) << number;
	if (result == 0) {
		cout << green << "[  OK  ]     " << setfill(' ') << setw(22) <<"-" << white;
	}
	else if (result == -1) {
		cout << endl << "Error, file not found! Exiting!";
		getchar();
		exit(-1);
	}
	else {
		cout << red << "[ FAIL ]     " << left << setfill(' ') << setw(22) << result << white;
	}
	cout
		<< amount_of_sec_chrono
		<< "."
		<< setfill('0') << setw(6) << amount_of_usec_chrono;
	cout
		<< "               "
		<<setfill(' ')
		<<setw(6)
		<<mem <<endl;
}

int main(int argc,char ** argv)
{
	ifstream config;
	string pathToProjectBinary;
	string filein; // 0.in
	string fileout; //0.out
	string fileinpath; //tests
	string fileoutpath; //outputs
	string tmp;
	int amountOfTests = 0;
	config.open("config.txt");
	if (config.fail()) {
		cout << "config.txt not found!" << endl;
		return -1;
	}
	getline(config, pathToProjectBinary); 
	getline(config, fileinpath);
	getline(config, fileoutpath);
	getline(config, tmp);

	//------strip everything after # because it's a comment
	pathToProjectBinary.erase(pathToProjectBinary.find(" #"),string::npos);
	fileinpath.erase(fileinpath.find(" #"),string::npos);
	fileoutpath.erase(fileoutpath.find(" #"),string::npos);
	tmp.erase(tmp.find(" #"),string::npos);
	//------

	amountOfTests = stoi(tmp);
	printHeader();

	for (int i = 0; i <= amountOfTests; i++) {
		filein = fileinpath + "\\" + to_string(i) + ".in";
		fileout = fileoutpath + "\\" + to_string(i) + ".out";
		ProcessData pd = startup(pathToProjectBinary.c_str(),filein,fileout);
		WaitForSingleObject(pd.pi.hProcess, INFINITE);
		// process has terminated...
		auto end = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::microseconds>(end - pd.beg).count();
		uint64_t wholeTime = chrono::duration_cast<chrono::microseconds>(end - pd.beg).count();
		uint64_t amount_of_sec_chrono = wholeTime / MICROSECONDS_DIVIDER;
		uint64_t amount_of_usec_chrono = wholeTime % MICROSECONDS_DIVIDER;

		//you can use Windows time measuring if you want
		/*
		FILETIME creationTime, endTime, lpKernelTime,lpUserspaceTime;
		GetProcessTimes(pd.pi.hProcess, &creationTime, &endTime, &lpKernelTime, &lpUserspaceTime);
		uint64_t us_end = (((uint64_t)endTime.dwHighDateTime << 32) | (uint64_t)endTime.dwLowDateTime) / 10;
		uint64_t us_creation = (((uint64_t)creationTime.dwHighDateTime << 32) | (uint64_t)creationTime.dwLowDateTime) / 10;
		uint64_t difference = us_end - us_creation;
		uint64_t amount_of_sec_windows = difference / MICROSECONDS_DIVIDER;
		uint64_t amount_of_usec_windows = difference % MICROSECONDS_DIVIDER;
*/
		//check if result is correct
		int pos = filein.find(".in");
		string filein_compare = filein;
		filein_compare.replace(pos, 3, ".out");

		//memory info
		PROCESS_MEMORY_COUNTERS pmc;
		ZeroMemory(&pmc,sizeof(pmc));
		pmc.cb = sizeof(pmc);
		GetProcessMemoryInfo(pd.pi.hProcess,&pmc,sizeof(pmc));
		SIZE_T mem = pmc.PeakWorkingSetSize/1024; //FIXME: Readings inaccurate compared to those displayed in STOS
		printResult(amount_of_sec_chrono, amount_of_usec_chrono, compareFile(filein_compare, fileout),i+1,mem);

		// Close handles. 
		CloseHandle(pd.pi.hProcess);
		CloseHandle(pd.pi.hThread);
		CloseHandle(pd.si.hStdOutput);
		CloseHandle(pd.si.hStdInput);
	}
	cout << "Finished!" <<endl;
	getchar(); //pause execution
}

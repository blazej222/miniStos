#include <iostream>
#include <fstream>
#include <string>
using namespace std;
int main(int argc,char ** argv)
{
	if (argc < 3) {
		cout << "Not enough arguments specified";
		return -1;
	}
	ifstream input;
	ifstream output;
	string inpstream;
	string outstream;
	string filein = argv[1];
	string fileout = argv[2];
	//cout << filein << fileout;
	//cin >> filein >> fileout;
	input.open(filein);
	output.open(fileout);
	if (input.fail() || output.fail()) {
		cout << "File not found!";
		return -1;
	}
	int linenum = 0;
	while (getline(input, inpstream)) {
		linenum++;
		getline(output, outstream);
		if (inpstream[inpstream.size() - 1] == ' ') inpstream.erase(inpstream.size() - 1, 1);
		if (outstream[outstream.size() - 1] == ' ') outstream.erase(outstream.size() - 1, 1);
		if (outstream != inpstream) {
			cout << linenum;
			//system("pause");
			return 0;
		}

	}
	cout << "OK!";

}

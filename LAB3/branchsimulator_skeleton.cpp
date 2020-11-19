#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <bitset>
#include <iomanip>

using namespace std;

int main(int argc, char **argv)
{
	int debug = 1;
	ifstream config;
	debug ? config.open("./config.txt") : config.open(argv[1]);
	int m, k;
	config >> m >> k;
	cout << "m:" << m << endl;
	config.close();

	ofstream out;
	string out_file_name = debug ? "trace.txt.out" : (string(argv[2]) + ".out");
	out.open(out_file_name.c_str());

	ifstream trace;
	debug ? trace.open("trace.txt") : trace.open(argv[2]);

	//k is unused
	vector<bitset<2>> PHT;
	//^ in c++ is xor
	PHT.resize(pow(2, m));

	//init
	for (int i = 0; i < pow(2, m); i++)
	{
		PHT[i] = bitset<2>("11");
		//cout << i << endl;
	}
	//cout << PHT[2].to_string() << endl;

	//stat
	int corrCnt = 0;
	int wrongCnt = 0;

	//var init
	bitset<32> addr;
	bitset<2> hist;
	int temp;

	while (!trace.eof())
	{
		unsigned long pc;
		bool taken;
		trace >> std::hex >> pc >> taken;
		bool prediction;
		//pc convert to bit
		addr = bitset<32>(pc);
		//select pht by bit
		hist = PHT[bitset<32>(addr.to_string().substr(31 - m, m)).to_ulong()];
		//predict using pht
		prediction = hist.to_ulong() >= 2 ? true : false;
		//compare prediction and real
		//stat
		//update pht
		if (prediction == taken)
		{
			cout<<pc<<" | taken : "<< taken<<" | predict "<<prediction << " | correct"<<endl;
			corrCnt++;
			temp = hist.to_ulong();
			temp++;
			temp = temp>3?3:temp;
			PHT[bitset<32>(addr.to_string().substr(31 - m, m)).to_ulong()]  = bitset<2>(temp);
		}
		else
		{
			cout<<pc<<" | taken : "<< taken<<" | predict "<<prediction << " | Wrong"<<endl;
			wrongCnt++;
			temp = hist.to_ulong();
			temp--;
			temp = temp<0?0:temp;
			PHT[bitset<32>(addr.to_string().substr(31 - m, m)).to_ulong()]  = bitset<2>(temp);
		};
		

		out << prediction;
	}
	cout << "STAT: " <<corrCnt<<" correct | "<<wrongCnt<<" wrong | Correct rate: " << setprecision(4)<< float(corrCnt)/float(corrCnt+wrongCnt) * 100<<"%"<<endl;
	trace.close();
	out.close();
}

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
	vector<bitset<2> > PHT;
	//^ in c++ is xor
	PHT.resize(pow(2, m));

	//init
	for (int i = 0; i < pow(2, m); i++)
	{
		PHT[i] = bitset< 2>(3);
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
	string buffer;
	while (!trace.eof())
	{
		unsigned long pc;
		bool taken;
		trace >> std::hex >> pc >> taken;

		//use the following to avoid empty line
		char c1 = trace.get();
        char c2 = trace.peek();
        if (c2 == '\n')
        {
            cout << "There is a line" << endl;
        }
		//cout<<c1<<"--"<<c2<<endl;

		bool prediction;
		//pc convert to bit
		addr = bitset<32>(pc);
		//select pht by bit
		hist = PHT[bitset<32>(addr.to_string().substr(32 - m, m)).to_ulong()];
		//predict using pht
		prediction = hist.to_ulong() >= 2 ? true : false;
		//compare prediction and real
		//stat
		//update pht
		cout << "pc" << pc << "bit pc" << addr.to_string() << " m bit : " << bitset<32>(addr.to_string().substr(32 - m, m)).to_string() << "row : " << bitset<32>(addr.to_string().substr(32 - m, m)).to_ulong() << " hist : " << hist.to_ulong() << endl;

		if (prediction == taken)
		{
			cout << pc << " | taken : " << taken << " | predict " << prediction << " | correct" << endl;
			corrCnt++;
		}
		else
		{
			cout << pc << " | taken : " << taken << " | predict " << prediction << " | Wrong" << endl;
			wrongCnt++;
		};
		temp = hist.to_ulong();
		taken ? temp++ : temp--;
		temp = temp > 3 ? 3 : temp;
		temp = temp < 0 ? 0 : temp;
		PHT[bitset<32>(addr.to_string().substr(32 - m, m)).to_ulong()] = bitset<2>(temp);

		out <<prediction;
		out << "\n";
	}
	cout << "STAT: " << corrCnt << " correct | " << wrongCnt << " wrong | Correct rate: " << setprecision(6) << float(corrCnt) / float(corrCnt + wrongCnt) * 100 << "% wrong rate: " << float(wrongCnt) / float(corrCnt + wrongCnt) * 100 << "%" << endl;
	trace.close();
	out.close();
}

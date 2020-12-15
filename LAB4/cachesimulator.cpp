/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss

struct config
{
    int L1blocksize;
    int L1waysize;
    int L1size;
    int L2blocksize;
    int L2waysize;
    int L2size;
};

/* you can define the cache class here, or design your own data structure for L1 and L2 cache
class cache {
      
      }
*/
struct cache
{
    vector<vector<string> > tag;
    vector<vector<int> > valid;
    vector<int> pointer;
};

int main(int argc, char *argv[])
{

    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    //cache_params.open(argv[1]);
    cache_params.open("cacheconfig.txt");
    while (!cache_params.eof()) // read config file
    {
        cache_params >> dummyLine;
        cache_params >> cacheconfig.L1blocksize;
        cache_params >> cacheconfig.L1waysize;
        cache_params >> cacheconfig.L1size;
        cache_params >> dummyLine;
        cache_params >> cacheconfig.L2blocksize;
        cache_params >> cacheconfig.L2waysize;
        cache_params >> cacheconfig.L2size;
    }
    //full associate
    int FA1 = 0;
    int FA2 = 0;
    if (cacheconfig.L1waysize == 0)
    {
        cacheconfig.L1waysize = cacheconfig.L1size * 1024/ cacheconfig.L1blocksize;
        FA1 = 1;
    }
    if (cacheconfig.L2waysize == 0)
    {
        cacheconfig.L2waysize = cacheconfig.L2size * 1024/ cacheconfig.L2blocksize;
        FA2 = 1;
    }
    // Implement by you:
    // initialize the hirearch cache system with those configs
    // probably you may define a Cache class for L1 and L2, or any data structure you like
    cache L1;

    int tempy = cacheconfig.L1size * 1024 / cacheconfig.L1waysize;
    L1.tag.resize(tempy);
    L1.valid.resize(tempy);
    L1.pointer.resize(tempy);
    for (int i = 0; i < L1.tag.size(); i++)
    {
        L1.tag[i].resize(cacheconfig.L1waysize, "t");
        L1.valid[i].resize(cacheconfig.L1waysize, 0);
        L1.pointer[i] = 0;
    }

    cache L2;
    tempy = cacheconfig.L2size * 1024 / cacheconfig.L2waysize;
    L2.tag.resize(tempy);
    L2.valid.resize(tempy);
    L2.pointer.resize(tempy);
    for (int i = 0; i < L2.tag.size(); i++)
    {
        L2.tag[i].resize(cacheconfig.L2waysize, "t");
        L2.valid[i].resize(cacheconfig.L2waysize, 0);
        L2.pointer[i] = 0;
    }
    cout <<"L1 index size "<< L1.tag.size() << " L1 Way size " << L1.tag[0].size() << " L2 index size" << L2.tag.size() << " L2 way size" << L2.tag[0].size() << endl;

    int L1AcceState = 0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
    int L2AcceState = 0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;

    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    traces.open(argv[2]);
    //outname = string("trace.txt") + ".out";
    //traces.open("trace.txt");
    tracesout.open(outname.c_str());

    string line;
    string accesstype;     // the Read/Write access type from the memory trace;
    string xaddr;          // the address from the memory trace store in hex;
    unsigned int addr;     // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;

    int blockbit1 = log2(cacheconfig.L1blocksize);
    int indexbit1 = log2(cacheconfig.L1size * 1024 / cacheconfig.L1blocksize / cacheconfig.L1waysize);
    int blockbit2 = log2(cacheconfig.L2blocksize);
    int indexbit2 = log2(cacheconfig.L2size * 1024 / cacheconfig.L2blocksize / cacheconfig.L2waysize);
    int waysize1 = cacheconfig.L1waysize;
    int waysize2 = cacheconfig.L2waysize;
    cout <<"L1 block bit "<< blockbit1 << " L1 index bit" << indexbit1 << " L2 block bit" << blockbit2 << " L2 index bit " << indexbit2 << " L1 way size " << waysize1 << " L2 way size " << waysize2 << endl;
    if (FA1)
    {
        indexbit1 = 0;
    }
    if (FA2)
    {
        indexbit2 = 0;
    }
    int count = 1;
    if (traces.is_open() && tracesout.is_open())
    {
        while (getline(traces, line))
        { // read mem access file and access Cache

            L1AcceState = 0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
            L2AcceState = 0;
            istringstream iss(line);
            if (!(iss >> accesstype >> xaddr))
            {
                break;
            }
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32>(addr);
            cout << "  " << endl;
            cout << "round : " << count << endl;
            count++;
            cout << accesstype << "    " << accessaddr << " " << xaddr << endl;
            // access the L1 and L2 Cache according to the trace;

            string strTag1 = accessaddr.to_string().substr(0, 32 - indexbit1 - blockbit1);
            string strIndex1 = accessaddr.to_string().substr(32 - indexbit1 - blockbit1, indexbit1);
            string strTag2 = accessaddr.to_string().substr(0, 32 - indexbit2 - blockbit2);
            string strIndex2 = accessaddr.to_string().substr(32 - indexbit2 - blockbit2, indexbit2);
            int intIndex1 = bitset<32>(strIndex1).to_ulong();
            int intIndex2 = bitset<32>(strIndex2).to_ulong();
            if (FA1)
            {
                indexbit1 = 0;
            }
            if (FA2)
            {
                intIndex2 = 0;
            }
            int waypointer1 = L1.pointer[intIndex1];
            int waypointer2 = L2.pointer[intIndex2];
            cout << "strTag1 " << strTag1 << " strIndex1 " << strIndex1 << " strTag2 " << strTag2 << " strIndex2 " << strIndex2 << endl;
            cout << "intIndex1 " << intIndex1 << " intIndex2 " << intIndex2 << " waypointer1 " << waypointer1 << " waypointer2 " << waypointer2 << endl;
            
            if (accesstype.compare("R") == 0)
            {
                //Implement by you:
                L1AcceState = 2;
                for (int w = 0; w < waysize1; w++)
                {
                    if (L1.tag[intIndex1][w] == strTag1)
                    {
                        cout << "     L1 - tag match " << strTag1 << "at way "<<w<<endl;
                        if (L1.valid[intIndex1][w])
                        {
                            cout << "         valid - read hit" << endl;
                            L1AcceState = 1;
                            break;
                        }
                    }
                }
                // read access to the L1 Cache,
                //  and then L2 (if required),
                if (L1AcceState == 2)
                { //l1 missed ,check l2
                    L2AcceState = 2;
                    for (int w = 0; w < waysize2; w++)
                    {
                        if (L2.tag[intIndex2][w] == strTag2)
                        {
                            cout << "     L2 - tag match " << strTag2 << "at way "<<w<<endl;
                            if (L2.valid[intIndex2][w])
                            {
                                cout << "         valid - read hit" << endl;
                                L2AcceState = 1;
                                L2.valid[intIndex2][w] = 0;
                                cout << "         marked as unvalid, will move to L1" << endl;
                                break;
                            }
                        }
                    }
                }
                //if L1 missed , move to L1
                if (L1AcceState == 2)
                {
                    cout << "Moving data to L1" << endl;
                    int AvailWayL1 = -1;
                    for (int w = 0; w < waysize1; w++)
                    {
                        if (L1.valid[intIndex1][w] == 0)
                        {
                            cout << "   L1 -  way " << w << " is empty" << endl;
                            AvailWayL1 = w;
                            break;
                        }
                    }

                    if (AvailWayL1 == -1)
                    { //L1 full, evict to l2
                        string tranferaddr = L1.tag[intIndex1][waypointer1] + bitset<32>(intIndex1).to_string().substr(32 - indexbit1, indexbit1) + bitset<32>(0).to_string().substr(0, blockbit1);
                        string strTag22 = tranferaddr.substr(0, 32 - indexbit2 - blockbit2);
                        string strIndex22 = tranferaddr.substr(32 - indexbit2 - blockbit2, indexbit2);
                        int intIndex22 = bitset<32>(strIndex2).to_ulong();
                        if (FA2)
                        {
                            intIndex22 = 0;
                        }
                        int waypointer22 = L2.pointer[intIndex22];
                        

                        cout << "    L1 full, evict to L2 transfer addr: " << tranferaddr << endl;
                        int AvailWayL2 = -1;
                        for (int w = 0; w < waysize2; w++)
                        {
                            //cout << "       checking way : " << w << endl;
                            if (L2.valid[intIndex22][w] == 0)
                            {
                                cout << "        L2 -  way " << w << " is empty" << endl;
                                AvailWayL2 = w;
                                break;
                            }
                        }
                        if (AvailWayL2 == -1)
                        {
                            cout << "    L2 full, evict" << endl;
                            //////
                            //ignore lower level
                            //////
                            //point to evict space
                            AvailWayL2 = L2.pointer[intIndex22];
                            cout << "    L2 point to" << AvailWayL2 << endl;
                            L2.pointer[intIndex22]++;
                            if (L2.pointer[intIndex22] >= waysize2)
                            {
                                cout << "    L2 pointer in index " << intIndex22 << " reset to 0 from" << L2.pointer[intIndex22] << endl;
                                L2.pointer[intIndex22] = 0;
                            }
                        }
                        // save to L2
                        L2.tag[intIndex22][AvailWayL2] = strTag22;
                        L2.valid[intIndex22][AvailWayL2] = 1;
                        cout << "    WRITE L2  index: " << intIndex22 << "  way: " << AvailWayL2 << "  tag : " << strTag22 << endl;
                        //point to evict space
                        AvailWayL1 = L1.pointer[intIndex1];
                        cout << "L1 will write to " << AvailWayL1 << endl;
                        L1.pointer[intIndex1]++;
                        if (L1.pointer[intIndex1] >= waysize1)
                        {
                            cout << "L1 pointer in index " << intIndex1 << " reset to 0 from" << L1.pointer[intIndex1] << endl;
                            L1.pointer[intIndex1] = 0;
                        }
                    }
                    //save to L1
                    L1.tag[intIndex1][AvailWayL1] = strTag1;
                    L1.valid[intIndex1][AvailWayL1] = 1;
                    cout << "WRITE L1  index: " << intIndex1 << "  way: " << AvailWayL1 << "  tag : " << strTag1 << endl;
                }

                //  update the L1 and L2 access state variable;
            }
            else
            {
                //Implement by you:
                // write access to the L1 Cache,
                L1AcceState = 4;

                for (int w = 0; w < waysize1; w++)
                {
                    if (L1.tag[intIndex1][w] == strTag1)
                    {
                        cout << "     L1 - tag match " << strTag1 << "at way "<<w<<endl;
                        if (L1.valid[intIndex1][w])
                        {
                            L1AcceState = 3;
                            cout << "         valid - write hit" << endl;
                            //L1.dirty[intIndex1][w] = 1;
                            break;
                        }
                    }
                }
                //and then L2 (if required),
                if (L1AcceState == 4)
                { //l1 missed ,check l2
                    L2AcceState = 4;
                    for (int w = 0; w < waysize2; w++)
                    {
                        if (L2.tag[intIndex2][w] == strTag2)
                        {
                            cout << "     L2 - tag match " << strTag2 << "at way "<<w<<endl;
                            if (L2.valid[intIndex2][w])
                            {
                                L2AcceState = 3;
                                cout << "         valid - write hit" << endl;
                                //L2.dirty[intIndex2][w] = 1;
                                break;
                            }
                        }
                    }
                }
                //update the L1 and L2 access state variable;
            }
            tracesout << L1AcceState << " " << L2AcceState << endl;
            cout << L1AcceState << " " << L2AcceState << endl; // Output hit/miss results for L1 and L2 to the output file;
        }
        traces.close();
        tracesout.close();
    }
    else
        cout << "Unable to open trace or traceout file ";

    return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
using namespace std;
bitset<32> bitadd(bitset<32> a, bitset<32> b)
{
    bitset<32> re = a;
    bitset<32> temp = b;
    bitset<32> newre;
    bitset<32> newtemp;
    do
    {
        newre = re ^ temp;
        newtemp = re & temp;
        newtemp <<= 1;
        re = newre;
        temp = newtemp;
    } while (temp.any());
    return re;
}

bitset<32> signextimm(bitset<16> ori)
{
    int msb = ori.to_string().substr(0, 1)=="1"?-1:0;
    bitset<32> res = bitset<32>(msb);
    res <<= 16;
    res = bitadd(res, bitset<32>(ori.to_string()));
    cout<<ori<<":res after immextend : "<<res<<endl;
    return res;
}


int main(){
    signextimm(bitset<16>(-1));

    return 0;
}
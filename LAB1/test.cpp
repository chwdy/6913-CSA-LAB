#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>

using namespace std;
int main()
{
bitset<32> b;
    unsigned int a;
    // printf("%s \n", bitset<32>("11111111111111111111111111111").to_string("0", "1"));
    // std::cout << bitset<32>(0b1111111111111111111111).to_string("0", "1") << "\n";
    // std::bitset<8> b(42);
    // std::cout << b.to_string() << '\n'
    //           << b.to_string('*') << '\n'
    //           << b.to_string('O', 'X') << '\n';

    

    b.set();
    b.set(15,false);
    b.set(31,false);
    a = bitset<22>(b.to_string().substr(11, 22)).to_ulong();
    cout << b.to_string().substr(1,3) <<endl;

    cout << b.to_string() << endl;
    cout << a << endl;
    cout << b.to_string().substr(11, 22) <<endl;

    b.reset();
    a = bitset<22>(b.to_string().substr(11, 22)).to_ulong();

    cout << b.to_string() << endl;
    cout << a << endl;
    return 0;
}

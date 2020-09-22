#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>

using namespace std;

#define ADDU 1
#define SUBU 3
#define AND 4
#define OR 5
#define NOR 7

// Memory size.
// In reality, the memory size should be 2^32, but for this lab and space reasons,
// we keep it as this large number, but the memory is still 32-bit addressable.
#define MemSize 65536
bitset<32> bitadd(bitset<32> a, bitset<32> b);

//////////////////
/*
TODO
branch should not be iswrite==1


*/////////////////////


class RF
{
public:
  bitset<32> ReadData1, ReadData2;
  RF()
  {
    Registers.resize(32);
    Registers[0] = bitset<32>(0);
  }

  void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
  {
    // TODO: implement!
    ReadData1 = Registers[RdReg1.to_ullong()];
    ReadData2 = Registers[RdReg2.to_ullong()];
    if(WrtEnable.to_ullong()){
      cout << "RFwrite pos" << WrtReg.to_ullong() <<" with data: "<<WrtData << endl;
      Registers[WrtReg.to_ullong()] = WrtData;
    }
  }

  void OutputRF()
  {
    ofstream rfout;
    rfout.open("RFresult.txt", std::ios_base::app);
    if (rfout.is_open())
    {
      rfout << "A state of RF:" << endl;
      for (int j = 0; j < 32; j++)
      {
        rfout << Registers[j] << endl;
      }
    }
    else
      cout << "Unable to open file";
    rfout.close();
  }

private:
  vector<bitset<32>> Registers;
};

class ALU
{
public:
  bitset<32> ALUresult;
  bitset<32> ALUOperation(bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
  {
    // TODO: implement!
    cout<< "ALU | opcode: " << ALUOP << " | oprand1: " << oprand1.to_ulong() <<" | oprand2:" << oprand2.to_ulong() << endl;
    switch(ALUOP.to_ulong())
    {
    // case 0: //add
    //   ALUresult = bitadd(oprand1,oprand2) ;
    //   break;
    case ADDU: //addu
      cout << "ALU | addu : " << oprand1 << " & " << oprand2 << endl;
      ALUresult = bitadd(oprand1,oprand2) ;
      break;
    case SUBU://sub
      cout << "ALU | subu : " << oprand1 << " & " << oprand2 << endl;
      bitset<32> temp = bitadd(~(oprand2),1);
      ALUresult = bitadd(oprand1,temp) ;
      break;
    // case 3://subu
    //   /* code */
    //   break;
    case AND://and
      ALUresult = oprand1 & oprand2 ;
      break;
    case OR://or
      ALUresult = oprand1 | oprand2 ;
      break;
    case 6://xor
      ALUresult = oprand1 ^ oprand2 ;
      break;
    case NOR://nor
      ALUresult = ~(oprand1 | oprand2) ;
      break;
    default:
    cout << "ALU | unsupport ALU operation : " << ALUOP.to_string() <<endl;
      break;
    }
    return ALUresult;
  }
};

class INSMem
{
public:
  bitset<32> Instruction;
  INSMem()
  {
    IMem.resize(MemSize);
    ifstream imem;
    string line;
    int i = 0;
    imem.open("imem2.txt");
    if (imem.is_open())
    {
      while (getline(imem, line))
      {
        IMem[i] = bitset<8>(line);
        i++;
      }
    }
    else
      cout << "Unable to open file";
    imem.close();
  }

  bitset<32> ReadMemory(bitset<32> ReadAddress)
  {
    Instruction = bitset<32>(0);
    bitset<32> addrptr = bitset<32>(ReadAddress.to_ullong());
    printf("Read INS memory | addr:%llu\n", Instruction.to_ullong(), addrptr.to_ullong());
    Instruction = bitadd(Instruction, bitset<32>(IMem[addrptr.to_ulong()].to_string()));

    Instruction <<= 8;
    addrptr = bitadd(addrptr, bitset<32>(1));
    Instruction = bitadd(Instruction, bitset<32>(IMem[addrptr.to_ulong()].to_string()));

    Instruction <<= 8;
    addrptr = bitadd(addrptr, bitset<32>(1));
    Instruction = bitadd(Instruction, bitset<32>(IMem[addrptr.to_ulong()].to_string()));

    Instruction <<= 8;
    addrptr = bitadd(addrptr, bitset<32>(1));
    Instruction = bitadd(Instruction, bitset<32>(IMem[addrptr.to_ulong()].to_string()));

    return Instruction;
  }

private:
  vector<bitset<8>> IMem;
};

class DataMem
{
public:
  bitset<32> readdata;
  DataMem()
  {
    DMem.resize(MemSize);
    ifstream dmem;
    string line;
    int i = 0;
    dmem.open("dmem.txt");
    if (dmem.is_open())
    {
      while (getline(dmem, line))
      {
        DMem[i] = bitset<8>(line);
        i++;
      }
    }
    else
      cout << "Unable to open file";
    dmem.close();
  }
  bitset<32> MemoryAccess(bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
  {
    int adrptr;
    readdata = bitset<32> (0);
    if (readmem.to_ullong()){
      adrptr = Address.to_ullong();
      readdata = bitadd(readdata, bitset<32>(DMem[adrptr].to_string()));
      readdata <<=8;
      adrptr +=1;
      readdata = bitadd(readdata, bitset<32>(DMem[adrptr].to_string()));
      readdata <<=8;
      adrptr +=1;
      readdata = bitadd(readdata, bitset<32>(DMem[adrptr].to_string()));
      readdata <<=8;
      adrptr +=1;
      readdata = bitadd(readdata, bitset<32>(DMem[adrptr].to_string()));
      cout << "Memory read | data check after read:" << readdata << endl;
    }
    if (writemem.to_ullong()){
      adrptr = Address.to_ullong();
      cout << "Memory write | Pos: " << adrptr <<" | data: "<<WriteData << endl;
      DMem[adrptr] = bitset<8>(WriteData.to_string().substr(0,8));
      DMem[adrptr+1] = bitset<8>(WriteData.to_string().substr(8,8));
      DMem[adrptr+2] = bitset<8>(WriteData.to_string().substr(16,8));
      DMem[adrptr+3] = bitset<8>(WriteData.to_string().substr(24,8));
      cout<<"Memory write | Wrote in data integrity check:" <<DMem[adrptr]<<DMem[adrptr+1]<<DMem[adrptr+2]<<DMem[adrptr+3]<<endl;
    }   
    
    return readdata;
  }

  void OutputDataMem()
  {
    ofstream dmemout;
    dmemout.open("dmemresult.txt");
    if (dmemout.is_open())
    {
      for (int j = 0; j < 1000; j++)
      {
        dmemout << DMem[j] << endl;
      }
    }
    else
      cout << "Unable to open file";
    dmemout.close();
  }

private:
  vector<bitset<8>> DMem;
};

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

bitset<32> signextimm(bitset<16> ori){
  bitset<32>res = bitset<32>(ori.to_string().substr(0,1));
  res <<=16;
  res = bitadd(res,bitset<32>(ori.to_string()));
return res;
}

int main()
{
  RF myRF = RF();
  ALU myALU;
  INSMem myInsMem = INSMem();
  DataMem myDataMem = DataMem();
  bitset<32> PC = bitset<32>(0);
  bitset<32> concat = bitset<32> (0);
  bitset<32> adder = bitset<32>(0);// jump on branch
  int count = 0;
  bool isLoad;
  bool isStore;
  bool Itype;
  bitset<3> ALUOp;
  bool Wrtenable;
  bool Jtype;
  bool isBranch;
  bool isEq;
  string strins;
  //debug
  int looplimit = 30;

  while (1)
  {
    // Fetch
    std::bitset<32> ins = myInsMem.ReadMemory(PC);
    count++;
    cout << "Round:" << count << " PC: " << PC.to_ullong() << "|||  " <<  ins << "\n";
    if (ins == bitset<32>(0b11111111111111111111111111111111) || count > looplimit)
    {
      if (count > looplimit){
        cout << "exit because loop reach limits : " << looplimit << endl;
      }
      break;
    }
    // decode(Read RF)
    strins = ins.to_string();
    isLoad = strins.substr(0, 6) == "100011" ? true : false;
    cout << "strins.substr(0, 6):" <<  strins.substr(0, 6) <<endl;
    isStore = strins.substr(0, 6) == "101011" ? true : false;
    Itype = (strins.substr(0, 6) != "000000" && strins.substr(0, 5) != "00001") ? true : false;
    Jtype = strins.substr(0, 6) == "000010" ? true : false;
    isBranch = strins.substr(0, 6) == "000100" ? true : false;
    if (isLoad || isStore)
    {
      ALUOp = bitset<3>("001");
    }
    else if (strins.substr(0, 6) == "000000")
    {
      ALUOp = bitset<3>(strins.substr(29, 3));
    }
    else
    {
      ALUOp = bitset<3>(strins.substr(3, 3));
    }
    Wrtenable = (isStore || isBranch || Jtype) ? false : true;
    if (Itype)
    {
      myRF.ReadWrite(bitset<5>(strins.substr(6, 5)), bitset<5>(strins.substr(11, 5)), bitset<5>(strins.substr(11, 5)), bitset<32>(0),Wrtenable);
    }
    else if (Jtype){

    }else
    {//rtype
      myRF.ReadWrite(bitset<5>(strins.substr(6, 5)), bitset<5>(strins.substr(11, 5)), bitset<5>(strins.substr(16, 5)), bitset<32>(0), bitset<1>(0));
    }
    concat = bitset<32>(
      bitset<4>(
        bitadd(PC,bitset<32>(4)).to_string().substr(0,4)
        ).to_string()//4bit
      +strins.substr(6, 26)+"00");
    //After RF
    adder = bitset<32>(
      signextimm(
        bitset<16>(strins.substr(16,16))
        ).to_string().substr(2,30)
       +"00");
    isEq = myRF.ReadData1 == myRF.ReadData2;
    if(isBranch && isEq){
      cout << "branch to: " << adder.to_ullong()<< endl;
      PC = adder;
    }else if(Jtype){
      cout << "jump to: " << concat.to_ullong()<<endl;
      PC = concat;
    }else{
      PC = bitadd(PC,bitset<32>(4));
    }


    // Execute
    myALU.ALUOperation(ALUOp, myRF.ReadData1,
     Itype? signextimm(bitset<16>(strins.substr(16,32))) :myRF.ReadData2
     );
    // Read/Write Mem
    myDataMem.MemoryAccess(myALU.ALUresult,myRF.ReadData2,isLoad,isStore);
    // Write back to RF
     if (Itype)
    {
      cout << "i type writeback stage | store:" <<isStore<< " wrtenable: "<< Wrtenable<<" || data:"<<myDataMem.readdata<<endl;
      myRF.ReadWrite(bitset<5>(strins.substr(6, 5)), bitset<5>(strins.substr(11, 5)), bitset<5>(strins.substr(11, 5)),
      isLoad? myDataMem.readdata: myALU.ALUresult,
      Wrtenable);
    }
    else if (Jtype){

    }else
    {//rtype
      myRF.ReadWrite(bitset<5>(strins.substr(6, 5)), bitset<5>(strins.substr(11, 5)), bitset<5>(strins.substr(16,5)), myALU.ALUresult, bitset<1>(1));
    }
    myRF.OutputRF(); // dump RF;
    cout <<"    "<<endl;
  }
  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}

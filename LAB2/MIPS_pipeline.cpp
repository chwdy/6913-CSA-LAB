#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct
{
    bitset<32> PC;
    bool nop;
};

struct IDStruct
{
    bitset<32> Instr;
    bool nop;
};

struct EXStruct
{
    bitset<32> Read_data1;
    bitset<32> Read_data2;
    bitset<16> Imm;
    bitset<5> Rs;
    bitset<5> Rt;
    bitset<5> Wrt_reg_addr;
    bool is_I_type;
    bool rd_mem;
    bool wrt_mem;
    bool alu_op; //1 for addu, lw, sw, 0 for subu
    bool wrt_enable;
    bool nop;
};

struct MEMStruct
{
    bitset<32> ALUresult;
    bitset<32> Store_data;
    bitset<5> Rs;
    bitset<5> Rt;
    bitset<5> Wrt_reg_addr;
    bool rd_mem;
    bool wrt_mem;
    bool wrt_enable;
    bool nop;
};

struct WBStruct
{
    bitset<32> Wrt_data;
    bitset<5> Rs;
    bitset<5> Rt;
    bitset<5> Wrt_reg_addr;
    bool wrt_enable;
    bool nop;
};

struct stateStruct
{
    IFStruct IF;
    IDStruct ID;
    EXStruct EX;
    MEMStruct MEM;
    WBStruct WB;
};

class RF
{
public:
    bitset<32> Reg_data;
    RF()
    {
        Registers.resize(32);
        Registers[0] = bitset<32>(0);
    }

    bitset<32> readRF(bitset<5> Reg_addr)
    {
        Reg_data = Registers[Reg_addr.to_ulong()];
        return Reg_data;
    }

    void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
    {
        Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
    }

    void outputRF()
    {
        ofstream rfout;
        rfout.open("RFresult.txt", std::ios_base::app);
        if (rfout.is_open())
        {
            rfout << "State of RF:\t" << endl;
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
        imem.open("imem.txt");
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

    bitset<32> readInstr(bitset<32> ReadAddress)
    {
        string insmem;
        insmem.append(IMem[ReadAddress.to_ulong()].to_string());
        insmem.append(IMem[ReadAddress.to_ulong() + 1].to_string());
        insmem.append(IMem[ReadAddress.to_ulong() + 2].to_string());
        insmem.append(IMem[ReadAddress.to_ulong() + 3].to_string());
        Instruction = bitset<32>(insmem); //read instruction memory
        return Instruction;
    }

private:
    vector<bitset<8>> IMem;
};

class DataMem
{
public:
    bitset<32> ReadData;
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

    bitset<32> readDataMem(bitset<32> Address)
    {
        string datamem;
        datamem.append(DMem[Address.to_ulong()].to_string());
        datamem.append(DMem[Address.to_ulong() + 1].to_string());
        datamem.append(DMem[Address.to_ulong() + 2].to_string());
        datamem.append(DMem[Address.to_ulong() + 3].to_string());
        ReadData = bitset<32>(datamem); //read data memory
        return ReadData;
    }

    void writeDataMem(bitset<32> Address, bitset<32> WriteData)
    {
        DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0, 8));
        DMem[Address.to_ulong() + 1] = bitset<8>(WriteData.to_string().substr(8, 8));
        DMem[Address.to_ulong() + 2] = bitset<8>(WriteData.to_string().substr(16, 8));
        DMem[Address.to_ulong() + 3] = bitset<8>(WriteData.to_string().substr(24, 8));
    }

    void outputDataMem()
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

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate << "State after executing cycle:\t" << cycle << endl;

        printstate << "IF.PC:\t" << state.IF.PC.to_ulong() << endl;
        printstate << "IF.nop:\t" << state.IF.nop << endl;

        printstate << "ID.Instr:\t" << state.ID.Instr << endl;
        printstate << "ID.nop:\t" << state.ID.nop << endl;

        printstate << "EX.Read_data1:\t" << state.EX.Read_data1 << endl;
        printstate << "EX.Read_data2:\t" << state.EX.Read_data2 << endl;
        printstate << "EX.Imm:\t" << state.EX.Imm << endl;
        printstate << "EX.Rs:\t" << state.EX.Rs << endl;
        printstate << "EX.Rt:\t" << state.EX.Rt << endl;
        printstate << "EX.Wrt_reg_addr:\t" << state.EX.Wrt_reg_addr << endl;
        printstate << "EX.is_I_type:\t" << state.EX.is_I_type << endl;
        printstate << "EX.rd_mem:\t" << state.EX.rd_mem << endl;
        printstate << "EX.wrt_mem:\t" << state.EX.wrt_mem << endl;
        printstate << "EX.alu_op:\t" << state.EX.alu_op << endl;
        printstate << "EX.wrt_enable:\t" << state.EX.wrt_enable << endl;
        printstate << "EX.nop:\t" << state.EX.nop << endl;

        printstate << "MEM.ALUresult:\t" << state.MEM.ALUresult << endl;
        printstate << "MEM.Store_data:\t" << state.MEM.Store_data << endl;
        printstate << "MEM.Rs:\t" << state.MEM.Rs << endl;
        printstate << "MEM.Rt:\t" << state.MEM.Rt << endl;
        printstate << "MEM.Wrt_reg_addr:\t" << state.MEM.Wrt_reg_addr << endl;
        printstate << "MEM.rd_mem:\t" << state.MEM.rd_mem << endl;
        printstate << "MEM.wrt_mem:\t" << state.MEM.wrt_mem << endl;
        printstate << "MEM.wrt_enable:\t" << state.MEM.wrt_enable << endl;
        printstate << "MEM.nop:\t" << state.MEM.nop << endl;

        printstate << "WB.Wrt_data:\t" << state.WB.Wrt_data << endl;
        printstate << "WB.Rs:\t" << state.WB.Rs << endl;
        printstate << "WB.Rt:\t" << state.WB.Rt << endl;
        printstate << "WB.Wrt_reg_addr:\t" << state.WB.Wrt_reg_addr << endl;
        printstate << "WB.wrt_enable:\t" << state.WB.wrt_enable << endl;
        printstate << "WB.nop:\t" << state.WB.nop << endl;
    }
    else
        cout << "Unable to open file";
    printstate.close();
}

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
    bitset<32> res = bitset<32>(ori.to_string().substr(0, 1));
    res <<= 16;
    res = bitadd(res, bitset<32>(ori.to_string()));
    return res;
}

int main()
{

    RF myRF = RF();
    INSMem myInsMem = INSMem();
    DataMem myDataMem = DataMem();
    stateStruct state;
    state.IF.nop = 0;
    state.ID.nop = 1;
    state.EX.nop = 1;
    state.MEM.nop = 1;
    state.WB.nop = 1;
    state.IF.PC = bitset<32>(0);
    bitset<32> ins;
    string strins;
    bool isLoad;
    bool isStore;
    bool Itype;
    bitset<3> ALUOp;
    bool Jtype;
    bool isBranch;

    bool Wrtenable;

    int cycle = 0;
    bool halt = false;
    int looplimit = 60; //for testting
    while (1)
    {
        stateStruct newState;

        /* --------------------- WB stage --------------------- */
        if (state.WB.nop)
        { // do nothing
        }
        else
        {
            if (state.WB.wrt_enable)
            {
                myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
            }
        }

        /* --------------------- MEM stage --------------------- */
        if (state.MEM.nop)
        {
        }
        else
        {
            if (state.MEM.rd_mem)
            {
                myDataMem.readDataMem(state.MEM.ALUresult);
            }
            if (state.MEM.wrt_mem)
            {
                myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
            }
            //TODO: need tp determine which data should pass: alu result to RF / mem result to
            newState.WB.Wrt_data = myDataMem.ReadData;
            newState.WB.Rs = state.MEM.Rs;
            newState.WB.Rt = state.MEM.Rt;
            newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
            newState.WB.wrt_enable = state.MEM.wrt_enable;
        }
        newState.WB.nop = state.MEM.nop;

        /* --------------------- EX stage --------------------- */

        if (state.EX.nop)
        {
        }
        else
        {
            bitset<32> oprand2 = state.EX.is_I_type ? signextimm(state.EX.Imm) : state.EX.Read_data2;
            bitset<32> oprand1 = state.EX.Read_data1;

            if (state.EX.alu_op)
            {
                cout << "ALU | addu : " << oprand1 << " & " << oprand2 << endl;
                newState.MEM.ALUresult = bitadd(oprand1, oprand2);
            }
            else
            {
                cout << "ALU | subu : " << oprand1 << " & " << oprand2 << endl;
                bitset<32> temp = bitadd(~(oprand2), 1);
                newState.MEM.ALUresult = bitadd(oprand1, temp);
            }
            newState.MEM.Store_data = state.EX.Read_data2;
            newState.MEM.Rs = state.EX.Rs;
            newState.MEM.Rt = state.EX.Rt;
            newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            newState.MEM.wrt_enable = state.EX.wrt_enable;
            newState.MEM.rd_mem = state.EX.rd_mem;
            newState.MEM.wrt_mem = state.EX.wrt_mem;
        }
        newState.MEM.nop = state.EX.nop;

        /* --------------------- ID stage --------------------- */
        if (state.ID.nop)
        {
        }
        else
        {

            strins = state.ID.Instr.to_string();
            isLoad = strins.substr(0, 6) == "100011" ? true : false;
            cout << "strins.substr(0, 6):" << strins.substr(0, 6) << endl;
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
                newState.EX.Wrt_reg_addr = bitset<5>(strins.substr(11, 5));
                newState.EX.alu_op = true;
                newState.EX.is_I_type = true;
            }
            else if (Jtype)
            {
            }
            else
            { //rtype
                newState.EX.Wrt_reg_addr = bitset<5>(strins.substr(16, 5));
                newState.EX.alu_op = ALUOp.to_ulong() == 1;
                newState.EX.is_I_type = false;
            }
            newState.EX.Read_data1 = myRF.readRF(bitset<5>(strins.substr(6, 5)));
            newState.EX.Read_data2 = myRF.readRF(bitset<5>(strins.substr(11, 5)));
            newState.EX.Rs = bitset<5>(strins.substr(6, 5));
            newState.EX.Rt = bitset<5>(strins.substr(11, 5));
            newState.EX.wrt_enable = Wrtenable;
            newState.EX.rd_mem = isLoad;
            newState.EX.wrt_mem = isStore;
            newState.EX.Imm = bitset<16>(strins.substr(16, 32));
        }
        newState.EX.nop = state.ID.nop;
        /* --------------------- IF stage --------------------- */

        if (state.IF.nop)
        {
        }
        else
        {
            myInsMem.readInstr(bitset<32>(state.IF.PC));
            cout << myInsMem.Instruction.to_string() << endl;

            if (myInsMem.Instruction == bitset<32>(-1) || cycle > looplimit)
            {
                if (cycle > looplimit)
                {
                    cout << "exit because loop reach limits : " << looplimit << endl;
                }
                halt = true;
            }
            newState.ID.Instr = myInsMem.Instruction;
        }

        if (halt)
        {
            newState.IF.PC = state.IF.PC;
            newState.ID.nop = 1;
            newState.IF.nop = 1;
        }
        else
        {
            newState.ID.nop = state.IF.nop;
            newState.IF.PC = bitadd(state.IF.PC, bitset<32>(4));
            newState.IF.nop = 0;
        }

        ////

        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
        {
            break;
        }

        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ...
        cycle++;
        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */
    }

    myRF.outputRF();           // dump RF;
    myDataMem.outputDataMem(); // dump data mem

    return 0;
}
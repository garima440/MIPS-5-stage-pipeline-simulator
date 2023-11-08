#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;

#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab csa23, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;  

    IFStruct()
    {
        PC = 0;
        nop = 0;
    }
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  

    IDStruct()
    {
        Instr = 0;
        nop = 1;
    }
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  

    EXStruct()
    {
        Read_data1 = 0;
        Read_data2 = 0;
        Imm = 0;
        Rs = 0;
        Rt = 0;
        Wrt_reg_addr = 0;
        is_I_type = 0;
        rd_mem = 0;
        wrt_mem = 0; 
        alu_op = 1;     
        wrt_enable = 0;
        nop = 1;  
    }
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;   
    bool        nop;    

    MEMStruct()
    {
        ALUresult = 0;
        Store_data = 0;
        Rs = 0;
        Rt = 0;
        Wrt_reg_addr = 0;
        rd_mem = 0;
        wrt_mem = 0;
        wrt_enable = 0;
        nop = 1;
    }
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop; 

    WBStruct()
    {
        Wrt_data = 0;
        Rs = 0;
        Rt =  0;
        Wrt_reg_addr = 0;
        wrt_enable = 0;
        nop = 1;
    }    
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
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
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
				for (int j = 0; j<32; j++)
				{        
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;	
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
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(line);
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
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
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();               
        }             
      
    private:
		vector<bitset<8> > DMem;      
};  

int32_t signExtend(bitset <16> immediate)
{
    string immBits = immediate.to_string();
    bitset <32> bits;
    int32_t signExtendedValue;
    if (immediate[0] == 0)
    {
        bits = bitset <32> ("0000000000000000" + immBits);
        signExtendedValue = bits.to_ulong();
    }
    else
    {
        bits = bitset <32> ("1111111111111111" + immBits);
        signExtendedValue = bits.to_ulong();
    }
    return signExtendedValue;
}

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}
 

int main()
{
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;

    stateStruct state;
    stateStruct newState = state;

    int cycle = 0;
    
    bitset <32> currInstruction;
    bitset <32> extendedImm;
			
             
    while (1) {

        /* --------------------- WB stage --------------------- */

        if (state.WB.nop == 0)
        {
            if (state.WB.wrt_enable == 1)
            {
                myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
            }
        }

        /* --------------------- MEM stage --------------------- */
        if(state.MEM.nop == 0)
        {      
            if ((state.WB.Wrt_reg_addr == state.MEM.Rt) && (state.WB.nop == 0)) // MEM to MEM forward
			{
				state.MEM.Store_data = state.WB.Wrt_data;
			}   

            if (state.MEM.rd_mem == 1)
			{
				newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
			}
			else
			{
				newState.WB.Wrt_data = state.MEM.ALUresult;
			}

			if (state.MEM.wrt_mem == 1)//store data
			{
				myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
				newState.WB.Wrt_data = state.WB.Wrt_data;
			}

			// update next state
			newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
			newState.WB.wrt_enable = state.MEM.wrt_enable;
			newState.WB.Rs = state.MEM.Rs;
			newState.WB.Rt = state.MEM.Rt;
			newState.WB.nop = state.MEM.nop;
		}
        else
        {
            newState.WB.nop = state.MEM.nop; 
        }

        /* --------------------- EX stage --------------------- */
        if (state.EX.nop == 0)
        {
            if (state.EX.wrt_enable == 1 && state.EX.is_I_type == 0 && state.EX.alu_op == 1)// add
			{
				if (state.WB.Wrt_reg_addr == state.EX.Rs && state.WB.nop == 0 && state.WB.wrt_enable == 1)// MEM to EX forward
				{
					state.EX.Read_data1 = state.WB.Wrt_data;
				}
				if (state.WB.Wrt_reg_addr == state.EX.Rt && state.WB.nop == 0 && state.WB.wrt_enable == 1)// MEM to EX forward
				{
					state.EX.Read_data2 = state.WB.Wrt_data;
				}
				if (state.MEM.Wrt_reg_addr == state.EX.Rs && state.MEM.nop == 0 && state.MEM.wrt_enable == 1)// EX to EX forward (should be behind MEM to EX forward)
				{
					state.EX.Read_data1 = state.MEM.ALUresult;
				}
				if (state.MEM.Wrt_reg_addr == state.EX.Rt && state.MEM.nop == 0 && state.MEM.wrt_enable == 1)// EX to EX forward
				{
					state.EX.Read_data2 = state.MEM.ALUresult;
				}

				newState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong());
			}
			else if (state.EX.wrt_enable == 1 && state.EX.is_I_type == 0 && state.EX.alu_op == 0)// sub
			{
				if (state.WB.Wrt_reg_addr == state.EX.Rs && state.WB.nop == 0 && state.WB.wrt_enable == 1)// MEM to EX forward
				{
					state.EX.Read_data1 = state.WB.Wrt_data;
				}
				if (state.WB.Wrt_reg_addr == state.EX.Rt && state.WB.nop == 0 && state.WB.wrt_enable == 1)// MEM to EX forward
				{
					state.EX.Read_data2 = state.WB.Wrt_data;
				}
				if (state.MEM.Wrt_reg_addr == state.EX.Rs && state.MEM.nop == 0 && state.MEM.wrt_enable == 1)// EX to EX forward (should be behind MEM to EX forward)
				{
					state.EX.Read_data1 = state.MEM.ALUresult;
				}
				if (state.MEM.Wrt_reg_addr == state.EX.Rt && state.MEM.nop == 0 && state.MEM.wrt_enable == 1)// EX to EX forward
				{
					state.EX.Read_data2 = state.MEM.ALUresult;
				}
				
				newState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong());
			}
			else if (state.EX.wrt_enable == 1 && state.EX.is_I_type == 1 && state.EX.alu_op == 1)// load
			{
				if (state.WB.Wrt_reg_addr == state.EX.Rt && state.WB.nop == 0 && state.WB.wrt_enable == 1)// MEM to EX forward, for add/sub to load dependency
				{
					state.EX.Read_data2 = state.WB.Wrt_data;
				}
				if (state.WB.Wrt_reg_addr == state.EX.Rs && state.WB.nop == 0 && state.WB.wrt_enable == 1)
				{
					state.EX.Read_data1 = state.WB.Wrt_data;
				}
				if (state.MEM.Wrt_reg_addr == state.EX.Rs && state.MEM.nop == 0 && state.MEM.wrt_enable == 1)// EX to EX forward (should be behind MEM to EX forward)
				{
					state.EX.Read_data1 = state.MEM.ALUresult;
				}
				if (state.MEM.Wrt_reg_addr == state.EX.Rt && state.MEM.nop == 0 && state.MEM.wrt_enable == 1)// EX to EX forward
				{
					state.EX.Read_data2 = state.MEM.ALUresult;
				}
				
				newState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() + signExtend(state.EX.Imm));
			}
			else if (state.EX.wrt_enable == 0 && state.EX.is_I_type == 1 && state.EX.alu_op == 1)// store
			{
				if (state.WB.Wrt_reg_addr == state.EX.Rt && state.WB.nop == 0 && state.WB.wrt_enable == 1)// MEM to EX forward, for add/sub to store dependency
				{
					state.EX.Read_data2 = state.WB.Wrt_data;
				}
				if (state.WB.Wrt_reg_addr == state.EX.Rs && state.WB.nop == 0 && state.WB.wrt_enable == 1)
				{
					state.EX.Read_data1 = state.WB.Wrt_data;
				}
				if (state.MEM.Wrt_reg_addr == state.EX.Rs && state.MEM.nop == 0 && state.MEM.wrt_enable == 1)// EX to EX forward (should be behind MEM to EX forward)
				{
					state.EX.Read_data1 = state.MEM.ALUresult;
				}
				if (state.MEM.Wrt_reg_addr == state.EX.Rt && state.MEM.nop == 0 && state.MEM.wrt_enable == 1)// EX to EX forward
				{
					state.EX.Read_data2 = state.MEM.ALUresult;
				}
				
				newState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() + signExtend(state.EX.Imm));
			}
            else
            {
                newState.MEM.ALUresult = 0;
            }
            

            newState.MEM.Store_data = state.EX.Read_data2;
            newState.MEM.Rs = state.EX.Rs;
            newState.MEM.Rt = state.EX.Rt;
            newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            newState.MEM.wrt_enable = state.EX.wrt_enable;
            newState.MEM.rd_mem = state.EX.rd_mem;
            newState.MEM.wrt_mem = state.EX.wrt_mem;
            //newState.MEM.is_branch = state.EX.is_branch;
        }
        
        newState.MEM.nop = state.EX.nop;

        /* --------------------- ID stage --------------------- */
        if (state.ID.nop == 0)
        {
            string instString = state.ID.Instr.to_string();
            newState.EX.Rs = bitset <5> (instString.substr(6, 5));
            newState.EX.Rt = bitset <5> (instString.substr(11, 5));
            newState.EX.Read_data1 = myRF.readRF(bitset<5>(instString.substr(6, 5)));
            newState.EX.Read_data2 = myRF.readRF(bitset<5>(instString.substr(11, 5)));
            //newState.EX.nop = state.ID.nop;
            
            // R-type
            if (instString.substr(0, 6) == "000000")
            {
                newState.EX.Wrt_reg_addr = bitset <5> (instString.substr(16, 5));
                newState.EX.is_I_type = 0;
                newState.EX.rd_mem = 0;
                newState.EX.wrt_mem = 0;
                newState.EX.wrt_enable = 1;
                
                // addu
                if (instString.substr(26, 6) == "100001")
                {
                    newState.EX.alu_op = 1;
                }
                
                // subu
                else
                {
                    newState.EX.alu_op = 0;
                }
            }
            
             // I-type
            else
            {
                newState.EX.Imm = bitset <16> (instString.substr(16, 16));
                extendedImm = signExtend(bitset <16> (instString.substr(16, 16)));
                newState.EX.is_I_type = 1;
                newState.EX.Wrt_reg_addr = bitset <5> (instString.substr(11, 5));
                
                // lw
                // Load Word lw I R[rt] = M[R[rs]+SignExtImm]
                if (instString.substr(0,6) == "100011")
                {
                    newState.EX.alu_op = 1;
                    newState.EX.rd_mem = 1;
                    newState.EX.wrt_mem = 0;
                    newState.EX.wrt_enable = 1;
                }
                
                // sw
                // M[R[rs]+SignExtImm] = R[rt]
                if (instString.substr(0,6) == "101011")
                {
                    newState.EX.alu_op = 1;
                    newState.EX.rd_mem = 0;
                    newState.EX.wrt_mem = 1;
                    newState.EX.wrt_enable = 0;
                }
                
                // bne
                if (instString.substr(0,6) == "000101")
                {
                    //cout << "Branch instruction is fetched";
                    newState.EX.alu_op = 0;
                    newState.EX.rd_mem = 0;
                    newState.EX.wrt_mem = 0;
                    newState.EX.wrt_enable = 0;
                    newState.EX.Wrt_reg_addr = 0;

                }
            }
            newState.EX.nop = state.ID.nop;
            //Load-Add stalling 
            if (state.EX.is_I_type == 1 && state.EX.rd_mem == 1)
			{
				if ((state.EX.Rt == bitset <5> ((state.ID.Instr.to_string()).substr(6, 5))) && (state.EX.nop == 0) && (((state.ID.Instr.to_string()).substr(0, 6)) == "000000"))
				{
					newState.EX.nop = 1;
				}
				if ((state.EX.Rt == bitset <5> ((state.ID.Instr.to_string()).substr(11, 5))) && (state.EX.nop == 0) && (((state.ID.Instr.to_string()).substr(0, 6)) == "000000"))
				{
					newState.EX.nop = 1;
				}
			}
			// Load-Store stalling
			if ((state.EX.is_I_type == 1) && (state.EX.rd_mem == 1) && (((state.ID.Instr.to_string()).substr(0, 6)) == "101011"))
			{
				if (state.EX.Rt == bitset <5> ((state.ID.Instr.to_string()).substr(6, 5)))
				{
					newState.EX.nop = 1;
				}
			}
        }
        else
        {
            newState.EX.nop = state.ID.nop;
            //cout << newState.EX.nop << "\n";
        }

        /** --------------------- IF stage --------------------- **/

        if (state.IF.nop == 0) 
        {
            currInstruction = myInsMem.readInstr(state.IF.PC);
            //cout << "Current PC is: " << newState.IF.PC.to_ulong() << "\n";
            //cout << "Current Instuction is: " << currInstruction << "\n\n";
            newState.ID.Instr = currInstruction;
            
            if (currInstruction.to_string() == "11111111111111111111111111111111") // if halt instruction is fetched
            {
                state.IF.nop = 1;
            }
            
            newState.ID.nop = state.IF.nop;
 
            // Load add stalling
            if ((state.EX.is_I_type == 1) && (state.EX.rd_mem == 1))
			{
				if ((state.EX.Rt == bitset <5> ((state.ID.Instr.to_string()).substr(6, 5))) && (state.EX.nop == 0) && (((state.ID.Instr.to_string()).substr(0, 6)) == "000000"))
				{
					newState.ID = state.ID;
				}
				if (state.EX.Rt == bitset <5> ((state.ID.Instr.to_string()).substr(11, 5)) && (state.EX.nop == 0) && (((state.ID.Instr.to_string()).substr(0, 6)) == "000000"))
				{
					newState.ID = state.ID;
				}
			}
			// Load-Store stall
			if ((state.EX.is_I_type == 1) && (state.EX.rd_mem == 1) && (((state.ID.Instr.to_string()).substr(0, 6)) == "101011"))
			{
				if (state.EX.Rt == bitset <5> ((state.ID.Instr.to_string()).substr(6, 5)))
				{
					newState.ID = state.ID;
				}
			}
            
            // bne
            if ((state.ID.Instr.to_string().substr(0, 6) == "000101") && (myRF.readRF(bitset <5> (state.ID.Instr.to_string().substr(6, 5)))) != myRF.readRF(bitset <5> (state.ID.Instr.to_string().substr(11, 5))))
			{
				if (state.ID.nop == 0)
				{
					newState.ID.nop = 1;
				}
			}
			
        }
        else
        {
            newState.ID.nop = state.IF.nop;
        }
        
             
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;


        if (myInsMem.readInstr(state.IF.PC).to_string() != "11111111111111111111111111111111")
		{
            if ((state.ID.Instr.to_string().substr(0, 6) == "000101") && (myRF.readRF(bitset <5> (state.ID.Instr.to_string().substr(6, 5))) != myRF.readRF(bitset <5>(state.ID.Instr.to_string().substr(11, 5)))))
			{
				if (state.ID.nop == 0)
				{
                    //cout << "The PC is: " << state.IF.PC.to_ulong() << "\n";
					newState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + (signExtend(bitset <16> (state.ID.Instr.to_string().substr(16, 16)))) * 4);
				}
				else
				{
					newState.IF.PC = state.IF.PC.to_ulong() + 4;
				}
			}
			else
			{
				newState.IF.PC = state.IF.PC.to_ulong() + 4;
			}   
        }
		newState.IF.nop = state.IF.nop;
        
        // load-Add stalling
        if ((state.EX.is_I_type == 1) && (state.EX.rd_mem == 1))
		{
			if (state.EX.Rt == bitset <5> (state.ID.Instr.to_string().substr(6, 5)) && (state.EX.nop == 0) && (((state.ID.Instr.to_string()).substr(0, 6)) == "000000"))
			{
				newState.IF = state.IF;
			}
			if (state.EX.Rt == bitset <5> (state.ID.Instr.to_string().substr(11, 5)) && (state.EX.nop == 0) && (((state.ID.Instr.to_string()).substr(0, 6)) == "000000"))
			{
				newState.IF = state.IF;
			}
		}
		// Load-Store stall
		if ((state.EX.is_I_type == 1) && (state.EX.rd_mem == 1) && (((state.ID.Instr.to_string()).substr(0, 6)) == "101011"))
		{
			if (state.EX.Rt == bitset <5> ((state.ID.Instr.to_string()).substr(6, 5)))
			{
				newState.IF = state.IF;
			}
		}
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       
        state = newState; /*** The end of the cycle and updates the current state with the values calculated in this cycle. csa23 ***/ 
        cycle++;	
    }
    
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}

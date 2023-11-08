# MIPS-5-stage-pipeline-simulator

Implement a cycle-accurate simulator for a 5-stage pipelined MIPS processor in C++. The simulator supports a subset of the MIPS instruction set and should model the execution of each instruction cycle by cycle. 
An example MIPS program is provided for the simulator as a text file “imem.txt”, which is used to initialize the Instruction Memory. Each line of the file corresponds to a Byte stored in the Instruction Memory in binary format, with the first line at address 0, the next line at address 1, and so on. Four contiguous lines correspond to one whole instruction. Note that the words stored in memory are in “Big-Endian” format, meaning that the most significant byte is stored first. 

The Data Memory is initialized using the “dmem.txt” file. The format of the stored words is the same as the Instruction Memory. As with the instruction memory, the data memory addresses also begin at 0 and increment by one in each line.

The instructions that the simulator supports and their encodings are shown in Table 1. Note that all instructions, except for “halt”, exist in the MIPS ISA. The MIPS Green Sheet defines the semantics of each instruction.

Table 1. Instruction encodings for a reduced MIPS ISA:
Name            Format Type            Opcode (Hex)            Func (Hex)
addu              R-Type                    00                     21
subu              R-Type                    00                     23
lw                I-Type                    23                      -
sw                I-Type                    2B                      - 
bne               I-Type                    05                      -
halt              J-Type                    3F                      -

Pipeline Structure
Your MIPS pipeline has the following 5 stages: 
	•	Fetch (IF): fetches an instruction from the instruction memory. Updates PC. 
	•	Decode (ID/RF): reads from the register RF and generates control signals required in subsequent stages. In addition, branches are resolved in this stage by checking for the branch condition and computing the effective address. 
	•	Execute (EX): performs an ALU operation. 
	•	Memory (MEM): loads or stores a 32-bit word from data memory. 
	•	Writeback (WB): writes back data to the RF. 

Dealing with Hazards:
Your processor must deal with two types of hazards. 
	•	RAW Hazards: RAW hazards are dealt with using either only forwarding (if possible) or, if not, using stalling + forwarding. You must follow the mechanisms described in Lecture to deal with RAW hazards. 
	•	Control Flow Hazards: You will assume that branch conditions are resolved in the ID/RF stage of the pipeline. Your processor deals with bne instructions as follows: 
a)  Branches are always assumed to be NOT TAKEN. That is, when a bne is fetched in the IF stage, the PC is speculatively updated as PC+4. 

b)  Branch conditions are resolved in the ID/RF stage. To make your life easier, we will ensure that every bne instruction has no RAW dependency with its previous two instructions. In other words, you do NOT have to deal with RAW hazards for branches! 

c)  Two operations are performed in the ID/RF stage: (i) Read_data1 and Read_data2 are compared to determine the branch outcome; (ii) the effective branch address is computed. 

d)  If the branch is NOT TAKEN, execution proceeds normally. However, if the branch is TAKEN, the speculatively fetched instruction from PC+4 is quashed in its ID/RF stage using the nop bit and the next instruction is fetched from the effective branch address. Execution now proceeds normally. 

The nop bit:
The nop bit for any stage indicates whether it is performing a valid operation in the current clock cycle. The nop bit for the IF stage is initialized to 0 and for all other stages is initialized to 1. (This is because in the first clock cycle, only the IF stage performs a valid operation.) 
In the absence of hazards, the value of the nop bit for a stage in the current clock cycle is equal to the nop bit of the prior stage in the previous clock cycle. 
However, the nop bit is also used to implement stalls that result from a RAW hazard or to squash speculatively fetched instructions if the branch condition evaluates to TAKEN. See slides for more details on implementing stalls and squashing instructions. 

The HALT Instruction:
The halt instruction is a “custom” instruction we introduced so you know when to stop the simulation. When a HALT instruction is fetched in the IF stage at cycle N, the nop bit of the IF stage in the next clock cycle (cycle N+1) is set to 1 and subsequently stays at 1. The nop bit of the ID/RF stage is set to 1 in cycle N+1 and subsequently stays at 1. The nop bit of the EX stage is set to 1 in cycle N+2 and subsequently stays at 1. The nop bit of the MEM stage is set to 1 in cycle N+3 and subsequently stays at 1. The nop bit of the WB stage is set to 1 in cycle N+4 and subsequently stays at 1. At the end of each clock cycle the simulator checks to see if the nop bit of each stage is 1. If so, the simulation halts. Note that this logic is already implemented in the skeleton code provided to you. 


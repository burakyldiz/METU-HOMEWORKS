from dataclasses import dataclass, field
import subprocess
from typing import Dict, Tuple
import sys
import os
from utils import *

@dataclass
class SimulationResult:
    execfail: bool = False
    type: str = ""
    status: str = ""
    reg_states: Dict[str, int] = field(default_factory=dict)
    mem_states: Dict[int, int] = field(default_factory=dict)
    isa_check: bool = False
    cycles: int = -1
    instructions: int = -1
    cpi: float = -1.0
    
    def get_reg(self, regname):
        return self.reg_states.get(regname, 0)
    
    def get_mem(self, addr):
        return self.mem_states.get(addr, 0)
    
    def pretty_str(self):
        result = ""
        result += "Execfail: " + str(self.execfail) + "\n"
        result += "Type: " + str(self.type) + "\n"
        result += "Status: " + str(self.status) + "\n"
        result += "ISA Check: " + str(self.isa_check) + "\n"
        result += "Cycles: " + str(self.cycles) + "\n"
        result += "Instructions: " + str(self.instructions) + "\n"
        result += "CPI: " + str(self.cpi) + "\n"
        result += "Registers:\n"
        for regname, regval in self.reg_states.items():
            result += f"{regname:3}: {regval:016X}\n"
        result += "Memory:\n"
        for addr, val in self.mem_states.items():
            result += f"{addr:04X}: {val:016X}\n"
        return result.rstrip("\n")
    
    def get_retval(self):
        return self.get_reg("rax")
    
    def terminated_successfully(self):
        return not self.execfail and self.isa_check and self.status == "HLT" and self.instructions > 0 and self.instructions < 10000
    
    def get_memrange(self, begin, n):
        return [self.get_mem(addr) for addr in range(begin,8*n+begin, 8)]
        

def parse_simulator_output(simout, simulator_type):
    result = SimulationResult(execfail=False, type=simulator_type)
    if simulator_type == "yis":
        result.isa_check = True
    result.reg_states = {"rdi": 0, "rsi":0, "rdx":0, "rcx":0, "r8":0, "r9":0, "rax":0, "rbx":0, "rbp":0, "rsp":0, "r10":0, "r11":0, "r12":0, "r13":0, "r14":0}
    simout = simout.strip()
    simout = simout.split("\n")
    if simulator_type == "ssim" and simout[-1].strip().startswith("ISA Check Fails"):
        result.isa_check = False
        return result
    if simulator_type == "psim" and simout[-2].strip().startswith("ISA Check Fails"):
        result.isa_check = False
        return result
    
    if simulator_type == "ssim" or simulator_type == "psim":   
        parsing_registers = False
        parsing_memory = False
        skipping = True
        
        for line in simout:
            line = line.strip()
            if not line:
                continue
            if line.startswith("Changed Register State"):
                parsing_registers = True
                continue
            if line.startswith("Changed Memory State"):
                parsing_registers = False
                parsing_memory = True
                continue
            if line.startswith("ISA Check"):
                result.isa_check = line.split()[2] == "Succeeds"
                parsing_memory = False
                parsing_registers = False
                continue
            
            if parsing_registers:
                words = line.split()
                result.reg_states[words[0][1:-1]] = int(words[-1], 16)
                assert(result.reg_states[words[0][1:-1]] >= 0)
            if parsing_memory:
                words = line.split()
                result.mem_states[int(words[0][:-1], 16)] = int(words[-1], 16)
                assert(result.mem_states[int(words[0][:-1], 16)] >= 0)
                assert(int(words[0][:-1], 16) >= 0)
            
            
            if skipping and not line.endswith("instructions executed"):
                continue
            skipping = False
            
            if line.endswith("instructions executed"):
                result.instructions = int(line.split()[0])
            if line.startswith("Status"):
                result.status = line.split()[2]

            if line.startswith("CPI:"):
                words = line.split()
                result.cycles = int(words[1])
                result.instructions = int(words[2][7:])
                result.cpi = float(words[-1])
    else:
        parsing_registers = False
        parsing_memory = False
        
        for line in simout:
            line = line.strip()
            if not line:
                continue
            if line.startswith("Changes to registers"):
                parsing_registers = True
                continue
            if line.startswith("Changes to memory"):
                parsing_registers = False
                parsing_memory = True
                continue
            if parsing_registers:
                words = line.split()
                result.reg_states[words[0][1:-1]] = int(words[-1], 16)
                assert(result.reg_states[words[0][1:-1]] >= 0)
            if parsing_memory:
                words = line.split()
                result.mem_states[int(words[0][:-1], 16)] = int(words[-1], 16)
                assert(result.mem_states[int(words[0][:-1], 16)] >= 0)
                assert(int(words[0][:-1], 16) >= 0)
            if line.startswith("Stopped in"):
                words = line.split()
                result.instructions = int(words[2])
                result.status = words[9][:-1].strip("'")
                assert(words[8] == "Status")
    return result

def execute_yo_file(simulator_path,  yo_file_path, simulator_type="yis"):
    simargs = [simulator_path, yo_file_path]
    if (simulator_type == "ssim" or simulator_type == "psim"):
        simargs.append("-t")
    try:
        simproc = subprocess.run(simargs, capture_output=True, text=True, timeout=5)
    except subprocess.TimeoutExpired:
        sys.stderr.write(f"Simulator with {simargs} failed(timed out)!\n")
        result = SimulationResult(execfail=True, type=simulator_type)
        return result
    
    if simproc.stderr:
        sys.stderr.write(f"Simulator with {simargs} has stderr:\n{simproc.stderr}\n")
        result = SimulationResult(execfail=True, type=simulator_type)
        return result
    
    if (simproc.returncode != 0):
        sys.stderr.write(f"Simulator with {simargs} failed with returncode {simproc.returncode}!\n")
        result = SimulationResult(execfail=True, type=simulator_type)
        return result
    
    try:
        result = parse_simulator_output(simproc.stdout, simulator_type)
    except Exception as e:
        sys.stderr.write(str(e),"\n")
        sys.stderr.write(f"Simulator with {simargs} failed to parse output! If your sim does not crash by itself, contact onem at metu edu tr!\n")
        result = SimulationResult(execfail=True, type=simulator_type)
        return result
    
    return result
        
if __name__ == "__main__":
    if len(sys.argv) < 3:
        sys.stderr.write("Usage: python3 sim_executor.py {yis, ssim, psim, drypsim, dryyis, dryssim} <path to yo file>\n")
        sys.exit(1)
    
    simulator_type = sys.argv[1]
    yo_file_path = os.path.abspath(sys.argv[2])
    if not os.path.exists(yo_file_path):
        sys.stderr.write(f"File {yo_file_path} does not exist!\n")
        sys.exit(1)
        
    if simulator_type == "yis":
        print(execute_yo_file(YIS_PATH, yo_file_path, simulator_type).pretty_str())
    elif simulator_type == "ssim":
        print(execute_yo_file(SSIM_PATH, yo_file_path, simulator_type).pretty_str())
    elif simulator_type == "psim":
        print(execute_yo_file(PSIM_PATH, yo_file_path, simulator_type).pretty_str())
    elif simulator_type.startswith("dry"):
        simulator_type = simulator_type[3:]
        print(parse_simulator_output(open(yo_file_path).read(), simulator_type).pretty_str())
    else:
        sys.stderr.write(f"Unknown simulator type {simulator_type}!\n")
        sys.exit(1)
    
    
    
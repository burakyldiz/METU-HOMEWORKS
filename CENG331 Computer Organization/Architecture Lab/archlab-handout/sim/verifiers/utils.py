import subprocess
import os
import sys

YIS_PATH = os.path.join(os.path.dirname(__file__), "..", "misc", "yis")
PSIM_PATH = os.path.join(os.path.dirname(__file__), "..", "pipe", "psim")
SSIM_PATH = os.path.join(os.path.dirname(__file__), "..", "seq", "ssim")
YAS_PATH = os.path.join(os.path.dirname(__file__), "..", "misc", "yas")

def get_sanitized_code(asm_file_path):
    with open(asm_file_path, 'r') as f:
        start = -1
        end = -1
        codestr = f.readlines()
        for i,line in enumerate(codestr):
            if line.startswith("#implement your code below"):
                start = i
            if line.startswith("#implement your code above"):
                end = i
        if start == -1 or end == -1:
            raise Exception("Could not find start or end of implementation! Keep those comments!")
        return "\n"+"".join(codestr[start+1:end])+"\n"

def convert_python_int_to_y86_int(i):
    if i >= 0:
        return i
    else:
        return (1 << 64) + i

def y86_asm_array(begin_label, end_label, lst):
    string = begin_label + ":\n"
    for i in lst:
        string += ".quad " + str(i) + "\n"
    if end_label:
        string += end_label + ":\n"
        string += ".quad 0\n"
    return string

def compile_with_yas(asm_file_path):
    yasproc = subprocess.run([YAS_PATH, asm_file_path], capture_output=True, text=True)
    if yasproc.stderr:
        sys.stderr.write(f"YAS with {asm_file_path} failed with stderr:\n{yasproc.stderr}\n")
        return False
    if yasproc.returncode != 0:
        sys.stderr.write(f"YAS with {asm_file_path} failed with returncode {yasproc.returncode}!\n")
        return False
    return True

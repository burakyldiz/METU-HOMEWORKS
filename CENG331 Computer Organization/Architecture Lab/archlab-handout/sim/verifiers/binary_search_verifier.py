from utils import *
from sim_executor import *
import os
import random

SEED = 331
random.seed(SEED)
TEST_CASE_DIR = "binary_search_test_cases"
ARRAY1_BEGIN_ADDR = 0x1008


def random_ascending_integer_list(length):
    random_list = random.sample(range(100, 200), length)
    random_list.sort()
    return random_list

def random_binary_search_test_case():
    lst = random_ascending_integer_list(random.randint(10,20))
    if random.random() < 0.7:
        query_value = random.choice(lst)
        query_index = lst.index(query_value)+1
    else:
        if random.random() < 0.5:
            query_value = random.randint(10, 50)
        else:
            query_value = random.randint(250, 300)
        query_index = -1
    
    return (lst, query_value, query_index)

def create_test_file(file_name, implementation, lst, query_value):
    prologue =f""".pos 0x0
irmovq stack, %rsp
irmovq ${query_value}, %rdx #queried value
irmovq array_begin, %rdi
irmovq array_end, %rsi
call binary_search
halt
"""

    epilogue =""".pos 0x1F00
stack:
.quad 0

"""
    array_str = y86_asm_array("array_begin", "array_end", lst)
    with open(file_name, 'w') as f:
        f.write(prologue)
        f.write(implementation)
        f.write(f"\n.pos 0x{ARRAY1_BEGIN_ADDR:04X}\n")
        f.write(array_str)
        f.write(epilogue)


if __name__ == "__main__":
    if len(sys.argv) < 3:
        sys.stderr.write("Usage: python3 binary_search_verifier.py {yis, ssim, psim} <path to binary_search.ys>\n")
        sys.exit(1)  

    test_cases = [([], 10, -1),
            ([40], 40, 1),
            ([40], 50, -1),
            ([40], 30, -1),
            ([40, 50], 40, 1),
            ([40, 50], 50, 2),
            ([40, 50], 30, -1)]
    
    test_cases.extend([random_binary_search_test_case() for _ in range(13)])
    
    num_test_cases = len(test_cases)
    num_success = 0
    test_case_dir = os.path.join(os.path.dirname(__file__), TEST_CASE_DIR)
    
    os.makedirs(test_case_dir, exist_ok=True)
    
    try:
        implementation = get_sanitized_code(sys.argv[2])
    except Exception as e:
        sys.stderr.write(f"Could not find start or end of implementation! Keep those comments!\n")
        sys.exit(1)
    
    for i, (lst, query_value, query_index) in enumerate(test_cases):
        
        test_case_path = os.path.join(test_case_dir, f"binary_search_test{i}.ys")
        compiled_test_case_path = os.path.join(test_case_dir, f"binary_search_test{i}.yo")
        
        print(f"Creating test case {i}...")
        create_test_file(test_case_path, implementation, lst, query_value)
        
        print(f"Compiling test case {i}...")
        if not compile_with_yas(test_case_path):
            print(f"Could not compile test case {i}!\n")
            print(f"Test case {i}: FAIL\n")
            continue
        
        print(f"Running test case {i}...")
        
        sim_path = None
        if sys.argv[1] == "yis":
            sim_path = YIS_PATH
        elif sys.argv[1] == "ssim":
            sim_path = SSIM_PATH
        elif sys.argv[1] == "psim":
            sim_path = PSIM_PATH
        else:
            sys.stderr.write(f"Unknown simulator type {sys.argv[1]}!\n")
            sys.exit(1)
        
        result = execute_yo_file(sim_path, compiled_test_case_path, simulator_type=sys.argv[1])
        #you can uncomment here for a quick summary of the end of simulation
        #print(result.pretty_str())
        if not result.terminated_successfully():
            print(f"Test case {i} does not terminate successfully!\n")
            print(f"Test case {i}: FAIL\n")
            continue
        
        query_index = convert_python_int_to_y86_int(query_index)
        if result.get_retval() != query_index:
            print(f"Test case {i} returns {result.get_retval():016X}, not the correct value {query_index:016X}!\n")
            print(f"Test case {i}: FAIL\n")
            continue
        
        num_success += 1
        print(f"Test case {i}: PASS\n")
    
    print(f"Passed {num_success} / {num_test_cases}")
          
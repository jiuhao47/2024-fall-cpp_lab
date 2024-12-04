import os
import subprocess
import sys
from termcolor import colored

def parse_testcase(f_name):
    # Parse the code, get test cases and the expected output.
    test_input = []
    expect_output = []
    cur_input = []
    cur_output = []
    in_test_case = False

    with open(f_name, 'r') as f:
        for line in f.readlines():
            line = line.strip()
            if len(line) == 0:
                continue
            if line == '/*':
                in_test_case = True
                continue
            if line == '*/':
                in_test_case = False
                test_input.append(cur_input)
                expect_output.append(cur_output)
                cur_input = []
                cur_output = []
            if in_test_case:
                if line.find('input:') != -1:
                    cur_input.append(int(line.replace('input:', '')))
                else:
                    cur_output.append(line)
    return test_input, expect_output

def run_and_check(f_name, test_input, expect_output):
    p = subprocess.Popen([irbuilder, f_name], stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
    for t in test_input:
        p.stdin.write(str.encode(str(t) + '\n'))
        p.stdin.flush()
    try:
        p.wait(3)
    except subprocess.TimeoutExpired:
        return 'TIMEOUT', None
    if p.returncode not in [0, 255]:
        return 'CRASH', None

    execute_res = []
    res = p.communicate()[0].decode('utf-8')
    err_msg = p.communicate()[1].decode('utf-8')
    is_output = False
    for line in res.split('\n'):
        line = line.strip()
        line = line.replace('input:', '')
        if len(line) == 0:
            continue

        if line.find('SAFEC EXECUTE BEGIN') != -1:
            is_output = True
            continue
        if line.find('SAFEC EXECUTE END') != -1:
            break
        if is_output:
            execute_res.append(line)
    if p.returncode == 255:
        err_msg = [ l.strip() for l in err_msg.split('\n') if len(l.strip()) > 0 ]
        execute_res += err_msg
    if len(expect_output) != len(execute_res):
        return 'FAIL', execute_res
    for i in range(len(expect_output)):
        if expect_output[i] != execute_res[i]:
            return 'FAIL', execute_res
    return 'PASS', execute_res


target = 'all'
if len(sys.argv) == 2:
    if sys.argv[1] != 'all':
        target = sys.argv[1]
        

os.chdir(os.getcwd())
irbuilder = os.path.join('build', 'irbuilder')
if not os.path.exists(irbuilder):
    print(irbuilder, 'not found.')
    exit(0)


failed = 0
testcases = os.listdir('tests')
testcases.sort()
tested = 0
for t in testcases:
    if target != 'all' and t != target:
        continue
    if t == 'trick.c':
        continue
    tested += 1
    print(t, end=' ')
    t = os.path.join('tests', t)
    test_input, expect_output = parse_testcase(t)

    # Execute each test case and check the result.
    correct = True
    for i in range(len(test_input)):
        res, execute_res = run_and_check(t, test_input[i], expect_output[i])
        if res == 'PASS':
            continue
        elif res == 'FAIL':
            print(colored('fail.', 'red'))
            print('\tInput:', test_input[i])
            print('\tExpected:', expect_output[i])
            print('\tOutput:', execute_res)
        elif res == 'TIMEOUT':
            print(colored('timeout.', 'red'))
            print('\tInput:', test_input[i])
        elif res == 'CRASH':
            print(colored('crash.', 'red'))
            print('\tInput:', test_input[i])
        else:
            assert(0)
        correct = False
        break
    if not correct:
        failed += 1
    else:
        print(colored('pass.', 'green'))

print('{} / {} Passed.'.format(tested - failed, tested))
            
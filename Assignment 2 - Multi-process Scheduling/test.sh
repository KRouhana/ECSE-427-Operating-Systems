#./test.sh starter-code/mysh test-cases
#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

if [[ $# -ne 2 ]]; then
    echo "Usage: ./test.sh <executable> <test-directory>"
    exit 1
fi

program=$1
tests=$2

function run_test() {
    test_dir=$1
    filename=$2
    name="${filename%.*}"
    mkdir tmp
    cd tmp

    echo -n -e "${NC}Running $name... "
    eval "../$program < ../$test_dir/$filename" >../output
    difference=$(diff -w -y --color=always ../output ../$test_dir/${name}_result.txt)
    ret=$?
    if [[ $ret -eq 0 ]]; then
        echo -e "${GREEN}Passed"
    else
        echo -e "${RED}Failed\n---Diff---${NC}"
        echo "$difference"
        echo -e "${RED}---End diff---${NC}"
    fi

    cd ..
    rm -r tmp
    rm output

    return $ret
}

if [[ -d "$tests" ]]; then
    files=$(ls "$tests" | grep -v '_result.txt')

    total=0
    passed=0
    failed=0
    for test in $files; do
        total=$((total + 1))
        run_test "$tests" "$test"
        if [[ $? -eq 0 ]]; then
            passed=$((passed + 1))
        else
            failed=$((failed + 1))
        fi
    done

    echo
    echo -e "${NC}---Summary---"
    echo -e "${GREEN}Passed:  $passed"
    echo -e "${RED}Failed:  $failed"
    echo -e "${NC}Total:   $total"
elif [[ -f "$tests" ]]; then
    run_test $(dirname "$tests") $(basename "$tests")
else
    echo "Invalid test file or directory."
    exit 1
fi

#!/bin/bash

TEST_SAMPLES="./tests/*.txt"
TEST_OUTPUT="./tests/test_output"
MY_GREP="./my_grep"

REGX=(
"error"
"123"
"hello"
"1.*9"
"^Hello"
"again$"
"[A-Z][a-z]+"
"[0-9]"
"[1]"
"[1-3]"
"hello"
"error"
"Error.*line"
"CaseInsensitiveTest"
"insensitive"
"hello.*world"
)

FLAGS=(
"-i"
"-n"
"-c"
"-v"
"-l"
"-e"
"-s"
"-h"
"-in"
"-ni"
"-ic"
"-cin"
"-iv"
"-ivn"
"-lv"
"-lvi"
"-lvic"
"-le"
"-lce"
"-o"
"-on"
"-oi"
"-oc"
"-oh"
"-ol"
"-oin"
"-oic"
"-och"
"-ocl"
"-oinh"
)

if [ ! -x $MY_GREP ]; then
    echo "Can't locate binary my_grep file;"
    exit 1
fi

mkdir -p $TEST_OUTPUT

TOTAL_TESTS=0
FAILED_TESTS=0

for file in $TEST_SAMPLES; do
    printf "Testing: $file without flags; --> \n"
    for reg in "${REGX[@]}"; do
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        
        $MY_GREP $reg "$file" > $TEST_OUTPUT/my_grep_out.txt
        grep $reg "$file" > $TEST_OUTPUT/real_grep_out.txt
        
        if ! diff -u $TEST_OUTPUT/my_grep_out.txt $TEST_OUTPUT/real_grep_out.txt > /dev/null; then
            echo "FAIL: regex '$reg' in $file"
            FAILED_TESTS=$((FAILED_TESTS + 1))
        fi
    done
    
    printf "Testing: $file with flags; --> \n"
    for flag in "${FLAGS[@]}"; do
        for reg in "${REGX[@]}"; do
            TOTAL_TESTS=$((TOTAL_TESTS + 1))
            
            $MY_GREP $flag $reg "$file" > $TEST_OUTPUT/my_grep_out.txt 2>/dev/null
            grep $flag $reg "$file" > $TEST_OUTPUT/real_grep_out.txt 2>/dev/null
            
            if ! diff -u $TEST_OUTPUT/my_grep_out.txt $TEST_OUTPUT/real_grep_out.txt > /dev/null; then
                echo "FAIL: flag '$flag' regex '$reg' in $file"
                FAILED_TESTS=$((FAILED_TESTS + 1))
            fi
        done
    done
done

if [ -f "./tests/patterns.txt" ]; then
    echo "Testing -f flag..."
    for file in $TEST_SAMPLES; do
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        
        $MY_GREP -f "./tests/patterns.txt" "$file" > $TEST_OUTPUT/my_grep_out.txt 2>/dev/null
        grep -f "./tests/patterns.txt" "$file" > $TEST_OUTPUT/real_grep_out.txt 2>/dev/null
        
        if ! diff -u $TEST_OUTPUT/my_grep_out.txt $TEST_OUTPUT/real_grep_out.txt > /dev/null; then
            echo "FAIL: -f flag in $file"
            FAILED_TESTS=$((FAILED_TESTS + 1))
        fi
    done
fi

printf "Testing multiple files with flags; --> \n"
for flag in "${FLAGS[@]}"; do
    for reg in "${REGX[@]}"; do
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
            
        $MY_GREP $flag $reg $TEST_SAMPLES > $TEST_OUTPUT/my_grep_out.txt 2>/dev/null
        grep $flag $reg $TEST_SAMPLES > $TEST_OUTPUT/real_grep_out.txt 2>/dev/null
            
        if ! diff -u $TEST_OUTPUT/my_grep_out.txt $TEST_OUTPUT/real_grep_out.txt > /dev/null; then
            echo "FAIL: flag '$flag' regex '$reg'"
            FAILED_TESTS=$((FAILED_TESTS + 1))
        fi
    done
done

if command -v valgrind > /dev/null; then
    echo "Quick memory leak test..."
    valgrind --leak-check=full --error-exitcode=1 $MY_GREP "hello" "${TEST_SAMPLES%% *}" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "MEMORY LEAK detected!"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
fi

echo "===== SUMMARY ====="
echo "Total tests: $TOTAL_TESTS"
echo "Failed tests: $FAILED_TESTS"

if [ $FAILED_TESTS -eq 0 ]; then
    echo "ALL TESTS PASSED!"
    exit 0
else
    echo "SOME TESTS FAILED"
    exit 1
fi

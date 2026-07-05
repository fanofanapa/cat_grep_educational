#!/bin/bash

FLAGS=(
  "-e"
  "-n"
  "-b"
  "-t"
  "-s"
  "-nb"
  "-bn"
  "-ns"
  "-bs"
  "-es"
  "-ts"
  "-ne"
  "-be"
  "-te"
  "-nt"
  "-bt"
  "-est"
  "-ens"
  "-bns"
  "-tns"
  "-bets"
  "-benst"
  "-ben"
  "-vt"
)
TEST_SAMPLES="./tests/test_cases/*.txt"
TEST_OUTPUT="./tests/test_output"
MY_CAT="./my_cat"

if [ ! -x $MY_CAT ]; then 
    echo "Can't locate binary my_cat file;"
    echo "Project structure should look like:"
    echo "src"
    echo " |- cat"
    echo " |   |- tests"
    echo " |   |   |- test_cases"
    echo " |   |   |   |- <.txt text sample files>"
    echo " |   |   |"
    echo " |   |   |- test_output"
    echo " |   |   |   |- <may be empty>"
    echo " |   |   |"
    echo " |   |   |- test.sh <file for run tests>"
    echo " |   |"
    echo " |   |- my_cat <executable>"
    echo " |   |"
    echo " |   |- Makefile <- You are here"

    exit 1
fi

for file in $TEST_SAMPLES; do

    printf "Testing: $file without flags --> "
    $MY_CAT "$file" > "$TEST_OUTPUT/my_cat_out.txt"
    cat "$file" > "$TEST_OUTPUT/cat_out.txt"

    if ! diff -u "$TEST_OUTPUT/my_cat_out.txt" "$TEST_OUTPUT/cat_out.txt"; then
        echo ""
        echo "Difference between outputs was detected"
        echo -e "\033[1;31mTEST FAIL: $file\033[0m"
        exit 1
    fi
    printf "\033[32mOK\033[0m\n"

    for flag in "${FLAGS[@]}"; do

        printf "Testing: $file with flag(s): $flag --> "
        $MY_CAT "$flag" "$file" > "$TEST_OUTPUT/my_cat_out.txt"
        cat "$flag" "$file" > "$TEST_OUTPUT/cat_out.txt"

        if ! diff -u "$TEST_OUTPUT/my_cat_out.txt" "$TEST_OUTPUT/cat_out.txt"; then
            echo ""
            echo "Difference between outputs was detected"
            echo -e "\033[1;31mTEST FAIL: $file\033[0m"
            exit 1
        fi
        printf "\033[32mOK\033[0m\n"
    done
done

echo ""
echo "No difference between outputs was detected"
echo -e "\033[1;32mTESTS PASSED\033[0m"

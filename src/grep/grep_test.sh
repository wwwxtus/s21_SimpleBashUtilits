#!/bin/bash
COUNTER_SUCCESS=0
COUNTER_FAIL=0
DIFF_RES=""
TEST_FILES=("test.txt test1.txt")
TEST_PATTERNS=("-e Len" "-e ')$'" "patterns.txt" "char" "-e '*'")
TEST_FLAGS=( "-i" "-v" "-s" "-h" "-n" "-l" "-c")
echo "" > log.txt
for pattern in "${TEST_PATTERNS[@]}"; do
    for test_file in  "${TEST_FILES[@]}"; do
        for var in "${TEST_FLAGS[@]}"; do
            for var2 in "${TEST_FLAGS[@]}"; do
                if [ $var != $var2 ]
                then
                    TEST1="$var $var2 $pattern $test_file"
                    echo "$TEST1 $COUNTER_SUCCESS $COUNTER_FAIL"
                    ./s21_grep $TEST1 > s21_grep.txt
                    grep $TEST1 > grep.txt
                    DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
                    if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
                    then
                        (( COUNTER_SUCCESS++ ))
                    else
                        echo "$TEST1" >> log.txt
                        (( COUNTER_FAIL++ ))
                    fi
                    rm s21_grep.txt grep.txt
                fi
            done
        done
    done
done

echo "SUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"
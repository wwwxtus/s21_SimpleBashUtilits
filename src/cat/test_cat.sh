#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""

declare -a tests=(
"VAR test1.txt test2.txt"
)

declare -a extra=(
"-s test1.txt"
"-b -e -n -s -t -v test1.txt"
"-t test1.txt"
"-n test2.txt"
"no_file.txt"
"-n -b test1.txt"
"-s -n -e test2.txt"
"test1.txt -n"
"-n test1.txt"
"-n test1.txt test2.txt"
"-v test1.txt"
)

flags=(
    "b"
    "e"
    "n"
    "s"
    "t"
    "v"
)

testing()
{
    t=$(echo $@ | sed "s/VAR/$var/")
    ./s21_cat $t > test_s21_cat.log
    cat $t > test_sys_cat.log
    DIFF_RES="$(diff -s test_s21_cat.log test_sys_cat.log)"
    (( COUNTER++ ))
    if [ "$DIFF_RES" == "Files test_s21_cat.log and test_sys_cat.log are identical" ]
    then
      (( SUCCESS++ ))
        echo "$FAIL $SUCCESS $COUNTER success cat $t"
    else
      (( FAIL++ ))
        echo "$FAIL $SUCCESS $COUNTER fail cat $t"
    fi
    rm test_s21_cat.log test_sys_cat.log
}

for i in "${extra[@]}"
do
    var="-"
    testing $i
done

# 1 параметр
for var1 in b e n s t v
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        testing $i
    done
done

# 2 параметра
for var1 in b e n s t v
do
    for var2 in b e n s t v
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                testing $i
            done
        fi
    done
done

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
echo "ALL: $COUNTER"
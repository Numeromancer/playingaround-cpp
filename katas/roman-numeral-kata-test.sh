#!/usr/bin/env bash

declare -a tests_pass=(
"1 I"
"4 IV"
"5 V"
"9 IX"
"10 X"
"11 XI"
"19 XIX"
"20 XX"
"40 XL"
"49 XLIX"
"99 XCIX"
"100 C"
"149 CXLIX"
"499 CDXCIX"
"999 CMXCIX"
"1000 M"
"2000 MM"
"3000 MMM"
"3999 MMMCMXCIX"
)

declare -a bad_arabic=(
"4000 MMMM"
)

declare -a bad_roman=(
"0 MMMMEE"
"0 ABCD"
)


ntests=$((${#tests_pass[@]}*2 + ${#bad_arabic[@]} + ${#bad_roman[@]}))

function do_passing_test() {
    local arabic=$1
    local roman=$2
    local outroman=$(./roman-numeral-kata $arabic)
    if [[ $outroman == $roman ]]; then
        printf "ok - %d -> %s\n" $arabic  $outroman
    else
        printf "not ok - %d -> %s (should be %s)\n" $arabic $outroman $roman
    fi
    local outarabic=$(./roman-numeral-kata -u $roman)
    if [[ $outarabic == $arabic ]]; then
        printf "ok - %s -> %d\n" $roman $arabic
    else
        printf "not ok - %s -> %d (should be %d)\n" $roman $outarabic $arabic
    fi
}

function do_failing_bad_arabic_test() {
    local arabic=$1
    local roman=$2
    ./roman-numeral-kata $arabic 2>&1 | sed 's/^/# /'
    local returnstatus=${PIPESTATUS[0]}
    if [[ ${returnstatus} == 0 ]]; then
        printf "not ok - %d -> %s (should not pass)\n" $arabic  $roman
    else
        printf "ok - %d -> %s (should fail)\n" $arabic $roman
    fi
}

function do_failing_bad_roman_test() {
    local arabic=$1
    local roman=$2
    ./roman-numeral-kata -u $roman 2>&1 | sed 's/^/# /'
    local returnstatus=${PIPESTATUS[0]}
    if [[ ${returnstatus} == 0 ]]; then
        printf "not ok - %s -> %d (should fail, return status is %d)\n" \
            $roman  $arabic ${returnstatus}
    else
        printf "ok - %d -> %s (should fail)\n" $arabic $roman
    fi
}


printf "1..%d\n" ${ntests}
for t in "${tests_pass[@]}";
do
    do_passing_test ${t% *} ${t#* }
done

for t in "${bad_arabic[@]}";
do
    do_failing_bad_arabic_test ${t% *} ${t#* }
done
    
for t in "${bad_roman[@]}";
do
    do_failing_bad_roman_test ${t% *} ${t#* }
done
    

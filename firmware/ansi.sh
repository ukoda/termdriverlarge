#!/bin/bash
port="/dev/ttyACM1"

normal="\033[0m"

stty -F ${port} 115200
echo -e "${normal}    30 31 32 33 34 35 36 37 90 91 92 93 94 95 96 97" > ${port}

for bg in {40..47}; do
    line=" ${bg}|"
    for fg in {30..37}; do
        line="${line}\033[${fg};${bg}mA1${normal}|"
    done
    for fg in {90..97}; do
        line="${line}\033[${fg};${bg}mA1${normal}|"
    done
    echo -e "${line}" > ${port}
done
for bg in {100..107}; do
    line="${bg}|"
    for fg in {30..37}; do
        line="${line}\033[${fg};${bg}mA1${normal}|"
    done
    for fg in {90..97}; do
        line="${line}\033[${fg};${bg}mA1${normal}|"
    done
    echo -e "${line}" > ${port}
done

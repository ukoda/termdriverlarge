#!/bin/bash
port="/dev/ttyACM1"

stty -F ${port} 115200

for line in {1..17}; do
    echo "[${line}]" > ${port}
done

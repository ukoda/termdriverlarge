#!/bin/bash
port="/dev/ttyACM1"

stty -F ${port} 115200

echo "0123456789" > ${port}
echo "ABCDFEGHIJKLMNOPQRSTUVWXYZ" > ${port}
echo "abcdefghijklmnopqrstuvwxyz" > ${port}
echo "@#$%^&*()_+-=[]{};:<>,./?|" > ${port}

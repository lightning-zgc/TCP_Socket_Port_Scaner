#!/bin/bash
rm -f tcpscan
gcc tcpscan.c -o tcpscan
chmod +x tcpscan
./tcpscan 127.0.0.1 1000 2000
#!/usr/bin/env python3


with open('data.log') as f:
    print("{",end="")
    for line in f.readlines():
        #line = line.strip()
        if line.startswith("shift"):
            if line.startswith("shift7"):
               chunk = line.split(' ')
               chunk[3].strip();
               print(chunk[3],end="")
            else:
               chunk = line.split(' ')
               chunk[3].strip();
               print(chunk[3]+",  ",end="")

        if line.startswith("theta"):
            if line.startswith("theta13"):
               chunk = line.split(' ')
               chunk[3].strip();
               print(chunk[3],end="")
            else:
               chunk = line.split(' ')
               chunk[3].strip();
               print(chunk[3]+",  ",end="")
        if line.startswith("xtot"):
            chunk = line.split(' ')
            chunk[3].strip();
            print(chunk[3]+",  ",end="")
        if line.startswith("ytot"):
            chunk = line.split(' ')
            chunk[3].strip();
            print(chunk[3]+",  ",end="")


    print("}")
    print()
with open('data.log') as f:
    for line in f.readlines():
        #line = line.strip()
        if line.startswith("shift"):
            chunk = line.split(' ')
            chunk[3].strip();
            print(chunk[3],end="")
        if line.startswith("theta"):
            chunk = line.split(' ')
            chunk[3].strip();
            print(chunk[3],end="")



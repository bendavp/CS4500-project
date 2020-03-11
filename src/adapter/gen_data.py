#!/usr/bin/python3

from random import *

def should_show():
    return randrange(10) < 8

def enclose(string):
    return "<" + string + ">"

def gen_row():
    out = ""
    
    if should_show(): out = out + enclose(str(randrange(100)));
    else: out = out + "<>"
    if should_show(): out = out + enclose("\"string\"");
    else: out = out + "<>"
    if should_show(): out = out + enclose(str(random()*10));
    else: out = out + "<>"
    if should_show(): out = out + enclose(str(randrange(1)));
    else: out = out + "<>"

    return out

def main():
    for i in range(10000):
        print(gen_row());

main()

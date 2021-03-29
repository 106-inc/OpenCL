#!/usr/bin/env python3

str_lst = input().split('\n')

for el in str_lst:
    pair = el.split()
    name = pair[0]
    print(f'case {name}: return "{name}";') 

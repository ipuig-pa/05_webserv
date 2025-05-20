#!/usr/local/bin/python3

import os

print("Content-Type: text/plain\n")

path_info = os.environ.get('PATH_INFO', '')

if not path_info:
	print("\033[31;1mNo PATH_INFO passed!\033[0m\n")
else:
    # print("\033[32;1mThis is path_info string value: \033[0m" + path_info)
    filename = path_info.lstrip('/');
    try:
        with open(filename, 'r') as f:
            print(f.read())
    except Exception as e:
        print(f"\033[32;1m{e}!\033[0m\n")
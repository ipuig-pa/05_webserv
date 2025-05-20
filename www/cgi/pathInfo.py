#!/usr/local/bin/python3
import os
path_info = os.environ.get('PATH_INFO', '')

print("Content-Type: text/plain\n")

if not path_info:
	print("\033[31;1mNo specific file required!\033[0m\n")
else:
    # print("\033[32;1mThis is path_info string value: \033[0m" + path_info)
    filename = path_info.lstrip('/');
    try:
        with open(filename, 'r') as f:
            print(f.read())
    except Exception as e:
        print(f"\033[32;1mERROR opening file: {e}!\033[0m\n")
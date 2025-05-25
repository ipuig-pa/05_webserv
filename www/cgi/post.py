#!/usr/local/bin/python3
import os
import sys

print("Content-Type: text/plain\n")

length = int(os.environ.get("CONTENT_LENGTH", 0))
data = sys.stdin.read(length)

print("POST Content from client is: ")
print(data)

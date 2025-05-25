#!/usr/local/bin/python3
import os
import sys

print("Content-Type: text/plain\n")

length = int(os.environ.get("CONTENT_LENGTH", 0))
data = sys.stdin.read(length)

print("POST Content from client is: ")
print(data)

# command: curl -X POST -d "POST content evalutor want to test" http://127.0.0.1:8080/www/cgi/post.py
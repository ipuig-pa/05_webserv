#!/usr/bin/python3
import os
import sys

# Print HTTP headers first (required for CGI)
print("Content-Type: text/plain\n")

# Print all environment variables
print("=== Environment Variables ===")
for key, value in os.environ.items():
    print(f"{key}: {value}")

# You can also print specific variables you're interested in
print("\n=== CGI Specific Variables ===")
print(f"PATH_INFO: {os.environ.get('PATH_INFO', 'Not set')}")
print(f"QUERY_STRING: {os.environ.get('QUERY_STRING', 'Not set')}")
print(f"REQUEST_METHOD: {os.environ.get('REQUEST_METHOD', 'Not set')}")
print(f"CONTENT_LENGTH: {os.environ.get('CONTENT_LENGTH', 'Not set')}")
print(f"CONTENT_TYPE: {os.environ.get('CONTENT_TYPE', 'Not set')}")
print(f"SCRIPT_FILENAME: {os.environ.get('SCRIPT_FILENAME', 'Not set')}")
print(f"SCRIPT_NAME: {os.environ.get('SCRIPT_NAME', 'Not set')}")
print(f"SERVER_NAME: {os.environ.get('SERVER_NAME', 'Not set')}")
print(f"SERVER_PORT: {os.environ.get('SERVER_PORT', 'Not set')}")
print(f"REMOTE_ADDR: {os.environ.get('REMOTE_ADDR', 'Not set')}")

# Print command line arguments
print("\n=== Command Line Arguments ===")
for i, arg in enumerate(sys.argv):
    print(f"Arg {i}: {arg}")
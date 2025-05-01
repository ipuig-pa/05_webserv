#!/usr/local/bin/python3

# Import the 'os' module to access environment variables
import os

# Print the HTTP headers
print("Content-Type: text/html")  # Specify the content type
print()                           # Empty line to separate headers from body

# Get the value of the QUERY_STRING environment variable
query_string = os.environ.get("QUERY_STRING", "")

# Parse the query string (very basic example)
name = "World"  # Default name
if query_string:
    params = query_string.split("&")
    for param in params:
        if param.startswith("name="):
            name = param[5:]  # Extract the name

# Generate the HTML response
print("<!DOCTYPE html>")
print("<html>")
print("<head>")
print("<title>Hello from CGI!</title>")
print("</head>")
print("<body>")
print("<h1>Hello, {}!</h1>".format(name))
print("<p>This is a simple CGI script written in Python.</p>")
print("</body>")
print("</html>")
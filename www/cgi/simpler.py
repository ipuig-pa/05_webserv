#!/usr/local/bin/python3
print("Content-Type: text/html\n")
print("<html><body><h1>Simpler python CGI test file</h1>")

import os
if 'SERVER_PORT' in os.environ:
	print("<p>server Port: {}</p>".format(os.environ['SERVER_PORT']))
if 'SCRIPT_NAME' in os.environ:
    print("<p>Script name: {}</p>".format(os.environ['SCRIPT_NAME']))
print("</body></html>")
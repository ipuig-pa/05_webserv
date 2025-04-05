MAIN TASK:
- Build HTTP server
	- handle HTTP *requests* (usr asks for webpage)
	- send back HTTP *response* (web content)
- TO implement
	- Networking: how servers listen on ports and handle connections
	- HTTP Protocol: HTTP methods (GET, POST, DELETE), status codes, headers
	- Non-blocking I/O: Your server must handle multiple connections without getting stuck
	- Poll/Select/Epoll: For managing multiple connections simultaneously
	- File I/O: Reading/writing files for serving webpages
	- Configuration Parsing: Reading and interpreting configuration files
	- CGI (Common Gateway Interface) - For executing scripts like PHP
- Implementation Steo:
	- parser for config file
	- set up server sockets to listen to ports
	- parse for HTTP requests
	- handle request: the logic for different HTTP methods (GET, POST, DELETE)
	- handle response: created formatted HTTP response
	- file server: serce static files
	- CGI: build sys to exec CGI scripts (eg: PHP)
	- error handling
- Phase
	- design/plan server architechture
	- design configuration file format
	- create class diagram
# STEPS
	- setup basic TCP server
		## create a socket, bind a port, and accept connections
		```c++
		int server_fd = socket(AF_INET, SOCK_STREAM, 0);
		bind(server_fd, (struct sockaddr*)&address, sizeof(address));
		listen(server_fd, 10);
		```
	- implement non blocking I/O:
		## use fcntl() to set sockets to non-blocking mode
		poll() to moniter multiple fd
	- parse HTTP request
		## read incmoing data and extract method, URI, headers, and body
		check validity (syntax), eg: Host header in HTTP/1.1
	- handle router configuration
		## Read the configuration file to define server settings.
		Match requests to routes based on URI and server name.
	- Serve Static Files
		## For GET requests, read files from the filesystem.
		Handle Content-Type headers (e.g., text/html, image/png).
	- Implement CGI Execution:
		## Use fork() and execve() to run scripts.
		Capture script output and send it as the HTTP response.
	- Add POST/DELETE Support:
		## Handle file uploads (save to specified directories).
		Delete resources based on URI.
	- test
		## use curl, telnet browser
- Possible labor split
	** core server & HTTP handling
		- socket setup&management
		- non-blocking I/O with poll()/epoll()/select()
		- HTTP request parsing
		- HTTP response construction
		- erro handling
	** configuration & feature
		- configuration file parsing
		- route handle
		- static file serving
		- CGI
		- file upload


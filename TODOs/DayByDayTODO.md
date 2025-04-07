**Achieve Step By Step Checklist**

PARSER to read confif file:
	ports
	server names
	root paths
	location blocks
	[...]
	Store everything in config structs (or classes?)

Listening socket
	Goal: Open sockets based on configuration
	Use socket(), bind(), listen()
	One listening socket per configured port
	Set sockets to non-blocking using fcntl()

Event loop (poll()/epoll())
	Goal: Build non-blocking event loop, able to handle many clients without blocking.

	Set up poll() to monitor:
	listening sockets
	client sockets
	Accept new clients when incoming
	Store client FDs and handle disconnection
	
Echo server (communication)
	Goal: Start reading and writing to clients
	Read data from client (recv())
	Send simple response (or echo back)
	Properly handle closed connections
	Track per-client state (in a Client class)
	Result: Server reads/writes to multiple clients asynchronously.

HTTP Parser
	Parse request line (GET /path HTTP/1.1)
	Parse headers (Host:, Content-Length, etc.)
	Track incomplete/multi-part requests
	Store parsed request in HttpRequest object
	Result: can now interpret real HTTP requests.

Basic Routing & Response
	Goal: Serve static files & build HTTP responses
	Match request path to root/locations
	Read requested file
	Send headers + body
	Add basic error handling (404, 500, etc.)
	Result: can serve HTML/CSS/images like a real web server.

Support POST & DELETE
	POST: save uploaded content to disk
	DELETE: remove files
	Handle body parsing via Content-Length
	Result: supports: GET, POST, DELETE.

Support CGI
	Goal: Run external programs (Python, PHP...)
	Detect CGI paths from config
	fork() + execve() to run script
	Pass environment and input correctly
	Capture output and send to client
	Result: Server can serve dynamic content via CGI.

Set Default error_page
	if not err_conf in configfile, show defalu
	404/timeout/[...]

Test & Clean

Bonus (if needed)
	cookies 
	multi CGI
Person 1: Core Server Infrastructure

HTTP request parsing and routing
TCP/IP socket programming
Server configuration (reading config files)
Main event loop and connection handling
Error handling and logging

Person 1: Core Server Infrastructure
1. HTTP Request Parsing and Routing

Socket Data Reception: Read raw data from client socket file descriptors
HTTP Parser Implementation:

Parse request line (method, URL, HTTP version)
Parse headers (key-value pairs)
Handle request body/payload (for POST/PUT requests)
Process URL-encoded data and multipart form data
Manage chunked transfer encoding


Client Object Management:

Create client object for each connection containing:

Client socket file descriptor
Connection state
Parsed request data
Buffer for incomplete requests
Timeout information




Request Validation:

Check for malformed requests
Validate HTTP protocol compliance
Check header format and requirements


Request Routing:

Match requested URLs to server configuration
Route requests to correct virtual hosts
Implement location blocks (similar to NGINX)



2. TCP/IP Socket Programming

Socket Creation and Configuration:

Create non-blocking sockets
Set socket options (reuse address, keep-alive, etc.)
Bind sockets to specified ports and addresses


Connection Handling:

Accept incoming connections
Manage connection states
Handle connection timeouts
Implement graceful connection closing


Buffer Management:

Create efficient read/write buffers
Handle partial reads/writes
Manage memory for large transfers



3. Server Configuration

Config File Parser:

Design a configuration file format
Parse server blocks, locations, and directives
Validate configuration syntax


Configuration Object Model:

Create data structures for server settings
Implement virtual hosts
Manage location contexts and inheritance


Runtime Configuration:

Handle server reload without downtime
Validate and apply configuration changes



4. Main Event Loop and Connection Handling

Event Multiplexing:

Implement select(), poll(), or epoll() (depending on OS)
Monitor multiple file descriptors
Handle read/write readiness events


Connection State Machine:

Track connection lifecycle (new, reading, writing, closing)
Implement timeout handling
Manage connection queues


Request Dispatching:

Forward parsed requests to appropriate handlers
Manage request priorities
Handle concurrent requests



5. Error Handling and Logging

Error Detection and Recovery:

Handle system call failures
Recover from non-fatal errors
Generate appropriate error responses


Logging Framework:

Implement different log levels
Log requests, responses, and errors
Rotate log files
Configure log verbosity
# Webserv

## Team

---

This project was developed by:

- U-Mina (Server configuration (config file parsing), HTTP request parsing, CGI script exectution and readirection handling)
- ipuig-pa (Implementation of HTTP methods (GET, HEAD, POST, DELETE), Directory listing generation, File handling and MIME type detection, HTTP response generation, Main event loop and connection handling, Error handling and Logging)

## Project Overview

---

This project consists of building a custom HTTP server in C++ that can handle real web browser requests.

The server implements core HTTP/1.1 functionality including GET, HEAD, POST, and DELETE methods, static file serving, file uploads, and CGI execution. It uses non-blocking I/O with poll() for high-performance concurrent request handling.

The server is fully compatible with standard web browsers and follows HTTP specifications for proper response codes and headers.

## Implementation

---

### Compilation

- Use `make` to compile the project.

### Running

- Run the server with a configuration file: `./webserv [config_file.conf]`
- If no configuration file is provided, the server will use a default configuration.
- The server will start listening on the configured ports and display startup logging information.

### Configuration File

The configuration file follows NGINX-inspired syntax and supports the following features:

**Configuration Parameters:**

- **Server Block Configuration:**
    - `listen`: Port for the server to listen on
    - `server_name`: Server name for virtual host handling
    - `host`: Host for the server to listen on
    - `error_page`: Custom error pages for specific HTTP status codes
    - `client_max_body_size`: Maximum allowed size for client request body
    - `root`: Root directory for serving files
- **Location Blocks Configuration:**
    - `root`: Root directory for serving files
    - `index`: Default file to serve for directory requests
    - `allow_methods`: Allowed HTTP methods (GET, HEAD, POST, DELETE)
    - `return`: HTTP redirection (status code and URL)
    - `autoindex`: Enable/disable directory listing (on/off)
    - `upload_store`: Directory where uploaded files are saved
    - `cgi_extension`: File extensions that trigger CGI execution
    - `cgi_path`: Path to CGI interpreter
    - If a parameter is not configured in a location block it inherits the server block one.

**Example Configuration File:**

```
server {
    listen 8080;
    server_name webserv_test;
    error_page 404 /errors/404.html;
    error_page 500 /errors/500.html;
    client_max_body_size 1000000;

    location / {
        root /var/www/html;
        index index.html index.htm;
        allow_methods GET POST;
        autoindex on;
    }

    location /uploads {
        root /var/www;
        allow_methods GET POST DELETE;
        upload_store /var/www/uploads;
    }

    location /redirect {
        return 301 http://www.google.com;
    }

    location /cgi-bin {
        root /var/www/cgi-bin;
        allow_methods GET POST;
        cgi_extension .php .py;
        cgi_path /usr/bin/php-cgi;
    }
}

server {
    listen 8081;
    server_name secondary_server;

    location / {
        root /var/www/secondary;
        index index.html;
        allow_methods GET;
    }
}
```

### **Supported Features**

- **HTTP Methods:** GET, HEAD, POST, DELETE
- **Static File Serving:** Efficient serving of HTML, CSS, JS, images, and other static content
- **File Uploads:** Handle multipart/form-data file uploads
- **CGI Execution:** Support for PHP, Python and bash CGI scripts
- **Virtual Hosts:** Multiple server configurations on different ports
- **Custom Error Pages:** Configurable error pages for different HTTP status codes
- **Directory Listing:** Optional automatic directory index generation
- **HTTP Redirections:** Support for 301/302 redirects
- **Non-blocking I/O:** High-performance concurrent request handling
- **Request Body Size Limiting:** Configurable maximum request body size
- **Chunked Transfer Encoding:** Proper handling of chunked requests and responses

### **HTTP Compliance**

- **HTTP/1.1 Protocol:** Full compliance with HTTP/1.1 specifications
- **Proper Status Codes:** Accurate HTTP response status codes (200, 404, 500, etc.)
- **Header Handling:** Correct parsing and generation of HTTP headers
- **Keep-Alive Support:** Persistent connections for improved performance
- **Content-Type Detection:** Automatic MIME type detection based on file extensions

### **Performance Features**

- **Non-blocking I/O:** Uses poll() for efficient multiplexing
- **Single-threaded Architecture:** No fork() except for CGI execution
- **Memory Efficient:** Careful memory management to prevent leaks
- **Connection Limits:** Configurable limits to prevent resource exhaustion

### Testing

The server has been tested with:

- **Web Browsers:** Chrome, Firefox, Safari compatibility
- **Load Testing:** Stress tests to ensure stability under high load
- **CGI Scripts:** Various PHP, Python and bash CGI implementations
- **File Operations:** Upload, download, and deletion of files
- **Edge Cases:** Malformed requests, large files, connection timeouts

### Error Handling

- **Graceful Error Recovery:** Server continues running even after encountering errors
- **Custom Error Pages:** Configurable error pages for better user experience
- **Logging:** Comprehensive error and access logging
- **Resource Cleanup:** Proper cleanup of file descriptors and memory
- **Timeout Handling:** Automatic cleanup of stale connections

## License

---

This project was developed as part of the curriculum at 42 School. Use of this code must comply with the school's academic integrity policies.

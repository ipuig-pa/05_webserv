#this dockerfile you may need to build, py, php installed and corrected path in dock-conf

# Use an official Debian base image
FROM debian:bullseye

# Install necessary packages
RUN apt-get update && \
    apt-get install -y \
	valgrind g++ make \
	python3 \
	php \
    && apt-get clean

# Create app directory
WORKDIR /app

# Copy your webserv source code into the container
COPY . /app

# Build the project
RUN make clean && make

# Set entrypoint for valgrind
ENTRYPOINT ["valgrind", "--leak-check=full", "./webserv"]


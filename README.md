# TCP Server

A simple TCP echo server in C++ using **epoll** for high-performance I/O multiplexing. The server accepts multiple clients, reads incoming messages, prints them to the console, and echoes them back to the client.

## Features

- Non-blocking I/O using `epoll`
- Handles multiple clients concurrently
- Echoes messages back to the sender
- Prints received messages to the console


## Requirements

- Linux-based OS (epoll is Linux-specific)
- C++17 or newer
- `g++` or any modern C++ compiler
- Basic networking headers (`sys/socket.h`, `netinet/in.h`, etc.)

## TEST
By default set to 9001 connect using
telnet 127.0.0.1 9001
# or
nc 127.0.0.1 9001

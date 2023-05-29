#!/usr/bin/env python3

# client.py
# a socket client example

import socket

socket_cli = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)

ip_addr = '127.0.0.1'
port = 55667

socket_cli.connect((ip_addr, port))

data = socket_cli.recv(1024)

if data:
    print("Client connect to server")
    print(data.decode('utf-8'))

    while data:
        msg_input = input().encode('utf-8')

        socket_cli.send(msg_input)

        data = socket_cli.recv(1024)
        if data:
            print(f"Server: {data.decode('utf-8')}")

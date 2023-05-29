#!/usr/bin/env python3

# srv.py
# Socket server

import socket
import string

# Create a socket
srv_obj = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)

ip_addr = '127.0.0.1'
port = 55667
srv_obj.bind((ip_addr, port))
srv_obj.listen()

conn, _ = srv_obj.accept()

if conn:
    print("A client connected to the server")

    # Send a message to the client
    conn.send(b"Input a message")

    # Server receive message from client
    data = conn.recv(1024)

    while data != b'quit':
        print(f"Client Message: {data.decode('utf-8')}")
        srv_reply = "echo back> " + data.decode('utf-8')
        conn.send(srv_reply.encode('utf-8'))
        data = conn.recv(1024)

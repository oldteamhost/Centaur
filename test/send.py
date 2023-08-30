import socket

# Адрес и порт локального хоста
host = '127.0.0.1'
port = 33008

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((host, port))
message = "lol"
client_socket.send(message.encode())
client_socket.close()

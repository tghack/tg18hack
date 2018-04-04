#test
# run on vigenereshouse.tghack.com:9696
import socket
import threading

BIND_IP = '0.0.0.0'
BIND_PORT = 9696

def handle_client(client_socket):

    client_socket.send('TG18{ egcialkooamlkacusakacumdeaoaalegramlsouimdeocisxsocaaxkaoisl}')
    client_socket.close()

def tcp_server():
    server = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
    server.bind(( BIND_IP, BIND_PORT))
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.listen(5)
    print"[*] Listening on %s:%d" % (BIND_IP, BIND_PORT)

    while 1:
        client, addr = server.accept()
        print "[*] Accepted connection from: %s:%d" %(addr[0], addr[1])
        client_handler = threading.Thread(target=handle_client, args=(client,))
        client_handler.start()

if __name__ == '__main__':
    tcp_server()

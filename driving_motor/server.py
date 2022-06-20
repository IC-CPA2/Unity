
from socket import *
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

s.bind(('0.0.0.0', 12000))
s.listen(1)
try:
    while True:
        print("we are running:")
        conn, address = s.accept()

        content = conn.recvfrom(32)[0]
        if len(content) == 0:
            break

        else:
            print("debug", content)
        cmsg = "0.1>"
        # cmsg = cmsg.decode();
        conn.send(cmsg.encode())

        print("Maintaining connection")
        # s.close()

except KeyboardInterrupt:
    s.shutdown(1)
    print("kwyboRD SHUT")
    s.close()

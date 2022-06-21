from socket import *
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
s.bind(('0.0.0.0', 12001))
print("server is running")
s.listen(1)
try:
    while True:
        conn, address = s.accept()

        content = conn.recvfrom(32)[0]
        if len(content) == 0:
            break

        else:
            print("debug", content)
        cmsg = input(
            "please enter your message in form: (int;int;int;fl1;fl2;fl3;kp;ki;fl4)")
        # sample input is : 1;2;3;0.1;0.2;0.3;0.4;0.5;0.6>
        # cmsg = cmsg.decode();
        conn.send(cmsg.encode())

        print("Maintaining connection")
        # s.close()
except KeyboardInterrupt:
    s.shutdown(1)
    print("kwyboRD SHUT")
    s.close()

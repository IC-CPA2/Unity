from socket import *

#s = socket(family=AF_INET, type=SOCK_DGRAM)
s = socket(family=AF_INET, type=SOCK_STREAM)
s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

s.bind(('172.20.10.14', 5000))
s.listen(1)
try:
    while True:
        (client,address) = s.accept()
        while True:
            
            #print("hi")
            #bytesAddressPair = s.recvfrom(32)
            content = client.recvfrom(32)[0]
            #content = bytesAddressPair[0]
            #address = bytesAddressPair[1]
            if len(content) == 0:
                break
            
            else:
            
                print(content)


        
        #client.sendto(str.encode("hello"), address)
        client.send(str.encode("hello"))
                
        print("Closing connection")
        #s.close()

except KeyboardInterrupt:
    s.shutdown(1)
    print("kwyboRD SHUT")
    s.close()
from socket import *
from struct import *

def main(username, password, addr, port):
    ulen = len(username)
    plen = len(password)

    sock = socket(AF_INET, SOCK_STREAM)
    sock.connect(addr, port)
    
    sock.send(b"\x05\x01\x02") #用户名密码认证
    data = sock.recv(2)
    mode = data[1]
    if mode == 0xFF:
        print("you fucked up")
        sock.close()
        exit() 

    authentication = "\x01"+str(ulen)+username+str(plen)+password
    sock.send(authentication.encode(encoding='utf-8'))
    data = sock.recv(2)

    if data[1] != '\x00':
        print("username or passwd wrong")
        sock.close()
        exit()

    request = '\x05\x01\x00'
    

from socket import *
from struct import *
from re import *
from selectors import *
from threading import *

def isIPv4(str):
    p = compile('^((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$')
    if p.match(str):
        return True
    else:
        return False

def isIPv6(address):
    try:
        addr= inet_pton(AF_INET6, address)
    except error: # not a valid address
        return False
    return True

def send_data(sock, data): #tcp send data
    print(data)
    byte_sent = 0
    while True:
        r = sock.send(data[byte_sent:])
        if r < 0:
            return r
        byte_sent += r
        if byte_sent == len(data):
            return byte_sent

def handle_tcp(sock, remote):
    try:
        fdset = [sock, remote]
        while True:
            r, w, e = select.select(fdset, [], [])
            if sock in r:
                data = sock.recv(4096)
                if len(data) <= 0:
                    break
                result = send_data(remote, data)
                if result < len(data):
                    raise Exception("failed to send all data")
            
            if remote in r:
                data = remote.recv(4096)
                if len(data) <= 0:
                    break
                result = send_data(sock, data)
                if result < len(data):
                    raise Exception("failed to send all data")
    except Exception as e:
        raise(e)
    finally:
        sock.close()
        remote.close()

def socks5_con(username, password, addr, port, des_addr, des_port, localport):
    ulen = len(username)
    plen = len(password)

    sock = socket(AF_INET, SOCK_STREAM)
    sock.connect(addr, port)

    local = socket(AF_INET, SOCK_STREAM)
    local.connect('127.0.0.1', localport)
    
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

    request = '\x05\x01\x00' #VER+CMD+RSV

    if isIPv4(des_addr):
        request += '0x01'
        request += str(inet_pton(AF_INET,  des_addr))
    elif isIPv6(des_addr):
        request += '0x04'
        request += str(inet_pton(AF_INET6, des_addr))
    else:
        request += '0x03'
        request += str(len(des_addr)) + des_addr

    request += des_port
    
    sock.send(bytes(request))
    sock.recv(128)


    handle_tcp(sock, localport)


def main():
    socketServer = socket(AF_INET, SOCK_STREAM)
    socketServer.connect(('127.0.0.1', 8888))

    try:
        while True:
            t = Thread(target=socks5_con, args=('admin', 'admin', '127.0.0.1', 8888, 'www.baidu.com', 80, 8888))
            t.start()
    except error as e:
        error(e)
    except KeyboardInterrupt:
        socketServer.close()


if __name__ == '__main__':
    main()
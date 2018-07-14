from socket import *
from selectors import *
from threading import *
from logging import *
from struct import *

def Authentication(username, password):
    user_list = {'admin':'admin'}
    if username not in user_list:
        print('User not exist')
        return False
    elif user_list[username] != password:
        print('password is wrong')
        return False
    else :
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

def handle_con(sock, addr):
    # 接受客户端来的请求，socks5 的 认证和连接过程

    sock.recv(256)
    
    # 无需进一步认证信息
    sock.send(b"\x05\x00") #用户名密码认证 x05x02
    data = sock.recv(4) or '\x00' * 4
    # CMD 为 0x01 也就是 CONNECT 继续
    mode = data[1]
    if mode != 1:
        return
    # DST.ADDR 有三种形式，分别做判断
    addr_type = data[3]
    if addr_type == 1:
        addr_ip = sock.recv(4)
        remote_addr = inet_ntoa(addr_ip)
    elif addr_type == 3:
        addr_len = int.from_bytes(sock.recv(1), byteorder='big')
        remote_addr = sock.recv(addr_len)
    elif addr_type == 4:
        addr_ip = sock.recv(16)
        remote_addr = inet_ntop(AF_INET6, addr_ip)
    else:
        return
    # DST.PORT
    remote_addr_port = unpack('>H', sock.recv(2))

    # 返回给客户端 success
    reply = b"\x05\x00\x00\x01"
    reply += inet_aton('0.0.0.0') + pack(">H", 8888)
    sock.send(reply)

    # 拿到 remote address 的信息后，建立连接
    try:
        remote = create_connection((remote_addr, remote_addr_port[0]))
        info('connecting %s:%d' % (remote_addr, remote_addr_port[0]))
    except error as e:
        error(e)
        return

    handle_tcp(sock, remote)
    
def main():
    socketServer = socket(AF_INET, SOCK_STREAM)
    socketServer.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

    socketServer.bind(('', 8888))
    socketServer.listen(5)

    try:
        while True:
            sock, addr = socketServer.accept()
            t = threading.Thread(target=handle_con, args=(sock, addr))
            t.start()
    except error as e:
        error(e)
    except KeyboardInterrupt:
        socketServer.close()


if __name__ == '__main__':
    main()
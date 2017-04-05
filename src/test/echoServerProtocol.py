#coding=utf-8
import socket;
import ProxyMsg_pb2 as Msg;
import struct;

LISTEN_ADDR = ("0.0.0.0", 7000);

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1);
s.bind(LISTEN_ADDR);
s.listen(100);

def encodeInt(lenx):
    return struct.pack("I", socket.htonl(lenx));
    
def decodeInt(lenx):
    return socket.ntohl(struct.unpack("I", lenx)[0])

def recvUtil(sock, lenx):
    data = "".encode();
    while(True):
        tmp = sock.recv(lenx);
        data += tmp;
        lenx -= len(tmp);
        if(lenx == 0):
            break;
    return data;
    
while(True):
    client, addr = s.accept();
    print("Got connect from:{}".format(addr));
    while(True):
        try:
            ##auth
            lenx = decodeInt(recvUtil(client, 4));
            data = recvUtil(client, lenx);
            pkg = Msg.ProxyMsg();
            pkg.ParseFromString(data);
            print("Recv pkg:{}".format(pkg));
            if(pkg.subcmd != 0x1):
                print("Invalid!!!");
            auth = Msg.ProxyConnectReq();
            auth.ParseFromString(pkg.body);
            print("Recv auth:{}".format(auth));
            
            retPkg = Msg.ProxyMsg();
            retPkg.cmd = 0x500;
            retPkg.subcmd = 0x1;
            retPkg.seq = pkg.seq;
            retPkg.err_code = 0;
            result = Msg.ProxyConnectRsp();    
            result.result = 0;
            retPkg.body = result.SerializeToString();
            msg = retPkg.SerializeToString();
            client.send(encodeInt(len(msg)) + msg);
            ##data    
            while(True):
                lenx = decodeInt(recvUtil(client, 4));
                data = recvUtil(client, lenx);
                pkg = Msg.ProxyMsg();
                pkg.ParseFromString(data);
                print("Recv len:{}, data:{}".format(len(pkg.body), pkg.body));
                client.send(encodeInt(len(data)) + data);
        except Exception as e:
            print("Catch exception:{}".format(e));
            break;
    client.close();

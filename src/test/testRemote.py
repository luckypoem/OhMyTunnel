#coding = utf-8;

import socket;
import ProxyMsg_pb2 as Msg;
import struct;
import sys;

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
s.connect(("localhost", 2888));

def recvUtil(sock, lenx):
    data = "".encode();
    while(True):
        tmp = sock.recv(lenx);
        data += tmp;
        lenx -= len(tmp);
        if(lenx == 0):
            break;
    return data;

###hand shake
msg = Msg.ProxyMsg()
msg.place_holder = "".encode();
msg.cmd = 0x500;
msg.subcmd = 0x1;
msg.err_code = 0;

conn = Msg.ProxyConnectReq()
conn.type = 3;
conn.address = "localhost";
conn.port = 7000;
conn.username = "sen";
conn.password = "123456789"
msg.body = conn.SerializeToString();

data = msg.SerializeToString();
msg = struct.pack("I", socket.htonl(len(data))) + data;

print(msg);

s.send(msg);
lenx = recvUtil(s, 4);
lenx = socket.ntohl(struct.unpack("I", lenx)[0]);
print("Data len:{}".format(lenx));
buf = s.recv(lenx);
rsp = Msg.ProxyMsg();
rsp.ParseFromString(buf);
print("Read rsp:{}".format(rsp));

##data

string = "";
for i in range(10000):
    string += str(i) + "_";

dataList = [
    string * 5
]

msg = Msg.ProxyMsg();
msg.cmd = 0x500;
msg.subcmd = 0x2;
msg.err_code = 0;

for i in range(1000):
    count = 0;
    rcount = 0;
    for item in dataList:
        count += len(item);
        msg.body = item.encode();
        sendBuf = msg.SerializeToString();
        s.send(struct.pack("I", socket.htonl(len(sendBuf))) + sendBuf);
    while(True):
        lenx = recvUtil(s, 4);
        lenx = socket.ntohl(struct.unpack("I", lenx)[0]);
        data = recvUtil(s, lenx);
        pkg = Msg.ProxyMsg();
        pkg.ParseFromString(data);
        print("Recv data, len:{}, body:{}".format(len(pkg.body), pkg.body));
        rcount += len(pkg.body);
        if(rcount != count):
            print("Total count:{}, recv:{}, continue recv!".format(count, rcount));
        else:
            break;
    print("Recv end, no data miss!"); 




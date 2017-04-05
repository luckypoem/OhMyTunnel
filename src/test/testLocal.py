#coding = utf-8

import socket;
import struct;
import time;

LOCAL_SVR = ("127.0.0.1", 1888)
REQUEST_ADDR = ("localhost", 7000);

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
s.connect(LOCAL_SVR);

msg = bytes([0x5, 0x1, 0x0]);
s.send(msg);
buf = s.recv(1024);
print(buf);

msg = bytes([0x5, 0x1, 0x0, 0x3, len(REQUEST_ADDR[0])]);
msg += REQUEST_ADDR[0].encode();
msg += struct.pack("H", socket.htons(REQUEST_ADDR[1]));

s.send(msg);
buf = s.recv(1024);
print(buf);

#==============Begin test=====================
string = "";
for i in range(10000):
    string += str(i) + "_";

dataList = [
    string * 5
]
for c in range(100):
    count = 0;
    rcount = 0;
    for i in dataList:
        count += len(i);
        s.send(i.encode());
    while(True):
        rev = s.recv(1024);
        rcount += len(rev);
        print("Recv count:{}, data:{}".format(len(rev), rev));
        if(rcount == count):
            break;
        else:
            print("Total count:{}, recv:{}, continue recv!".format(count, rcount));
    print("Recv end, no data miss!");  
    #time.sleep(1);
s.close();

#coding = utf-8

import socket;
import threading;

LISTEN_ADDR = ("0.0.0.0", 7000);

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1);
s.bind(LISTEN_ADDR);
s.listen(100);

def process(client, addr):
    try:
        while(True):
            buf = client.recv(1024);
            if(len(buf) == 0):
                break;
            print("Recv len:{}, data:{}".format(len(buf), buf));
            client.send(buf);
    except Exception as e:
        print("Catch exception:{}".format(e));
        pass;
    print("Close client:{}".format(client));    
    client.close();    

while(True):
    client, addr = s.accept();
    print("Got connect from:{}, client:{}".format(addr, client));
    threading.Thread(target = process, args = (client, addr)).start();
    
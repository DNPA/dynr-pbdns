#!/usr/bin/python
import os
import json
import sys

if __name__ == '__main__':
    if os.system("/usr/bin/pbr-checkconfig.py"):
        sys.exit(1)
    infile=open("/etc/pbrouting.json","r")
    config=json.load(infile)
    infile.close()
    count=0
    for clientnet in config["devices"]["clients"]:
        ip=clientnet["ip"];
        print "Starting pbdnsd", count, "(listen on",ip,")"
        command="/usr/sbin/pbdnsd " + str(count) + " " + ip
        os.system(command)
        count=count+1;

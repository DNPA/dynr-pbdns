#!/usr/bin/python

# Copyright (C) 2004-2006 Red Hat Inc. <http://www.redhat.com/>
# Copyright (C) 2005-2007 Collabora Ltd. <http://www.collabora.co.uk/>
# Copyright (C) 2011 Dutch national police agency (KLPD)
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

import sys
import gobject
import os
import json
import dbus
import dbus.service
import dbus.mainloop.glib
import IPy
import daemon
import dns.query
import dns.message
import pwd
import syslog

class ConfigHelper:
    def __init__(self,config):
        self.config=config
    def getWorkstationGroup(self,workstationip):
        try:
            wsip=IPy.IP(workstationip)
        except ValueError:
            return None
        for clientnet in self.config["devices"]["clients"]:
            netrange=IPy.IP(clientnet["net"])
            if (netrange.overlaps(wsip)):
                return clientnet["groupname"]
        return None
    def checkGroupAllowed(self,clientgroup,gatewayip):
        for gateway in self.config["gateways"]:
            if (gateway["ip"] == gatewayip):
                for group in gateway["allowedgroups"]:
                    if group == clientgroup:
                        return True
        return False
    def checkAllowed(self,wsip,gwip):
        group=self.getWorkstationGroup(wsip)
        if group:
            if self.checkGroupAllowed(group,gwip):
                return True
        return False
    def getWsNum(self,workstationip):
        try:
            wsip=IPy.IP(workstationip)
        except ValueError:
            return None
        for clientnet in self.config["devices"]["clients"]:
            netrange=IPy.IP(clientnet["net"])
            if (netrange.overlaps(wsip)):
                num=wsip.ip-netrange.ip
                return num
        return None
    def getRtrNum(self,gatewayip):
        try:
            gwip=IPy.IP(gatewayip)
        except ValueError:
            return None
        for gateway in self.config["gateways"]:
            if gateway["ip"] == gatewayip:
                return gateway["tableno"]
        return None
    def findDnsdIP(self,wsip):
        try:
            wsip=IPy.IP(wsip)
        except ValueError:
            return None
        for clientnet in self.config["devices"]["clients"]:
            netrange=IPy.IP(clientnet["net"])
            if (netrange.overlaps(wsip)):
                dnsip=clientnet["ip"]
                return dnsip
        return None
        

class DaemonManager(dbus.service.Object):
    def __init__(self,object_path,config):
        name = dbus.service.BusName("nl.dnpa.pbdns.DaemonManager", dbus.SystemBus())
        self.confighelper=ConfigHelper(config)
        dbus.service.Object.__init__(self, dbus.SystemBus(), object_path)
    @dbus.service.method("nl.dnpa.pbdns.DaemonManager",in_signature='ss', out_signature='b')
    def setGateway(self, workstation, gateway):
        if (not self.confighelper.checkAllowed(workstation, gateway)):
            syslog.syslog(syslog.LOG_ERR,'setGateway : not allowed to set gateway to ' + str(gateway) + " for workstation "+ str(workstation) )
            return False
        wsNum  = self.confighelper.getWsNum(workstation)
        rtrNum = self.confighelper.getRtrNum(gateway)
        magicdnsname = "ws" + str(wsNum) + "-gw" + str(rtrNum) +  ".magicdomain.internal."
        dnsrequest = dns.message.make_query(magicdnsname,dns.rdatatype.A,dns.rdataclass.IN)
        dnsIp=self.confighelper.findDnsdIP(workstation)
        try:
            dnsresponse=dns.query.udp(dnsrequest,dnsIp,2)
        except dns.exception.Timeout:
            syslog.syslog(syslog.LOG_ERR,'setGateway : timeout while waiting for reply from dns server ' + str(dnsIp) + " for the query " + magicdnsname )
            return False
        rcode = dnsresponse.rcode()
        if (rcode == 0):
            if ((len(dnsresponse.answer) == 0) or (len(dnsresponse.answer[0].items) == 0) or (dnsresponse.answer[0].items[0].address == '127.0.0.1')):
                syslog.syslog(syslog.LOG_ERR,'setGateway : no YES ip as response from server ' + str(dnsIp) + ' for ' + magicdnsname )
                return False 
            return True
        else:
            syslog.syslog(syslog.LOG_ERR,'setGateway : rcode ' + str(rcode) +  ' != 0 from ' + str(dnsIp) + ' for the query ' + magicdnsname)
            return False
    @dbus.service.method("nl.dnpa.pbdns.DaemonManager",in_signature='s', out_signature='b')
    def clear(self,workstation):
        wsNum  = self.confighelper.getWsNum(workstation)
        magicname = "ws" + str(wsNum) + "-clear.magicdomain.internal."
        dnsrequest = dns.message.make_query(magicdnsname,dns.rdatatype.A,dns.rdataclass.IN)
        dnsIp=self.confighelper.findDnsdIP(workstation)
        try:
            dnsresponse=dns.query.udp(dnsrequest,dnsIp,2)
        except dns.exception.Timeout:
            return False
        rcode = dnsresponse.rcode
        if (rcode == 0):
            if ((len(dnsresponse.answer) == 0) or (len(dnsresponse.answer[0].items) == 0) or (dnsresponse.answer[0].items[0].address == '127.0.0.1')):
                return False
            return True
        return False

if __name__ == '__main__':
    if os.system("/usr/bin/pbr-checkconfig.py"):
        sys.exit(1)
    uid = gid = None
    uid = pwd.getpwnam("pbdnsdbs").pw_uid
    gid = pwd.getpwnam("pbdnsdbs").pw_gid
    with daemon.DaemonContext(uid=uid, gid=gid):
        syslog.openlog()
        syslog.syslog(syslog.LOG_ERR,'pbdns-dbus started')
        infile=open("/etc/pbrouting.json","r")
        config=json.load(infile)
        infile.close()
        dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
        name = dbus.service.BusName("nl.dnpa.pbdns.DaemonManager", dbus.SystemBus())
        object = DaemonManager('/DaemonManager',config)
        mainloop = gobject.MainLoop()
        mainloop.run()

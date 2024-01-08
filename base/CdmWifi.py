# -*- coding: utf-8 -*-
#
# (c) Copyright 2003-2015 HP Development Company, L.P.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# Author: Nitin Kumar
#

# StdLib
import time
import io
import binascii
import xml.parsers.expat
from string import *
import json, ast

# Local
from .g import *
from . import device, utils
from .sixext import to_bytes_utf8
from base.sixext import PY3, to_bytes_utf8, to_unicode, to_string_latin, to_string_utf8, xStringIO
from .sixext.moves import http_client
import json, ast
import sys
import time

MAX_RETRIES = 5
token = ''
adaptorId = ''
hostname=''

CDM_AUTH_REQ = "/cdm/oauth2/v1/token"
CDM_ADP_CONF = "/cdm/ioConfig/v2/adapterConfigs"
CDM_WIFI_SCAN = "/cdm/ioConfig/v2/wifiScan"
#CDM_WLAN_PROFILE = "/cdm/ioConfig/v2/wlanProfiles" #Old and deprecated replaced by wirelessConfig service
CDM_WIRELESS_CONFIG = "/cdm/ioConfig/v2/wirelessConfig"
CDM_WIFI_DIAG = "/ioConfig/v2/wifiDiagnostics"
CDM_SERVICE_DISCOVERY = "/cdm/servicesDiscovery" #this can be used to query device CDM services tree

HTTP_OK = 200
HTTP_CREATED = 201
HTTP_ACCEPTED = 202
HTTP_NOCONTENT = 204
HTTP_ERROR = 500
HTTP_UNAUTHORIZED = 401

def flushThePort(dev):
    response = io.BytesIO()
    timeout = 1
    dev.openEWS_LEDM()
    try:
        dev.readLEDMAllData(dev.readEWS_LEDM, response, timeout)
    except Error:            
        log.debug("Unable to read LEDM Channel")
    finally:
        dev.closeLEDM()

def getCDMToken(dev, uname, password):
    flushThePort(dev)
    global token
    data = {}
    data['grant_type'] = "password"
    data['username'] = uname
    data['password'] = password
    data = json.dumps(data)

    auth = 'token'
    data, respcode = http_post_req(dev, CDM_AUTH_REQ, data, auth)
    if not(respcode == HTTP_OK):
        log.debug("Request Failed With Response Code %d, enter correct credentials" % respcode)
        return False
    else:
        data = json.loads(data.strip())
        data = ast.literal_eval(json.dumps(data))
        token = data['access_token']
        return True

#Post Request
def http_post_req(dev, URI, data_json ,auth = None):
    global token
    dev.openEWS_LEDM()
    if token:        
        dev.writeEWS_LEDM("""POST %s HTTP/1.1\r\nContent-Type: application/json\r\nUser-Agent: hplip\r\nAccept: */*\r\nCache-Control: no-cache\r\nHost:localhost\r\nConnection: keep-alive\r\nContent-Length: %s\r\nAuthorization: Bearer %s\r\n\r\n"""%(URI,len(data_json), token))
    else:
        dev.writeEWS_LEDM("""POST %s HTTP/1.1\r\nContent-Type: application/json\r\nUser-Agent: hplip\r\nAccept: */*\r\nCache-Control: no-cache\r\nHost:localhost\r\nConnection: keep-alive\r\nContent-Length: %s\r\n\r\n"""%(URI,len(data_json)))

    dev.writeEWS_LEDM(data_json)
    reply = xStringIO()
    try:
        dev.readLEDMData(dev.readEWS_LEDM,reply)
        reply.seek(0)
        response = http_client.HTTPResponse(reply)
        response.begin()
        respcode = response.getcode()
        data = response.read()
        return data, respcode
    except :
        dev.closeEWS_LEDM()
        log.debug("Unable to read EWS_LEDM Channel")
        return "",HTTP_ERROR

#Patch Request
def http_patch_req(dev, URI, data):
    global token
    data_json = json.dumps(data)

    dev.openEWS_LEDM()
    if token:
        dev.writeEWS_LEDM("""PATCH %s HTTP/1.1\r\nContent-Type: application/json\r\nUser-Agent: hplip\r\nAccept: */*\r\nHost:localhost\r\nConnection: keep-alive\r\nContent-Length: %s\r\nAuthorization: Bearer %s\r\n\r\n"""%(URI,len(data_json), token))
    else:
        dev.writeEWS_LEDM("""PATCH %s HTTP/1.1\r\nContent-Type: application/json\r\nUser-Agent: hplip\r\nAccept: */*\r\nHost:localhost\r\nConnection: keep-alive\r\nContent-Length: %s\r\n\r\n"""%(URI,len(data_json)))
    dev.writeEWS_LEDM(data_json)
    reply = xStringIO()
    try:
        dev.readLEDMData(dev.readEWS_LEDM,reply)
        reply.seek(0)  
        response = http_client.HTTPResponse(reply)
        result = 10
        while result:
            try:
                response.begin()
                result = 0
            except:
                log.debug("Unable to begin response, retrying ...")
                time.sleep(5)
                result -= 1
                pass
        respcode = response.getcode()
        data = response.read()
        return data,respcode
    except Error:
        dev.closeEWS_LEDM()
        log.debug("Unable to read EWS_LEDM Channel")

#Get Request
def http_get_req(dev, URI):
    global token
    #headers = "{'Authorization': 'Bearer %s'}"%(token)
    dev.openEWS_LEDM()
    if token:
        dev.writeEWS_LEDM("""GET %s HTTP/1.1\r\nContent-Type: application/json\r\nUser-Agent: hplip\r\nAccept: */*\r\nCache-Control: no-cache\r\nHost:localhost\r\nConnection: keep-alive\r\nContent-Length: %s\r\nAuthorization: Bearer %s\r\n\r\n"""%(URI,len(URI),token))
    else:
        dev.writeEWS_LEDM("""GET %s HTTP/1.1\r\nContent-Type: application/json\r\nUser-Agent: hplip\r\nAccept: */*\r\nCache-Control: no-cache\r\nHost:localhost\r\nConnection: keep-alive\r\nContent-Length: %s\r\n\r\n"""%(URI,len(URI)))

    reply = xStringIO()
    try:
        dev.readLEDMData(dev.readEWS_LEDM,reply, 30)
        reply.seek(0)
        response = http_client.HTTPResponse(reply)
        response.begin()
        respcode = response.getcode()
        data = response.read()
        return data, respcode
    except Error:
        dev.closeEWS_LEDM()
        log.debug("Unable to read EWS_LEDM Channel")

def eth_connect_check(dev):
    global adaptorId
    data, respcode = http_get_req(dev, adaptorId)
    if not(respcode == HTTP_OK):
        log.debug("Request Failed With Response Code %d" % respcode)
        return
    data = json.loads(data.strip())
    #data = ast.literal_eval(json.dumps(data))

    rdata=""
    max_tries = 0
    while max_tries < MAX_RETRIES:
        max_tries += 1
        if data['connectionState'] == 'connected':
            break
        time.sleep(5)
        rdata, respcode = http_get_req(dev, adaptorId)
        if not(respcode == HTTP_OK):
            log.debug("Request Failed With Response Code %d" % respcode)
            return

    data = json.loads(rdata.strip())
    data = ast.literal_eval(json.dumps(data))
    return (data['connectionState'] == 'connected')

def getHostname(dev):
    global hostname
 
    return hostname

def getWifiAdaptorID(dev):
    global adaptorId
    rVal = []
    data, respcode = http_get_req(dev, CDM_ADP_CONF)
    if not(respcode == HTTP_OK):
        log.debug("Request Failed With Response Code %d" % respcode)
        return rVal
    data = json.loads(data.strip())
    data = ast.literal_eval(json.dumps(data))
    for each in data:
        if each == 'wifi0' or each == 'wifi1':
            rVal.append(data[each]['links'][0]['href'])
            rVal.append(data[each]['adapterName'])
    if(rVal):
        adaptorId = rVal[0]
    return rVal

def setAdaptorPower(dev, adaptor_list):
    state = ''
    presense = ''
    adaptor_id=-1
    adaptorName =""
    adaptor_id = adaptor_list[0]
    adaptorName = adaptor_list[1]
    d = {'enabled' : 'true'}
    data,respcode = http_patch_req(dev, adaptor_id, d)
    if respcode not in [HTTP_ACCEPTED,HTTP_NOCONTENT,HTTP_OK]:
        log.debug("Request Failed With Response Code %d" % respcode)
        return
    return adaptor_id, adaptorName, state, presense

def performScan(dev, adaptor_id):
    ret ={}
    '''
    #use this block to enable CDM service discovery
    data, respcode = http_get_req(dev, CDM_SERVICE_DISCOVERY)
    if respcode not in [HTTP_ACCEPTED,HTTP_NOCONTENT,HTTP_OK]:
        log.debug("CDM service discovery Failed With Response Code %d" % respcode)
    else:
        data = json.loads(data.strip())
        data = ast.literal_eval(json.dumps(data))
        log.debug("CDM service discovery tree: /n%s" %data)
    '''

    data, respcode = http_get_req(dev, CDM_WIFI_SCAN)
    if respcode not in [HTTP_ACCEPTED,HTTP_NOCONTENT,HTTP_OK]:
        log.debug("get cdm wifiscan Request Failed With Response Code %d" % respcode)
        return

    data = {'state' : 'scanProcessing', 'scanType'  : 'undirected'}
    rdata,respcode = http_patch_req(dev, CDM_WIFI_SCAN, data)
    if respcode not in [HTTP_ACCEPTED,HTTP_NOCONTENT,HTTP_OK]:
        log.debug("patch cdm wifiscan Request Failed With Response Code %d" % respcode)
        return

    scan_state = "scanProcessing"
    while(scan_state != "readyToScan"):
        data, respcode = http_get_req(dev, CDM_WIFI_SCAN)
        if respcode not in [HTTP_ACCEPTED,HTTP_NOCONTENT,HTTP_OK]:
            log.debug("get cdm wifiscan Request Failed With Response Code %d" % respcode)
            return
        else:
            data = json.loads(data.strip())
            data = ast.literal_eval(json.dumps(data))
            scan_state = data["state"]


    URI = "%s/%s" % (CDM_WIFI_SCAN, "wifiNetworks")
    data, respcode = http_get_req(dev, URI)
    if not(respcode == HTTP_OK):
        log.debug("get cdm wifiNetworks list Request Failed With Response Code %d" % respcode)
        return
    data = json.loads(data.strip())
    data = ast.literal_eval(json.dumps(data))
    elementCount = len(data['wifiNetworkList'])
    ret['numberofscanentries'] = elementCount
    ret['signalstrengthmin'] = 5
    ret['signalstrengthmax'] = 0
    if elementCount == 1:
        pass
    else:
        for a in range(elementCount):
            ret['ssid-%d' % a] = data['wifiNetworkList'][a]['ssid']
            ret['channel-%d' % a] = data['wifiNetworkList'][a]['channel']
            ret['communicationmode-%d' % a] = data['wifiNetworkList'][a]['communicationMode']
            ret['dbm-%d' % a] = data['wifiNetworkList'][a]['signalStrength']['dBm']
            ret['encryptiontype-%d' % a] = data['wifiNetworkList'][a]['encryptionType']
            ret['authenticationMode-%d' % a] =  data['wifiNetworkList'][a]['authenticationMode']
            try:
                ret['bssid-%d' % a] = data['wifiNetworkList'][a]['bssid']
            except KeyError:
                log.debug("Bssid not present in network -%s" %data['wifiNetworkList'][a]['ssid'])
                ret['bssid-%d' % a] = None

            try:
                ret['signalstrength-%d' % a] =  int(data['wifiNetworkList'][a]['signalStrength']['signalStrength'])
            except KeyError:
                ret['signalstrength-%d' % a] =  1
            try:
                ret['wpaVersionPreference-%d' % a] =  data['wifiNetworkList'][a]['wpaVersion']
            except KeyError:
                ret['wpaVersionPreference-%d' % a] =  None
            if ret['signalstrengthmax'] < int(ret['signalstrength-%d' % a]):
                ret['signalstrengthmax'] = int(ret['signalstrength-%d' % a])
            if ret['signalstrengthmin'] > int(ret['signalstrength-%d' % a]):
                ret['signalstrengthmin'] = int(ret['signalstrength-%d' % a])
    return ret

def getIPConfiguration(dev, adapterName):
    global hostname
    data, respcode = http_get_req(dev, CDM_ADP_CONF)
    if not(respcode == HTTP_OK):
        log.debug("get CDM_ADP_CONF Request Failed With Response Code %d" % respcode)
        return
    data = json.loads(data.strip())
    data = ast.literal_eval(json.dumps(data))
    for each in data:
        if each == 'wifi0' or each == 'wifi1':
            ip = data[each]['ipv4']['address']['ip']
            subnetmask = data[each]['ipv4']['address']['subnet']
            gateway = data[each]['ipv4']['address']['gateway']
            pridns = data[each]['ipv4']['dnsServer']['primary']['address']
            sec_dns = data[each]['ipv4']['dnsServer']['secondary']['address']
            hostname = data[each]['identity']['hostname']['name']
            addressmode = data[each]['ipv4']['address']['requestedConfigMethod']
            return ip, hostname, addressmode, subnetmask, gateway, pridns, sec_dns

def associate(dev, wpaVersionPreference, ssid, authenticationMode, security, key):
    if authenticationMode == 'wpaOrWpa2':
        authenticationMode = 'auto'
    ret= {}

    data, respcode = http_get_req(dev, CDM_WIRELESS_CONFIG)
    if not(respcode == HTTP_OK):
        log.error("get CDM_WIRELESS_CONFIG Request Failed With Response Code %d" % respcode)
        return
    data = json.loads(data.strip())
    data = ast.literal_eval(json.dumps(data))

    del data['version']

    preferredProfile=data['preferredProfile']
    data[preferredProfile]['authenticationMode'] = authenticationMode
    data[preferredProfile]['encryptionType'] = security
    data[preferredProfile]['ssid'] = ssid
    data[preferredProfile]['wpaVersionPreference'] = wpaVersionPreference
    data[preferredProfile]['passPhrase'] = key
        
    data, respcode = http_patch_req(dev, CDM_WIRELESS_CONFIG, data)
    if not(respcode == HTTP_NOCONTENT):
        log.debug("patch CDM_WIRELESS_CONFIG Request Failed With Response Code %d" % respcode)
        return

    if not(eth_connect_check(dev)):
        log.debug("wifi not connected, remove ethernet and try")

    return ret


def getVSACodes(dev, adapterName):
    ret = []
    return ret  

def getSignalStrength(dev, adapterName, ssid, adaptor_id=0):
    ss_max, ss_min, ss_val, ss_dbm = 5, 0, 0, -200
    return  ss_max, ss_min, ss_val, ss_dbm

def getCryptoSuite(dev, adapterName):
    alg, mode, secretid = '', '', ''
    return  alg, mode, secretid

def checkAuthrequired(dev):
    result = False
    _data, respcode = http_get_req(dev, CDM_ADP_CONF)
    if respcode in [HTTP_UNAUTHORIZED]:
        result = True
    return result
    

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
# Author: k,shunmugaraj
# Date Created: 10/10/2010

from __future__ import division
from pickle import NONE
import cupsext

# Std Lib
import sys
import os
import time
from base.sixext import BytesIO, to_bytes_utf8, to_unicode
import re
import threading
import struct
import time
import xml.parsers.expat as expat

from stat import *
# Local
from base.g import *
from base.codes import *
from base import device, utils, codes, dime, status
from base.sixext import to_bytes_utf8
from .fax import *

# StdLib
import time
import io
import binascii
import xml.parsers.expat
from string import *
import json
import ast

# Local
from base import device, utils
from base.sixext import PY3, to_bytes_utf8, to_unicode, to_string_latin, to_string_utf8, xStringIO
from base.sixext.moves import http_client
import json
import ast
import sys
import time
from prnt import cups

# **************************************************************************** #

CDM_AUTH_REQ = "/cdm/oauth2/v1/token"
CDM_FAX_MODEM_CONF = "/cdm/faxModem/v1/configuration"

REQ_NO_TOKEN = """%s %s HTTP/1.1\r\nContent-Type: application/json\r\nUser-Agent: hplip\r\nAccept: */*\r\nCache-Control: no-cache\r\nHost:%s\r\nConnection: keep-alive\r\nContent-Length: %s\r\n\r\n"""
REQ_WITH_TOKEN = """%s %s HTTP/1.1\r\nContent-Type: application/json\r\nUser-Agent: hplip\r\nAccept: */*\r\nCache-Control: no-cache\r\nHost:%s\r\nConnection: keep-alive\r\nContent-Length: %s\r\nAuthorization: Bearer %s\r\n\r\n"""

HTTP_UNAUTHORIZED = 401
HTTP_OK = 200
HTTP_ACCEPTED = 202
HTTP_CREATED = 201
HTTP_ERROR = 500
HTTP_SERVICE_UNAVALIABLE = 503


class CDMFaxDevice(FaxDevice):

    def __init__(self, device_uri=None, printer_name=None,
                 callback=None,
                 fax_type=FAX_TYPE_NONE,
                 disable_dbus=False):

        FaxDevice.__init__(self, device_uri,
                           printer_name,
                           callback, fax_type,
                           disable_dbus)
        self.token = ""
        self.send_fax_thread = None
        self.upload_log_thread = None
        self.device_uri = device_uri
        self.printer_name = printer_name
        if self.bus == 'net':
            self.http_host = self.host
        else:
            self.http_host = 'localhost'

    def isAuthRequired(self):
        return True

    def __flushThePort(self):
        response = io.BytesIO()

        # self.openEWS_LEDM()
        try:
            self.readLEDMAllData(self.readEWS_LEDM, response, 1)
        except Error:
            log.debug("Unable to read LEDM Channel")
        finally:
            self.closeLEDM()

    # Patch Request
    def __usb_reqeust(self, request_type, url, data_json=None, WithToken=False):

        if data_json == None:
            contentLen = len(url)
        else:
            contentLen = len(data_json)

        if WithToken == True:
            self.writeEWS_LEDM(REQ_WITH_TOKEN %
                               (request_type, url, self.http_host, contentLen, self.token))
        else:
            self.writeEWS_LEDM(REQ_NO_TOKEN %
                               (request_type, url, self.http_host, contentLen))

        if data_json != None:
            self.writeEWS_LEDM(data_json)

        reply = xStringIO()
        try:
            self.readLEDMData(self.readEWS_LEDM, reply)
            reply.seek(0)
            response = http_client.HTTPResponse(reply)
            response.begin()
            respcode = response.getcode()
            data = response.read()
            return respcode, data
        except Error:
            self.closeEWS_LEDM()
            log.debug("Unable to read EWS_LEDM Channel")

    def getCDMToken(self, uname, password):
        # self.__flushThePort()
        
        data = {}
        data['grant_type'] = "password"
        data['username'] = uname
        data['password'] = password
        data = json.dumps(data)
        respcode, data = self.__do_request('POST', CDM_AUTH_REQ, data)
        if not(respcode == HTTP_OK):
            log.debug(
                "Request Failed With Response Code %d, enter correct credentials" % respcode)
            return respcode, ""
        else:
            data = json.loads(data.strip())
            data = ast.literal_eval(json.dumps(data))
            self.token = data['access_token']
            return respcode

    def __do_request(self, request_type, url, data=None, WithToken=False):
        try:
            if self.bus == 'net':
                respcode, data = self.__network_reqeust(
                    request_type, url, data, WithToken)
                return respcode, data
            else:
                respcode, data = self.__usb_reqeust(
                    request_type, url, data, WithToken)
                return respcode, data
        except:
            log.debug("IO Error")
        return 400, ""

    def __network_reqeust(self, request_type, url, data=None, WithToken=False):

        import http.client
        import ssl
        header = {'Content-Type': 'application/json', 'User-Agent': 'hplip',
                  'Cache-Control': 'no-cache', 'Connection': 'keep-alive'}
        header['Host'] = self.host
        if WithToken == True:
            header['Authorization'] = 'Bearer %s' % self.token
        try:
            connection = http.client.HTTPSConnection(
                self.host, context=ssl._create_unverified_context())
            if data != None:
                connection.request(method=request_type,
                                   url=url, body=data, headers=header)
            else:
                connection.request(method=request_type,
                                   url=url, headers=header)
        except:
            log.debug("Error while connecting the device")
            return 400, ""

        response = connection.getresponse()
        respcode = response.status
        data = response.read()
        return respcode, data

    def setPhoneNum(self, num):
        rtn = False
        data = {}
        dataFaxNum = {}
        dataFaxNum['faxNumber'] = str(num)
        data['analogFaxSetup'] = dataFaxNum
        data = json.dumps(data)
        log.debug(data)
        try:
            respcode, data = self.__do_request(
                "PATCH", CDM_FAX_MODEM_CONF, data, WithToken=True)
            rtn = True
        except:
            log.debug("SetPhoneNum response is ", respcode)
        return rtn

    def getPhoneNum(self):
        try:
            fax_modem_details = cupsext.getFaxModemAttributes(
                self.device_uri, self.printer_name)
            log.dbg(fax_modem_details)
        except:
            return ''
        return (fax_modem_details.get('printer-fax-modem-number')).replace('tel:', '')

    phone_num = property(getPhoneNum, setPhoneNum)

    def setStationName(self, name):
        rtn = False
        #self.token = token
        data = {}
        dataComName = {}
        dataComName['companyName'] = str(name)
        data['analogFaxSetup'] = dataComName
        data = json.dumps(data)
        try:
            respcode, data = self.__do_request(
                "PATCH", CDM_FAX_MODEM_CONF, data, WithToken=True)
            rtn = True
        except:
            log.debug("setStationName response is ", respcode)
        return rtn

    def getStationName(self):
        try:
            fax_modem_details = cupsext.getFaxModemAttributes(
                self.device_uri, self.printer_name)
            log.dbg(fax_modem_details)
        except:
            return ''
        return fax_modem_details.get('printer-fax-modem-name')

    station_name = property(getStationName, setStationName)

    def setDateAndTime(self):
        t = time.localtime()
        date_buf = "%4d-%02d-%02dT%02d:%02d:%02d" % (
            t[0], t[1], t[2], t[3], t[4], t[5])
        return True

    def sendFaxes(self, phone_num_list, fax_file_list, cover_message='', cover_re='',
                  cover_func=None, preserve_formatting=False, printer_name='',
                  update_queue=None, event_queue=None):

        if not self.isSendFaxActive():

            self.send_fax_thread = CDMFaxSendThread(self, self.service, self.device_uri, self.printer_name,
                                                    phone_num_list, fax_file_list,
                                                    cover_message, cover_re, cover_func,
                                                    preserve_formatting,
                                                    update_queue,
                                                    event_queue)

            self.send_fax_thread.start()
            return True
        else:
            return False

# **************************************************************************** #


class CDMFaxSendThread(FaxSendThread):
    def __init__(self, dev, service, device_uri, printer_name, phone_num_list, fax_file_list,
                 cover_message='', cover_re='', cover_func=None, preserve_formatting=False,
                 update_queue=None, event_queue=None):

        FaxSendThread.__init__(self, dev, service, phone_num_list, fax_file_list,
                               cover_message, cover_re, cover_func, preserve_formatting,
                               printer_name, update_queue, event_queue)
        self.device_uri = device_uri
        self.printer_name = printer_name
        if dev.bus == 'net':
            self.http_host = "%s:8080" % self.dev.host
        else:
            self.http_host = 'localhost:8080'

        print(fax_file_list)

    def run(self):

        STATE_DONE = 0
        STATE_ABORTED = 10
        STATE_SUCCESS = 20
        STATE_BUSY = 25
        STATE_READ_SENDER_INFO = 30
        STATE_PRERENDER = 40
        STATE_COUNT_PAGES = 50
        STATE_NEXT_RECIPIENT = 60
        STATE_COVER_PAGE = 70
        STATE_SINGLE_FILE = 80
        STATE_MERGE_FILES = 90
        STATE_SINGLE_FILE = 100
        STATE_SEND_FAX = 110
        STATE_CLEANUP = 120
        STATE_ERROR = 130

        next_recipient = self.next_recipient_gen()

        state = STATE_READ_SENDER_INFO
        error_state = STATUS_ERROR
        self.rendered_file_list = []
        num_tries = 0

        while state != STATE_DONE:  # --------------------------------- Fax state machine
            if self.check_for_cancel():
                state = STATE_ABORTED

            log.debug("STATE=(%d, 0, 0)" % state)

            # ----------------------------- Aborted (10, 0, 0)
            if state == STATE_ABORTED:
                log.error("Aborted by user.")
                self.write_queue((STATUS_IDLE, 0, ''))
                state = STATE_CLEANUP

            # --------------------------- Success (20, 0, 0)
            elif state == STATE_SUCCESS:
                log.debug("Success.")
                self.write_queue((STATUS_COMPLETED, 0, ''))
                state = STATE_CLEANUP

            # ----------------------------- Error (130, 0, 0)
            elif state == STATE_ERROR:
                log.error("Error, aborting.")
                self.write_queue((error_state, 0, ''))
                state = STATE_CLEANUP

            # ------------------------------ Busy (25, 0, 0)
            elif state == STATE_BUSY:
                log.error("Device busy, aborting.")
                self.write_queue((STATUS_BUSY, 0, ''))
                state = STATE_CLEANUP


            elif state == STATE_READ_SENDER_INFO: # ------------------ Get sender info (30, 0, 0)
                log.debug("%s State: Get sender info" % ("*"*20))
                try:
                    self.sender_name = self.dev.station_name
                    log.debug("Sender name=%s" % self.sender_name)
                    self.sender_fax = self.dev.phone_num
                    log.debug("Sender fax=%s" % self.sender_fax)
                except Error:
                    log.error("CDM ipp request failed!")
                    state = STATE_ERROR
                state = STATE_PRERENDER

            elif state == STATE_PRERENDER: # --------------------------------- Pre-render non-G4 files (40, 0, 0)
                log.debug("%s State: Pre-render non-G4 files" % ("*"*20))
                state = self.pre_render(STATE_COUNT_PAGES)

            elif state == STATE_COUNT_PAGES: # -------------------------------- Get total page count (50, 0, 0)
                log.debug("%s State: Get total page count" % ("*"*20))
                #state = self.count_pages(STATE_NEXT_RECIPIENT)
                self.recipient_file_list = self.rendered_file_list[:]
                self.job_total_pages = len(self.fax_file_list)
                state = STATE_NEXT_RECIPIENT

            elif state == STATE_NEXT_RECIPIENT: # ----------------------------- Loop for multiple recipients (60, 0, 0)
                log.debug("%s State: Next recipient" % ("*"*20))
                state = STATE_COVER_PAGE

                try:
                    recipient = next(next_recipient)
                    log.debug("Processing for recipient %s" % recipient['name'])
                    self.write_queue((STATUS_SENDING_TO_RECIPIENT, 0, recipient['name']))
                except StopIteration:
                    state = STATE_SUCCESS
                    log.debug("Last recipient.")
                    continue

                recipient_file_list = self.rendered_file_list[:]

            elif state == STATE_COVER_PAGE: # ---------------------------------- Create cover page (70, 0, 0)
                log.debug("%s State: Render cover page" % ("*"*20))
                state = self.cover_page(recipient)

            elif state == STATE_SINGLE_FILE: # --------------------------------- Special case for single file (no merge) (80, 0, 0)
                log.debug("%s State: Handle single file" % ("*"*20))
                #state = self.single_file(STATE_SEND_FAX)
                state = self.merge_cdm_fax_files(STATE_SEND_FAX)
                #self.f = self.recipient_file_list[0][0]
                log.error(self.f)
                #state = STATE_SEND_FAX                

            elif state == STATE_MERGE_FILES: # --------------------------------- Merge multiple G4 files (90, 0, 0)
                log.debug("%s State: Merge multiple files" % ("*"*20))
                state = self.merge_cdm_fax_files(STATE_SEND_FAX)

            elif state == STATE_SEND_FAX: # ------------------------------------ Send fax state machine (110, 0, 0)
                log.debug("%s State: Send fax" % ("*"*20))
                state = STATE_NEXT_RECIPIENT

                FAX_SEND_STATE_DONE = 0
                FAX_SEND_STATE_ABORT = 10
                FAX_SEND_STATE_ERROR = 20
                FAX_SEND_STATE_BUSY = 25
                FAX_SEND_STATE_SUCCESS = 30
                FAX_SEND_STATE_DEVICE_OPEN = 40
                FAX_SEND_STATE_BEGINJOB = 50
                FAX_SEND_STATE_DOWNLOADPAGES = 60
                FAX_SEND_STATE_ENDJOB = 70
                FAX_SEND_STATE_CANCELJOB = 80
                FAX_SEND_STATE_CLOSE_SESSION = 170

                monitor_state = False
                fax_send_state = FAX_SEND_STATE_DEVICE_OPEN

                while fax_send_state != FAX_SEND_STATE_DONE:

                    if self.check_for_cancel():
                        log.error("Fax send aborted.")
                        fax_send_state = FAX_SEND_STATE_ABORT

                    log.debug("STATE=(%d, %d, 0)" % (STATE_SEND_FAX, fax_send_state))

                    if fax_send_state == FAX_SEND_STATE_ABORT: # ----------------- Abort (110, 10, 0)
                        monitor_state = False
                        fax_send_state = FAX_SEND_STATE_CANCELJOB
                        state = STATE_ABORTED

                    elif fax_send_state == FAX_SEND_STATE_ERROR: # --------------- Error (110, 20, 0)
                        log.error("Fax send error.")
                        monitor_state = False
                        fax_send_state = FAX_SEND_STATE_CLOSE_SESSION
                        state = STATE_ERROR

                    elif fax_send_state == FAX_SEND_STATE_BUSY: # ---------------- Busy (110, 25, 0)
                        log.error("Fax device busy.")
                        monitor_state = False
                        fax_send_state = FAX_SEND_STATE_CLOSE_SESSION
                        state = STATE_BUSY

                    elif fax_send_state == FAX_SEND_STATE_SUCCESS: # ------------- Success (110, 30, 0)
                        log.debug("Fax send success.")
                        monitor_state = False
                        fax_send_state = FAX_SEND_STATE_CLOSE_SESSION
                        state = STATE_NEXT_RECIPIENT

                    elif fax_send_state == FAX_SEND_STATE_DEVICE_OPEN: # --------- Device open (110, 40, 0)
                        log.debug("%s State: Open device" % ("*"*20))
                        fax_send_state = FAX_SEND_STATE_BEGINJOB

                    elif fax_send_state == FAX_SEND_STATE_BEGINJOB: # -------------- BeginJob (110, 50, 0)
                     
                        faxnum = recipient['fax']
                        aJobID = cupsext.sendFaxOutJob(self.device_uri,self.printer_name,self.f,faxnum) 
                        fax_send_state = FAX_SEND_STATE_ENDJOB
                    
                    elif fax_send_state == FAX_SEND_STATE_ENDJOB: # -------------- EndJob (110, 70, 0)
                        fax_send_state = FAX_SEND_STATE_SUCCESS                        

                    elif fax_send_state == FAX_SEND_STATE_CANCELJOB: # -------------- CancelJob (110, 80, 0)
                        log.debug("%s State: CancelJob" % ("*"*20))
                        if aJobID > 0:
                           cups.cancelJob(aJobID)                        
                        fax_send_state = FAX_SEND_STATE_CLOSE_SESSION                         

                    elif fax_send_state == FAX_SEND_STATE_CLOSE_SESSION: # -------------- Close session (110, 170, 0)
                        log.debug("%s State: Close session" % ("*"*20))
                        log.debug("Closing session...")                       
                        fax_send_state = FAX_SEND_STATE_DONE # Exit inner state machine

            elif state == STATE_CLEANUP: # --------------------------------- Cleanup (120, 0, 0)
                log.debug("%s State: Cleanup" % ("*"*20))

                if self.remove_temp_file:
                    log.debug("Removing merged file: %s" % self.f)
                    try:
                        os.remove(self.f)
                        log.debug("Removed")
                    except OSError:
                        log.debug("Not found")

                state = STATE_DONE # Exit outer state machine

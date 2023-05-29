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


# **************************************************************************** #

http_result_pat = re.compile(b"""HTTP/\d.\d\s(\d+)""", re.I)

HTTP_OK = 200
HTTP_ACCEPTED = 202
HTTP_CREATED = 201
HTTP_ERROR = 500
HTTP_SERVICE_UNAVALIABLE = 503

MAX_TRIES = 3

PIXELS_PER_LINE = 1728

# **************************************************************************** #
setDateTimeXML = """<?xml version=\"1.0\" encoding=\"UTF-8\"?><!-- THIS DATA SUBJECT TO DISCLAIMER(S) INCLUDED WITH THE PRODUCT OF ORIGIN.--><prdcfgdyn2:ProductConfigDyn xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:dd=\"http://www.hp.com/schemas/imaging/con/dictionaries/1.0/\" xmlns:prdcfgdyn2=\"http://www.hp.com/schemas/imaging/con/ledm/productconfigdyn/2009/03/16\" xmlns:prdcfgdyn=\"http://www.hp.com/schemas/imaging/con/ledm/productconfigdyn/2007/11/05\" xsi:schemaLocation=\"http://www.hp.com/schemas/imaging/con/ledm/productconfigdyn/2009/03/16 ../schemas/ledm2/ProductConfigDyn.xsd http://www.hp.com/schemas/imaging/con/ledm/productconfigdyn/2007/11/05 ../schemas/ProductConfigDyn.xsd http://www.hp.com/schemas/imaging/con/dictionaries/1.0/ ../schemas/dd/DataDictionaryMasterLEDM.xsd\"><prdcfgdyn2:ProductSettings><dd:TimeStamp>%s</dd:TimeStamp></prdcfgdyn2:ProductSettings></prdcfgdyn2:ProductConfigDyn>"""

setPhoneNumXML = """<?xml version=\"1.0\" encoding=\"UTF-8\"?><!--Sample XML file generated by XMLSPY v5 rel. 4 U (http://www.xmlspy.com)--><faxcfgdyn:FaxConfigDyn xmlns:faxcfgdyn=\"http://www.hp.com/schemas/imaging/con/ledm/faxconfigdyn/2009/03/03\" xmlns:dd=\"http://www.hp.com/schemas/imaging/con/dictionaries/1.0/\" xmlns:fax=\"http://www.hp.com/schemas/imaging/con/fax/2008/06/13\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.hp.com/schemas/imaging/con/ledm/faxconfigdyn/2009/03/03 ../schemas/FaxConfigDyn.xsd\"><faxcfgdyn:SystemSettings><dd:PhoneNumber>%s</dd:PhoneNumber></faxcfgdyn:SystemSettings></faxcfgdyn:FaxConfigDyn>"""

setStationNameXML = """<?xml version=\"1.0\" encoding=\"UTF-8\"?><!--Sample XML file generated by XMLSPY v5 rel. 4 U (http://www.xmlspy.com)--><faxcfgdyn:FaxConfigDyn xmlns:faxcfgdyn=\"http://www.hp.com/schemas/imaging/con/ledm/faxconfigdyn/2009/03/03\" xmlns:dd=\"http://www.hp.com/schemas/imaging/con/dictionaries/1.0/\" xmlns:fax=\"http://www.hp.com/schemas/imaging/con/fax/2008/06/13\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.hp.com/schemas/imaging/con/ledm/faxconfigdyn/2009/03/03 ../schemas/FaxConfigDyn.xsd\"><faxcfgdyn:SystemSettings><dd:CompanyName>%s</dd:CompanyName></faxcfgdyn:SystemSettings></faxcfgdyn:FaxConfigDyn>"""

createJobXML = """<?xml version=\"1.0\" encoding=\"UTF-8\"?><FaxPCSendDyn xmlns=\"http://www.hp.com/schemas/imaging/con/ledm/printtofaxdyn/2008/11/24\" xmlns:dd=\"http://www.hp.com/schemas/imaging/con/dictionaries/1.0/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.hp.com/schemas/imaging/con/ledm/printtofaxdyn/2008/11/24 ../schemas/FaxPCSendDyn.xsd\"><FaxPCSendConfig><FaxTxPhoneNumber>%s</FaxTxPhoneNumber><NumPages>%d</NumPages><TTI_Control>TTI_Off</TTI_Control></FaxPCSendConfig></FaxPCSendDyn>"""

pageConfigXML = """<?xml version=\"1.0\" encoding=\"UTF-8\" ?><FaxPCSendDyn xmlns=\"http://www.hp.com/schemas/imaging/con/ledm/printtofaxdyn/2008/11/24\" xmlns:dd=\"http://www.hp.com/schemas/imaging/con/dictionaries/1.0/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.hp.com/schemas/imaging/con/ledm/printtofaxdyn/2008/11/24 ../schemas/FaxPCSendDyn.xsd\"><PageConfig><PageNum>%d</PageNum><Width>1728</Width><Height>2200</Height><ImageType>BW</ImageType><Compression>mh</Compression><HorizontalDPI>%d</HorizontalDPI><VerticalDPI>%d</VerticalDPI></PageConfig></FaxPCSendDyn>"""

cancelJobXML = """<?xml version=\"1.0\" encoding=\"UTF-8\"?><Job xmlns=\"http://www.hp.com/schemas/imaging/con/ledm/jobs/2009/04/30\" xmlns:dd=\"http://www.hp.com/schemas/imaging/con/dictionaries/1.0/\" xmlns:fax=\"http://www.hp.com/schemas/imaging/con/fax/2008/06/13\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.hp.com/schemas/imaging/con/ledm/jobs/2009/04/30 ../schemas/Jobs.xsd\"><JobUrl>%s</JobUrl><JobState>Canceled</JobState></Job>"""

# **************************************************************************** #
class LEDMFaxDevice(FaxDevice):

    def __init__(self, device_uri=None, printer_name=None,
                 callback=None,
                 fax_type=FAX_TYPE_NONE,
                 disable_dbus=False):

        FaxDevice.__init__(self, device_uri,
                           printer_name,
                           callback, fax_type,
                           disable_dbus)

        self.send_fax_thread = None
        self.upload_log_thread = None

        if self.bus == 'net':
            self.http_host = self.host
        else:
            self.http_host = 'localhost'  

    def isAuthRequired(self):
        return False;  
       
    def put(self, url, post):
        data = """PUT %s HTTP/1.1\r
Connection: Keep-alive\r
User-agent: hplip/2.0\r
Host: %s\r
Content-length: %d\r
\r
%s""" % (url, self.http_host, len(post), post)
        log.log_data(data)
        self.writeLEDM(data.encode('utf-8'))
        response = BytesIO()

        while self.readLEDM(512, response, timeout=5):
            pass

        response = response.getvalue()
        log.log_data(response)
        self.closeLEDM()        
        
        match = http_result_pat.match(response)
        if match is None: return HTTP_OK
        try:
            code = int(match.group(1))
        except (ValueError, TypeError):
            code = HTTP_ERROR

        return code == HTTP_OK


    def setPhoneNum(self, num):
        
        xml = setPhoneNumXML %(num)
        log.debug("SetPhoneNum:xml Value:%s" %xml)
        return self.put("/DevMgmt/FaxConfigDyn.xml", xml)


    def getPhoneNum(self):
        return self.readAttributeFromXml("/DevMgmt/FaxConfigDyn.xml",'faxcfgdyn:faxconfigdyn-faxcfgdyn:systemsettings-dd:phonenumber')       
    phone_num = property(getPhoneNum, setPhoneNum)


    def setStationName(self, name):
        try:
           xml = setStationNameXML % name
        except(UnicodeEncodeError, UnicodeDecodeError):
           log.error("Unicode Error")
        
        return self.put("/DevMgmt/FaxConfigDyn.xml", xml)


    def getStationName(self):
        return to_unicode(self.readAttributeFromXml("/DevMgmt/FaxConfigDyn.xml",'faxcfgdyn:faxconfigdyn-faxcfgdyn:systemsettings-dd:companyname'))

    station_name = property(getStationName, setStationName) 

    def sendFaxes(self, phone_num_list, fax_file_list, cover_message='', cover_re='',
                  cover_func=None, preserve_formatting=False, printer_name='',
                  update_queue=None, event_queue=None):

        if not self.isSendFaxActive():

            self.send_fax_thread = LEDMFaxSendThread(self, self.service, phone_num_list, fax_file_list,
                                                     cover_message, cover_re, cover_func,
                                                     preserve_formatting,
                                                     printer_name, update_queue,
                                                     event_queue)

            self.send_fax_thread.start()
            return True
        else:
            return False

    def setDateAndTime(self):
        t = time.localtime()
        date_buf = "%4d-%02d-%02dT%02d:%02d:%02d" % (t[0], t[1], t[2], t[3], t[4], t[5])
        xml = setDateTimeXML %(date_buf)
        log.debug("setDateTimeXML Value:%s" %xml)
        
        if self.put("/DevMgmt/ProductConfigDyn.xml", xml):
            return True
        else:
            log.debug ("Failed to set date and time. Set date and time using front panel.")
            return False
    

# **************************************************************************** #
class LEDMFaxSendThread(FaxSendThread):
    def __init__(self, dev, service, phone_num_list, fax_file_list,
                 cover_message='', cover_re='', cover_func=None, preserve_formatting=False,
                 printer_name='', update_queue=None, event_queue=None):

        FaxSendThread.__init__(self, dev, service, phone_num_list, fax_file_list,
             cover_message, cover_re, cover_func, preserve_formatting,
             printer_name, update_queue, event_queue)       

        if dev.bus == 'net':
            self.http_host = "%s:8080" % self.dev.host
        else:
            self.http_host = 'localhost:8080'
        

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

        while state != STATE_DONE: # --------------------------------- Fax state machine
            if self.check_for_cancel():
                state = STATE_ABORTED

            log.debug("STATE=(%d, 0, 0)" % state)

            if state == STATE_ABORTED: # ----------------------------- Aborted (10, 0, 0)
                log.error("Aborted by user.")
                self.write_queue((STATUS_IDLE, 0, ''))
                state = STATE_CLEANUP


            elif state == STATE_SUCCESS: # --------------------------- Success (20, 0, 0)
                log.debug("Success.")
                self.write_queue((STATUS_COMPLETED, 0, ''))
                state = STATE_CLEANUP


            elif state == STATE_ERROR: # ----------------------------- Error (130, 0, 0)
                log.error("Error, aborting.")
                self.write_queue((error_state, 0, ''))
                state = STATE_CLEANUP           


            elif state == STATE_BUSY: # ------------------------------ Busy (25, 0, 0)
                log.error("Device busy, aborting.")
                self.write_queue((STATUS_BUSY, 0, ''))
                state = STATE_CLEANUP


            elif state == STATE_READ_SENDER_INFO: # ------------------ Get sender info (30, 0, 0)
                log.debug("%s State: Get sender info" % ("*"*20))
                state = STATE_PRERENDER
                try:
                    try:
                        self.dev.open()
                    except Error as  e:
                        log.error("Unable to open device (%s)." % e.msg)
                        state = STATE_ERROR
                    else:
                        try:
                            self.sender_name = self.dev.station_name
                            log.debug("Sender name=%s" % self.sender_name)
                            self.sender_fax = self.dev.phone_num
                            log.debug("Sender fax=%s" % self.sender_fax)
                        except Error:
                            log.error("LEDM GET failed!")
                            state = STATE_ERROR

                finally:
                    self.dev.close()


            elif state == STATE_PRERENDER: # --------------------------------- Pre-render non-G4 files (40, 0, 0)
                log.debug("%s State: Pre-render non-G4 files" % ("*"*20))
                state = self.pre_render(STATE_COUNT_PAGES)

            elif state == STATE_COUNT_PAGES: # -------------------------------- Get total page count (50, 0, 0)
                log.debug("%s State: Get total page count" % ("*"*20))
                state = self.count_pages(STATE_NEXT_RECIPIENT)

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
                state = self.single_file(STATE_SEND_FAX)

            elif state == STATE_MERGE_FILES: # --------------------------------- Merge multiple G4 files (90, 0, 0)
                log.debug("%s State: Merge multiple files" % ("*"*20))
                state = self.merge_files(STATE_SEND_FAX)

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

                    if monitor_state:
                        fax_state = self.getFaxDownloadState()
                        if not fax_state in (pml.UPDN_STATE_XFERACTIVE, pml.UPDN_STATE_XFERDONE):
                            log.error("D/L error state=%d" % fax_state)
                            fax_send_state = FAX_SEND_STATE_ERROR
                            state = STATE_ERROR

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
                        try:
                            self.dev.open()
                        except Error as e:
                            log.error("Unable to open device (%s)." % e.msg)
                            fax_send_state = FAX_SEND_STATE_ERROR
                        else:
                            if self.dev.device_state == DEVICE_STATE_NOT_FOUND:
                                fax_send_state = FAX_SEND_STATE_ERROR

                    elif fax_send_state == FAX_SEND_STATE_BEGINJOB: # -------------- BeginJob (110, 50, 0)
                        log.debug("%s State: BeginJob" % ("*"*20))
                        try:
                            ff = open(self.f, 'rb')
                        except IOError:
                            log.error("Unable to read fax file.")
                            fax_send_state = FAX_SEND_STATE_ERROR
                            continue
                        
                        try:
                            header = ff.read(FILE_HEADER_SIZE)
                        except IOError:
                            log.error("Unable to read fax file.")
                            fax_send_state = FAX_SEND_STATE_ERROR
                            continue                        
                        
                        magic, version, total_pages, hort_dpi, vert_dpi, page_size, \
                            resolution, encoding, reserved1, reserved2 = self.decode_fax_header(header)

                        if magic != to_bytes_utf8('hplip_g3'):
                            log.error("Invalid file header. Bad magic.")
                            fax_send_state = FAX_SEND_STATE_ERROR
                        else:
                            log.debug("Magic=%s Ver=%d Pages=%d hDPI=%d vDPI=%d Size=%d Res=%d Enc=%d" %
                                      (magic, version, total_pages, hort_dpi, vert_dpi, page_size,
                                       resolution, encoding))
                        
                        faxnum = recipient['fax'] 

                        createJob = createJobXML  %(faxnum, total_pages)
                        data = self.format_http_post("/FaxPCSend/Job",len(createJob),createJob)
                        log.log_data(data)

                        self.dev.openLEDM()
                        self.dev.writeLEDM(to_bytes_utf8(data))
                        response = BytesIO()
                        try:
                            while self.dev.readLEDM(512, response, timeout=5):
                                pass
                        except Error:
                            fax_send_state = FAX_SEND_STATE_ERROR
                            self.dev.closeLEDM() 
                            break
                        self.dev.closeLEDM()

                        response = response.getvalue()
                        log.log_data(response)
                        if self.get_error_code(response) == HTTP_CREATED:
                            fax_send_state = FAX_SEND_STATE_DOWNLOADPAGES
                        elif self.get_error_code(response) == HTTP_SERVICE_UNAVALIABLE and num_tries <= MAX_TRIES:
                            fax_send_state = FAX_SEND_STATE_BEGINJOB
                            num_tries += 1
                        else:
                            if num_tries > MAX_TRIES:
                                log.error("HTTP ERROR CODE: 531, Server Temporary Unavailable")
                            fax_send_state = FAX_SEND_STATE_ERROR
                            log.error("Create Job request failed")
                            break
                        pos = response.find(b"/Jobs/JobList/",0,len(response))
                        pos1 = response.find(b"Content-Length",0,len(response))
                        pos2 = response.find(b"Cache-Control",0,len(response))
                        jobListURI = response[pos:pos1].strip()
                        jobListURI = jobListURI.replace(b'\r',b'').replace(b'\n',b'')
                        if jobListURI == b'':
                            jobListURI = response[pos:pos2].strip()
                            jobListURI = jobListURI.replace(b'\r',b'').replace(b'\n',b'')
                            log.debug("jobListURI = [%s] type=%s" %(jobListURI, type(jobListURI)))
                        if type(jobListURI) != str:
                             jobListURI = jobListURI.decode('utf-8')

                    elif fax_send_state == FAX_SEND_STATE_DOWNLOADPAGES: # -------------- DownloadPages (110, 60, 0)
                        log.debug("%s State: DownloadPages" % ("*"*20))
                        page = BytesIO()
                        log.debug("Total Number of pages are:%d" %total_pages)
                        for p in range(total_pages):

                            if self.check_for_cancel():
                                fax_send_state = FAX_SEND_STATE_ABORT

                            if fax_send_state == FAX_SEND_STATE_ABORT:
                                break

                            try:
                                header = ff.read(PAGE_HEADER_SIZE)
                            except IOError:
                                log.error("Unable to read fax file.")
                                fax_send_state = FAX_SEND_STATE_ERROR
                                continue

                            page_num, ppr, rpp, bytes_to_read, thumbnail_bytes, reserved2 = \
                                self.decode_page_header(header)

                            log.debug("Page=%d PPR=%d RPP=%d BPP=%d Thumb=%d" %
                                      (page_num, ppr, rpp, bytes_to_read, thumbnail_bytes))

                            if ppr != PIXELS_PER_LINE:
                                log.error("Pixels per line (width) must be %d!" % PIXELS_PER_LINE)

                            page.write(ff.read(bytes_to_read))
                            thumbnail = ff.read(thumbnail_bytes) # thrown away for now (should be 0 read)
                            page.seek(0)

                            try:
                                data = page.read(bytes_to_read)                                
                            except IOError:
                                log.error("Unable to read fax file.")
                                fax_send_state = FAX_SEND_STATE_ERROR
                                break

                            if data == b'':
                                log.error("No data!")
                                fax_send_state = FAX_SEND_STATE_ERROR
                                break
                            
                            pageConfigURI = self.dev.readAttributeFromXml(jobListURI,"j:job-faxpcsendstatus-resourceuri")
                            log.debug("pageConfigURI:[%s]" %pageConfigURI)  

                            pageConfig = pageConfigXML %(page_num,hort_dpi,vert_dpi)
                            xmldata = self.format_http_post(pageConfigURI,len(pageConfig),pageConfig)
                            log.log_data(xmldata) 
                           
                            self.dev.openLEDM()
                            try:
                                self.dev.writeLEDM(xmldata)
                            except Error:
                                fax_send_state = FAX_SEND_STATE_ERROR
                                self.dev.closeLEDM() 
                                break

                            response = BytesIO()
                            try:
                                while self.dev.readLEDM(512, response, timeout=5):
                                    pass
                            except Error:
                                fax_send_state = FAX_SEND_STATE_ERROR
                                self.dev.closeLEDM()
                                break

                            self.dev.closeLEDM()
                            response = (response.getvalue())
                            log.log_data(response)
                            if self.get_error_code(response) != HTTP_ACCEPTED:
                                fax_send_state = FAX_SEND_STATE_ERROR
                                log.error("Page config data is not accepted by the device")
                                break                                                    
                                                   
                            pageImageURI = self.dev.readAttributeFromXml(jobListURI,"j:job-faxpcsendstatus-resourceuri")                                
                            while(True):
                                if self.check_for_cancel():
                                    fax_send_state = FAX_SEND_STATE_ABORT
                                    break
 
                                Status, Fax_State = self.checkForError(jobListURI)
                                if Status == FAX_SEND_STATE_ERROR and (Fax_State == STATUS_ERROR_IN_TRANSMITTING or
                                    Fax_State == STATUS_ERROR_IN_CONNECTING or Fax_State == STATUS_ERROR_PROBLEM_IN_FAXLINE or
                                    Fax_State == STATUS_JOB_CANCEL):
                                    log.debug("setting state to FAX_SEND_STATE_ERROR")
                                    fax_send_state = FAX_SEND_STATE_ERROR
                                    error_state = Fax_State
                                    break
                                elif Status == FAX_SEND_STATE_SUCCESS:
                                    break  
                         
                            if fax_send_state == FAX_SEND_STATE_ABORT or fax_send_state  == FAX_SEND_STATE_ERROR:
                                break
                          
                            
                            xmldata = self.format_http_post(pageImageURI,len(data),"","application/octet-stream")
                            log.debug("Sending Page Image XML Data [%s] to the device" %xmldata)                           
                            self.dev.openLEDM()
                            self.dev.writeLEDM(xmldata)
                            log.debug("Sending Raw Data to printer............")
                            try:
                                self.dev.writeLEDM(data)
                            except Error:
                                fax_send_state = FAX_SEND_STATE_ERROR
                                self.dev.closeLEDM() 
                                break  
                              
                            response = BytesIO()
                            try:
                                while self.dev.readLEDM(512, response, timeout=30):
                                    pass
                            except Error:
                                fax_send_state = FAX_SEND_STATE_ERROR
                                self.dev.closeLEDM()
                                break
                            
                            self.dev.closeLEDM()
                            response = response.getvalue()
                            log.log_data(response)
    
                            if self.get_error_code(response) != HTTP_ACCEPTED:
                                log.error("Image Data is not accepted by the device")
                                fax_send_state = FAX_SEND_STATE_ERROR
                                break                   
                                               
                            page.truncate(0)
                            page.seek(0)                           

                        else:
                            fax_send_state = FAX_SEND_STATE_ENDJOB


                    elif fax_send_state == FAX_SEND_STATE_ENDJOB: # -------------- EndJob (110, 70, 0)
                        fax_send_state = FAX_SEND_STATE_SUCCESS
                        

                    elif fax_send_state == FAX_SEND_STATE_CANCELJOB: # -------------- CancelJob (110, 80, 0)
                        log.debug("%s State: CancelJob" % ("*"*20))                        
                        
                        xmldata = cancelJobXML %(jobListURI)                        
                        data = self.format_http_put(jobListURI,len(xmldata),xmldata)
                        log.log_data(data)
                        
                        self.dev.openLEDM()
                        self.dev.writeLEDM(to_bytes_utf8(data))
                        
                        response = BytesIO()
                        try:
                            while self.dev.readLEDM(512, response, timeout=10):
                                pass
                        except Error:
                            fax_send_state = FAX_SEND_STATE_ERROR
                            self.dev.closeLEDM()
                            break
                        self.dev.closeLEDM()
                        response = response.getvalue()
                        log.log_data(response)

                        if self.get_error_code(response) == HTTP_OK:
                            fax_send_state = FAX_SEND_STATE_CLOSE_SESSION
                        else:
                            fax_send_state = FAX_SEND_STATE_ERROR
                            log.error("Job Cancel Request Failed")
                          

                    elif fax_send_state == FAX_SEND_STATE_CLOSE_SESSION: # -------------- Close session (110, 170, 0)
                        log.debug("%s State: Close session" % ("*"*20))
                        log.debug("Closing session...")                       

                        try:
                            ff.close()
                        except NameError:
                            pass

                        #time.sleep(1)

                        self.dev.closeLEDM()
                        self.dev.close()

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


    def get_error_code(self, ret):
        if not ret: return HTTP_ERROR
        
        match = http_result_pat.match(ret)
        if match is None: return HTTP_OK
        try:
            code = int(match.group(1))
        except (ValueError, TypeError):
            code = HTTP_ERROR
        return code        
        
    def checkForError(self,uri):
        stream = BytesIO()
        data = self.dev.FetchLEDMUrl(uri)
        if not data:
            log.error("Unable To read the XML data from device")
            return ""

        xmlDict = utils.XMLToDictParser().parseXML(data)
        log.debug("Read Attribute:%s and it is value:%s" %(uri,data))

        FAX_SEND_STATE_ERROR = 20
        FAX_SEND_STATE_SUCCESS = 30
        state = FAX_SEND_STATE_ERROR
        Fax_send_state = STATUS_ERROR   

        if cmp(xmlDict['j:job-faxpcsendstatus-faxtxmachinestatus'],"Transmitting")==0 \
            and cmp(xmlDict['j:job-faxpcsendstatus-faxtxerrorstatus'],"CommunicationError")== 0:
            state = FAX_SEND_STATE_ERROR
            Fax_send_state = STATUS_ERROR_IN_TRANSMITTING
        elif(cmp(xmlDict['j:job-faxpcsendstatus-faxtxmachinestatus'],"Connecting")==0 \
            and cmp(xmlDict['j:job-faxpcsendstatus-faxtxerrorstatus'],"NoAnswer")== 0):
            state = FAX_SEND_STATE_ERROR
            Fax_send_state = STATUS_ERROR_IN_CONNECTING
        elif(cmp(xmlDict['j:job-faxpcsendstatus-faxtxerrorstatus'],"PcDisconnect")==0 \
            and cmp(xmlDict['j:job-faxpcsendstatus-pagestatus-state'],"Error")== 0):
            state = FAX_SEND_STATE_ERROR
            Fax_send_state = STATUS_ERROR_PROBLEM_IN_FAXLINE
        elif(cmp(xmlDict['j:job-faxpcsendstatus-faxtxerrorstatus'],"Stop")==0 \
            and cmp(xmlDict['j:job-faxpcsendstatus-pagestatus-state'],"Error")== 0):
            state = FAX_SEND_STATE_ERROR
            Fax_send_state = STATUS_JOB_CANCEL 
        elif(cmp(xmlDict['j:job-faxpcsendstatus-faxtxmachinestatus'],"Transmitting")== 0):
            state = FAX_SEND_STATE_SUCCESS
            Fax_send_state = FAX_SEND_STATE_SUCCESS
        return state,Fax_send_state   

    def format_http_post(self, requst, ledmlen, xmldata, content_type="text/xml; charset=utf-8"):
        host = self.http_host      
        
        return utils.cat(
"""POST $requst HTTP/1.1\r
Host: $host\r
User-Agent: hplip/2.0\r
Content-Type: $content_type\r
Content-Length: $ledmlen\r
Connection: Keep-alive\r
SOAPAction: ""\r
\r
$xmldata""")
    
    def format_http_put(self, requst, ledmlen, xmldata, content_type="text/xml; charset=utf-8"):
        host = self.http_host
        return  utils.cat(
"""PUT $requst HTTP/1.1\r
Host: $host\r
User-Agent: hplip/2.0\r
Content-Type: $content_type\r
Content-Length: $ledmlen\r
\r
$xmldata""")

        





   

# -*- coding: utf-8 -*-
#
# (c) Copyright 20013 HP Development Company, L.P.
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
# Author: Sarbeswar Meher
#
# HPLIP
import sys
import os
from .g import *
import socket
from subprocess import Popen, PIPE
from . import utils
from .sixext import  to_string_utf8
import re


def detectLocalDevices(ttl=4, timeout=10):
	found_devices = {}

	if utils.which("avahi-browse") == '':
		log.error("Avahi-browse is not installed")
		return found_devices

	# Obtain all the resolved services which has service type '_ipp._tcp' from avahi-browse
	p = Popen(['avahi-browse', '-kprt', '_ipp._tcp'], stdout=PIPE)
	output = to_string_utf8(p.communicate()[0])
	#log.error("output is %s " % output)
	for line in output.splitlines():
		if line.startswith('='):
			bits = line.split(';')  
                        # Filter out only local usb printers         
			if bits[1] == 'lo' and bits[2] == 'IPv4':
				try:					
					y = {'printer_name':'', 'printer_uri':''}
					details = bits[9].split('" "')
					for item in details:
						key, value = item.split('=', 1)
						if key == 'usb_MDL':
							y['printer_name'] =value                               
					unformattedString = str(bits[3])
					#log.debug('Unformatted String: '+ bits[3])
					formattedString = unformattedString.replace('\\032','%20').replace('\\091','%5B').replace('\\093','%5D').replace('\\040','(').replace('\\041',')')
					#log.debug('Formatted URI at end: ', formattedString)
					final_uri = "ipp://"+ formattedString +"._ipp._tcp.local/"
					log.debug(final_uri)
					y['printer_uri'] =final_uri
					found_devices = y                       
				except socket.gaierror:
					pass
	log.debug("Found %d devices" % len(found_devices))

	return found_devices


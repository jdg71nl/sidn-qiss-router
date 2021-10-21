#!/usr/bin/env python

# Python info:
# https://docs.python.org/2.7/library/argparse.html
# https://docs.python.org/2/howto/argparse.html
import argparse

import json
import pprint
import re # regular expression
import sys
import os
import time

# http://stromberg.dnsalias.org/~strombrg/reblock.html
# https://docs.python.org/2.7/howto/sockets.html
# https://docs.python.org/2/library/socket.html
# use ?? https://docs.python.org/2/library/select.html
import socket

# https://en.wikipedia.org/wiki/Ncurses
# https://en.wikipedia.org/wiki/Curses_(programming_library)
# http://invisible-island.net/ncurses/ncurses.faq.html
# http://www.c-for-dummies.com/ncurses/
# https://docs.python.org/2/library/curses.html
# https://docs.python.org/2/howto/curses.html#curses-howto
# http://www.tldp.org/HOWTO/html_single/NCURSES-Programming-HOWTO/
import curses

# another way then curses:
# http://stackoverflow.com/questions/3173320/text-progress-bar-in-the-console
# Sys.stdout.write('%s [%s] %s%s %s\r' % (prefix, bar, percents, '%', suffix)),
# Sys.stdout.flush()

# https://docs.python.org/3/library/ipaddress.html
# not in 2.7 .... :-(
# Ah! here it is:
# https://pypi.python.org/pypi/ipaddress
#import ipaddress

# how to use PyPi ??
# https://python-packaging-user-guide.readthedocs.org/en/latest/
# https://github.com/pypa/python-packaging-user-guide

# http://stackoverflow.com/questions/846038/convert-a-python-int-into-a-big-endian-string-of-bytes
# http://stackoverflow.com/questions/27506474/how-to-byte-swap-a-32-bit-integer-in-python
# https://docs.python.org/2/library/struct.html
import struct

# https://docs.python.org/2/library/datetime.html
import datetime

# python - tutorial.pdf:
# from collections import deque
# queue = deque(["Eric", "John", "Michael"])
# queue.append("Terry")
# myString = queue.popleft()
# https://docs.python.org/2/library/collections.html
# https://docs.python.org/2/library/collections.html#collections.deque
# collections.deque(["Eric", "John", "Michael"])
#import collections

# https://docs.python.org/2/library/string.html
# https://docs.python.org/2/library/stdtypes.html#string-formatting
# string formatting using %

# https://docs.python.org/2/library/math.html
import math

# https://docs.python.org/2/library/string.html (incl deprecated string functions, use below):
# https://docs.python.org/2/library/stdtypes.html#string-methods
# https://docs.python.org/2/library/stdtypes.html#string-formatting
# string formatting using %

qiss_ssdis_port = 24011		# qiss-ssdis   24011/udp
qiss_ssreg_port = 24012		# qiss-ssreg   24012/udp
qiss_nwdis_port = 24021		# qiss-nwdis   24021/udp
									# qiss-nwdis   24021/tcp
qiss_nwres_port = 24022		# qiss-nwres   24022/udp
qiss_modis_port = 24031		# qiss-modis   24031/udp
qiss_mocon_port = 24032		# qiss-mocon   24032/tcp
qiss_modat_port = 24033		# qiss-modat   24033/udp
qiss_tgcon_port = 24041		# qiss-tgcon   24041/udp
qiss_nscon_port = 24051		# qiss-nscon   24051/udp

port_qsdev = 24081
port_qsbox = 24082
port_qssim = 24083
port_qsgna = 24084
port_qsgnb = 24085

PROGNAME = "box-monitor.py - QISS live flow.table monitor"

const_EOF = "{EOF}"
const_EOL = "\n"

parser = argparse.ArgumentParser(
  formatter_class=argparse.RawDescriptionHelpFormatter,
  description= PROGNAME+" (live monitor of /proc/qiss/tcpo/flows.table)",
)
parser.add_argument('--window', action='store_true', help='Start the monitor in fullscreen window')
parser.add_argument('--lines', action='store_true', help='Start the monitor in line-output')
parser.add_argument('--screen', action='store_true', help='Start the monitor in screen-output')
parser.add_argument('--flows', action='store_true', help='Start the monitor in flow-export mode')
#parser.add_argument('mode', choices=['window', 'lines', 'screen', 'flows'])

parser.add_argument('--local', action='store_true', help='Monitor based on local PROC file access')
parser.add_argument('--remote', metavar='ADDRESS', help='Monitor based on remote TCP access')
parser.add_argument('--remote1', action='store_true', help='Monitor based on remote TCP access to Lab1')
parser.add_argument('--remote2', action='store_true', help='Monitor based on remote TCP access to Lab2')
parser.add_argument('--interval', metavar='INTERVAL', help='interval in [10..1000] milliseconds')
parser.add_argument('--blocks', metavar='BLOCKS', help='number of screen blocks on 1 screen')
args = parser.parse_args()
if len(sys.argv)==1:
	parser.print_help()
	sys.exit(1)

interval = 1000
if args.interval:
	interval = int( args.interval )
	if not ( ( interval >= 10) or (interval >= 1000) ):
		print "# error: provide 'interval' between 10..1000 milliseconds."
		sys.exit(1)

blocks = 1000
if args.blocks:
	blocks = int( args.blocks )
	if not ( ( blocks >= 1) or (blocks <= 10) ):
		print "# error: provide 'blocks' between 1..10"
		sys.exit(1)

flows_____filename = "/proc/qiss/tcpo/flows.table"
timestamp_filename = "/proc/qiss/tcpo/timestamp"

timestamp_defaults = {"timestamp": {"seconds":0,"microseconds":0}}
flows_____defaults = {"flows":[{"key":"value"}]}

linerate_log10 = 8
linerate = pow(10, linerate_log10)	# in bps: log10=8 -> linerate = 1E8 = 100 Mbps
alpha = (1.0/50.0)
active_limit = 1E1 #linerate / 1E5

def LogHex(rate, linerate_log10):
	if rate==0:
		return 0
	log_f = math.log(rate, 10) * (16.0/linerate_log10)
	log = min(max( int(log_f), 0),15)
	#return (log_f, log)
	return log

def LogHexString(linerate_log10):
	bps = ["x"]*16
	for s in range(16):
		x = pow(10, float(s)*float(linerate_log10)/16.0)
		if (x/1E3) < 1:
			bps[s] = "%X:%db" % (s, x)
		elif (x/1E6) < 1:
			bps[s] = "%X:%dk" % (s, x/1E3)
		elif (x/1E9) < 1:
			bps[s] = "%X:%dM" % (s, x/1E6)
		elif (x/1E12) < 1:
			bps[s] = "%X:%dG" % (s, x/1E9)
		else:
			bps[s] = "%X:%dTbps" % (s, x/1E12, bpsstring)
	return "LogHex[0-F]=16*Log10(rate-bps)/Log10(linerate-bps) " + ",".join(bps)

#print str( ipaddress.IPv4Address(530999897) )
# 31.166.106.89
RING_SIZE = 104
TIME_RING = [ ["_","_"] ] * RING_SIZE
#FLOW_RING = [ { "key": "_" } ] * RING_SIZE
#FLOW_RING = [ {} ] * RING_SIZE
#BARE_RING = [ { "time": ["_","_"], "flow": { "key": "_" } } ] * RING_SIZE

remote = ""
if args.remote1:
	remote = "10.8.0.20"
if args.remote2:
	remote = "10.88.0.20"
if args.remote:
	remote = args.remote

method = "monitor"
if args.flows:
	method = "flowexport"
if args.window:
	method = "monitor"

def read_multiMessageString(mySocket):
	#
	dataString = ""
	multiMessageString = ""
	completeString = False
	count = 0
	while not completeString:
		dataString = mySocket.recv(1024*10)
		multiMessageString = multiMessageString + dataString
		if dataString.count(const_EOF) >= 1:
			completeString = True
			multiMessageString = multiMessageString.replace(const_EOF, "")
		else:
			if dataString.count(const_EOL) >= 1:
				completeString = True
		count = count + 1
		if count > 10:
			print "# ERROR mySocket.recv() count too much"
			continue
	#
	return multiMessageString
	#


mySocket = 0 # default in case --local
if remote:
	#remote = args.remote
	print "# starting remote monitoring on address '{}' and interval {} milliseconds...".format(remote, interval)
	out_json = '{"jsonrpc": "2.0","method":"%s","params":{"interval":%s},"id":1}' % (method, interval)
	print "out_json = " + out_json
	#exit(1)
	mySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try:
		#mySocket.connect((remote, qiss_mocon_port))
		mySocket.connect( (remote, port_qsbox) )
		mySocket.sendall(out_json)
	except:
		print "# ERROR: not able to connect to remote port {}:{}".format(remote, port_qsbox)
		exit(1)
	#
	multiMessageString = read_multiMessageString(mySocket)
	#
	print "in_json = {}".format(multiMessageString)
	try:
		a_struct = json.loads(multiMessageString)
		if not a_struct["jsonrpc"] == "2.0" and a_struct["result"]["success"]:
			print "# Error no valid JSON response"
			exit(1)
	except:
		print "# ERROR parsing JSON from TCP socket"
		exit(1)
	#

def main(win):
	global stdscr
	stdscr = win
	# a color pair consists of a foreground and background color. start_color() initializes 8 basic colors when it activates color mode. They are: 0:black, 1:red, 2:green, 3:yellow, 4:blue, 5:magenta, 6:cyan, and 7:white. The curses module defines named constants for each of these colors: curses.COLOR_BLACK, curses.COLOR_RED, and so forth.
	curses.start_color()
	if curses.has_colors():
		curses.init_pair(1, curses.COLOR_RED,		curses.COLOR_BLACK)
		curses.init_pair(2, curses.COLOR_GREEN,	curses.COLOR_BLACK)
		curses.init_pair(3, curses.COLOR_YELLOW,  curses.COLOR_BLACK)
		curses.init_pair(4, curses.COLOR_BLUE,    curses.COLOR_BLACK)
		curses.init_pair(5, curses.COLOR_MAGENTA,	curses.COLOR_BLACK)
		curses.init_pair(6, curses.COLOR_CYAN,    curses.COLOR_BLACK)
		curses.init_pair(7, curses.COLOR_WHITE,   curses.COLOR_BLACK)
#	begin_x = 20; begin_y = 7
#	height = 5; width = 40
#	win = curses.newwin(height, width, begin_y, begin_x)
#	win.border()
	stdscr.nodelay(1)		# nodelay(0) makes getch() pause and waits for the user to hit a key, nodelay(1) not
	#stdscr.halfdelay(5)	# makes getch() pause for halfdelay(tenths of a second)
	(maxheight, maxwidth) = stdscr.getmaxyx()
	#
	col_var = maxwidth - 3 - RING_SIZE - 11
	r_flow = 6
	stdscr.border()
	stdscr.addstr(1,2,PROGNAME+"   -- Last update:")
	stdscr.addstr(3, col_var, "Time (sec):") # string=11
	stdscr.addstr(4, col_var, "  (sec/10):")
	stdscr.refresh()
	#
	LogHexString_global = LogHexString(linerate_log10)
	PTR = 0
	FLOW_RING_DB = {}
	while 1:
		a_struct = read_format_flows()
		#onesec = a_struct["onesec"]
		hourminsec = a_struct["timestamp"]["hourminsec"]
		onesec = a_struct["timestamp"]["onesec"]
		onemic = a_struct["timestamp"]["onemic"]
		#
		stdscr.addstr(1,74,"{}.{}".format(hourminsec, onemic))
		#
		TIME_RING[PTR] = [onesec, onemic]
		#BARE_RING[PTR]['time'] = [onesec, onemic]
		#stdscr.addstr(3,3, "PTR=%02d" % (PTR) )
		#
		for key in sorted( a_struct["flows"].keys() ):
			f = a_struct["flows"][key]
			if not key in FLOW_RING_DB:
				FLOW_RING_DB[key] = {}
				FLOW_RING_DB[key]["faX"] = ["_"] * RING_SIZE
				FLOW_RING_DB[key]["laX"] = ["_"] * RING_SIZE
			FLOW_RING_DB[key]["faX"][PTR] = "%X" % (f["faX"])
			FLOW_RING_DB[key]["laX"][PTR] = "%X" % (f["laX"])
			FLOW_RING_DB[key]["PTR"] = PTR	# to tell when this PTR record was last updated
#			if not "faMA" in FLOW_RING_DB[key]:
#				FLOW_RING_DB[key]["faMA"] = active_limit * 5
#			else:
#				FLOW_RING_DB[key]["faMA"] = FLOW_RING_DB[key]["faMA"] - FLOW_RING_DB[key]["faMA"] * alpha + a_struct["flows"][key]["favg"] * alpha
#			FLOW_RING_DB[key]["active"] = ( ( FLOW_RING_DB[key]["faMA"] > active_limit ) and (a_struct["flows"][key]["did"] == "089.106.166.031" ) )
			#FLOW_RING_DB[key]["active"] = ( a_struct["flows"][key]["did"] == "89.106.166.31" )
			FLOW_RING_DB[key]["active"] = 1
			#
			linestring = "{} - {} - {} {}".format( f["sid"], f["did"], f["dir"], f["fic"] )
			FLOW_RING_DB[key]["linestring"] = linestring
			FLOW_RING_DB[key]["dir"] = f["dir"]
		#
		line = r_flow
		lines_per_flow = 2
		for key in sorted( FLOW_RING_DB): #, reverse=True):
			if FLOW_RING_DB[key]["PTR"] != PTR:
				FLOW_RING_DB[key]["faX"][PTR] = "_"
				FLOW_RING_DB[key]["laX"][PTR] = "_"
				FLOW_RING_DB[key]["PTR"] = PTR
			if not FLOW_RING_DB[key]["active"]:
				continue
			if FLOW_RING_DB[key]["dir"] == "U":
				stdscr.addstr(line,2, FLOW_RING_DB[key]["linestring"], curses.color_pair(6))
				stdscr.addstr(line,col_var, "   Log(FA):", curses.color_pair(6))
			else:
				stdscr.addstr(line,2, FLOW_RING_DB[key]["linestring"])
				stdscr.addstr(line,col_var, "   Log(FA):")
			#
			stdscr.addstr(line+1,col_var, "   Log(LA):", curses.color_pair(3))
			line = line + lines_per_flow
			if line >= (maxheight-2):
				break
		#
		for l in xrange(line, maxheight-2,1):
			stdscr.addstr(l,1, " "*(maxwidth-2))
		for x in range(RING_SIZE):		# from: 0 .. to: RING_SIZE-1
			index = (PTR-x) % RING_SIZE
			ring_pos = maxwidth - 3 - x
			ring_time = TIME_RING[index]
			ring_sec = str( ring_time[0] )
			ring_mic = str( ring_time[1] )
			stdscr.addstr(3, ring_pos, ring_sec)
			stdscr.addstr(4, ring_pos, ring_mic)
			line = r_flow
			for key in sorted( FLOW_RING_DB): #, reverse=True):
				if not FLOW_RING_DB[key]["active"]:
					continue
				faX = FLOW_RING_DB[key]["faX"][index]
				laX = FLOW_RING_DB[key]["laX"][index]
				if FLOW_RING_DB[key]["dir"] == "U":
					stdscr.addstr(line, ring_pos, faX[0], curses.color_pair(6))
				else:
					stdscr.addstr(line, ring_pos, faX[0])
				#
				stdscr.addstr(line+1, ring_pos, laX[0], curses.color_pair(3))
				#stdscr.addstr(6+linecount, ring_pos, ring_sec)
				line = line + lines_per_flow
				if line >= (maxheight-2):
					break
		#
#		bottom_window = curses.newwin(maxheight-(7+linecount), maxwidth-2, 7+linecount, 1)
#		bottom_window.clear()
#		bottom_window.refresh()
		#
		stdscr.addstr(maxheight-2, 2, LogHexString_global)
		#stdscr.move(r_flow+1+linecount, 2)
		#
		PTR = (PTR + 1) % RING_SIZE
		stdscr.refresh()
		#
#		linecount = 1
#		for f in sorted( a_struct["flows"].keys() ):
#			#linestring = "{}.{}: {} - {}".format(sec, sub, f, a_struct["flows"][f]["dump"])
#			linestring = "{} - {} - {} {} - {}".format( a_struct["flows"][f]["sid"], a_struct["flows"][f]["did"], a_struct["flows"][f]["dir"], a_struct["flows"][f]["fic"], a_struct["flows"][f]["dump"] )
#			stdscr.addstr(3+linecount,2, linestring)
#			linecount = linecount+1
		#
		c = stdscr.getch()	# does not pause if stdscr.nodelay(1)
		if c != curses.ERR:	# test is user input
			if c == ord('q'):
				break  # Exit the while()
			elif c == curses.KEY_HOME:
				x = y = 0
		#time.sleep(loop_speed);
		time.sleep (interval / 1000.0);
		#
	#
	# def main(win):


#linux> cat /proc/qiss/tcpo/flows.table
#{"flows":[
#			 {"eth1:3103852554:530999897:D:0" : "0:9079:0:0"},
#			 {"eth1:3103852554:530999897:U:0" : "0:462:0:0"},
#			 {"eth1:1795229706:530999897:D:0" : "0:19:0:19"},
#			 {"eth1:1795229706:530999897:U:0" : "0:19:0:0"},
#			 {"eth1:352389130:530999897:U:0" : "0:19:0:0"},
#			 {"eth1:352389130:530999897:D:0" : "0:19:0:0"},
#			 {"eth1:1879115786:530999897:U:0" : "0:19:0:0"},
#			 {"eth1:1879115786:530999897:D:0" : "0:19:0:0"},
#			 {"eth1:218171402:530999897:U:0" : "0:19:0:0"},
#			 {"eth1:218171402:530999897:D:0" : "0:19:0:0"}
#			 ]}
# linux> cat /proc/qiss/tcpo/timestamp
# {"timestamp":{"seconds":1461589746,"microseconds":284674}}

#import copy
#a_struct["flows"] = copy.deepcopy( flows_struct["flows"] )
#a_struct["timestamp"] = copy.deepcopy( time_struct["timestamp"] )

def read_format_flows():
	a_struct = {}
	if isinstance(mySocket, socket.socket):
		#
		multiMessageString = read_multiMessageString(mySocket)
		#
		#print "from socket: in_json = {}".format(multiMessageString)
		try:
			a_struct = json.loads(multiMessageString)
		except:
			print "# Error parsing JSON from TCP socket"
			pass
	else:
		flows_struct = readfile(flows_____filename, flows_____defaults)
		time_struct  = readfile(timestamp_filename, timestamp_defaults)
		a_struct = {}
		a_struct["flows"] = flows_struct["flows"]
		a_struct["timestamp"] = time_struct["timestamp"]
	#
	a_struct["flows"] = format_flows_structure( a_struct["flows"] )
	#print "a_struct: {}".format(a_struct)
	#exit(0)
	#
#	try:
#		this_second = int( time_struct["timestamp"]["seconds"] )
#		this_micro = int( time_struct["timestamp"]["microseconds"] )
#	except:
#		print "# Error extracting seconds/microseconds in JSON-file: ", time____filename
#		this_second = 0
#		this_micro = 0
	#
#	a_struct["seconds"] = int( this_second )
#	a_struct["microseconds"] = int( this_micro )
	a_struct["timestamp"]["onesec"] = int( a_struct["timestamp"]["seconds"] % 10 )
	a_struct["timestamp"]["onemic"] = int( (a_struct["timestamp"]["microseconds"] / 100000) % 10)
	#
	this_datetime = datetime.datetime.fromtimestamp( a_struct["timestamp"]["seconds"] )
	#(hour, minute, second, microsecond, tzinfo) = this_datetime.time
	a_struct["timestamp"]["hourminsec"] = "%2s:%2s:%2s" % (this_datetime.hour, this_datetime.minute, this_datetime.second)
	#flows_struct = {"flows": [ { "key": "value" } ] }
	#pprint.pprint(flows_struct)
	return a_struct

def readfile(filename, defaults):
	out_struct = defaults
	try:
		in_file = open(filename, 'r')
		in_json = in_file.read()
		in_file.close()
		#in_json = re.sub('},]', '}]', in_json)
		(in_json, number_of_subs_made) = re.subn('},]', '}]', in_json)
		if number_of_subs_made > 0:
			print "# Semi-Error: found some 'trailing-commas' but I could remove them"
		try:
			out_struct = json.loads(in_json)
		except:
			#print "# Error parsing JSON in file: ", filename
			pass
		finally:
			return out_struct
	except:
		#print "# Error reading file: ", filename
		pass
	finally:
		return out_struct

def format_flows_structure(in_list):
	#print in_struct; exit(0)
	out_struct = {}
#	if not "flows" in in_struct:
#		return out_struct
#	flows = in_struct["flows"]
	#print flows; exit(0)
	if not isinstance(in_list, list):
		return out_struct
	for flow_hash in in_list:
		if not isinstance(flow_hash, dict):
			continue
		#
		#	Traceback (most recent call last):
		#		File "./box-monitor.py", line 525, in <module>
		#			a_struct = read_format_flows()
		#				File "./box-monitor.py", line 418, in read_format_flows
		#		 a_struct["flows"] = format_flows_structure( a_struct["flows"] )
		#			 File "./box-monitor.py", line 477, in format_flows_structure
		#	key_string = flow_hash.keys()[0]
		#	IndexError: list index out of range
		#
		#key_string = flow_hash.keys()[0]
		#
		try:
			key_string = flow_hash.keys()[0]
			value_string = flow_hash[key_string]
			if key_string == "key":
				continue
		except:
			continue
		#
		#print "key_string: {}".format(key_string)
		#exit(0)
		iface, sid, did, dir, fic = key_string.split(":")
		fmax, favg, lmax, lavg = value_string.split(":")
#		sid = struct.unpack("<I", struct.pack(">I", int(sid)))[0]
#		did = struct.unpack("<I", struct.pack(">I", int(did)))[0]
#		#print explodeIP(sid), " - " , explodeIP(did)
#		sid = explodeIP(sid)
#		did = explodeIP(did)
		T1dump = [fmax,favg,lmax,lavg]
		T2dump = map(int, T1dump)
		dump = "FM%08d-FA%08d-LM%08d-LA%08d" % (T2dump[0], T2dump[1], T2dump[2], T2dump[3])
		#sid2 = re.sub('.', '', sid)
#				sid2 = sid.replace('.','')
#				did2 = did.replace('.','')
#				key2 = "%s%s%s%s" % ( sid2, did2, ("1" if dir=="U" else "2"), str(int(fic)+1) )
#				key = int(key2)
		#print "key2: {}".format(key2)
		key = "%s:%s:%s:%s:%s" % (iface, sid, did, dir, fic)
		#key = key_string.split(":")
		out_struct[key] = {}
		out_struct[key]["iface"] = iface
		out_struct[key]["sid"] = sid
		out_struct[key]["did"] = did
		out_struct[key]["dir"] = dir
		out_struct[key]["fic"] = fic
		out_struct[key]["fmax"] = T2dump[0] * 8
		out_struct[key]["favg"] = T2dump[1] * 8
		out_struct[key]["lmax"] = T2dump[2] * 8
		out_struct[key]["lavg"] = T2dump[3] * 8
		out_struct[key]["dump"] = dump
		out_struct[key]["faX"] = LogHex( T2dump[1]*8, linerate_log10 )
		out_struct[key]["laX"] = LogHex( T2dump[3]*8, linerate_log10 )
	return out_struct

#def explodeIP(ip_int):
#	ip_str = str( ipaddress.IPv4Address(ip_int) )
#	T1 = ip_str.split(".")
#	T2 = map(int, T1)
#	return "%03d.%03d.%03d.%03d" % (T2[0], T2[1], T2[2], T2[3])

if args.lines:
	try:
		while 1:
			a_struct = read_format_flows()
			#print a_struct; exit(0)
	#		for f in ["010.008.001.185-089.106.166.031-D-0"]:#, "010.008.001.185:089.106.166.031:U:0"]:
	#			#print this_second, ": ", "%s.%s"%(sec,sub), ": ", f, " - ", new_struct[f]["dump"]
	#			sec = a_struct["sec"]
	#			sub = a_struct["sub"]
	#			print "%s.%s" % (sec,sub) , ":", f, " - ", a_struct[f]["dump"]
			#onesec = a_struct["timestamp"]["onesec"]
			#onemic = a_struct["timestamp"]["onemic"]
			print "{}.{}".format( a_struct["timestamp"]["seconds"], a_struct["timestamp"]["microseconds"] )
			for f in sorted( a_struct["flows"].keys() ):
				#print "%s.%s" % (onesec,onemic) , ":", f, " - ", a_struct["flows"][f]["dump"]
				print "%42s" % f, " - ", a_struct["flows"][f]["dump"]
				#pprint.pprint(f)
			print "#"
			time.sleep (interval / 1000.0);
	except KeyboardInterrupt:
		print ""
		sys.exit(0)

if args.screen:
	myBlocks = 0
	try:
		while 1:
			#
			myBlocks = (myBlocks + 1) % blocks
			if myBlocks == 0:
				# http://stackoverflow.com/questions/2084508/clear-terminal-in-python
				os.system('cls' if os.name == 'nt' else 'clear')
			#
			a_struct = read_format_flows()
			onesec = a_struct["timestamp"]["onesec"]
			onemic = a_struct["timestamp"]["onemic"]
			print "{}.{}".format( a_struct["timestamp"]["seconds"], a_struct["timestamp"]["microseconds"] )
			for f in sorted( a_struct["flows"].keys() ):
				#print "%s.%s" % (onesec,onemic) , ":", "%42s" % f, " - ", a_struct["flows"][f]["dump"]
				print "%42s" % f, " - ", a_struct["flows"][f]["dump"]
			#pprint.pprint(f)
			#print "#"
			time.sleep (interval / 1000.0);
	except KeyboardInterrupt:
		print ""
		sys.exit(0)

typedef_COOKED={
	"config"		: {
		"loglinerate"	: 7,
		"alpha"			: 0.4,
		"active3"		: 0.1,		# 1E-1	=   1 Mbps @10Mbps
		"active2"		: 0.01,		# 1E-2	= 100 kbps @10Mbps
		"active1"		: 0.001,		# 1E-3	=  10 kbps @10Mbps
		"refresh"		: 1.0,		#
	},
	"timestamp" : {
		"epoch"	: 17283712.528,	# Double epoch time rounded to millisecs
		"mmsst"	: "12:34.5",		# 0m:0s.t	0m=minutes(00..59), 0s=seconds(00..59), t=tensofsecond(0..9)
		"DSID"	: "7545"				# 10*( 60*m +s) + t -- so this is unique ID on Deci-Second level
	},
	"global"		: {
		"regpol"	: 0,					# 0: disable, 1: enabled
		"fupol"	: 0,					# 0: disable, 1: enabled
		"short"	: 50,					# ms
		"long"	: 1000,				# ms
		"fasupdate" : 11				# the last (rounded/integer) clock second (0..9) that FAS was updated
	},
	"flows" : {
		"SID:DID": {								# unique key is combined copy of SID:DID
			"IF"	: "some",
			"SID"	: "012.034.056.078",			# prepended with zeros
			"DID"	: "223.255.255.255",			# prepended with zeros
			"upstream"		: {
				"FM":"00E0","FA":"00E0",		# exp.format.bps: 12E3 = 12 kbps
				"LM":"00E0","LA":"00E0",
				"FAS":"00E0",						# exp.format.bps: 12E3 = 12 kbps
				"FAX":"0","LAX":"0",				# LogHexChar
				"ACT":0								# 0=unknown, 1=silent, etc.
			},
		"downstream"	: (), # copy.deepcopy(newDir)
	}
},
	"upstream" : {
		"capacity"	: {"total":0, "registered":0, "free":0, "fairuse":0, "limact3":0, "limact2":0, "limact1":0},
		"clients"	: {"registered":0, "active3":0, "active2":0, "active1":0, "active0":0, "equivalent":0},
},
	"downstream" : {
		"capacity"	: {"total":0, "registered":0, "free":0, "fairuse":0, "limact3":0, "limact2":0, "limact1":0},
		"clients"	: {"registered":0, "active3":0, "active2":0, "active1":0, "active0":0, "equivalent":0},
},
	"reservations" : {},
}
C_array = ["TOT", "REG", "FREE", "FAIR", "LA3", "LA2", "LA1", "AC3", "AC2", "AC1", "AC0", "EQV" ]
F_array = ["FM", "FA", "LM", "LA", "FAS", "ACT"]

def main_flows(win):
	global stdscr
	stdscr = win
	curses.start_color()
	if curses.has_colors():
		curses.init_pair(1, curses.COLOR_RED,		curses.COLOR_BLACK)
		curses.init_pair(2, curses.COLOR_GREEN,	curses.COLOR_BLACK)
		curses.init_pair(3, curses.COLOR_YELLOW,  curses.COLOR_BLACK)
		curses.init_pair(4, curses.COLOR_BLUE,    curses.COLOR_BLACK)
		curses.init_pair(5, curses.COLOR_MAGENTA,	curses.COLOR_BLACK)
		curses.init_pair(6, curses.COLOR_CYAN,    curses.COLOR_BLACK)
		curses.init_pair(7, curses.COLOR_WHITE,   curses.COLOR_BLACK)
	#
	stdscr.nodelay(1)		# nodelay(0) makes getch() pause and waits for the user to hit a key, nodelay(1) not
	(maxheight, maxwidth) = stdscr.getmaxyx()
	#
	col_var = maxwidth - 3 - RING_SIZE - 11
	stdscr.border()
	r=1
	stdscr.addstr(r,2,PROGNAME+"   -- Last update:")
	stdscr.addstr(r,84,"DSID (Deci-Second ID):")
	stdscr.addstr(r,114,"FASupdate:")
	r=r+2
	stdscr.addstr(r,42,"Capacity")
	stdscr.addstr(3,66,"Limit Active")
	stdscr.addstr(3,84,"Clients") ;r=r+1
	stdscr.addstr(r,42,"----------------------")
	stdscr.addstr(r,66,"----------------")
	stdscr.addstr(r,84,"---------------------------") ;r=r+1
	for i in range( len(C_array) ):
		stdscr.addstr(r,42+6*i, C_array[i] )
	r=r+2
	C_row = r
	stdscr.addstr(r,2,"Upstream") ; stdscr.addstr(r,36,"U") ;r=r+1
	stdscr.addstr(r,2,"Downstream") ; stdscr.addstr(r,36,"D") ;r=r+2
	stdscr.addstr(r,42,"Forwarding") ; stdscr.addstr(r,56,"Loss") ; stdscr.addstr(r,70,"Smooth") ;r=r+1
	stdscr.addstr(r,42,"-------------")
	stdscr.addstr(r,56,"-------------")
	stdscr.addstr(r,70,"------") ;r=r+1
	stdscr.addstr(r,2,"Flow Key (SID:DID)")
	for i in range( len(F_array) ):
		stdscr.addstr(r,42+7*i, F_array[i] )
	r=r+2
	stdscr.refresh()
	#
	while 1:
		row = r
		a_struct = read_flowexport()
		try:
			mmsst			= a_struct["timestamp"]["mmsst"]
			DSID			= a_struct["timestamp"]["DSID"]
			fasupdate	= a_struct["global"]["fasupdate"]
		except:
			mmsst			= "00:00.0"
			DSID			= 0.0
			fasupdate	= 0
		stdscr.addstr(1,74,"{}".format( mmsst ))
		stdscr.addstr(1,107,"{}".format( DSID ))
		stdscr.addstr(1,125,"{}".format( fasupdate ))
		#
		cr = C_row
		for dir2 in ["upstream", "downstream"]:
			for i in range( len(C_array) ):
				#name = C_array[i]
				if C_array[i] == "TOT": str = "{}".format( a_struct[dir2]["capacity"]["total"] )
				if C_array[i] == "REG": str = "{}".format( a_struct[dir2]["capacity"]["registered"] )
				if C_array[i] == "FREE": str = "{}".format( a_struct[dir2]["capacity"]["free"] )
				if C_array[i] == "FAIR": str = "{}".format( a_struct[dir2]["capacity"]["fairuse"] )
				if C_array[i] == "LA3": str = "{}".format( a_struct[dir2]["capacity"]["limact3"] )
				if C_array[i] == "LA2": str = "{}".format( a_struct[dir2]["capacity"]["limact2"] )
				if C_array[i] == "LA1": str = "{}".format( a_struct[dir2]["capacity"]["limact1"] )
				if C_array[i] == "AC3": str = "{}".format( a_struct[dir2]["clients"]["active3"] )
				if C_array[i] == "AC2": str = "{}".format( a_struct[dir2]["clients"]["active2"] )
				if C_array[i] == "AC1": str = "{}".format( a_struct[dir2]["clients"]["active1"] )
				if C_array[i] == "AC0": str = "{}".format( a_struct[dir2]["clients"]["active0"] )
				if C_array[i] == "EQV": str = "{}".format( a_struct[dir2]["clients"]["equivalent"] )
				stdscr.addstr(cr,42+6*i, str )
			cr = cr + 1
		#
		for key in sorted( a_struct["flows"].keys() ):
			f = a_struct["flows"][key]
			for dir2 in ["upstream", "downstream"]:
				if dir2 == "upstream":
					dir = "U"
				else:
					dir = "D"
				stdscr.addstr(row,2,"{} - {}".format(key, dir) )
				for i in range( len(F_array) ):
					name = F_array[i]
					str = "{}".format( a_struct["flows"][key][dir2][name] )
					stdscr.addstr(row,42+7*i, str)
				row = row + 1
				#stdscr.addstr(r+1,2,"123.123.123.123:123.123.123 - U")
				# /for dir2
			row = row + 1
			# /for key
		#
		stdscr.refresh()
		#
		c = stdscr.getch()	# does not pause if stdscr.nodelay(1)
		if c != curses.ERR:	# test is user input
			if c == ord('q'):
				break  # Exit the while()
			elif c == curses.KEY_HOME:
				x = y = 0
		time.sleep (interval / 1000.0);
		#
	# def main_flows(win):

def read_flowexport():
	#
	multiMessageString = read_multiMessageString(mySocket)
	#
	#print "# in_json = {}".format(multiMessageString)
	a_struct = {}
	try:
		a_struct = json.loads(multiMessageString)
	except:
		print "# Error parsing JSON from TCP socket"
		pass
	#
	return a_struct

if args.flows:
	os.environ["NCURSES_NO_UTF8_ACS"] = "1"
	curses.wrapper(main_flows)

if args.window:
	os.environ["NCURSES_NO_UTF8_ACS"] = "1"
	curses.wrapper(main)

#

#!/usr/bin/env python

# ------+++------------+++------------+++------------+++------------+++------
#
#>echo '{"jsonrpc": "2.0","method":"check","params":{},"id":1}' | nc 10.88.0.20 24082
#>echo '{"jsonrpc": "2.0","method":"check","params":{},"id":1}' | nc 127.0.0.1 24082
#>echo '{"jsonrpc": "2.0","method":"check","params":{},"id":1}' | nc 172.16.53.140 24082
#>echo '{"jsonrpc": "2.0","method":"monitor","params":{"interval":1000},"id":1}' | nc 10.88.0.20 24082
#>echo '{"jsonrpc": "2.0","method":"flowexport","params":{"interval":1000},"id":1}' | nc 10.8.0.20 24082
#>echo '{"jsonrpc": "2.0","method":"flowexport","params":{"interval":1000},"id":1}' | nc 10.88.0.20 24082
#>echo '{"jsonrpc": "2.0","method":"flowexport","params":{"interval":1000},"id":1}' | nc 172.16.53.140 24082
#>echo '{"jsonrpc": "2.0","method":"getflow","params":{},"id":1}' | nc 10.8.0.20 24082
#>echo '{"jsonrpc": "2.0","method":"getflow","params":{},"id":1}' | nc 10.88.0.20 24082
#>echo '{"jsonrpc": "2.0","method":"writeallconf","params":{"lab":2,"config":{},"links":{},"das":{},"policer":{}},"id":1}' | nc 10.88.0.20 24082
#>echo '{"jsonrpc": "2.0","method":"setglobal","params":{"regpol":1,"fupol":1,"short":50,"long":1000},"id":1}' | nc 10.8.0.20 24082
#>echo '{"jsonrpc": "2.0","method":"setglobal","params":{"regpol":0,"fupol":0,"short":50,"long":1000},"id":1}' | nc 10.8.0.20 24082

# ------+++------------+++------------+++------------+++------------+++------
#
# SocketServer:
# http://stackoverflow.com/questions/16904780/python-socket-and-socketserver
# https://pymotw.com/2/SocketServer/
# http://kmkeen.com/socketserver/
# http://kmkeen.com/socketserver/2009-02-07-07-52-15.html
# http://www.bogotobogo.com/python/python_network_programming_socketserver_framework_for_network_servers.php
# https://docs.python.org/2/library/socketserver.html
#import SocketServer

# https://docs.python.org/2/library/threading.html
# http://www.tutorialspoint.com/python/python_multithreading.htm
# http://jeffknupp.com/blog/2012/03/31/pythons-hardest-problem/
import threading
# https://docs.python.org/2/library/thread.html
# import thread

# forks (processes) vs. threads
# http://stackoverflow.com/questions/2482926/should-i-use-fork-or-threads

# alternative to thread or fork:
# https://docs.python.org/2/library/asyncore.html

# mixin: A mixin is a special kind of multiple inheritance.
# http://stackoverflow.com/questions/533631/what-is-a-mixin-and-why-are-they-useful
# https://www.ianlewis.org/en/mixins-and-python

# https://docs.python.org/2/library/socket.html
#import socket

# multi-threading TCP Server example:
# https://github.com/edaniszewski/MultithreadedTCP/blob/master/multithreadedServer.py
# from threading import Lock, Thread
# class ClientHandler(Thread):
# newThread = ClientHandler(addr[0], addr[1], sock, response_message, thread_lock)

# JSON-RPC:
# https://github.com/niligulmohar/python-symmetric-jsonrpc
# https://github.com/mbr/tinyrpc

# http://www.jsonrpc.org/specification
# --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
# <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
# --> {"jsonrpc": "2.0", "method": "subtract", "params": [23, 42], "id": 2}
# <-- {"jsonrpc": "2.0", "result": -19, "id": 2}
#
# Errors
# <-- {"jsonrpc": "2.0", "error": {"code": -32700, "message": "Parse error"}, "id": null}
# <-- {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null}
# <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
# https://en.wikipedia.org/wiki/JSON
import json

# try except raise
# https://docs.python.org/2/tutorial/errors.html

# http://stackoverflow.com/questions/5844650/how-to-constantly-send-stdout-to-my-python-tcp-server
# https://docs.python.org/2/library/subprocess.html
import subprocess

# http://stackoverflow.com/questions/4789837/how-to-terminate-a-python-subprocess-launched-with-shell-true
# pro = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
# os.killpg(os.getpgid(pro.pid), signal.SIGTERM)
#import os
#import signal

# http://www.python-course.eu/deep_copy.php
# https://docs.python.org/2/library/copy.html
import copy

# https://docs.python.org/2/library/datetime.html
import datetime

# https://docs.python.org/2/library/functions.html#isinstance
import sys

# https://docs.python.org/2/library/math.html
import math

# https://docs.python.org/2/library/string.html
# https://docs.python.org/2/library/stdtypes.html#string-formatting
# string formatting using %

# https://docs.python.org/2/library/logging.html
# https://docs.python.org/2/howto/logging.html
#logging.basicConfig(filename='qsboxta.log',level=logging.DEBUG)
#logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)
#logging.basicConfig(format='%(asctime)s %(message)s')
#logging.basicConfig(format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
#logging.debug('This message should go to the log file')
#logging.info('So should this')
#logging.warning('And this, too')
import logging
# sh> sudo touch /var/log/qsboxta.log
# sh> sudo chmod a+rw /var/log/qsboxta.log
# sh> sudo ln -s /var/log/qsboxta.log /home/jdg/code/qsboxta/qsboxta.log
# sh> tailf /var/log/qsboxta.log
#
logging.basicConfig(filename='/var/log/qsboxta.log', level=logging.DEBUG, format='%(asctime)s %(levelname)s: %(message)s')
logging.info('start qsboxta.py')

# ------+++------------+++------------+++------------+++------------+++------
#
import SocketServer
import json
import subprocess
import time
import argparse

qiss_ssdis_port = 24011		# qiss-ssdis   24011/udp
qiss_ssreg_port = 24012		# qiss-ssreg   24012/udp
qiss_nwdis_port = 24021		# qiss-nwdis   24021/udp
#									# qiss-nwdis   24021/tcp
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

qsprotver = "2.0"

def check_NTP_sync():
	try:
		#subprocess.Popen("/usr/bin/ntpstat")
		output = subprocess.check_output(["/usr/bin/ntpstat"])
	except subprocess.CalledProcessError, e:
		logging.info('check_NTP_sync exception CalledProcessError e={}'.format(e))
		return 0
	except:
		logging.info('check_NTP_sync general exception (ntpstat not installed?)')
		return 0
	# if ntpstat is success (NTP in sync) then return 'true' as int:1
	return 1

parser = argparse.ArgumentParser(description="qsboxta - QISS Box Traffic Agent")
parser.add_argument('--start',   action='store_true', help='Start on Command-Line')
parser.add_argument('--daemon',  action='store_true', help='Start as Daemon')
args = parser.parse_args()
if len(sys.argv)==1:
	parser.print_help()
	sys.exit(1)

config____filename = "/proc/qiss/config"
links_____filename = "/proc/qiss/tci/links.table"
das_______filename = "/proc/qiss/tci/das.table"
policer___filename = "/proc/qiss/tci/policer.table"
flows_____filename = "/proc/qiss/tcpo/flows.table"
timestamp_filename = "/proc/qiss/tcpo/timestamp"

police2___filename = "/home/jdg/code/qsboxta/policer.table"
# touch /home/jdg/code/qsboxta/policer.table
# chown jdg.jdg /home/jdg/code/qsboxta/policer.table
# chmod 666 /home/jdg/code/qsboxta/policer.table

timestamp_defaults = {"timestamp": {"seconds":0,"microseconds":0}}
flows_____defaults = {"flows":[{}]}
config____defaults = {"short":50,"long":1000,"alive":300,"period":20}
links_____defaults = {"links":[{"name": "br1","icap": 1175000,"ecap": 1175000}]}
das_______defaults = {"das":[{ "596AA67F":["0A080000/16","596AA600/25"]},{"DFFFFFFF":["00000000/00"]}]}
das1______defaults = {"das":[{ "596AA67F":["0A080000/16","596AA600/25"]},{"DFFFFFFF":["00000000/00"]}]}
das2______defaults = {"das":[{ "596AA6FF":["0A580000/16","596AA680/25"]},{"DFFFFFFF":["00000000/00"]}]}
policer___defaults = {"policer":[{"SID":"10.8.1.78","DID":"89.106.166.26","DIR":"U","AVG":100,"ABKT":80,"PEAK":200,"PBKT":80}]}
policer2__defaults = {"policer":[{"SID":"10.8.1.78","DID":"89.106.166.26","DIR":"U","AVG":100,"ABKT":80,"PEAK":200,"PBKT":80},{"SID":"0.0.0.0","DID":"0.0.0.0","DIR":"U","AVG":200,"ABKT":80,"PEAK":200,"PBKT":80}]}

# ------+++------------+++------------+++------------+++------------+++------
#
#config_defaults = {		# replaced by: typedef_COOKED["config"]
#	"log10linerate": 7,
#	"alpha"			: 0.4,
#	"active3"		: 0.1,		# 1E-1	=   1 Mbps @10Mbps
#	"active2"		: 0.01,		# 1E-2	= 100 kbps @10Mbps
#	"active1"		: 0.001,		# 1E-3	=  10 kbps @10Mbps
#}
#linerate			= pow(10, log10linerate)	# in bps: log10=8 -> linerate = 1E8 = 100 Mbps

# about alpha:
# x[t=0]=1E6, x[t>0]=0, which alpha gives 1% in 60 steps/seconds?
# x[1] = a*x[0] + (1-a)*0 = x[1] = a*x[0]
# x[n] = a^n * x[0]		-> .....
# a=0.9, n = 60 -> x[60] = x[0] * a^n  -> a^n = 8.6E-2
# a=0.8, n = 60 -> x[60] = x[0] * a^n  -> a^n = 1.5E-6
# a=0.7, n = 60 -> x[60] = x[0] * a^n  -> a^n = 5.0E-10
# now for activity: factor 100 in 5 sec -> a=0.4 0.4^5=0.01

# global:
RAW_FLOWS	= {"flows":[{}]}
COOKED		= {"config":{},"global":{},"timestamp":{},"flows":{},"upstream":{},"downstream":{},"reservations":{}}

newDir = {"FM":"00E0","FA":"00E0","LM":"00E0","LA":"00E0","FAA":0,"FAS":"00E0","FAX":"0","LAX":"0","ACT":0}

typedef_RAW_FLOWS={"flows":[{}]}
typedef_COOKED={
	"config"		: {
		"log10linerate": 7,			# int, used for LogHexChar()
		"linerate"		: 60E5,
		"alpha"			: 0.4,
		"active3"		: 0.13,		# 1E-1	=   1 Mbps @10Mbps
		"active2"		: 0.017,		# 1E-2	= 100 kbps @10Mbps
		"active1"		: 0.0015,	# 1E-3	=  10 kbps @10Mbps
		"refresh"		: 0.1			#
	},
	"global"		: {
		"regpol"			: 0,			# 0: disable, 1: enabled
		"fupol"			: 0,			# 0: disable, 1: enabled
		"short"			: 50,			# ms
		"long"			: 1000,		# ms
		"upstream"		: 0,			# bps forced-fup (fair-use policy) upstream (before: ffupu)
		"downstream"	: 0,			# bps forced-fup (fair-use policy) downstream (before: ffupd)
		"fasupdate"		: 11				# the last (rounded/integer) clock second (0..9) that FAS was updated
	},
	"timestamp" : {
#		"seconds"		: 17283712,	# directly from /proc/qiss/tcpo/timestamp
#		"microseconds"	: 52822812,
		"epoch"	: 17283712.528,	# Double (float) epoch time in seconds with partial rounded to millisecs
		"mmsst"	: "12:34.5",		# 0m:0s.t	0m=minutes(00..59), 0s=seconds(00..59), t=tensofsecond(0..9)
		"DSID"	: "7545",				# 10*( 60*m +s) + t -- so this is unique ID on Deci-Second level
#		"mm"		: 12,
#		"ss"		: 34,
#		"t"		: 5,
		"onesec"	: 4
#		"secmic"	: "4.5",				# string
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
				"FAA":0,								# counter (int, not exp!) updated COOKED["config"]["refresh"] and averaged every 1 second
				"FAX":"0","LAX":"0",				# LogHexChar
				"ACT":0								# 0=unknown, 1=silent, etc.
			},
			"downstream"	: copy.deepcopy(newDir)
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
	"reservations" : {
		"SID": { "upstream":"00E0","downstream":"00E0" }
	},
}

# > python -c 'i=555 ; s1="%1.1E"%(i/10); s=s1.replace(".","").replace("+","").replace("E0","E") ; print "s={}".format(s)'
# s=55E1
#
# > python -c 's="55E1" ; i=int(float(s)); print "i={}".format(i)'
# i=550

def int2exp(int_in):
	if int_in > 0 and int_in < 10:
		int_in = 10		# will become 10E0, 0 will become 00E0
	#
	string1		= "%1.1E" % ( float(int_in) / 10.0 )
	string_out	= string1.replace(".","").replace("+","").replace("E0","E")
	return string_out

def exp2int(string_in):
	int_out = int( float( string_in ) )
	return int_out

def init_COOKED():
	replace_defaults( COOKED["config"] , typedef_COOKED["config"] )
	#
	COOKED["global"] = {}
	COOKED["global"]["regpol"]			= 0
	COOKED["global"]["fupol"]			= 0
	COOKED["global"]["short"]			= 20
	COOKED["global"]["long"]			= 500
	COOKED["global"]["upstream"]		= 0		# ffupu
	COOKED["global"]["downstream"]	= 0		# ffupd
	COOKED["global"]["fasupdate"]		= 11
	total_int		= int( COOKED["config"]["linerate"] * 0.94 )
	total_f			= float( total_int )
	total				= int2exp( total_int )
	for dir2 in ["upstream", "downstream"]:
		COOKED[dir2]	= {}
		COOKED[dir2]["capacity"] = {}
		COOKED[dir2]["capacity"]["total"]		= total
		COOKED[dir2]["capacity"]["registered"]	= "00E0"
		COOKED[dir2]["capacity"]["free"]			= total
		COOKED[dir2]["capacity"]["fairuse"]		= total	# total / equivalent=1
		COOKED[dir2]["capacity"]["limact1"]		= int2exp( int( total_f * COOKED["config"]["active1"] ) )
		COOKED[dir2]["capacity"]["limact2"]		= int2exp( int( total_f * COOKED["config"]["active2"] ) )
		COOKED[dir2]["capacity"]["limact3"]		= int2exp( int( total_f * COOKED["config"]["active3"] ) )
		COOKED[dir2]["clients"] = {}
		COOKED[dir2]["clients"]["registered"]	= 0
		COOKED[dir2]["clients"]["active3"]		= 0
		COOKED[dir2]["clients"]["active2"]		= 0
		COOKED[dir2]["clients"]["active1"]		= 0
		COOKED[dir2]["clients"]["active0"]		= 0
		COOKED[dir2]["clients"]["equivalent"]	= 1.0		# cannot be zero, needs to be >=1

def active_for_fas_dir(fas, dir2):
	act = 0
#	free = exp2int( COOKED[dir2]["capacity"]["free"] )
#	if fas >= int(free * COOKED["config"]["active1"] ):
#		act = 1
	if fas >= exp2int( COOKED[dir2]["capacity"]["limact1"] ):
		act = 1
	if fas >= exp2int( COOKED[dir2]["capacity"]["limact2"] ):
		act = 2
	if fas >= exp2int( COOKED[dir2]["capacity"]["limact3"] ):
		act = 3
	return act

def LogHex(rate, log10linerate):
	if rate==0:
		return 0
	log_f = math.log(rate, 10) * (16.0/log10linerate)
	log = min(max( int(log_f), 0),15)
	#return (log_f, log)
	return log

def LogHexChar(rate, log10linerate):
	log = LogHex(rate, log10linerate)
	return "%X" % (log)

def LogHexString(log10linerate):
	bps = ["x"]*16
	for s in range(16):
		x = pow(10, float(s)*float(log10linerate)/16.0)
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

def prependZeroIP(string):
	T1 = string.split(".")
	T2 = map(int, T1)
	return "%03d.%03d.%03d.%03d" % (T2[0], T2[1], T2[2], T2[3])

def prependZeroTimeHHMMSS(string):
	T1 = string.split(":")
	T2 = map(int, T1)
	return "%02d:%02d:%02d" % (T2[0], T2[1], T2[2])

def prependZeroTimeMMSS(string):
	T1 = string.split(":")
	T2 = map(int, T1)
	return "%02d:%02d" % (T2[0], T2[1])

def readfile(filename, defaults):
	out_struct = defaults
	try:
		in_file = open(filename, 'r')
		in_json = in_file.read()
		in_file.close()
		try:
			out_struct = json.loads(in_json)
		except:
			print "# readfile: Error parsing JSON in file: ", filename
		finally:
			return out_struct
	except:
		print "# readfile: Error reading file: ", filename
	finally:
		return out_struct

def writefile(in_str, filename):
	# ONLY converts string to struct:
	print "# writefile: filename={} in_str={}".format(filename, in_str)
	in_struct = {}
	try:
		in_struct = json.loads(in_str)
		# now call other func to DO it:
		writefile_struct(in_struct, filename)
	except:
		print "# writefile: Error parsing JSON - in_str={}".format(in_str)

def writefile_struct(in_struct, filename):
	try:
		out_json = json.dumps(in_struct)
		out_string = "# writefile: writing JSON to file: {} this JSON: {}".format(filename, out_json)
		print out_string
		logging.info(out_string)
		try:
			out_file = open(filename, 'w')
			out_file.write( out_json + "\n" )
			out_file.close()
		except:
			print "# writefile: Error writing to file: ", filename
	except:
		print "# writefile: Error parsing JSON - in_struct={}".format(in_struct)

def calcPolicerEntry(in_struct):
	out_struct = {}
	try:
		AVG = 0.0 # float
		if "AVG" in in_struct:							# AVG is defined in kBps (e.g. 200 kBps)
			AVG = float( in_struct["AVG"] )
		if "RATE" in in_struct:							# RATE defined in bps
			AVG = float( in_struct["RATE"] / ( 1000 * 8 ) )
		#
		#ABKT = float( AVG * 1.2 * 0.005 )	# Juniper: burst size = max bandwidth over 5 ms over max burst time
		ABKT = float( AVG * 0.005 )	# Juniper: burst size = max bandwidth over 5 ms over max burst time
		ABKT = min( max( ABKT, 15.0), 1000.0)	# Juniper: minimum 10x MTU (10 x 1500 bytes = 15 kB)
		#
		if AVG == 0:
			ABKT = 0
		#
		out_struct["AVG"]	= int( AVG )
		out_struct["PEAK"]	= int( AVG * 1.2 )
		out_struct["ABKT"]	= int( ABKT )
		out_struct["PBKT"]	= int( ABKT * 1.2 )
		#
		out_struct["SID"]		= in_struct["SID"]
		out_struct["DID"]		= in_struct["DID"]
		out_struct["DIR"]		= in_struct["DIR"]
	except:
		print "# Error parsing JSON"
	return out_struct

def replace_defaults(mystruct, defaults):
	for key in defaults.keys():
		value = defaults[key]
		if not (key in mystruct):
			mystruct[key] = value
		else:
			struct_value = mystruct[key]
			if isinstance(value, dict) and isinstance(struct_value, dict):
				replace_defaults(struct_value, value)

def updateReg(aSID, aUS, aDS):
	COOKED["reservations"][aSID] = {}
	COOKED["reservations"][aSID]["upstream"]		= int2exp( aUS )
	COOKED["reservations"][aSID]["downstream"]	= int2exp( aDS )
	#
	# DONT remove, we need to write '0' to policer.table !
#	if aUS == 0 and aDS == 0:
#		if aSID in COOKED["reservations"]:
#			COOKED["reservations"].remove(aSID)
#			# /if
#		# /if
	#
	writePolicerTable()
	#
	# /def updateReg()

def read_format_RAW_FLOWS():
	global RAW_FLOWS
	global COOKED
	flows_struct = readfile(flows_____filename, flows_____defaults)
	time_struct  = readfile(timestamp_filename, timestamp_defaults)
	RAW_FLOWS["flows"]	= flows_struct["flows"]
	#
	sec = time_struct["timestamp"]["seconds"]
	mic = time_struct["timestamp"]["microseconds"]
	#COOKED["timestamp"]["epoch"] = "{}.{}".format(sec, int( mic/1000 ) )
	COOKED["timestamp"]["epoch"] = sec + int(mic/1000) / 1000.0
	onemic = int( ( float(mic) / 100000.0) % 10)
	this_datetime = datetime.datetime.fromtimestamp( sec )
	time_string_spaces = "%2s:%2s" % (this_datetime.minute, this_datetime.second)
	hourminsec = prependZeroTimeMMSS(time_string_spaces)
	COOKED["timestamp"]["mmsst"]	= "{}.{}".format(hourminsec, onemic)
	COOKED["timestamp"]["DSID"]	= 600 * this_datetime.minute + 10 * this_datetime.second + onemic
	COOKED["timestamp"]["onesec"]	= this_datetime.second % 10
	#

def format_flows_COOKED():
	global RAW_FLOWS
	global COOKED
	#
	in_list = RAW_FLOWS["flows"]
	if not isinstance(in_list, list):
		return
	#
	alpha = COOKED["config"]["alpha"]	# float
	#
	for flow_hash in in_list:
		if not isinstance(flow_hash, dict):
			continue
		try:
			key_string		= flow_hash.keys()[0]
			value_string	= flow_hash[key_string]
		except:
			continue
		if key_string == "key":
			continue
		iface, sid, did, dir, fic	= key_string.split(":")
		fmax, favg, lmax, lavg		= value_string.split(":")
		if dir != "U" and dir != "D":
			logging.info('# Error: invalid dir="{}"'.format(dir))
			continue
		#
		sid = prependZeroIP(sid)
		did = prependZeroIP(did)
		#
		T1dump = [fmax,favg,lmax,lavg]
		T2dump = map(int, T1dump)
		fm = T2dump[0] * 8
		fa = T2dump[1] * 8
		lm = T2dump[2] * 8
		la = T2dump[3] * 8
		#dump = "FM%08d-FA%08d-LM%08d-LA%08d" % (T2dump[0], T2dump[1], T2dump[2], T2dump[3])
		#key = "%s:%s:%s:%s:%s" % (iface, sid, did, dir, fic)
		key = "%s:%s" % (sid, did)
		if not key in COOKED["flows"]:
			COOKED["flows"][key] = {}
			COOKED["flows"][key]["IF"] = iface
			COOKED["flows"][key]["SID"] = sid
			COOKED["flows"][key]["DID"] = did
			#out_struct[key]["dir"] = dir
			#out_struct[key]["fic"] = fic
			#
			# !! otherwise both keys have link to same object.. -> copy.deepcopy(newDir) !!
			COOKED["flows"][key]["upstream"] = copy.deepcopy(newDir)
			COOKED["flows"][key]["downstream"] = copy.deepcopy(newDir)
		#
		if dir == "U":
			dir2 = "upstream"
		else:
			dir2 = "downstream"
		#
#		fas_int = exp2int( COOKED["flows"][key][dir2]["FAS"] )
#		fas_int = int( alpha * fas_int + (1-alpha) * fa )
		#
#		try:
#			free = COOKED[dir2]["capacity"]["free"]
#		except:
#			free = 0
#		#
		COOKED["flows"][key][dir2]["FM"] = int2exp( fm )
		COOKED["flows"][key][dir2]["FA"] = int2exp( fa )
		COOKED["flows"][key][dir2]["LM"] = int2exp( lm )
		COOKED["flows"][key][dir2]["LA"] = int2exp( la )
		COOKED["flows"][key][dir2]["FAX"] = LogHexChar( fa, COOKED["config"]["log10linerate"] )
		COOKED["flows"][key][dir2]["LAX"] = LogHexChar( la, COOKED["config"]["log10linerate"] )
		#
		COOKED["flows"][key][dir2]["FAA"] = COOKED["flows"][key][dir2]["FAA"] + fa
		#
#		COOKED["flows"][key][dir2]["FAS"] = int2exp( fas_int )
#		COOKED["flows"][key][dir2]["ACT"] = active_for_fas_dir(fas_int, dir2)
		#
		# /end: for flow_hash in in_list:
	#
	# when done with analyzing 'unidir' RAW_FLOWS["flows"], create orphan other-dirs:
	for key in COOKED["flows"]:
		for dir2 in ["upstream", "downstream"]:
			if not (dir2 in COOKED["flows"][key]):
				COOKED["flows"][key][dir2] = copy.deepcopy(newDir)
	#
#	# remove keys that have: up.fas=0 and down.fas=0   --> DONT do it here, to it in ffmon
#	for key in COOKED["flows"]:
#		ufas = exp2int( COOKED["flows"][key]["upstream"]["FAS"]		)
#		dfas = exp2int( COOKED["flows"][key]["downstream"]["FAS"]	)
#		if ufas == 0 and dfas == 0 :
#			COOKED["flows"].remove(key)
	# /end: def format_flows_COOKED():

def update_clients_capacity_COOKED():
	global RAW_FLOWS
	global COOKED
	#
	alpha = COOKED["config"]["alpha"]		# float
	#
	for key in COOKED["flows"]:
		for dir2 in ["upstream", "downstream"]:
			#
			faa			= COOKED["flows"][key][dir2]["FAA"]
			refresh		= COOKED["config"]["refresh"]
			if refresh == 0:
				refresh = 1
			factor		= float( 1.0 / float( refresh ) ) # bug: factor = 1000.0 / refresh
			faa_scaled	= int( float( faa ) / factor )  
			#
			fas_int = exp2int( COOKED["flows"][key][dir2]["FAS"] )
			#fa_int  = exp2int( COOKED["flows"][key][dir2]["FA"]  )
			#fas_int = int( alpha * fas_int + (1-alpha) * fa_int )
			#
			fas_int = int( alpha * fas_int + (1-alpha) * faa_scaled )
			#
			COOKED["flows"][key][dir2]["FAA"] = 0
			COOKED["flows"][key][dir2]["FAS"] = int2exp( fas_int )
			COOKED["flows"][key][dir2]["ACT"] = active_for_fas_dir(fas_int, dir2)
			# /for
		# /for
	#
	fairuse_changed = {}
	fairuse_changed["upstream"]	= 0
	fairuse_changed["downstream"]	= 0
	#
	for dir2 in ["upstream", "downstream"]:
		#
		registered = 0
		for aSID in COOKED["reservations"]:
			aRes = exp2int( COOKED["reservations"][aSID][dir2] )
			registered = registered + aRes
			#
		COOKED[dir2]["capacity"]["registered"] = int2exp( registered )
		#
		# update free
		total			= exp2int( COOKED[dir2]["capacity"]["total"]			)
		registered	= exp2int( COOKED[dir2]["capacity"]["registered"]	)
		free_int		= max(0, (total - registered))
		COOKED[dir2]["capacity"]["free"] = int2exp( free_int )
		#
		# update limact3/2/1
		free_f = float( free_int )
		COOKED[dir2]["capacity"]["limact1"] = int2exp( int( free_f * COOKED["config"]["active1"] ) )
		COOKED[dir2]["capacity"]["limact2"] = int2exp( int( free_f * COOKED["config"]["active2"] ) )
		COOKED[dir2]["capacity"]["limact3"] = int2exp( int( free_f * COOKED["config"]["active3"] ) )
		#
		# update active
		active = [0,0,0,0]
		for flowkey in COOKED["flows"]:
			flowvalue = COOKED["flows"][flowkey]
			flowvaluedir = flowvalue[dir2]
			act = flowvaluedir["ACT"]
			try:
				active[act] += 1
			except:
				logging.info('# Error: update_clients_capacity_COOKED() index act out of bounds!')
			#
			# /end: for flowkey in COOKED["flows"]:
		#
		COOKED[dir2]["clients"]["active3"] = active[3]
		COOKED[dir2]["clients"]["active2"] = active[2]
		COOKED[dir2]["clients"]["active1"] = active[1]
		COOKED[dir2]["clients"]["active0"] = active[0]
		#
		# update equivalent
		equivalent = active[3] + active[2] * COOKED["config"]["active3"] + active[1] * COOKED["config"]["active2"] + active[0] * COOKED["config"]["active1"]
		equivalent = max(1.0, equivalent)
		COOKED[dir2]["clients"]["equivalent"] = equivalent
		#
		fairuse_prev = exp2int( COOKED[dir2]["capacity"]["fairuse"] )
		# update fairuse:
		fairuse_int		= int( float(free_int) / float(equivalent) )
		fairuse			= int2exp( fairuse_int )
		COOKED[dir2]["capacity"]["fairuse"] = fairuse
		#
		#print( "fairuse_prev={} fairuse_int={}".format(fairuse_prev, fairuse_int) )
		#if fairuse_prev <> fairuse_int:	# bug: fairuse_int is not yet rounded by int2exp function...
		fairuse_rounded = exp2int( fairuse )
		#print( "fairuse_prev={} fairuse_rounded={}".format(fairuse_prev, fairuse_rounded) )
		if fairuse_prev <> fairuse_rounded:
				fairuse_changed[dir2] = 1
		# /end: for dir2 in ["upstream", "downstream"]:
	#
	# now apply it (if fupol=true):
	if COOKED["global"]["fupol"] == 1:
#		u_fairuse = exp2int( COOKED["upstream"]["capacity"]["fairuse"] )
#		d_fairuse = exp2int( COOKED["downstream"]["capacity"]["fairuse"] )
#		u_fairuse_avg = int( u_fairuse_int )
#		d_fairuse_avg = int( d_fairuse_int )
#		u_struct = {"SID":"0.0.0.0","DID":"223.255.255.255","DIR":"U","RATE":u_fairuse_avg}
#		d_struct = {"SID":"0.0.0.0","DID":"223.255.255.255","DIR":"D","RATE":d_fairuse_avg}
#		u_outstruct = calcPolicerEntry(u_struct)
#		d_outstruct = calcPolicerEntry(d_struct)
#		policer_stuct = {}
#		policer_stuct["policer"] = [ u_outstruct, d_outstruct ]
		#
		#print( "fairuse_changed upstream = {}, downstream = {}".format(fairuse_changed["upstream"], fairuse_changed["downstream"]) )
		if fairuse_changed["upstream"] > 0 or fairuse_changed["downstream"] > 0:
			writePolicerTable()
	#
	# /def update_clients_capacity_COOKED()

def writePolicerTable():
	#
	# the policer.table is made up from one or more of these params:
	# COOKED.global.upstream/downstream
	# COOKED.upstream//downstream.fairuse
	# COOKED.reservations.SID.upstream/downstream
	#
	fupol = COOKED["global"]["fupol"]
	policer_stuct = {}
	policer_stuct["policer"] = []		# declare empy array
	#
	#
	for dir2 in ["upstream", "downstream"]:
		#
		if dir2 == "upstream":
			dir = "U"
		else:
			dir = "D"
		#
		if fupol == 1:
			fairuse = exp2int( COOKED[dir2]["capacity"]["fairuse"] )
		else:
			fairuse = COOKED["global"][dir2]
		#
		fairuse = int( min( max( fairuse, 0), 10E6 ) )
		in_struct = {"SID":"0.0.0.0","DID":"223.255.255.255","DIR":dir,"RATE":fairuse}
		out_struct = calcPolicerEntry( in_struct )
		policer_stuct["policer"].append( out_struct )
		#
		for aSID in COOKED["reservations"]:
			rate = exp2int( COOKED["reservations"][aSID][dir2] )
			rate = int( min( max( rate, 0), 10E6 ) )
			in_struct = {"SID":"{}".format(aSID),"DID":"223.255.255.255","DIR":dir,"RATE":rate}
			out_struct = calcPolicerEntry( in_struct )
			policer_stuct["policer"].append( out_struct )
		#
	myLog = '# write to file "{}" this struct: {}'.format(policer___filename, policer_stuct)
	#myLog = '# write to file "{}" this struct: {}'.format(policer___filename, json.dumps( policer_stuct, sort_keys=True, indent=2) )
	print myLog
	logging.info(myLog)
	writefile_struct(policer_stuct,	policer___filename)
	writefile_struct(policer_stuct,	police2___filename)
	#
	# /def writePolicerTable()

#	# write to file "/proc/qiss/tci/policer.table" this struct: {'policer': [{'PBKT': 109,
#	'ABKT': 91,
#	'DID': '223.255.255.255',
#	'PEAK': 18240,
#	'SID': '0.0.0.0',
#	'AVG': 15200,				# 15.2 kBps --> 1.9 Mbps good!
#	'DIR': 'U'
#	}, {
#	'PBKT': 103,
#	'ABKT': 86,
#	'DID': '223.255.255.255',
#	'PEAK': 17280,
#	'SID': '0.0.0.0',
#	'AVG': 14400,
#	'DIR': 'D'
#	}]}



# ------+++------------+++------------+++------------+++------------+++------
#
class classFlowProcessing(threading.Thread):
	def run(self):
		global COOKED
		init_COOKED()
		while True:
			#
			# DO every interval = COOKED["config"]["refresh"] :
			read_format_RAW_FLOWS()
			format_flows_COOKED()
			#
			# DO every interval = 1 second:
			this_clockSecond = COOKED["timestamp"]["onesec"]
			if this_clockSecond != COOKED["global"]["fasupdate"]:
				COOKED["global"]["fasupdate"] = this_clockSecond
				update_clients_capacity_COOKED()
			#
			time.sleep( COOKED["config"]["refresh"] )

# ------+++------------+++------------+++------------+++------------+++------
#
class RPChandler:
	req_method		= ""
	req_params		= {}
	req_id			= 0
	req_address		= ""
	thread_handle	= 0
	methods = [
				  "showmethods"	,	# 0
				  "check"			,	# 1
				  "reboot"			,	# 2
				  "halt"				,	# 3
				  "monitor"			,	# 4
				  "flowexport"		,	# 5
				  "getflow"			,
				  "setglobal"		,
				  "updateReg"		,
				  "reloadqstcp"   ,
				  "writepolicer"	,
				  "writeallconf"
	]
	def __init__(self, req_method, req_params, req_id, req_address, thread_handle, cur_thread):
		self.req_method		= req_method
		self.req_params		= req_params
		self.req_id				= req_id
		self.req_address		= req_address
		self.thread_handle	= thread_handle
		self.cur_thread		= cur_thread
		self.reqTime			= 0
		if not req_method in self.methods:
			#print "RPChandler __init__ raise"
			logging.info('# Error: req_method="{}" not in RPChandler.methods[]'.format(req_method))
			raise
	def send_result(self, response_result):
		response_struct = {"jsonrpc": "2.0", "result": response_result, "id": self.req_id}
		response_struct["result"]["reqTime"] = self.reqTime
		response_struct["result"]["qsprotver"] = qsprotver
		response_json = json.dumps(response_struct)
		logging.info('# sending response_json={}'.format(response_json))
		self.thread_handle.request.sendall( "{}".format(response_json) + "{EOF}\n")
		return ""
	def handle(self):
		global COOKED
		response_result = {}
		#
		if ("reqTime" in self.req_params):
			self.reqTime = self.req_params["reqTime"]
		#
		if self.req_method == "showmethods":
			return self.send_result( {"success": "showmethods", "methods": copy.deepcopy(self.methods) } )
		#
		if self.req_method == "check":
			ntpsync = check_NTP_sync()
			regpol	= 0
			fupol		= 0
			ffupu		= 0
			ffupd		= 0
			try:
				regpol	= COOKED["global"]["regpol"]
				fupol		= COOKED["global"]["fupol"]
				ffupu		= COOKED["global"]["upstream"]
				ffupd		= COOKED["global"]["downstream"]
			except:
				return
			return self.send_result({ "success": "agent running", "ntpsync": ntpsync, "regpol":regpol, "fupol":fupol, "ffupu":ffupu, "ffupd":ffupd })
		#
		if self.req_method == "reboot":
			msg = "# reboot!"
			print msg
			logging.info(msg)
			self.send_result( {"success": "reboot" } )
			subprocess.Popen("/sbin/reboot")
			return
		#
		if self.req_method == "halt":
			msg = "# halt!"
			print msg
			logging.info(msg)
			self.send_result( {"success": "halt" } )
			subprocess.Popen("/sbin/halt")
			return
		#
		if self.req_method == "monitor":
			if not ("interval" in self.req_params):
				return self.send_result( {"error": "no params.interval (in 10..1000 milliseconds) provided"} )
			interval = self.req_params["interval"]
			if interval < 10 or interval > 1000:
				return self.send_result( {"error": "interval not in range 10..1000 milliseconds" } )
			self.send_result( {"success": "start monitor flow export untill TCP connection end"} )
			try:
				while True:
					flows_struct = readfile(flows_____filename, flows_____defaults)
					time_struct  = readfile(timestamp_filename, timestamp_defaults)
					out_struct = {}
					out_struct["flows"] = flows_struct["flows"]
					out_struct["timestamp"] = time_struct["timestamp"]
					out_json = json.dumps(out_struct)
					out_json = out_json + "{EOF}\n"
					self.thread_handle.request.sendall( out_json )
					time_str = "{}.{}".format( time_struct["timestamp"]["seconds"], time_struct["timestamp"]["microseconds"] )
					print "# send flows.json to {} in thread {} at time {}".format(self.req_address, self.cur_thread.name, time_str)
					time.sleep (interval/1000.0); # time.sleep(secs)
			except:
				logging.info('# Error trying while req_method="{}" (or just a regular TCP.close)'.format(self.req_method))
			return
		#
		if self.req_method == "flowexport":
			if not ("interval" in self.req_params):
				return self.send_result( {"error": "no params.interval (in 10..1000 milliseconds) provided"} )
			interval = self.req_params["interval"]
			if interval < 10 or interval > 1000:
				return self.send_result( {"error": "interval not in range 10..1000 milliseconds" } )
			self.send_result( {"success": "start monitor flow export untill TCP connection end"} )
			try:
				while True:
					out_json = json.dumps(COOKED)
					out_json = out_json + "{EOF}\n"
					self.thread_handle.request.sendall( out_json )
					print_msg = "# send flows.json to {} in thread {} at time {}".format(self.req_address, self.cur_thread.name, COOKED["timestamp"]["mmsst"] )
					#print print_msg
					logging.info(print_msg)
					time.sleep (interval/1000.0)  # time.sleep(secs)
			except:
				logging.info('# Error trying while req_method="{}" (or just a regular TCP.close)'.format(self.req_method))
			return
			#
			#
		if self.req_method == "getflow":
			try:
				out_json = json.dumps(COOKED)
				out_json = out_json + "{EOF}\n"
				self.thread_handle.request.sendall( out_json )
				print_msg = "# getflow to {} in thread {} at time {}".format(self.req_address, self.cur_thread.name, COOKED["timestamp"]["mmsst"] )
				logging.info(print_msg)
			except:
				logging.info('# Error trying in req_method="{}"'.format(self.req_method))
			return
		#
		#
		if self.req_method == "setglobal":
			#
			if not ("regpol" in self.req_params):
				return self.send_result( {"error": "no params.regpol provided"} )
			regpol = self.req_params["regpol"]
			#
			if not ("fupol" in self.req_params):
				return self.send_result( {"error": "no params.fupol provided"} )
			fupol = self.req_params["fupol"]
			#
			if not ("short" in self.req_params):
				return self.send_result( {"error": "no params.short provided"} )
			short = self.req_params["short"]
			#
			if not ("long" in self.req_params):
				return self.send_result( {"error": "no params.long provided"} )
			long = self.req_params["long"]
			#
			if not ("ffupu" in self.req_params):
				return self.send_result( {"error": "no params.ffupu provided"} )
			ffupu = self.req_params["ffupu"]
			#
			if not ("ffupd" in self.req_params):
				return self.send_result( {"error": "no params.ffupd provided"} )
			ffupd = self.req_params["ffupd"]
			#
#			if fupol == 1:
#				ffupu = 0
#				ffupd = 0
			#
			COOKED["global"]["regpol"]			= regpol
			COOKED["global"]["fupol"]			= fupol
			COOKED["global"]["short"]			= short
			COOKED["global"]["long"]			= long
			COOKED["global"]["upstream"]		= ffupu
			COOKED["global"]["downstream"]	= ffupd
			#
			config_struct = {"short":short,"long":long,"alive":300,"period":20}
			writefile_struct(config_struct, config____filename)
			#
			#if ffupu > 0 or ffupd > 0:	# NO: always write table
			writePolicerTable()
			#
			return self.send_result({"success": "setglobal done", "regpol":regpol, "fupol":fupol, "short":short, "long":long})
		#
		if self.req_method == "updateReg":
			#
			if not ("SID" in self.req_params):
				return self.send_result( {"error": "no params.SID provided"} )
			mySID = self.req_params["SID"]
			if not ("DID" in self.req_params):
				return self.send_result( {"error": "no params.DID provided"} )
			myDID = self.req_params["DID"]
			if not ("US" in self.req_params):
				return self.send_result( {"error": "no params.US provided"} )
			myUS = self.req_params["US"]
			if not ("DS" in self.req_params):
				return self.send_result( {"error": "no params.DS provided"} )
			myDS = self.req_params["DS"]
			#
			updateReg(mySID, myUS, myDS)
			#writePolicerTable()
			#
		if self.req_method == "reloadqstcp":
			msg = "# reload qstcp.ko !"
			print msg
			logging.info(msg)
			self.send_result( {"success": "reloadqstcp" } )
			subprocess.Popen("/home/jdg/code/qsboxta/qstcp-reload.sh")
			return
			#
		if self.req_method == "writepolicer":
			#writefile("{}", policer___filename, policer___defaults)
			if not ("policer" in self.req_params):
				return self.send_result( {"error": "no params.policer provided"} )
			policer_struct = self.req_params["policer"]
			return
		#
		if self.req_method == "writeallconf":
			if not ("lab" in self.req_params):
				return self.send_result( {"error": "no params.lab provided"} )
			lab = self.req_params["lab"]
			#			if not ("allconf" in self.req_params):
			#				return self.send_result( {"error": "no params.allconf provided"} )
			#			allconf = self.req_params["allconf"]
			try:
				config	= self.req_params["config"]
				links		= self.req_params["links"]
				das		= self.req_params["das"]
				policer	= self.req_params["policer"]
			except:
				return self.send_result( {"error": "no params.allconf no: config/links/das/policer"} )
			#
			#config	= replace_defaults(config	, config____defaults)
			replace_defaults(config	, config____defaults)
			replace_defaults(links	, links_____defaults)
			replace_defaults(policer	, policer___defaults)
			if lab == 1:
				replace_defaults(das		, das1______defaults)
			else:
				replace_defaults(das		, das2______defaults)
			#print "#	writeallconf: das={}".format(das)
			try:
				writefile_struct(config,		config____filename)
				print "#	writeallconf: done writefile(config)"
				writefile_struct(links,		links_____filename)
				print "#	writeallconf: done writefile(links)"
				writefile_struct(das,			das_______filename)
				print "#	writeallconf: done writefile(das)"
				writefile_struct(policer,	policer___filename)
				print "#	writeallconf: done writefile(policer)"
			except:
				return self.send_result( {"error": "error try write allconf"} )
			#
			#			try:
			#				config	= readfile(config____filename, {})
			#				links		= readfile(links_____filename, {})
			#				das		= readfile(das_______filename, {})
			#				policer	= readfile(policer___filename, {})
			#			except:
			#				return self.send_result( {"error": "error try read allconf"} )
			#
			return self.send_result( {"success": "written allconf"} )
		#
		self.thread_handle.request.sendall( "{EOF}\n" )
		return # this will return to myThreadedTCPRequestHandler and end the thread
		#
	# /def handle(self):
# /class RPChandler

# ------+++------------+++------------+++------------+++------------+++------
#
class myThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):
	def handle(self):
		request_json = self.request.recv(1400)
		cur_thread = threading.current_thread()
		req_address = self.client_address[0]
		logging.info("# request from {} in thread {}".format(req_address, cur_thread.name))
		response_json = self.JSON_RPC_dispatch(request_json, req_address, thread_handle=self, cur_thread=cur_thread)
	#
	#self.request.sendall( "{}\n".format(response_json) )
	#cant do?: self.wfile.write(response_json)
	#
	def JSON_RPC_dispatch(self, request_json, req_address, thread_handle, cur_thread):
		logging.info('received request_json={}'.format(request_json))
		response_struct	= {}
		myRPChandler		= ""
		request_struct		= {"jsonrpc": "2.0", "result": {}, "id": 0}
		help_str = ""
		try:
			request_struct = json.loads(request_json)
			try:
				req_jsonrpc = request_struct["jsonrpc"]
				req_method	= request_struct["method"]
				req_params	= request_struct["params"]
				req_id		= request_struct["id"]
				if req_jsonrpc != "2.0":
					help_str = " (jsonrpc != 2.0)"
					raise
				if not isinstance(req_params, dict):
					help_str = " (no list of params)"
					raise
				if req_id < 1 or req_id > 1023:
					help_str = " (id not 1..1023)"
					raise
				try:
					#print "# valid JSON, now check method.name at RPChandler()"
					# this will fail if method unknown:
					myRPChandler = RPChandler(req_method, req_params, req_id, req_address, thread_handle, cur_thread=cur_thread)
				#print "JSON_RPC_dispatch after RPChandler()"
				except:
					response_struct = {"jsonrpc": "2.0", "error": {"code": -32601, "message": "JSON-RPC Method not found"}, "id": req_id}
			except:
				response_struct = {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid JSON Request{}".format(help_str)}, "id": 0}
		except:
			response_struct = {"jsonrpc": "2.0", "error": {"code": -32700, "message": "JSON parse error"}, "id": 0}
		#
		if response_struct != {}:
			logging.info("# request failed from {} in thread {}".format(req_address, cur_thread.name))
			response_json = json.dumps(response_struct)
			thread_handle.request.sendall( "{}".format(response_json) + "{EOF}\n" )
			return
		#
		# from this point, all checks are ok, execute the 'handle':
		logging.info("# request succeeded from {} in thread {}".format(req_address, cur_thread.name))
		if isinstance(myRPChandler, RPChandler):
			response_result = myRPChandler.handle()
			#response_struct = {"jsonrpc": "2.0", "result": response_result, "id": req_id}
			#response_json = json.dumps(response_struct)
			return #response_json

class myThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
	pass

# ------+++------------+++------------+++------------+++------------+++------
#
if __name__ == "__main__":
	logging.info('start daemon')
	#
	#HOST, PORT = "0.0.0.0", qiss_mocon_port
	HOST, PORT = "0.0.0.0", port_qsbox
	#
	# problem:
	#> ./qsboxta.py --daemon
	#Traceback (most recent call last):
	#	File "./qsboxta.py", line 284, in <module>
	#		myServer = myThreadedTCPServer((HOST, PORT), myThreadedTCPRequestHandler)
	#			File "/usr/lib/python2.7/SocketServer.py", line 420, in __init__
	#    self.server_bind()
	#		 File "/usr/lib/python2.7/SocketServer.py", line 434, in server_bind
	#self.socket.bind(self.server_address)
	#	File "/usr/lib/python2.7/socket.py", line 224, in meth
	#		return getattr(self._sock,name)(*args)
	#socket.error: [Errno 98] Address already in use
	#
	# that is because of TIME_WAIT :
	# http://stackoverflow.com/questions/6380057/python-binding-socket-address-already-in-use
	#
	#> fuser -uv 24032/tcp
	#							 USER        PID ACCESS COMMAND
	#24032/tcp:           root       6462 F.... (root)python
	
	#ip, port = server.server_address
	# Start a thread with the server -- that thread will then start one more thread for each request
	#	myServer_thread = threading.Thread(target=myServer.serve_forever)
	#	# Exit the server thread when the main thread terminates
	#	myServer_thread.daemon = True
	#	myServer_thread.start()
	#	print "Server loop running in thread:", myServer_thread.name
	#	while 1:
	#		time.sleep(250)
	#
	FlowProcessing_thread = classFlowProcessing()
	FlowProcessing_thread.daemon = True
	FlowProcessing_thread.start()
	logging.info("# FlowProcessing_thread running in thread: {}".format(FlowProcessing_thread.name) )
	#
	logging.info("# Starting Server on host:port = {}:{}".format(HOST, PORT))
	try:
		# special start with 'allow_reuse_address = True'
		myServer = myThreadedTCPServer((HOST, PORT), myThreadedTCPRequestHandler, False)
		myServer.allow_reuse_address = True		# Prevent 'cannot bind to address' errors on restart
		myServer.server_bind()						# Manually bind, to support allow_reuse_address
		myServer.server_activate()					# (see above comment)
		myServer.serve_forever()
	except KeyboardInterrupt:
		logging.info("\n# Exiting Server (KeyboardInterrupt)\n")
		myServer.shutdown()
		myServer.server_close()
		sys.exit(0)
	except OSError:
		msg = "\n# Server Start (Open/Bind) ERROR (OSError='{}') \n".format( OSError )
		logging.info("# Error Popen() OSError {}".format( OSError ))
	except:
		exc_type, exc_value, exc_traceback = sys.exc_info()
		msg = "\n# Server Start (Open/Bind) ERROR (exc_type='{}') \n".format( exc_type )
		print msg
		logging.info(msg)
		logging.exception('')
		sys.exit(0)

# ------+++------------+++------------+++------------+++------------+++------
#

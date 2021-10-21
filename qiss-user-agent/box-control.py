#!/usr/bin/env python

#./box-control.py --write-policer-simple '{"SID":"010.008.001.021","DID":"089.106.166.031","AVG":62}'
#./box-control.py --write-policer-simple '{"SID":"010.008.001.021","DID":"089.106.166.031","AVG":625}'
#./box-control.py --write-policer-simple '{"SID":"010.008.001.021","DID":"089.106.166.031","AVG":6250}'
#./box-control.py --write-policer-simple '{"SID":"010.008.001.021","DID":"089.106.166.031","AVG":11750}'

#> ./box-control.py --write-policer-simple '{"SID":"010.008.001.021","DID":"089.106.166.031","AVG":6250}'
## writing JSON to file: /proc/qiss/tci/policer.table
#{"policer": [{"PBKT": 45.0, "DID": "089.106.166.031", "ABKT": 37.5, "PEAK": 7500.0, "SID": "010.008.001.021", "AVG": 6250, "DIR": "U"}, {"PBKT": 45.0, "DID": "089.106.166.031", "ABKT": 37.5, "PEAK": 7500.0, "SID": "010.008.001.021", "AVG": 6250, "DIR": "D"}]}

# ArgumentParser example from:
# https://github.com/edaniszewski/MultithreadedTCP

# Python info:
# https://docs.python.org/2.7/library/argparse.html
# https://docs.python.org/2/howto/argparse.html
import argparse

import json
import pprint

# https://docs.python.org/2.7/library/re.html?highlight=regular
# re.subn(pattern, repl, string, count=0, flags=0)
# Perform the same operation as sub(), but return a tuple (new_string, number_of_subs_made).

import re # regular expression

# https://www.python.org/dev/peps/pep-3119/
# PEP 3119 -- Introducing Abstract Base Classes
#import inspect
#pprint.pprint( inspect.getmembers(time_struct) )

# https://docs.python.org/2/library/functions.html#isinstance
import sys

# https://docs.python.org/2/library/string.html
# https://docs.python.org/2/library/stdtypes.html#string-formatting
# string formatting using %

# https://docs.python.org/2/library/math.html
import math

# Python built-in functions
# https://docs.python.org/2/library/functions.html

parser = argparse.ArgumentParser(
  formatter_class=argparse.RawDescriptionHelpFormatter,
  description="qsbox commandline control program.\nRead/write /proc/qiss/ files using commandline arguments.",
  epilog="Note when writing that the JSON structures do not need to be complete (missing keys will get a default value).\n"
         "Example JSON string (use the 'single-quotes'): '{\"short\":50,\"long\":1000,\"alive\":300,\"period\":20}'\n"
			"Use '{}' to get all defaults."
)
parser.add_argument('--read-flows',	action='store_true', help='Read and JSON-parse /proc/qiss/tcpo/flows.table')
parser.add_argument('--read-time',	action='store_true', help='Read and JSON-parse /proc/qiss/tcpo/timestamp')
parser.add_argument('--write-config',	metavar='JSON', help='Write JSON to /proc/qiss/config')
parser.add_argument('--write-links',	metavar='JSON', help='Write JSON to /proc/qiss/tci/links.table')
parser.add_argument('--write-das',		metavar='JSON', help='Write JSON to /proc/qiss/tci/das.table')
parser.add_argument('--write-policer', metavar='JSON', help='Write JSON to /proc/qiss/tpi/policer.table')
parser.add_argument('--write-simple', metavar='JSON', help='Write JSON to /proc/qiss/tpi/policer.table')
parser.add_argument('--start-udp',	metavar='RATE', help='Start qsgen UDP traffic generator at X Mbps symmetrical')
parser.add_argument('--start-tcp',	metavar='RATE', help='Start qsgen TCP traffic generator at X Mbps symmetrical')
parser.add_argument('--stop-udp',	metavar='RATE', help='Stop qsgen UDP traffic generator')
parser.add_argument('--stop-tcp',	metavar='RATE', help='Stop qsgen TCP traffic generator')
parser.add_argument('--show-calcs', metavar='RATE', help='Show Logarithmic calculations for given line-rate')

# ./box-control.py --write-config '{"short":50,"long":1000,"alive":300,"period":20}'

args = parser.parse_args()
#pprint.pprint( vars(args) )

if len(sys.argv)==1:
	parser.print_help()
	sys.exit(1)

config__filename = "/proc/qiss/config"
links___filename = "/proc/qiss/tci/links.table"
das_____filename = "/proc/qiss/tci/das.table"
policer_filename = "/proc/qiss/tci/policer.table"
flows___filename = "/proc/qiss/tcpo/flows.table"
time____filename = "/proc/qiss/tcpo/timestamp"
# tests:
#config__filename = "proc_qiss_config"
#links___filename = "proc_qiss_tci_links.table"
#das_____filename = "proc_qiss_tci_das.table"
#policer_filename = "proc_qiss_tpi_policer.table"

config____defaults = {"short":50,"long":1000,"alive":300,"period":20}
config2___defaults = {"short":50,"long":1000,"alive":300,"period":20,"dscp_decimal":{"fic0":0,"fic1":34,"fic2":44,"fic3":46}}
links_____defaults = {"links":[{"name": "br1","icap": 94000,"ecap": 94000}]}

#das_______defaults = {"das":[{ "5698FFFF":["02000000/08","4D040000/16"]},{"DFFFFFFF":["00000000/00"]}]}
#das_______defaults = {"das":[{ "596AA67F":["0A080000/16","596AA600/25"]},{"DFFFFFFF":["00000000/00"]}]}
#das_______defaults = {"das":[{ "596AA6FF":["0A580000/16","596AA680/25"]},{"DFFFFFFF":["00000000/00"]}]}
das_______defaults = {"das":[{"223.255.255.255":["0.0.0.0/0"]}]}

policer___defaults = {"policer":[{"SID":"10.8.1.78","DID":"89.106.166.26","DIR":"U","AVG":100,"ABKT":80,"PEAK":200,"PBKT":80},{"SID":"0.0.0.0","DID":"0.0.0.0","DIR":"U","AVG":200,"ABKT":80,"PEAK":200,"PBKT":80}]}
timestamp_defaults = {"timestamp": {"seconds":0,"microseconds":0}}
flows_____defaults = {"flows":[{}]}

# > ./box-control.py --write-das '{}'
# # writing JSON to file: /proc/qiss/tci/das.table
# {"das": [{"596AA6FF": ["0A080000/16", "0A580000/16", "596AA680/25"]}, {"DFFFFFFF": ["00000000/00"]}]}
# > cat /proc/qiss/tci/das.table
# 10.8.0.0-89.106.166.255-16
# 10.88.0.0-89.106.166.255-16
# 89.106.166.128-89.106.166.255-25
# 0.0.0.0-223.255.255.255-0

def readfile(filename):
	try:
		in_file = open(filename, 'r')
		in_json = in_file.read()
		in_file.close()
		print "# read file: ", filename
		#in_json.replace("},]", "}]")
		(in_json, number_of_subs_made) = re.subn('},]', '}]', in_json)
		if number_of_subs_made > 0:
			print "# Semi-Error: found some 'trailing-commas' but I could remove them"
		#print "# contents of file: ", filename
		#print in_json
		try:
			in_struct = json.loads(in_json)
			print "# Success parsing JSON"
			#replace_lists(in_struct)
			#pprint.pprint( in_struct )
			out_struct = in_struct
			#msg_struct['timestamp'] = time_struct['timestamp']
			out_json = json.dumps(out_struct, indent=1)
			print "# Dump back to JSON:"
			print out_json
		except:
			print "# Error parsing JSON in file: ", filename
	except:
		print "# Error reading file: ", filename

def replace_lists(mydict):
	if isinstance(mydict, dict):
		for key in mydict.keys():
			value = mydict[key]
			if isinstance(value, dict):
				replace_lists(value)
			if isinstance(value, list):
				mydict[key] = 'replace_lists({0}:items)'.format( len(value) )

def replace_defaults(mystruct, defaults):
	for key in defaults.keys():
		value = defaults[key]
		if not (key in mystruct):
			mystruct[key] = value
		else:
			struct_value = mystruct[key]
			if isinstance(value, dict) and isinstance(struct_value, dict):
				replace_defaults(struct_value, value)

def writefile(in_str, filename, defaults):
	try:
		in_struct = json.loads(in_str)
		try:
			replace_defaults(in_struct, defaults)
		except:
			print "# Error processing defaults"
		out_json = json.dumps(in_struct)
		print "# writing JSON to file:", filename
		print out_json
		try:
			out_file = open(filename, 'w')
			out_file.write( out_json + "\n" )
			out_file.close()
		except:
			print "# Error writing to file: ", filename
	except:
		print "# Error parsing JSON"

# test:
# ./box-control.py --write-simple '{"SID":"0.0.0.0","DID":"223.255.255.255","AVG":200}'		# AVG=200 -> Limit=1.6 Mbps, so AVG in kBps

def writefile_simple(in_str, filename, defaults):
	try:
		in_struct = json.loads(in_str)
		AVG = 0
		if "AVG" in in_struct:			# in kBps
			AVG = in_struct["AVG"]
		if "RATE" in in_struct:			# in bps
			AVG = in_struct["RATE"] * 1000 / 8
		#
		in_struct["AVG"] = int( AVG )
		in_struct["ABKT"] = float( AVG * 0.005 )
		in_struct["ABKT"] = int( min(max(in_struct["ABKT"],15),1000) )
		in_struct["PEAK"] = int( AVG * 1.2 )
		in_struct["PBKT"] = int( in_struct["ABKT"] * 1.2 )
		in_struct["DIR"] = "U"
		in2_struct = {}
		in2_struct["SID"] = in_struct["SID"]
		in2_struct["DID"] = in_struct["DID"]
		in2_struct["AVG"] = in_struct["AVG"]
		in2_struct["ABKT"] = in_struct["ABKT"]
		in2_struct["PEAK"] = in_struct["PEAK"]
		in2_struct["PBKT"] = in_struct["PBKT"]
		in2_struct["DIR"] = "D"
		new_struct = {}
		new_struct["policer"] = [ in_struct, in2_struct ]
		out_json = json.dumps(new_struct)
		print "# writing JSON to file:", filename
		print out_json
		try:
			out_file = open(filename, 'w')
			out_file.write( out_json + "\n" )
			out_file.close()
		except:
			print "# Error writing to file: ", filename
	except:
		print "# Error parsing JSON"

if args.read_time:
	readfile(time____filename)

if args.read_flows:
	readfile(flows___filename)

if args.write_config:
	writefile(args.write_config, config__filename, config____defaults)

if args.write_links:
	writefile(args.write_links, links___filename, links_____defaults)

if args.write_das:
	writefile(args.write_das, das_____filename, das_______defaults)

if args.write_policer:
	writefile(args.write_policer, policer_filename, policer___defaults)

if args.write_simple:
	writefile_simple(args.write_simple, policer_filename, policer___defaults)

if args.start_udp:
	rate = args.start_udp

def LogHex(rate, linerate_log10):
	log_f = math.log(float(rate), 10) * (16.0/int(linerate_log10))
	log = min(max( int(log_f), 0),15)
	return (log_f, log)

def LogHexString(linerate_log10):
	bps = ["x"]*16
	bpsstring = "" #"bps"
	for s in range(16):
		x = pow(10, float(s)*float(linerate_log10)/16.0)
		if (x/1E3) < 1:
			bps[s] = "%X:%d%s" % (s, x, bpsstring)
		elif (x/1E6) < 1:
			bps[s] = "%X:%dk%s" % (s, x/1E3, bpsstring)
		elif (x/1E9) < 1:
			bps[s] = "%X:%dM%s" % (s, x/1E6, bpsstring)
		elif (x/1E12) < 1:
			bps[s] = "%X:%dG%s" % (s, x/1E9, bpsstring)
		else:
			bps[s] = "%X:%dTbps" % (s, x/1E12, bpsstring)
	return "LogHex[0-F]=16*Log10(rate-bps)/Log10(linerate-bps) " + ",".join(bps)

if args.show_calcs:
	linerate = float(args.show_calcs)
	linerate_log10 = int( math.log(linerate, 10) )
	print "show-calcs (linerate: %1.1e bps, linerate_log %d):" % (linerate, linerate_log10)
	#print "LogHex[0-F]=16*Log10(rate-bps)/Log10(linerate-bps)"
	steps = 3
	for s in range( (linerate_log10)*steps+1 ):
		x = pow(10, float(s)/float(steps) )
		if (x/1E3) < 1:
			bps = "%7.0f  bps" % (x)
		elif (x/1E6) < 1:
			bps = "%7.0f kbps" % (x/1E3)
		elif (x/1E9) < 1:
			bps = "%7.0f Mbps" % (x/1E6)
		elif (x/1E12) < 1:
			bps = "%7.0f Gbps" % (x/1E9)
		elif (x/1E15) < 1:
			bps = "%7.0f Tbps" % (x/1E12)
		else:
			bps = "%7.0f Fbps" % (x/1E15)
		(log_f, log) = LogHex(x, linerate_log10)
		print "%2.1E = %s : log = %1.1f logx=%X" % (x, bps, log_f, log)
	#
	print LogHexString(linerate_log10)

#	> ./box-control.py --show-calcs 1E7
#	show-calcs (linerate: 1.0e+07 bps, linerate_log 7):
#	1.0E+00 =       1  bps : log = 0.0 logx=0
#	2.2E+00 =       2  bps : log = 0.8 logx=0
#	4.6E+00 =       5  bps : log = 1.5 logx=1
#	1.0E+01 =      10  bps : log = 2.3 logx=2
#	2.2E+01 =      22  bps : log = 3.0 logx=3
#	4.6E+01 =      46  bps : log = 3.8 logx=3
#	1.0E+02 =     100  bps : log = 4.6 logx=4
#	2.2E+02 =     215  bps : log = 5.3 logx=5
#	4.6E+02 =     464  bps : log = 6.1 logx=6
#	1.0E+03 =       1 kbps : log = 6.9 logx=6
#	2.2E+03 =       2 kbps : log = 7.6 logx=7
#	4.6E+03 =       5 kbps : log = 8.4 logx=8
#	1.0E+04 =      10 kbps : log = 9.1 logx=9
#	2.2E+04 =      22 kbps : log = 9.9 logx=9
#	4.6E+04 =      46 kbps : log = 10.7 logx=A
#	1.0E+05 =     100 kbps : log = 11.4 logx=B
#	2.2E+05 =     215 kbps : log = 12.2 logx=C
#	4.6E+05 =     464 kbps : log = 13.0 logx=C
#	1.0E+06 =       1 Mbps : log = 13.7 logx=D
#	2.2E+06 =       2 Mbps : log = 14.5 logx=E
#	4.6E+06 =       5 Mbps : log = 15.2 logx=F
#	1.0E+07 =      10 Mbps : log = 16.0 logx=F
#	LogHex[0-F]=16*Log10(rate-bps)/Log10(linerate-bps) 0:1,1:2,2:7,3:20,4:56,5:153,6:421,7:1k,8:3k,9:8k,A:23k,B:64k,C:177k,D:486k,E:1M,F:3M

#


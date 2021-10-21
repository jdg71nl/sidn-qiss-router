#!/usr/bin/python

#> echo "test" | nc 127.0.0.1 24041
#Thread-4: test
#
#> echo "test" | nc 172.16.53.140 24041
#Thread-4: test
#
#> echo '{"jsonrpc": "2.0","method": "setGenerator", "params": {"prot": "tcp", "speed": 50 }, "id": 1}' | nc 172.16.53.140 24041

# > echo '{"jsonrpc": "2.0","method":"start","params":{"x":1,"y":2},"id":1}' | nc 172.16.53.140 24041
# {"jsonrpc": "2.0", "result": {"request_address": "172.16.53.1", "req_method": "start", "req_params": {"y": 2, "x": 1}}, "id": 1}
#
#> echo '{"jsonrpc": "2.0","method":"some","params":{"x":1,"y":2},"id":1}' | nc 172.16.53.140 24041
# {"jsonrpc": "2.0", "id": 1, "error": {"message": "Method not found", "code": -32601}}
#
#> echo '{"jsonrpc": "2.0","method":"start","params":{"x":1,"y":2},"id":0}' | nc 172.16.53.140 24041
# {"jsonrpc": "2.0", "id": 0, "error": {"message": "Invalid Request", "code": -32600}}
#
#> echo '{"not_jsonrpc": "2.0","method":"start","params":{"x":1,"y":2},"id":1}' | nc 172.16.53.140 24041
# {"jsonrpc": "2.0", "id": 0, "error": {"message": "Invalid Request", "code": -32600}}
#
#> echo '"jsonrpc": "2.0","method":"start","params":{"x":1,"y":2},"id":1}' | nc 172.16.53.140 24041
# {"jsonrpc": "2.0", "id": 0, "error": {"message": "Parse error", "code": -32700}}

# SocketServer:
# http://stackoverflow.com/questions/16904780/python-socket-and-socketserver
# https://pymotw.com/2/SocketServer/
# http://kmkeen.com/socketserver/
# http://www.bogotobogo.com/python/python_network_programming_socketserver_framework_for_network_servers.php
# https://docs.python.org/2/library/socketserver.html
import SocketServer

# https://docs.python.org/2/library/threading.html
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
import socket

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

import argparse
import time
import sys

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

parser = argparse.ArgumentParser(description="qsgen - QISS Traffic Generator")
parser.add_argument('--start',   action='store_true', help='Start on Command-Line')
parser.add_argument('--daemon',  action='store_true', help='Start as Daemon')
args = parser.parse_args()
if len(sys.argv)==1:
	parser.print_help()
	sys.exit(1)

# global:
PID	= 0
PROC	= 0

class RPChandler:
	req_method		= ""
	req_params		= {}
	req_id			= 0
	req_address		= ""
	thread_handle	= 0
	methods = [
		"start",
		"stop",
		"check"
	]
	def __init__(self, req_method, req_params, req_id, req_address, thread_handle):
		self.req_method	= req_method
		self.req_params	= req_params
		self.req_id			= req_id
		self.req_address	= req_address
		self.thread_handle = thread_handle
		if not req_method in self.methods:
			#print "RPChandler __init__ raise"
			raise
	def send_result(self, response_result):
		response_struct = {"jsonrpc": "2.0", "result": response_result, "id": self.req_id}
		response_json = json.dumps(response_struct)
		self.thread_handle.request.sendall( "{}\n".format(response_json) )
		return ""
	def handle(self):
		response_result = {}
		global PID
		global PROC
		if self.req_method == self.methods[0]:			# "start"
			if PID > 0:
				return self.send_result( {"error": "another process is running", "pid": "{}".format(PID) } )
				# if we return here, it will end myThreadedTCPRequestHandler
			else:
				if not ("cmd" in self.req_params):
					return self.send_result( {"error": "no params.cmd provided"} )
				cmd = self.req_params["cmd"]
				try:
					#cmdstr = "/usr/bin/find /home -type f"
					#cmdstr = "ls"
					PROC = subprocess.Popen("exec " + cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
					#PROC = subprocess.Popen("exec " + cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True, preexec_fn=os.setsid)
					PROC = subprocess.Popen("exec " + cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True, preexec_fn=os.setsid)
					PID = PROC.pid
					self.send_result( {"success": "started new process", "pid": "{}".format(PID) } )
					#self.thread_handle.request.sendall( "{}\n".format(response_json) )
					if ("stdout" in self.req_params):
						while True:
							#line = PROC.communicate("")
							line = PROC.stdout.readline()
							self.thread_handle.request.sendall( line )
							PROC.poll()
							if PROC.returncode != None:
								break
				except:
					#print "Unexpected error:", sys.exc_info()[0]
					#raise
					pass
				finally:
					PID = 0
					if isinstance(PROC, subprocess.Popen):
						PROC.kill()
						#os.killpg( os.getpgid( PROC.pid ), signal.SIGTERM )
				return
				# if we return here, it will end myThreadedTCPRequestHandler
		#
		if self.req_method == self.methods[1]:			# "stop"
			if not ("pid" in self.req_params):
				return self.send_result( {"error": "no params.pid provided"} )
			if self.req_params["pid"] == "any":
				if PID > 0:
					self.req_params["pid"] = PID
				else:
					return self.send_result( {"success": "but no running process to stop" } )
			elif self.req_params["pid"] != PID:
				return self.send_result( {"error": "no such process pid: {}".format(self.req_params["pid"]) } )
			PID = 0
			if isinstance(PROC, subprocess.Popen):
				#PROC.terminate()
				PROC.kill()
				#os.killpg( os.getpgid( PROC.pid ), signal.SIGTERM )
			return self.send_result( {"success": "stopped process pid: {}".format(self.req_params["pid"]) } )

		#
		if self.req_method == self.methods[2]:			# "check"
			if PID > 0:
				return self.send_result( {"success": "a process is running", "pid": "{}".format(PID) } )
			else:
				return self.send_result( {"success": "no process is running" } )
		#
		self.thread_handle.request.sendall( "{}\n" )
		return # this will return to myThreadedTCPRequestHandler and end the thread

def JSON_RPC_dispatch(request_json, req_address, thread_handle):
	response_struct	= {}
	myRPChandler		= ""
	request_struct		= {"jsonrpc": "2.0", "result": {}, "id": 0}
	try:
		request_struct = json.loads(request_json)
		try:
			req_jsonrpc = request_struct["jsonrpc"]
			req_method	= request_struct["method"]
			req_params	= request_struct["params"]
			req_id		= request_struct["id"]
			if req_jsonrpc != "2.0" or not isinstance(req_params, dict) or req_id < 1 or req_id > 1023:
				raise
			try:
				#print "JSON_RPC_dispatch before RPChandler()"
				# this will fail if method unknown:
				myRPChandler = RPChandler(req_method, req_params, req_id, req_address, thread_handle)
				#print "JSON_RPC_dispatch after RPChandler()"
			except:
				response_struct = {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": req_id}
		except:
			response_struct = {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": 0}
	except:
		response_struct = {"jsonrpc": "2.0", "error": {"code": -32700, "message": "Parse error"}, "id": 0}
	#
	# from this point, all checks are ok, execute the 'handle':
	if isinstance(myRPChandler, RPChandler):
		response_result = myRPChandler.handle()
		#response_struct = {"jsonrpc": "2.0", "result": response_result, "id": req_id}
	#response_json = json.dumps(response_struct)
	return #response_json

class myThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):
	def handle(self):
		request_json = self.request.recv(1400)
		#cur_thread = threading.current_thread()
		#response = "{}: {}".format(cur_thread.name, data)
		req_address = self.client_address[0]
		response_json = JSON_RPC_dispatch(request_json, req_address, thread_handle=self)
		#
		#self.request.sendall( "{}\n".format(response_json) )
		#cant do?: self.wfile.write(response_json)

class myThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
	pass

if __name__ == "__main__":
	HOST, PORT = "0.0.0.0", qiss_tgcon_port
	myServer = myThreadedTCPServer((HOST, PORT), myThreadedTCPRequestHandler)
	#ip, port = server.server_address
	# Start a thread with the server -- that thread will then start one more thread for each request
	myServer_thread = threading.Thread(target=myServer.serve_forever)
	# Exit the server thread when the main thread terminates
	myServer_thread.daemon = True
	myServer_thread.start()
	print "Server loop running in thread:", myServer_thread.name
	while 1:
		try:
			time.sleep(250)
		except KeyboardInterrupt:
			print "\nExiting Server\n"
			break
		finally:
			myServer.shutdown()
			myServer.server_close()

#

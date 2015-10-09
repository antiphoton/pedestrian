#!/usr/bin/python3
import http.server
import socketserver
import os

PORT = 1432

if os.fork():

	Handler = http.server.SimpleHTTPRequestHandler

	httpd = socketserver.TCPServer(("", PORT), Handler)

	httpd.serve_forever()
else:
	url='/opt/chromium/chrome --incognito 127.0.0.1:%d/monitor.html' % PORT;
	if os.environ['DISPLAY']:
		os.system(url);
	else:
		print(url);


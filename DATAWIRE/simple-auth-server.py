#!/usr/bin/env python

import sys

# import functools
# import json
import logging
import pprint

from flask import Flask, Response, jsonify, request

__version__ = '0.0.1'

logging.basicConfig(
    # filename=logPath,
    level=logging.DEBUG, # if appDebug else logging.INFO,
    format="%%(asctime)s ambassador %s %%(levelname)s: %%(message)s" % __version__,
    datefmt="%Y-%m-%d %H:%M:%S"
)

logging.info("initializing")

app = Flask(__name__)

@app.before_request
def before():
    print("---- Incoming Request Headers")
    pprint.pprint(request)

    for header in sorted(request.headers.keys()):
        print("%s: %s" % (header, request.headers[header]))

    print("----")

@app.route('/', defaults={'path': ''})
@app.route('/<path:path>')
def catch_all(path):
    resp = Response('You want path: %s' % path)

    if path.endswith("/good/"):
        resp.status_code = 200
        resp.headers['X-Hurkle'] = 'Oh baby, oh baby.'
    elif path.endswith("/nohdr/"):
        resp.status_code = 200
        # Don't add the header.
    else:
        resp.status_code = 403

    resp.headers['X-Test'] = 'Should not be seen.'
    return resp

app.run(host='127.0.0.1', port=3000, debug=True)

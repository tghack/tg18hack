#!/bin/bash

socat tcp-listen:1999,fork,reuseaddr exec:"python2 server.py"

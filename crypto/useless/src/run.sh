#!/bin/bash

socat -v tcp-listen:1050,fork,reuseaddr EXEC:"python2 server.py"

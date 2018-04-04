#!/bin/bash

socat -v tcp-listen:8888,fork,reuseaddr EXEC:"sage server.sage"
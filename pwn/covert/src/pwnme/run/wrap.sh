#!/bin/sh

socat -v tcp-listen:6060,fork,reuseaddr exec:/pwnme,stderr

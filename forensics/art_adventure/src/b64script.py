#!python2.7

import base64
import codecs

flag="TG18{PIET IS FUN!}"
rot13flag=codecs.encode(flag, "rot-13")
mycode= """
var='""" + rot13flag +"""'
print var
"""
secret =base64.b64encode(mycode)
print("secret: " +secret)
secret1="CnZhcj0nR1QxOHtDVlJHIFZGIFNIQSF9JwpwcmludCB2YXIK"
decoded = base64.b64decode(secret1)
print decoded
stuff=eval(compile(decoded,'<string>','exec'))
###
# base64: cHJpbnQgJ0VSMTh7QVRQRSBURCBRRlkhfScg
# rot11: ER18{ATPE TD QFY!}
##

flag= codecs.decode((decoded),"rot-13")
print flag

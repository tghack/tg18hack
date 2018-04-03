# Unbreakable Cipher
**Points: 200**

**Author: kristebo**

Some of the worlds strongest minds have been at this.
Can you decode this [encoded text](encoded.txt) and fetch the flag?


```
Old mac donald had a farm i a i a o!
```

We have some stolen code that may help you:

```python
def decode(msg,key):
    dc = ''
    key_length = len(key)
    alphabet_length = len(alphabet)
    for i, char in enumerate(msg):
        msgInt = alphabet.find(char)
        encInt = -alphabet.find(key[i % key_length])

        if msgInt == -1 or encInt == -1:
            dc+=' '

        decoded = (msgInt + encInt) % alphabet_length

        dc += alphabet[decoded]
    remapped=remap(dc)    
    return remapped

def remap(msg):
      print(msg)
      msg=msg.replace('m','-')
      msg=msg.replace('a','.')
      msg=msg.replace('p','/')
      msg=msg.replace('s',' ')

      return msg

```


```python
filter(lambda p: all(p % n for n in range(3, int(__import__("math").sqrt(p)) + 1, 2)), range(3, 10000, 2))[893]+2
```

Don't forget `TG18{...}` when submitting the flag!

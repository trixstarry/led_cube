# The Pickler!
# Author: Thao-Trang "Valerie" Hoang

import pickle

def isString(s):
    try:
        s += ''
    except:
        return False
    return True

def hexify(num):
    num = "%x" % num
    if len(num) % 2:
        num = '0'+num
    return num.decode('hex')

pattern = []

#f1 = [""]
f1 = ["\x01\xf8\xc6\x3f\x00\x07\x29\xc0\x00\x00\x10\x00\x00\x07\x39\xc0\x01\xf8\xd6\x3f","\x00\x00\x00\x00\x01\xf8\xc6\x3f\x00\x07\x29\xc0\x01\xf8\xd6\x3f\x01\xf8\xd6\x3f","\x00\x00\x00\x00\x00\x00\x00\x00\x01\xf8\xc6\x3f\x01\xff\xff\xff\x00\x00\x00\x00",500]
f2 = ["\x00\x10\xfc\x21\x00\x02\x10\x84\x00\x00\x3c\x00\x00\x42\x10\x80\x01\x17\x39\xd1","\x00\x00\x78\x00\x01\xf2\x10\x84\x00\x00\x3c\x00\x00\x42\x10\x9f\x01\x04\x10\x41","\x00\x00\x78\x00\x00\x02\x10\x84\x01\x08\x7e\x10\x00\x42\x10\x9f\x00\x13\x29\x90",500]
f3 = ["\x01\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\xff\xff\xff","\x00\x00\x00\x00\x01\xff\xff\xff\x00\x00\x00\x00\x01\xff\xff\xff\x01\xec\xd6\x6f","\x00\x00\x00\x00\x00\x00\x00\x00\x01\xff\xff\xff\x01\xff\xff\xff\x00\x13\x29\x90",500]

#p1 = ["Pattern1",[[f1],[f2],[f3]]]
p1 = {"patterns":"Pattern1","frames":[f1,f2,f3]}

#print p1["frames"]
#print p1["frames"][0][0]
for i,x in enumerate(p1['frames']):
    for j,y in enumerate(x):
        if(isString(y)):
            z = y
            p1['frames'][i][j] = int(y.encode('hex'),16)
            print p1['frames'][i][j]
            if (hexify(p1['frames'][i][j]) == z):
                print "Success"
            else:
                print "Failed!"
print p1
pattern.append(p1)
pattern.append("this is a stringy string")
pickle.dump(pattern,open("patterns.pkl","wb"))

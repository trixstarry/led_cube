import pickle

def hexify(num):
    num = "%x" % num
    if len(num) % 2:
        num = '0'+num
    return num.decode('hex')

with open("patterns.pkl","rb") as fd:
    abc = pickle.load(fd)

#print abc
for k,w in enumerate(abc):
    #print w
    for i,x in enumerate(w['frames']):
        for j,y in enumerate(x):
            #if type(y) is dict:
            print abc[k]['frames'][i][j]
            z = hexify(abc[k]['frames'][i][j])
            z = ((20-len(z))*'\x00') + z
            abc[k]['frames'][i][j] = z

print abc


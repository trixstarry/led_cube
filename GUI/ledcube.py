#!/usr/bin/python

class Ledcube:
    def __init__(self,id,channel):
        self.ID = id
        self.channel = channel

    def Send(self):
        id = hexify(self.ID)
        if self.RUNNING1 == True:
            self.Stop1(widget)
            index = 0
            self.RUNNING1 == False
            yield 100
            #return
        self.RUNNING1 = True
        selected = self.patterns[formatCombo]['patterns']
        response = '\x00'
        speed = 1000
        self.output = "\r\n".join((self.output,''.join((cube,selected," Enabled"))))
        pattern = '\x05'

        while self.RUNNING1 == True:
            self.comm.set_channel('\x46')
            data = ''.join(['\x04\x05\x06\x07\x08\x09\x10\x11', self.patterns[formatCombo]['frames'][index][0]])
            self.pattern_select(id,pattern,index,response,data,cube,selected)
            data = ''.join(['\x04\x05\x06\x07\x08\x09\x10\x11', self.patterns[formatCombo]['frames'][index][2]])
            self.pattern_select(id,pattern,index,response,data,cube,selected)
            data = ''.join(['\x04\x05\x06\x07\x08\x09\x10\x11', self.patterns[formatCombo]['frames'][index][1]])
            self.pattern_select(id,pattern,index,response,data,cube,selected)
            index = (index+1)%len(self.patterns[formatCombo]['frames'])
            yield self.patterns[formatCombo]['frames'][index][3]


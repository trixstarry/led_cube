#!/usr/bin/python
#
# testgui.py
#
# Doug Dziggel
# Oct 22, 2012
#
# This is a test gui 
# This GUI current allows for serial communication
import pygtk
pygtk.require('2.0')
import gtk
import gtk, gobject, glib
import communication
import time
import threading
import pickle

#This function allows the GUI to continue to run during long calculations                                                                    
def yieldsleep(func):
    def start(*args, **kwds):
        iterable = func(*args, **kwds)
        def step(*args, **kwds):
            try:
              time = next(iterable)
              glib.timeout_add(time, step)
            except StopIteration:
              pass
        glib.idle_add(step)
    return start

#class transmit_thread(threading.Thread):
#    def run(self):
#
#    def transmit(self):


class PyApp(gtk.Window):
    FORMAT_DEFAULT = 0
    output = "\r\n"
    RUNNING1 = False
    RUNNING2 = False
    RUNNING3 = False
    RUNNING4 = False
    PATTERN = ["Pattern 1","Pattern 2","Pattern 3","Pattern 4","Pattern 5","Master Pattern"]
    ACTIVE = [False,False,False,False]
    num = 0
    #PATTERN = ["Power Duration","Cube Connection","Dual Algorithms","Input Timing","Moving Pattern"]
    side = 0
    i1 = 0
    i2 = 0
    i3 = 0
    i4 = 0
    instr = '\x00'
    frame = ['\x00','\x01','\x02','\x03','\x04','\x05','\x06','\x07','\x08','\x09','\x0a','\x0b','\x0c','\x0d','\x0e','\x0f','\x10','\x11','\x12','\x13','\x14','\x15','\x16','\x17','\x18']

    pattern1 = ['\x06','\x0a','\x0b','\x0c','\x07','\x0c','\x0b','\x0a', '\x06']
    pattern2 = [6,11,7,8,5]
    touching = [5,6,7,8,11]
    patterns = []

    def __init__(self):
        super(PyApp,self).__init__()

        self.initialize()

        self.set_title("LED Cube Test GUI")
        self.connect("destroy", gtk.main_quit)
        self.set_size_request(300,350)
        self.set_position(gtk.WIN_POS_CENTER)
        self.set_border_width(5)

        table = gtk.Table(5,5,False)
        table.set_col_spacings(1)

        vBox = gtk.VBox(False,0)
        hBox = gtk.HBox(False,0)

        #scroll_wins = gtk.ScrolledWindow()
        #scroll_wins.set_border_width(0)
        #scroll_wins.set_policy(gtk.POLICY_AUTOMATIC,gtk.POLICY_ALWAYS)

        #self.model = gtk.ListStore(gobject.TYPE_STRING)
        #self.tree_view = gtk.TreeView(self.model)
        #scroll_wins.add_with_viewport(self.tree_view)
        #self.tree_view.show()

        #cell = gtk.CellRendererText()
        #column = gtk.TreeViewColumn("Active Cubes",cell,text=0)
        #self.tree_view.append_column(column)

        #table.attach(scroll_wins,0,5,0,1,gtk.FILL|gtk.EXPAND,gtk.FILL|gtk.EXPAND,1,1)

        hBox = gtk.HBox(False,0)

        patternLabel = gtk.Label("Cube 1:")
        hBox.pack_start(patternLabel,False,False,0)
        #self.patternDropDown = gtk.Entry(20)
        liststore1 = gtk.ListStore(gobject.TYPE_STRING)            
        for pattern in self.patterns:
            liststore1.append([pattern['patterns']])

        self.formatCombo1 = gtk.ComboBox(liststore1)
        cell = gtk.CellRendererText()
        self.formatCombo1.pack_start(cell)
        self.formatCombo1.add_attribute(cell,'text',0)
        self.formatCombo1.set_size_request(120,30)
        self.formatCombo1.set_wrap_width(1)
        self.formatCombo1.set_model(liststore1)
        self.formatCombo1.set_active(self.FORMAT_DEFAULT)
        hBox.pack_start(gtk.Label('       '),False,False,0)
        hBox.pack_start(self.formatCombo1,False,False,3)
        #vBox_op.pack_start(hBox,False,False,1)

        #self.patternDropDown.set_size_request(150,30)
        #hBox.pack_start(self.patternDropDown,False,False,0)

        sendBtn = gtk.Button("Start")
        #sendBtn.connect("clicked",self.set_num1)
        sendBtn.connect("clicked",self.Send1)
        hBox.pack_start(sendBtn,False,False,0)

        stopBtn = gtk.Button("Stop")
        #stopBtn.connect("clicked",self.set_num1)
        stopBtn.connect("clicked",self.Stop1)
        hBox.pack_start(stopBtn,False,False,0)
        vBox.pack_start(hBox,False,False,0)

        hBox = gtk.HBox(False,0)


        hBox = gtk.HBox(False,0)

        patternLabel = gtk.Label("Cube 2:")
        hBox.pack_start(patternLabel,False,False,0)
        #self.patternDropDown = gtk.Entry(20)
        liststore2 = gtk.ListStore(gobject.TYPE_STRING)            
        #print self.patterns
        for pattern in self.patterns:
            liststore2.append([pattern['patterns']])
        self.formatCombo2 = gtk.ComboBox(liststore2)
        cell = gtk.CellRendererText()
        self.formatCombo2.pack_start(cell)
        self.formatCombo2.add_attribute(cell,'text',0)
        self.formatCombo2.set_size_request(120,30)
        self.formatCombo2.set_wrap_width(1)
        self.formatCombo2.set_model(liststore2)
        self.formatCombo2.set_active(self.FORMAT_DEFAULT)
        hBox.pack_start(gtk.Label('       '),False,False,0)
        hBox.pack_start(self.formatCombo2,False,False,3)
        #vBox_op.pack_start(hBox,False,False,1)

        #self.patternDropDown.set_size_request(150,30)
        #hBox.pack_start(self.patternDropDown,False,False,0)

        sendBtn = gtk.Button("Start")
        #sendBtn.connect("clicked",self.set_num2)
        sendBtn.connect("clicked",self.Send2)
        hBox.pack_start(sendBtn,False,False,0)

        stopBtn = gtk.Button("Stop")
        #stopBtn.connect("clicked",self.set_num2)
        stopBtn.connect("clicked",self.Stop2)
        hBox.pack_start(stopBtn,False,False,0)
        vBox.pack_start(hBox,False,False,0)
        hBox = gtk.HBox(False,0)

        patternLabel = gtk.Label("Cube 3:")
        hBox.pack_start(patternLabel,False,False,0)
        #self.patternDropDown = gtk.Entry(20)
        liststore3 = gtk.ListStore(gobject.TYPE_STRING)            
        for pattern in self.patterns:
            liststore3.append([pattern['patterns']])
        self.formatCombo3 = gtk.ComboBox(liststore3)
        cell = gtk.CellRendererText()
        self.formatCombo3.pack_start(cell)
        self.formatCombo3.add_attribute(cell,'text',0)
        self.formatCombo3.set_size_request(120,30)
        self.formatCombo3.set_wrap_width(1)
        self.formatCombo3.set_model(liststore3)
        self.formatCombo3.set_active(self.FORMAT_DEFAULT)
        hBox.pack_start(gtk.Label('       '),False,False,0)
        hBox.pack_start(self.formatCombo3,False,False,3)
        #vBox_op.pack_start(hBox,False,False,1)

        #self.patternDropDown.set_size_request(150,30)
        #hBox.pack_start(self.patternDropDown,False,False,0)

        sendBtn = gtk.Button("Start")
        sendBtn.connect("clicked",self.Send3)
        hBox.pack_start(sendBtn,False,False,0)

        stopBtn = gtk.Button("Stop")
        stopBtn.connect("clicked",self.Stop3)
        hBox.pack_start(stopBtn,False,False,0)
        vBox.pack_start(hBox,False,False,0)
        hBox = gtk.HBox(False,0)

        patternLabel = gtk.Label("Cube 4:")
        hBox.pack_start(patternLabel,False,False,0)
        #self.patternDropDown = gtk.Entry(20)
        liststore4 = gtk.ListStore(gobject.TYPE_STRING)            
        for pattern in self.patterns:
            liststore4.append([pattern['patterns']])
        self.formatCombo4 = gtk.ComboBox(liststore4)
        cell = gtk.CellRendererText()
        self.formatCombo4.pack_start(cell)
        self.formatCombo4.add_attribute(cell,'text',0)
        self.formatCombo4.set_size_request(120,30)
        self.formatCombo4.set_wrap_width(1)
        self.formatCombo4.set_model(liststore4)
        self.formatCombo4.set_active(self.FORMAT_DEFAULT)
        hBox.pack_start(gtk.Label('       '),False,False,0)
        hBox.pack_start(self.formatCombo4,False,False,3)
        #vBox_op.pack_start(hBox,False,False,1)

        #self.patternDropDown.set_size_request(150,30)
        #hBox.pack_start(self.patternDropDown,False,False,0)

        sendBtn = gtk.Button("Start")
        sendBtn.connect("clicked",self.Send4)
        hBox.pack_start(sendBtn,False,False,0)

        stopBtn = gtk.Button("Stop")
        stopBtn.connect("clicked",self.Stop4)
        hBox.pack_start(stopBtn,False,False,0)
        vBox.pack_start(hBox,False,False,0)

       #hBox = gtk.HBox(False,0)
       #self.cube1Check = gtk.CheckButton("Cube 1")
       #self.cube1Check.set_active(self.ACTIVE[0])
       #hBox.pack_start(self.cube1Check,False,False,0)

       #self.cube2Check = gtk.CheckButton("Cube 2")
       #self.cube2Check.set_active(self.ACTIVE[1])
       #hBox.pack_start(self.cube2Check,False,False,0)

       #self.cube3Check = gtk.CheckButton("Cube 3")
       #self.cube3Check.set_active(self.ACTIVE[2])
       #hBox.pack_start(self.cube3Check,False,False,0)

       #self.cube4Check = gtk.CheckButton("Cube 4")
       #self.cube4Check.set_active(self.ACTIVE[3])
       #hBox.pack_start(self.cube4Check,False,False,0)
       #vBox.pack_start(hBox,False,False,0)
        hBox = gtk.HBox(False,0)
        sendBtn = gtk.Button("Moving Alg")
        sendBtn.connect("clicked",self.Send_Moving)
        hBox.pack_start(sendBtn,False,False,0)

        stopBtn = gtk.Button("Stop")
        stopBtn.connect("clicked",self.Stop_Moving)
        hBox.pack_start(stopBtn,False,False,0)
        vBox.pack_start(hBox,False,False,0)

        table.attach(vBox,0,5,1,2,gtk.FILL,gtk.FILL,1,1)

        textWindow = gtk.TextView()
        self.textbuffer = textWindow.get_buffer()
        textbuffer = textWindow.get_buffer()
        textWindow.set_editable(False)
        textWindow.modify_fg(gtk.STATE_NORMAL, gtk.gdk.Color(5140,5140,5140))
        textWindow.set_cursor_visible(False)
        textWindow.show()

        self.output_win = gtk.ScrolledWindow()
        self.output_win.set_border_width(0)
        self.output_win.set_policy(gtk.POLICY_AUTOMATIC,gtk.POLICY_ALWAYS)
        self.output_win.add_with_viewport(textWindow)
        self.scroll_VAdjustment = self.output_win.get_vadjustment()
        table.attach(self.output_win,0,5,2,3,gtk.FILL|gtk.EXPAND,gtk.FILL|gtk.EXPAND,1,1)
        
        hBox = gtk.HBox(False,0)
        
        '''
        btn1 = gtk.Button("Scan")
        btn1.connect("clicked",self.scan)
        hBox.pack_start(btn1,False,False,0)
        '''
        
        #btn2 = gtk.Button("Upload")
        #hBox.pack_start(btn2,False,False,0)

        #btn3 = gtk.Button("Test3")
        #hBox.pack_start(btn3,False,False,0)


        helpBtn = gtk.Button("Help")
        helpBtn.set_size_request(70,30)
        helpBtn.connect("clicked",self.helpMe)
        #hBox.pack_end(helpBtn,False,False,0)

        quit = gtk.Button("Quit")
        quit.set_size_request(50,30)
        quit.connect("released",gtk.main_quit)
        hBox.pack_end(quit,False,False,0) 
        hBox.pack_end(helpBtn,False,False,0)

        table.attach(hBox,0,5,3,4,gtk.FILL,gtk.FILL,1,1)
        #self.display()

        
        
        self.add(table)
        self.show_all()

    def initialize(self):
        data = 1
        self.comm = communication.Communication()
        try:
            with open("patterns.pkl","rb") as fd:
                self.patterns = pickle.load(fd)
            for k,w in enumerate(self.patterns):
                #print w
                for i,x in enumerate(w['frames']):
                    for j,y in enumerate(x):
                        if (j < 3):
                            #print abc[k]['frames'][i][j]
                            z = self.hexify(self.patterns[k]['frames'][i][j])
                            z = ((20-len(z))*'\x00') + z
                            self.patterns[k]['frames'][i][j] = z
        except IOError:
            print "No Patterns found"

    def hexify(self,num):
        num = "%x" % num
        if len(num) % 2:
            num = '0'+num
        return num.decode('hex')

    def text_out(self):
        self.textbuffer.set_text(self.output)
        self.scroll_VAdjustment.set_value(self.scroll_VAdjustment.get_upper())

    def pattern_select(self,id,pattern,index,response,data,cube,selected):
        pattern = '\x05'
        if selected == self.PATTERN[0]:
            pattern = '\x01'
            self.Transmit(id,pattern,index,response,data,cube,selected)
        elif selected == self.PATTERN[1]:
            pattern = '\x03'
            response = '\x01'
            self.Transmit(id,pattern,self.pattern2[index],response,data,cube,selected)
        elif selected == self.PATTERN[2]:
            pattern = '\x03'
            self.Transmit(id,pattern,index,response,data,cube,selected)
        elif selected == self.PATTERN[3]:
            pattern = '\x04'
            self.Transmit(id,pattern,index,response,data,cube,selected)
        elif selected == self.PATTERN[4]:
            pattern = '\x04'
            self.Transmit(id,pattern,index,response,data,cube,selected)
        elif selected == self.PATTERN[5]:
            pattern = '\x06'
            self.Transmit(id,pattern,index,response,data,cube,selected)
        else:
            pattern = '\x05'
            self.Transmit(id,pattern,index,response,data,cube,selected)

    def Transmit(self,id,pattern,index,response,data,cube,selected):
        if response == '\x00':
            data_out = ''.join((self.instr,id,pattern,self.frame[index],response,data))
            self.comm.Transmit(data_out) 
            #self.output = "\r\n".join((self.output,''.join((cube,selected," Enabled"))))
        if response == '\x01':
            data_out = ''.join((self.instr,id,pattern,self.frame[self.touching[self.side]],response,data))
            self.comm.Transmit(data_out) 
            sensors = self.Receive()
            #sensors = 2
            if sensors == 2:
                self.side = 0
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 1"))))
            elif sensors == 1:
                self.side = 1
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 2"))))
            elif sensors == 4:
                self.side = 2
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 3"))))
            elif sensors == 8:
                self.side = 3
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 4"))))
            else:
                pattern = '\x01'
                self.side = 4
                #self.output = "\r\n".join((self.output,''.join((cube,"Nothing Detected"))))
            #data_out = ''.join((self.instr,id,pattern,self.frame[self.side],response,data))
        self.text_out()

    def incrementer(self,index,selected):
        if selected == self.PATTERN[0]:
            return (index+1)%5;
        elif selected == self.PATTERN[1]:
            return (index+1)%5;
        elif selected == self.PATTERN[2]:
            return (index+1)%5;
        elif selected == self.PATTERN[3]:
            return (index+1)%5;
        elif selected == self.PATTERN[4]:
            return (index+1)%5;
        elif selected == self.PATTERN[5]:
            return (index+1)%19;
        else:
            return (index+1)%5;

    @yieldsleep
    def Send_Moving(self,widget):
        id = '\x01'
        #running = self.RUNNING1
        cube = 'Cube 1: '
        index = 0
        index2 = 0
        self.i1 = ((self.i1 +1)%5)
        formatCombo = self.formatCombo1.get_active()
        side1 = 3
        side2 = 3
        toggle = 0
        cnt = 0
        bck = 0
        jmp = 0
            
        if self.RUNNING1 == True:
            self.Stop1(widget)
            index = 0
            self.RUNNING1 == False
            yield 1000
            #return
        self.RUNNING1 = True
        selected = self.PATTERN[formatCombo]
        response = '\x01'
        while self.RUNNING1 == True:
            if(((side1 == 1) or (side1 == 2)) and (side2 == 1)):
                
                if (index == 5) and (toggle == 0):
                    #toggle cube 2
                    #if toggle:
                    #    toggle = 0
                    #else:
                    if bck == 0:
                        toggle = 1
                        #index = 4
                        index2= 0
                        cnt = 1
                    else:
                        bck = 0
                
                if (index2 == 1) and (toggle == 1) and (cnt == 0):
                    #toggle cube 2
                    #if toggle:
                    if jmp:
                        toggle = 0
                        #else:
                        #    toggle = 1
                        #index2= 1
                        index = 4 
                        bck = 1
                        jmp = 0
                    else:
                        jmp = 1

                cnt = 0

                #print ("Toggle: " + str(toggle))
            
            self.comm.set_channel('\x46')
            cube = 'Cube 1: '
            id = '\x01'
            data = '\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
            pattern = '\x05'
            if toggle == 0:
                data_out = ''.join((self.instr,id,pattern,self.pattern1[index],response,data))
            else:
                data_out = ''.join((self.instr,id,pattern,'\x00',response,data))
            self.comm.Transmit(data_out) 
            sensors = self.Receive()
            #sensors = 2
            if sensors == 2:
                self.side = 0
                side1 = 0
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 1"))))
            elif sensors == 1:
                self.side = 2
                side1 = 1
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 2"))))
            elif sensors == 4:
                self.side = 2
                side1 = 2
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 3"))))
            elif sensors == 8:
                self.side = 3
                side1 = 3
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 4"))))
            else:
                pattern = '\x01'
                side1 = 0
                self.side = 1
                self.output = "\r\n".join((self.output,''.join((cube,"Nothing Detected"))))
            data_out = ''.join((self.instr,id,pattern,self.pattern1[self.side],response,data))
            self.text_out()
            index = (index+1)%8

            time.sleep(.15)
             
            self.comm.set_channel('\x3c')
            cube = 'cube 2: '
            id = '\x02'
            data = '\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
            pattern = '\x05'
            if (toggle == 0):
                data_out = ''.join((self.instr,id,pattern,'\x00',response,data))
            else:
                data_out = ''.join((self.instr,id,pattern,self.pattern1[index2],response,data))
            self.comm.Transmit(data_out) 
            sensors = self.Receive()
            #sensors = 2
            if sensors == 2:
                self.side = 0
                side2 = 0
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 1"))))
            elif sensors == 1:
                self.side = 2
                side2 = 1
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 2"))))
            elif sensors == 4:
                self.side = 2
                side2 = 2
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 3"))))
            elif sensors == 8:
                self.side = 3
                side2 = 3
                self.output = "\r\n".join((self.output,''.join((cube,"cube detected on side 4"))))
            else:
                pattern = '\x01'
                self.side = 1
                side2 = 0
                self.output = "\r\n".join((self.output,''.join((cube,"Nothing Detected"))))
            data_out = ''.join((self.instr,id,pattern,self.pattern1[self.side],response,data))
            self.text_out()
            index2 = (index2+1)%8

           # if(((side1 == 1) or (side1 == 2)) and (side2 == 1)):
           #     if (index2 == 0) and (toggle == 1):
           #         #untoggle cube 2
           #         if toggle:
           #             toggle = 0
           #         index2 = 0
           #         index = 4

            #time.sleep(.1)
            yield 50

    def Stop_Moving(self,widget):
        self.Stop1(self)
        self.Stop2(self)

    @yieldsleep
    def Send1(self,widget):
        id = '\x01'
        #running = self.RUNNING1
        cube = 'Cube 1: '
        index = self.i1
        self.i1 = ((self.i1 +1)%5)
        formatCombo = self.formatCombo1.get_active()
            
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
        print self.patterns

        while self.RUNNING1 == True:
            #self.comm.set_channel('\x46')
            #cube = 'Cube 2: '
            #id = '\x02'
            data = ''.join(['\x04\x05\x06\x07\x08\x09\x10\x11', self.patterns[formatCombo]['frames'][index][0]])
            #print data
            #\x01\xf8\xc6\x3f\x00\x07\x29\xc0\x00\x00\x10\x00\x00\x07\x39\xc0\x01\xf8\xd6\x3f' 
            pattern = '\x05'
            self.pattern_select(id,pattern,index,response,data,cube,selected)
            data = ''.join(['\x04\x05\x06\x07\x08\x09\x10\x11', self.patterns[formatCombo]['frames'][index][2]])
            #print data
            #data = '\x04\x05\x06\x07\x08\x09\x10\x11\x00\x00\x00\x00\x01\xf8\xc6\x3f\x00\x07\x29\xc0\x01\xf8\xd6\x3f\x01\xf8\xd6\x3f' 
            self.pattern_select(id,pattern,index,response,data,cube,selected)
            #data = '\x04\x05\x06\x07\x08\x09\x10\x11\x00\x00\x00\x00\x00\x00\x00\x00\x01\xf8\xc6\x3f\x01\xff\xff\xff\x00\x00\x00\x00' 
            data = ''.join(['\x04\x05\x06\x07\x08\x09\x10\x11', self.patterns[formatCombo]['frames'][index][1]])
            #print data
            self.pattern_select(id,pattern,index,response,data,cube,selected)
            #index = self.incrementer(index,selected)
            index = (index+1)%len(self.patterns[formatCombo]['frames'])
            #time.sleep(.1)
            #print self.patterns[formatCombo]['frames'][index][3]
            yield self.patterns[formatCombo]['frames'][index][3]

    def Stop1(self,widget):
        self.comm.set_channel('\x46')
        self.RUNNING1 = False
        id = '\x01'
        pattern = '\x05'
        data = '\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' 
        response = '\x00'
        filler = '\x00'
        data_out = ''.join((self.instr,id,pattern,filler,response,data))
        time.sleep(.15)
        self.comm.Transmit(data_out) 
        self.comm.Transmit(data_out) 
        self.comm.Transmit(data_out) 
        self.output = "\r\n".join((self.output,"Cube 1: Stopped."))
        self.text_out()

    @yieldsleep
    def Send2(self,widget):
        id = '\x02'
        #running = self.RUNNING2
        cube = 'Cube 2: '
        index = self.i2
        self.i2 = ((self.i2 +1)%5)
        formatCombo = self.formatCombo2.get_active()
            
        if self.RUNNING2 == True:
            self.Stop2(widget)
            index = 0
            self.RUNNING2 == False
            yield 1000
            #return
        self.RUNNING2 = True
        selected = self.PATTERN[formatCombo]
        response = '\x00'
        while self.RUNNING2 == True:
            self.comm.set_channel('\x3C')
            #cube = 'Cube 2: '
            #id = '\x02'
            data = '\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
            pattern = '\x05'
            self.pattern_select(id,pattern,index,response,data,cube,selected)
            #index = (index+1)%19;
            index = self.incrementer(index,selected)
            #time.sleep(.1)
            yield 1000

    def Stop2(self,widget):
        self.comm.set_channel('\x3C')
        self.RUNNING2 = False
        self.i2 = 0
        id = '\x02'
        pattern = '\x05'
        data = '\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' 
        response = '\x00'
        filler = '\x00'
        data_out = ''.join((self.instr,id,pattern,filler,response,data))
        #self.comm.Transmit(data_out) 
        time.sleep(.15)
        self.comm.Transmit(data_out) 
        #self.comm.Receive()
        self.output = "\r\n".join((self.output,"Cube 2: Stopped."))
        self.text_out()

    @yieldsleep
    def Send3(self,widget):
        id = '\x03'
        #running = self.RUNNING2
        cube = 'Cube 3: '
        index = self.i3
        self.i3 = ((self.i3 +1)%5)
        formatCombo = self.formatCombo3.get_active()
            
        if self.RUNNING3 == True:
            self.Stop3(widget)
            index = 0
            self.RUNNING3 == False
            yield 1000
            #return
        self.RUNNING3 = True
        selected = self.PATTERN[formatCombo]
        response = '\x00'
        while self.RUNNING3 == True:
            self.comm.set_channel('\x32')
            #cube = 'Cube 2: '
            #id = '\x02'
            data = '\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
            pattern = '\x05'
            self.pattern_select(id,pattern,index,response,data,cube,selected)
            index = (index+1)%19;
            #time.sleep(.1)
            yield 1000

    def Stop3(self,widget):
        self.comm.set_channel('\x32')
        self.RUNNING3 = False
        self.i3 = 0
        id = '\x03'
        pattern = '\x05'
        data = '\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' 
        response = '\x00'
        filler = '\x00'
        data_out = ''.join((self.instr,id,pattern,filler,response,data))
        #self.comm.Transmit(data_out) 
        time.sleep(.15)
        self.comm.Transmit(data_out) 
        #self.comm.Receive()
        self.output = "\r\n".join((self.output,"Cube 3: Stopped."))
        self.text_out()

    @yieldsleep
    def Send4(self,widget):
        if self.RUNNING4 == True:
            self.Stop4(widget)
            self.i4 = 0
            self.RUNNING4 == False
            yield 1000
            #return
        self.RUNNING = True
        selected = self.PATTERN[self.formatCombo4.get_active()]
        while self.RUNNING4 == True:
            self.comm.set_channel('\x28')
            cube = 'Cube 4: '
            id = '\x04'
            data = '\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
            pattern = '\x05'
            self.pattern_select(id,pattern,index,response,data,cube,selected)
            index = (index+1)%5;
            #time.sleep(.1)
            yield 1000

    def Stop4(self,widget):
        self.comm.set_channel('\x28')
        self.RUNNING4 = False
        id = '\x04'
        pattern = '\x05'
        data = '\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' 
        data_out = ''.join((self.instr,id,pattern,data))
        self.comm.Transmit(data_out) 
        time.sleep(.15)
        self.comm.Transmit(data_out) 
        self.output = "\r\n".join((self.output,"Cube 4: Stopped."))
        self.text_out()

    def Receive(self):
        data = self.comm.Receive()
        print data
        #for char in data:
        #    print int(char.encode('hex'),16)
        '''try:
            print "Bytes:"
                print int(data[0].encode('hex'),16)
            print int(data[1].encode('hex'),16)
            print int(data[2].encode('hex'),16)
            print "end Bytes"
        except:
            print "Error"
            '''
        #text_out(data)
        return data

    def ping(self):
        data = '\x02\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' 
        self.comm.Transmit(data)
        #self.Receive()

    def scan(self,widget):
        ping(1)
        ping(2)
        ping(3)
        ping(4)



    def helpMe(self,widget):
        about = gtk.AboutDialog()
        about.set_program_name("Wireless LED Cube")
        about.set_version("0.90")
        about.set_authors(["Alex Dziggel - CpE","Patrick Meison - CPE","Thao-Trang Hoang - EE"])
        about.set_copyright("Created October 21, 2012")
        about.set_comments("""Instructions:\n
        This program was created to interface with the wireless LED Cubes and to provide them instructions and patterns by which to display.
        'Help' Button: Brings up this window!
        'Quit' Button: Quits the program.""")
        about.run()
        about.destroy()


PyApp()
gtk.main()

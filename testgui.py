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

class PyApp(gtk.Window):
    FORMAT_DEFAULT = 1
    output = "\r\n"
    RUNNING = False
    PATTERN = ["Pattern 1","Pattern 2","Pattern 3","Pattern 4","Pattern 5"]
    #PATTERN = ["Power Duration","Cube Connection","Dual Algorithms","Input Timing","Moving Pattern"]
    i = 0

    def __init__(self):
        super(PyApp,self).__init__()

        self.initialize()

        self.set_title("LED Cube Test GUI")
        self.connect("destroy", gtk.main_quit)
        self.set_size_request(300,200)
        self.set_position(gtk.WIN_POS_CENTER)
        self.set_border_width(5)

        table = gtk.Table(5,5,False)
        table.set_col_spacings(1)

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

        patternLabel = gtk.Label("Pattern:")
        hBox.pack_start(patternLabel,False,False,0)
        #self.patternDropDown = gtk.Entry(20)
        liststore2 = gtk.ListStore(gobject.TYPE_STRING)            
        liststore2.append([self.PATTERN[0]])
        liststore2.append([self.PATTERN[1]])
        liststore2.append([self.PATTERN[2]])
        liststore2.append([self.PATTERN[3]])
        liststore2.append([self.PATTERN[4]])
        self.formatCombo = gtk.ComboBox(liststore2)
        cell = gtk.CellRendererText()
        self.formatCombo.pack_start(cell)
        self.formatCombo.add_attribute(cell,'text',0)
        self.formatCombo.set_size_request(120,30)
        self.formatCombo.set_wrap_width(1)
        self.formatCombo.set_model(liststore2)
        self.formatCombo.set_active(self.FORMAT_DEFAULT)
        hBox.pack_start(gtk.Label('       '),False,False,0)
        hBox.pack_start(self.formatCombo,False,False,3)
        #vBox_op.pack_start(hBox,False,False,1)

        #self.patternDropDown.set_size_request(150,30)
        #hBox.pack_start(self.patternDropDown,False,False,0)

        sendBtn = gtk.Button("Start")
        sendBtn.connect("clicked",self.Send)
        hBox.pack_start(sendBtn,False,False,0)

        stopBtn = gtk.Button("Stop")
        stopBtn.connect("clicked",self.Stop)
        hBox.pack_start(stopBtn,False,False,0)

        table.attach(hBox,0,5,1,2,gtk.FILL,gtk.FILL,1,1)

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
        
        btn1 = gtk.Button("Scan")
        btn1.connect("clicked",self.scan)
        hBox.pack_start(btn1,False,False,0)
        
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

    def text_out(self):
        self.textbuffer.set_text(self.output)
        self.scroll_VAdjustment.set_value(self.scroll_VAdjustment.get_upper())

    def display(self):
        selected = self.PATTERN[self.formatCombo.get_active()]
        if selected == "Power Duration":
            data = '\x01\x04\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
            #data2 = '\x01\x08\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
            self.comm.Transmit(data) 
            self.output = "\r\n".join((self.output,"Power Duration Pattern Enabled"))
            self.text_out()
            #time.sleep(.2)
            #self.comm.Transmit(data2)
        elif selected == "Cube Connection":
            #print self.Receive()
            data = '\x01\x03\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
            self.comm.Transmit(data) 
            sensors = self.Receive()
            if sensors == '\x01':
                time.sleep(0.15)
                data = '\x01\x03\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                self.comm.Transmit(data) 
                self.output = "\r\n".join(self.output,"Cube1 detects abother cube on side 1")
                self.text_out()

            if sensors == '\x02':
                time.sleep(0.15)
                data = '\x01\x03\x01\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                self.comm.Transmit(data) 
            if sensors == '\x04':
                time.sleep(0.15)
                data = '\x01\x03\x02\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                self.comm.Transmit(data) 
            if sensors == '\x08':
                time.sleep(0.15)
                data = '\x01\x03\x03\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                self.comm.Transmit(data) 
               
            self.output = "\r\n".join((self.output,"Cube Connection Pattern Enabled"))
            self.text_out()

        elif selected == "Dual Algorithms":
            #stuff
            data = '\x01\x04\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
            self.comm.Transmit(data) 
            self.output = "\r\n".join((self.output,"Dual Algorithms Enabled"))
            self.text_out()
        elif selected == "Input Timing":
            #stuff
            data = '\x01\x04\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
            self.comm.Transmit(data) 
            self.output = "\r\n".join((self.output,"Input Timing Test Enabled"))
            self.text_out()
        elif selected == "Moving Pattern":
            #stuff
            """
            while self.STOPPED == False:
                data = '\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                i = 0;
                id = '\x01'
                frame = ['\x00','\x01','\x02','\x03','\x04']

                self.comm.Transmit(''.join((id,frame[i],data))) 
                i = (i+1)%5;
                self.output = "\r\n".join((self.output,"Moving Pattern Test Enabled"))
                self.text_out()
                yield 1
            self.STOPPED = True
            """
            data = '\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
            #i = 0;
            id = '\x01'
            pattern = '\x02'
            frame = ['\x00','\x01','\x02','\x03','\x04']

            data_out = ''.join((id,pattern,frame[self.i],data))

            self.comm.Transmit(data_out) 
            self.i = (self.i+1)%5;
            self.output = "\r\n".join((self.output,"Moving Pattern Test Enabled"))
            self.text_out()
        else:
                
            data = '\x01\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' 
            #self.inputEntry.get_text()
            #data2 = "\x01\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" 
            #self.inputEntry.get_text()
            self.comm.Transmit(data) 
            #print 'test\x01'
            #print data
            #time.sleep(.2)
            #self.comm.Transmit(data2)
            #print data2#self.inputEntry.get_text()
            #print self.Receive()

    @yieldsleep
    def Send(self,widget):
        if self.RUNNING == True:
            return
        self.RUNNING = True
        while self.RUNNING == True:
            selected = self.PATTERN[self.formatCombo.get_active()]
            if selected == self.PATTERN[0]:
                data = '\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                #i = 0;
                id = '\x01'
                pattern = '\x01'
                frame = ['\x00','\x01','\x02','\x03','\x04']

                data_out = ''.join((id,pattern,frame[self.i],data))

                self.comm.Transmit(data_out) 
                self.i = (self.i+1)%5;

                self.output = "\r\n".join((self.output,"Power Duration Pattern Enabled"))
                self.text_out()
                #time.sleep(.2)
                #self.comm.Transmit(data2)
            elif selected == self.PATTERN[1]:
                #print self.Receive()
                data = '\x01\x03\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                self.comm.Transmit(data) 
                #sensors = self.Receive()
                if sensors == '\x01':
                    time.sleep(0.15)
                    data = '\x01\x03\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                    self.comm.Transmit(data) 
                    self.output = "\r\n".join(self.output,"Cube1 detects abother cube on side 1")
                    self.text_out()

                if sensors == '\x02':
                    time.sleep(0.15)
                    data = '\x01\x03\x01\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                    self.comm.Transmit(data) 
                if sensors == '\x04':
                    time.sleep(0.15)
                    data = '\x01\x03\x02\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                    self.comm.Transmit(data) 
                if sensors == '\x08':
                    time.sleep(0.15)
                    data = '\x01\x03\x03\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                    self.comm.Transmit(data) 
                   
                self.output = "\r\n".join((self.output,"Cube Connection Pattern Enabled"))
                self.text_out()

            elif selected == self.PATTERN[2]:
                #stuff
                data = '\x01\x04\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                self.comm.Transmit(data) 
                self.output = "\r\n".join((self.output,"Dual Algorithms Enabled"))
                self.text_out()
            elif selected == self.PATTERN[3]:
                #stuff
                data = '\x01\x04\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                self.comm.Transmit(data) 
                self.output = "\r\n".join((self.output,"Input Timing Test Enabled"))
                self.text_out()
            elif selected == self.PATTERN[4]:
                #stuff
                """
                while self.STOPPED == False:
                    data = '\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                    i = 0;
                    id = '\x01'
                    frame = ['\x00','\x01','\x02','\x03','\x04']

                    self.comm.Transmit(''.join((id,frame[i],data))) 
                    i = (i+1)%5;
                    self.output = "\r\n".join((self.output,"Moving Pattern Test Enabled"))
                    self.text_out()
                    yield 1
                self.STOPPED = True
                """
                data = '\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff' 
                #i = 0;
                id = '\x01'
                pattern = '\x02'
                frame = ['\x00','\x01','\x02','\x03','\x04']

                data_out = ''.join((id,pattern,frame[self.i],data))

                self.comm.Transmit(data_out) 
                self.i = (self.i+1)%5;
                self.output = "\r\n".join((self.output,"Moving Pattern Test Enabled"))
                self.text_out()
            else:
                    
                data = '\x01\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' 
                #self.inputEntry.get_text()
                #data2 = "\x01\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" 
                #self.inputEntry.get_text()
                self.comm.Transmit(data) 
                #print 'test\x01'
                #print data
                #time.sleep(.2)
                #self.comm.Transmit(data2)
                #print data2#self.inputEntry.get_text()
                #print self.Receive()
            yield 1000
            #while gtk.events_pending():
            #    gtk.main_iteration()

    def Stop(self,widget):
        self.RUNNING = False
        data = '\x01\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' 
        self.comm.Transmit(data) 
        time.sleep(.15)
        data = '\x02\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' 
        self.comm.Transmit(data) 
        time.sleep(.15)
        data = '\x02\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' 
        self.comm.Transmit(data) 
        self.output = "\r\n".join((self.output,"All Patterns Stopped"))
        self.text_out()

    def Receive(self):
        data = self.comm.Receive()
        print "data: "+data[2]
        #text_out(data)
        return data[2]

    def ping(self):
        data = '\x02\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' 
        self.comm.Transmit(data)
        self.Receive()

    def scan(self,widget):
        ping(1)
        ping(2)
        ping(3)
        ping(4)


    def helpMe(self,widget):
        about = gtk.AboutDialog()
        about.set_program_name("Wireless LED Cube")
        about.set_version("0.01")
        about.set_authors(["Alex Dziggel - CpE","Patrick Meison - CPE","Thao Hoang - EE"])
        about.set_copyright("Created October 21, 2012")
        about.set_comments("""Instructions:\n
        This program was created to interface with the wireless LED Cubes and to provide them instructions and patterns by which to display.
        'Help' Button: Brings up this window!
        'Quit' Button: Quits the program.""")
        about.run()
        about.destroy()


PyApp()
gtk.main()

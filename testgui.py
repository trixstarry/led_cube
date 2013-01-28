#!/usr/bin/python
#
# testgui.py
#
# Doug Dziggel
# Oct 22, 2012
#
# This is a test gui 
import pygtk
pygtk.require('2.0')
import gtk
import gtk, gobject, glib
import communication


class PyApp(gtk.Window):
    def __init__(self):
        super(PyApp,self).__init__()

        self.initialize()

        self.set_title("LED Cube Test GUI")
        self.connect("destroy", gtk.main_quit)
        self.set_size_request(500,300)
        self.set_position(gtk.WIN_POS_CENTER)
        self.set_border_width(5)

        table = gtk.Table(5,5,False)
        table.set_col_spacings(1)

        hBox = gtk.HBox(False,0)

        scroll_wins = gtk.ScrolledWindow()
        scroll_wins.set_border_width(0)
        scroll_wins.set_policy(gtk.POLICY_AUTOMATIC,gtk.POLICY_ALWAYS)

        self.model = gtk.ListStore(gobject.TYPE_STRING)
        self.tree_view = gtk.TreeView(self.model)
        scroll_wins.add_with_viewport(self.tree_view)
        self.tree_view.show()

        cell = gtk.CellRendererText()
        column = gtk.TreeViewColumn("Active Cubes",cell,text=0)
        self.tree_view.append_column(column)

        table.attach(scroll_wins,0,5,0,1,gtk.FILL|gtk.EXPAND,gtk.FILL|gtk.EXPAND,1,1)

        hBox = gtk.HBox(False,0)

        inputLabel = gtk.Label("Input:")
        hBox.pack_start(inputLabel,False,False,0)
        self.inputEntry = gtk.Entry(20)
        self.inputEntry.set_size_request(150,30)
        hBox.pack_start(self.inputEntry,False,False,0)

        sendBtn = gtk.Button("Transmit")
        sendBtn.connect("clicked",self.Send)
        hBox.pack_start(sendBtn,False,False,0)

        table.attach(hBox,0,5,1,2,gtk.FILL,gtk.FILL,1,1)

        textWindow = gtk.TextView()
        textbuffer = textWindow.get_buffer()
        textWindow.set_editable(False)
        textWindow.modify_fg(gtk.STATE_NORMAL, gtk.gdk.Color(5140,5140,5140))
        textWindow.set_cursor_visible(False)
        textWindow.show()

        output_win = gtk.ScrolledWindow()
        output_win.set_border_width(0)
        output_win.set_policy(gtk.POLICY_AUTOMATIC,gtk.POLICY_ALWAYS)
        output_win.add_with_viewport(textWindow)
        table.attach(output_win,0,5,2,3,gtk.FILL|gtk.EXPAND,gtk.FILL|gtk.EXPAND,1,1)
        
        hBox = gtk.HBox(False,0)
        
        btn1 = gtk.Button("Scan")
        hBox.pack_start(btn1,False,False,0)
        
        btn2 = gtk.Button("Upload")
        hBox.pack_start(btn2,False,False,0)

        btn3 = gtk.Button("Test3")
        hBox.pack_start(btn3,False,False,0)


        helpBtn = gtk.Button("Help")
        helpBtn.set_size_request(70,30)
        helpBtn.connect("clicked",self.helpMe)
        hBox.pack_start(helpBtn,False,False,0)

        quit = gtk.Button("Quit")
        quit.set_size_request(50,30)
        quit.connect("released",gtk.main_quit)
        hBox.pack_start(quit,False,False,0) 

        table.attach(hBox,0,5,3,4,gtk.FILL,gtk.FILL,1,1)

        
        
        self.add(table)
        self.show_all()

    def initialize(self):
        data = 1
        #self.comm = communication.Communication()

    def Send(self,widget):
        comm.transmit(self.get_text()) 

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

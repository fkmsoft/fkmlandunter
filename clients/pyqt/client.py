#!/usr/bin/env python

import sys
from PyQt4 import QtGui
from PyQt4 import QtCore
from PyQt4 import QtNetwork

# Some important global shit
MAXPLAYERS = 5
DEFNAME = "Guilord"
DEFHOST = "localhost"
DEFPORT = 1337

# class for the (main) window of a fkmlandunter client
class FkmlandunterMainWin(QtGui.QMainWindow):

    def __init__(self, win_parent = None):
        # Init the base class
        QtGui.QMainWindow.__init__(self, win_parent)
        self.sock = QtNetwork.QTcpSocket()
        self.creat_widgets()
        self.connected = False
        self.haveDeck = False
        self.haveBelts = False
        self.haveWeather = False
        self.haveCard = -1
        self.playernum = 0
        self.names = []
        self.myname = DEFNAME
        self.lastCard = -1
        #self.connect(DEFNAME, DEFHOST, DEFPORT)

    def creat_widgets(self):
        # Players
        self.player_box = []
        self.playerWaterBar = []
        self.playerWaterLbl = []
        self.playerLifeBar = []
        self.playerLifeLbl = []
        self.player_lastCard = []
        topbox = QtGui.QHBoxLayout()
        for i in range(MAXPLAYERS):
            # Create Widgets
            self.player_box.append(QtGui.QGroupBox(self.tr("Player") + " " + `i+1`))
            self.player_box[i].setAlignment(QtCore.Qt.AlignHCenter)
            self.playerWaterBar.append(QtGui.QProgressBar())
            self.playerWaterBar[i].setMinimum(0)
            self.playerWaterBar[i].setMaximum(12)
            self.playerWaterBar[i].setFormat("Waterlevel %v")
            self.playerWaterBar[i].setValue(i)
            self.playerWaterBar[i].setOrientation(QtCore.Qt.Vertical)
            #self.playerWaterBar[i].setAlignment(QtCore.Qt.AlignHCenter)
            self.playerWaterLbl.append(QtGui.QLabel(" 00"))
            #self.playerWaterLbl[i].setAlignment(QtCore.Qt.AlignCenter)
            self.playerLifeBar.append(QtGui.QProgressBar())
            self.playerLifeBar[i].setMinimum(0)
            self.playerLifeBar[i].setMaximum(12)
            self.playerLifeBar[i].setValue(i)
            self.playerLifeBar[i].setOrientation(QtCore.Qt.Vertical)
            self.playerLifeBar[i].setFormat("%v/%m " + self.tr("Lifebelts"))
            #self.playerLifeBar[i].setAlignment(QtCore.Qt.AlignHCenter)
            #self.playerLifeBar[i].setInvertedAppearance(True)
            self.playerLifeLbl.append(QtGui.QLabel(" 0/0"))
            #self.playerLifeLbl[i].setAlignment(QtCore.Qt.AlignHCenter)
            self.player_lastCard.append(QtGui.QLabel(self.tr("Last move:") + " 0"))
            # Layout
            #hbox = QtGui.QHBoxLayout()
            grid = QtGui.QGridLayout()
            #hbox.addWidget(self.playerWaterBar[i])
            #hbox.addWidget(self.playerLifeBar[i])
            #hbox.addWidget(self.player_lastCard[i])
            grid.addWidget(self.playerWaterBar[i], 0, 0)
            grid.addWidget(self.playerLifeBar[i], 0, 1)
            grid.addWidget(self.playerWaterLbl[i], 1, 0)
            grid.addWidget(self.playerLifeLbl[i], 1, 1)
            grid.addWidget(self.player_lastCard[i], 2, 0, 1, -1)
            #vbox.addStretch(1)
            #self.player_box[i].setLayout(hbox)
            self.player_box[i].setLayout(grid)
            topbox.addWidget(self.player_box[i])
        # Deck
        # Create Widgets
        self.wLabel = QtGui.QLabel(self.tr("Weather:"))
        #self.deckChoose = QtGui.QComboBox()
        #self.cardGroup = QtGui.QButtonGroup()
        self.btns = []
        for i in range(12):
            self.btns.append(QtGui.QRadioButton(self.tr("Card") + " " + `i + 1`))
            self.btns[i].setEnabled(False)
            #self.cardGroup.addButton(self.btns[i], i)
        policy = QtGui.QSizePolicy(QtGui.QSizePolicy.PushButton)
        policy.setVerticalPolicy(QtGui.QSizePolicy.MinimumExpanding)
        self.deckPlay = QtGui.QPushButton(self.tr("Play"))
        self.deckPlay.setEnabled(False)
        self.deckPlay.setSizePolicy(policy)
        # Chat
        # Create Widgets
        # Layout
        self.chat_box = QtGui.QTextEdit(
                "Welcome to the pyQt Fkmlandunter client!")
        self.chat_box.setFontItalic(True)
        self.chat_box.append("Use /quit to exit.")
        self.chat_box.setReadOnly(True)
        self.chat_box.setFocusPolicy(QtCore.Qt.NoFocus)
        self.chat_input = QtGui.QLineEdit()
        # Connect signals
        QtCore.QObject.connect(self.chat_input,
                QtCore.SIGNAL("returnPressed()"),
                self.on_input)
        QtCore.QObject.connect(self.sock,
                QtCore.SIGNAL("readyRead()"),
                self.parse)
        QtCore.QObject.connect(self.deckPlay,
                QtCore.SIGNAL("clicked()"),
                self.play)
        #QtCore.QObject.connect(self.deckPlay,
                #QtCore.SIGNAL("returnPressed()"),
                #self.play)
        # Master layout
        layout = QtGui.QGridLayout()
        layout.addLayout(topbox, 0, 0, 1, -1)
        for i in range(3):
            for j in range(4):
                layout.addWidget(self.btns[4 * i + j], i + 1, j)
        layout.addWidget(self.wLabel, 1, 4)
        layout.addWidget(self.deckPlay, 2, 4, 2, 1)
        layout.addWidget(self.chat_box, 4, 0, 1, -1)
        layout.addWidget(self.chat_input, 5, 0, 1, -1)
        #layout.addStretch(1)
        central_widget = QtGui.QWidget()
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)
        self.setWindowTitle("Fkmlandunter")
        self.chat_input.setFocus()
        self.btns[0].toggle()

    def on_input(self):
        msg = unicode(self.chat_input.displayText())
        if len(msg) == 0: return
        if msg[0] != '/':
            for char in msg:
                if char == '%':
                    self.chat_box.append("% not allowed in messages")
                    self.chat_input.clear()
                    return
            self.sock.writeData("MSG ")
            self.sock.writeData(msg.encode("utf-8"))
            self.sock.writeData("\n")
            self.sock.flush()
            self.chat_box.setFontItalic(False)
            self.chat_box.append(QtCore.QString('<' + self.myname + "> "
                + msg))
            self.chat_box.setFontItalic(True)
        elif msg.startswith("/q"):
            if self.connected:
                self.sock.writeData("LOGOUT\n")
                self.sock.close()
            exit()
        elif msg.startswith("/c"):
            words = msg.split()
            if len(words) == 3:
                self.connect(self.myname, words[1], int(words[2]))
            elif len(words) == 2:
                self.connect(self.myname, words[1], DEFPORT)
            else:
                self.connect(self.myname, DEFHOST, DEFPORT)
        elif msg.startswith("/s"):
            self.sock.writeData("START\n")
            self.sock.flush()
            self.chat_box.append("Requesting game start")
        elif msg.startswith("/n"):
            if self.connected:
                self.chat_box.append("Game has already started")
                self.chat_input.clear()
                return
            for char in msg:
                if char == '%':
                    self.chat_box.append("% not allowed in nicknames")
                    self.chat_input.clear()
                    return
            words = msg.split()
            if len(words) == 2:
                self.myname = words[1]
                self.chat_box.append("Changed nick to " + self.myname)
        else:
            self.chat_box.append(QtCore.QString("Unknown command: \""
                + msg + '"'))
        self.chat_input.clear()

    def parse(self):
        while self.sock.canReadLine():
            msg = unicode(self.sock.readLine())
            msg = msg.strip()
            if msg.startswith("MSGFROM"):
                words = msg.split(None, 2)
                self.chat_box.setFontItalic(False)
                self.chat_box.append('<' + words[1] + "> " + words[2])
                self.chat_box.setFontItalic(True)
            elif msg.startswith("ACK"):
                print "ACKed"
            elif msg.startswith("JOIN"):
                words = msg.split(None, 1)
                self.playernum += 1
                #self.names.append(words[1])
                #self.player_box[self.playernum -1].setTitle(
                        #self.names[self.playernum -1] + ": 0 points")
                self.chat_box.append(words[1] + " joined the game")
            elif msg.startswith("LEAVE"):
                words = msg.split(None, 1)
                self.player_box.append(words[1] + " left the game")
                #i = 0
                #for name in self.names:
                    #if name == words[1]:
                        #self.player_box[i].setTitle(name + " (left)")
                    #i += 1
            elif msg.startswith("START"):
                words = msg.split(None, 2)
                self.names = words[2].split(None, int(words[1]))
                i = 0
                for name in self.names:
                    self.player_box[i].setTitle(name + ": 0 points")
                    i += 1
                for j in range(i, MAXPLAYERS):
                    self.player_box[j].hide()
                self.playernum = i
                print "have " + `i` + " Players"
            elif msg.startswith("DECK"):
                if self.haveDeck == False:
                    words = msg.split(None, 1)
                    vals = words[1].split()
                    srt = []
                    for val in vals:
                        srt.append(int(val))
                    srt.sort()
                    for i in range(12):
                        self.btns[i].setText(`srt[i]`)
                        self.btns[i].setEnabled(True)
                        #print "adding %s" % val
                    self.haveDeck = True
            elif msg.startswith("RINGS"):
                i = 0
                words = msg.split(None, 1)
                for num in words[1].split():
                    if self.haveBelts == False:
                        self.playerLifeBar[i].setFormat("%v/%m Lifebelts")
                        self.playerLifeBar[i].setMaximum(int(num))
                        self.playerWaterBar[i].setValue(0)
                        self.playerWaterLbl[i].setText("0")
                    n = int(num)
                    if n < 0:
                        self.playerLifeBar[i].setFormat("drowned")
                        self.playerLifeLbl[i].setText("--")
                    else:
                        self.playerLifeBar[i].setValue(n)
                        self.playerLifeLbl[i].setText(num + "/%d"
                                % self.playerLifeBar[i].maximum())
                    i += 1
                self.haveBelts = True
            elif msg.startswith("WEATHER"):
                words = msg.split(None, 1)
                self.wLabel.setText("Weather: " + words[1])
                self.haveWeather = True
                #if self.haveCard > 0: self.deckPlay.setEnabled(True)
                self.deckPlay.setEnabled(True)
            elif msg.startswith("PLAYED"):
                words = msg.split(None, 1)
                i = 0
                for num in words[1].split():
                    self.chat_box.append(self.names[i] + " played " + num)
                    self.player_lastCard[i].setText("Last move: " + num)
                    i += 1
            elif msg.startswith("WLEVELS"):
                words = msg.split(None, 1)
                i = 0
                for num in words[1].split():
                    self.playerWaterBar[i].setValue(int(num))
                    self.playerWaterLbl[i].setText(num)
                    i += 1
            elif msg.startswith("POINTS"):
                self.haveDeck = False
                self.haveBelts = False
                #self.deckChoose.clear()
                #self.deckPlay.setEnabled(True)
                self.btns[0].toggle()
                words = msg.split(None, 1)
                pts = words[1].split(None, self.playernum)
                for i in range(self.playernum):
                    self.playerWaterBar[i].setValue(0)
                    self.playerWaterLbl[i].setText("0")
                    self.playerLifeBar[i].setValue(0)
                    self.playerLifeLbl[i].setText("0/0")
                    self.player_box[i].setTitle(self.names[i] + ": " +
                            pts[i] + " points")
                self.chat_box.append("Points distributed")
            elif msg.startswith("FAIL"):
                self.chat_box.append("Communication error, please play again")
                self.btns[self.lastCard].setEnabled(True)
                self.deckPlay.setEnabled(True)
            elif msg.startswith("TERMINATE"):
                words = msg.split(None, 1)
                self.chat_box.append("Game ended: " + words[1])
                self.sock.close()
                self.connected = False
            else:
                self.chat_box.append(QtCore.QString("Unknown server command: "
                    + msg))

    def connect(self, name, host, port):
        if self.connected: return
        self.sock.connectToHost(host, port, QtCore.QIODevice.ReadWrite)
        if (self.sock.waitForConnected()):
           self.chat_box.append("Connected")
           self.connected = True
        else:
            self.chat_box.append("Connection failed")
            print "Server unavailable"
            exit()
        self.sock.writeData("LOGIN ") 
        self.sock.writeData(name.encode("utf-8"))
        self.sock.writeData("\n")
        self.sock.flush()
        self.chat_box.append("Logged in")

    def play(self):
        dummy = -1
        for i in range(12):
            if self.btns[i].isChecked() and self.btns[i].isEnabled():
                self.lastCard = dummy = i
        self.sock.writeData("PLAY ")
        self.sock.writeData(self.btns[dummy].text())
        self.sock.writeData("\n")
        self.sock.flush()
        self.deckPlay.setEnabled(False)
        self.btns[dummy].setEnabled(False)
        for i in range(12):
            if self.btns[i].isEnabled():
                self.btns[i].toggle()
                break
        #self.btns[dummy].setChecked(False)
        #self.deckChoose.removeItem(self.deckChoose.currentIndex())

    def cardsel(self):
        for i in range(12):
            if self.btns[i].isChecked():
                self.haveCard = i
        self.chat_box.append("Card " + `haveCard` + "selected")
        print "Card " + `haveCard` + "selected"
        if self.haveWeather: self.deckPlay.setEnabled(True)

if __name__ == "__main__":
    # direct launch
    # create qapp
    app = QtGui.QApplication(sys.argv)
    locale = QtCore.QLocale.system().name()
    print "Locale is %s" % locale
    # translate it
    qtTranslator = QtCore.QTranslator()
    if qtTranslator.load("qt_" + locale, ""):
        app.installTranslator(qtTranslator)
    appTranslator = QtCore.QTranslator()
    if appTranslator.load("client_" + locale, ""):
        app.installTranslator(appTranslator)
    # set icon
    app.setWindowIcon(QtGui.QIcon("icon.png"))
    # create main window
    main_window = FkmlandunterMainWin()
    main_window.show()
    # enter main loop
    app.exec_()

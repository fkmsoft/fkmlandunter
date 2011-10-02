class Game(object):

    def __init__(self, ownname):
        self.__OwnName = ownname
        self.__Position = 0
        self.__NumPlayers = 0
        self.__Round = 0
        self.__CurWeather = []
        self.__Points = []
        self.__AllDecks = []
        self.__CurDeck = []
        self.__Rings = []
        self.__Names = []
        self.__Wlevels = []
        self.__PlayedCards = []
        self.__MyTurn = False
        self.__Terminate = False

    def parseServerMsg(self, msg):
        msg = msg.split("\n")
        while msg:
            curmsg = msg.pop(0)
            curmsg = curmsg.split()
            if curmsg != []:
                msghead = curmsg.pop(0)
            else:
                msghead = None
            if msghead == "START":
                #parse NumPlayers & Names
                self.__NumPlayers = int(curmsg.pop(0))
                self.__Names = []
                while curmsg:
                    self.__Names.append(curmsg.pop(0))
                #parse Position
                for i in range(len(self.__Names)):
                    if self.__Names[i] == self.__OwnName:
                        self.__Position = i
                #initialize some attributes
                self.__PlayedCards = []
                self.__AllDecks = []
                for i in range(self.__NumPlayers):
                    self.__PlayedCards.append([])
                    self.__AllDecks.append([])
            elif msghead == "DECK":
                self.__CurDeck = []
                while curmsg:
                    self.__CurDeck.append(int(curmsg.pop(0)))
                #save current deck to all decks
                if self.__Round == 0:
                    self.__AllDecks[self.__Position] = self.__CurDeck
            elif msghead == "RINGS":
                self.__Rings = []
                while curmsg:
                    self.__Rings.append(int(curmsg.pop(0)))
                pass
            elif msghead == "WEATHER":
                self.__CurWeather = []
                while curmsg:
                    self.__CurWeather.append(int(curmsg.pop(0)))
                self.__MyTurn = True
            elif msghead == "ACK":
                pass
            elif msghead == "FAIL":
                pass
            elif msghead == "PLAYED":
                for i in range(len(curmsg)):
                    self.__PlayedCards[i].append(curmsg[i])
                self.__Round += 1
            elif msghead == "WLEVELS":
                self.__Wlevels = []
                while curmsg:
                    self.__Wlevels.append(int(curmsg.pop(0)))
            elif msghead == "POINTS":
                #reset played cards
                self.__PlayedCards = []
                for i in range(self.__NumPlayers):
                    self.__PlayedCards.append([])
                #rotate decks
                for i in range(self.__Position + self.__NumPlayers - 1, self.__Position, -1):
                    self.__AllDecks[i % self.__NumPlayers] = self.__AllDecks[(i - 1) % self.__NumPlayers]
                #new round
                self.__Round = 0
                #parse points
                self.__Points = []
                while curmsg:
                    self.__Points.append(int(curmsg.pop(0)))
            elif msghead == "TERMINATE":
                self.__Terminate = True
            else:
                pass

    # return deckcard chochen by ai or
    #   0 if game state is not yet ready or
    #   -1 if game is over 
    def playDeckCard(self):
        card = 0
        if self.__MyTurn:
            card = self.__CurDeck[self.__Round]
            self.__MyTurn = False
        if self.__Terminate:
            card = -1
        return card 

    # Debugging functions
    def getOwnName(self):
        return self.__OwnName
    def getPosition(self):
        return self.__Position
    def getNumPlayers(self):
        return self.__NumPlayers
    def getRound(self):
        return self.__Round
    def getCurWeather(self):
        return self.__CurWeather
    def getWlevels(self):
        return self.__Wlevels
    def getPoints(self):
        return self.__Points
    def getAllDecks(self):
        return self.__AllDecks
    def getCurDeck(self):
        return self.__CurDeck
    def getNumRings(self):
        return self.__Rings
    def getNames(self):
        return self.__Names
    def getPlayedCards(self):
        return self.__PlayedCards

# vim: set sw=4 ts=4 et fdm=syntax:

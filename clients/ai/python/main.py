import socket
import game

name = "RYTHON"
host = "127.0.0.1"
port = 1337

#connect
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
s.connect(("127.0.0.1", 1337))

#login
s.send("LOGIN %s\n" % name)
print "%s connected" % name

#try to start game
s.send("START\n")

#creating new game object which contains ai logic and game state
g = game.Game(name)
while True:
    input = s.recv(1024)
    g.parseServerMsg(input)
    card = g.playDeckCard()
    if card != 0:
        s.send("PLAY %d\n" % card)
        #print all infos
        print "=====DEBUG INFOS====="
        print "OwnName=", g.getOwnName()
        print "Position=", g.getPosition()
        print "NumPlayers=", g.getNumPlayers()
        print "Round=", g.getRound()
        print "CurWeather=", g.getCurWeather()
        print "Wlevels=", g.getWlevels()
        print "Points=", g.getPoints()
        print "AllDecks=", g.getAllDecks()
        print "CurDeck=", g.getCurDeck()
        print "Rings=", g.getNumRings()
        print "Names=", g.getNames()
        print "PlayedCards=", g.getPlayedCards()
        print "====================="
        print "I play %d" % card
    if card == -1:
        break

#close connection
s.close()


# ToDo:
# -argument passing
# -exception handling

# vim: set sw=4 ts=4 et fdm=syntax:

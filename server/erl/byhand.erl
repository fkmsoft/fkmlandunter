-module(byhand).

-compile(export_all).

-define(PORT,1337).

start() ->
    openConnection(?PORT).

openConnection(Port) ->
    {ok,LSock}=gen_tcp:listen(Port, [binary, {packet, 0}, {active, false}]),
    startAccepting(LSock,self()),
    waitForPlayers(0,0,LSock,[]).

waitForPlayers(5,_,L,Ps) ->
    startGame(L,Ps);
waitForPlayers(LoggedIn,Connected,L,PPids) ->
    io:put_chars("waiting\n"),
    receive
	{connect,PPid} ->
	    waitForPlayers(LoggedIn,Connected+1,L,[PPid|PPids]);
	{login} ->
	    waitForPlayers(LoggedIn+1,Connected,L,PPids);
	{disconnect,PPid} ->
	    waitForPlayers(LoggedIn,Connected-1,L,lists:substract(PPids,[PPid]));
	{logout,PPid} ->
	    waitForPlayers(LoggedIn-1,Connected-1,L,lists:substract(PPids,[PPid]))
    end.

startAccepting(Sock,Ctl) ->
    Pid=spawn_link(?MODULE,acceptPlayers,[Sock,Ctl]),
    ok=gen_tcp:controlling_process(Sock,Pid),
    io:format("control passed on\n"),
    Pid.

acceptPlayers(Sock,Ctl) ->
    io:put_chars("accepting\n"),
    {ok,PSock}=gen_tcp:accept(Sock),
    %PPid=playerProc(PSock,Sock),
    Ctl ! {connect,self()},
    startAccepting(Sock,Ctl),
    needLogin(PSock,Ctl).
    %acceptPlayers(Sock,Ctl).

playerProc(PSock,MSock) ->
    Pid=spawn_link(?MODULE,needLogin,[PSock,MSock]),
    ok=gen_tcp:controlling_process(PSock,Pid),
    io:put_chars("returning pid\n"),
    Pid.

needLogin(Sock,Ctl) ->
    io:put_chars("waiting for login\n"),
    inet:setopts(Sock,[{active,once}]),
    io:put_chars("set sock to active\n"),
    receive
	{tcp,Sock,Data} ->
	    io:format("got ~p from player~n",[Data]),
	    gen_tcp:send(Sock,<<"ACK">>),
	    play(Sock,Ctl);
	{tcp_closed} ->
	    Ctl ! {disconnect,self()}
    end.

play(_S,_C) ->
    io:put_chars("now playing\n").

startGame(_,_) ->
    io:put_chars("game started\n").

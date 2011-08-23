-module(byhand).

-compile(export_all).

-define(PORT,1337).

start() ->
    openConnection(?PORT).

openConnection(Port) ->
    {ok,LSock}=gen_tcp:listen(Port, [list, {packet, 0}, {active, false}]),
    startAccepting(LSock,self()),
    waitForPlayers(0,0,LSock,[]).

waitForPlayers(2,_,L,Ps) ->
    startGame(L,Ps);
waitForPlayers(LoggedIn,Connected,L,PPids) ->
    io:format("waiting ~w ~w~n",[LoggedIn,Connected]),
    receive
	{connect,PPid} ->
	    waitForPlayers(LoggedIn,Connected+1,L,[PPid|PPids]);
	{login} ->
	    waitForPlayers(LoggedIn+1,Connected,L,PPids);
	{disconnect,PPid} ->
	    waitForPlayers(LoggedIn,Connected-1,L,lists:subtract(PPids,[PPid]));
	{logout,PPid} ->
	    waitForPlayers(LoggedIn-1,Connected-1,L,lists:subtract(PPids,[PPid]));
	Other ->
	    io:format("strange message ~p~n",[Other]),
	    waitForPlayers(LoggedIn,Connected,L,PPids)
    end.

startAccepting(Sock,Ctl) ->
    Pid=spawn_link(?MODULE,acceptPlayers,[Sock,Ctl]),
    ok=gen_tcp:controlling_process(Sock,Pid),
    Pid.

acceptPlayers(Sock,Ctl) ->
    io:put_chars("accepting another player\n"),
    {ok,PSock}=gen_tcp:accept(Sock),
    Ctl ! {connect,self()},
    startAccepting(Sock,Ctl),
    needLogin(PSock,Ctl).

playerProc(PSock,MSock) ->
    Pid=spawn_link(?MODULE,needLogin,[PSock,MSock]),
    ok=gen_tcp:controlling_process(PSock,Pid),
    Pid.

needLogin(Sock,Ctl) ->
    io:put_chars("waiting for login\n"),
    inet:setopts(Sock,[{active,once}]),
    receive
	{tcp,Sock,Data} ->
	    io:format("got ~p from player~n",[Data]),
	    case lists:prefix("LOGIN ",Data) of
		true ->
		    ["LOGIN",Name]=string:tokens(Data," \r\n"),
		    gen_tcp:send(Sock,"ACK\r\n"),
		    Ctl ! {login},
		    inLobby(Sock,Ctl,Name);
		false ->
		    ok=gen_tcp:close(Sock),
		    Ctl ! {logout,self()}
	    end;
	{tcp_closed} ->
	    Ctl ! {disconnect,self()}
    end.

inLobby(Sock,Ctl,Name) ->
    io:format("~s now waiting in Lobby~n",[Name]),
    inet:setopts(Sock,[{active,once}]),
    receive
	{tcp,Sock,Data} ->
	    lobbyCmd(Sock,lists:subtract(Data,"\r\n"),Ctl,Name);
	{tcp_closed} ->
	    ok=gen_tcp:close(Sock),
	    Ctl ! {logout,self()}
    end.

lobbyCmd(Sock,[],Ctl,Name) ->
    inLobby(Sock,Ctl,Name);
lobbyCmd(Sock,Str,Ctl,Name) ->
    Toks=string:tokens(Str," "),
    case hd(Toks) of
	"MSG" ->
	    Ctl ! {message,Name,Toks},
	    inLobby(Sock,Ctl,Name);
	"LOGOUT" ->
	    gen_tcp:send(Sock,"TERMINATE logged out\r\n"),
	    Ctl ! {logout,self()};
	"START" ->
	    Ctl ! {start_req,Name,self()},
	    inLobby(Sock,Ctl,Name);
	Other ->
	    io:format("inv cmd ~p/~w~n",[Other,Toks]),
	    gen_tcp:send(Sock,"FAIL invalid command\r\n"),
	    inLobby(Sock,Ctl,Name)
    end.

startGame(_,_) ->
    io:put_chars("game started\n").

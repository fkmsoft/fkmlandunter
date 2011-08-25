-module(byhand).

-compile(export_all).

-define(PORT,1337).
-define(CARDCOUNT,60).
-define(DECKSIZE,12).
-define(MINPLAYERS,3).
-define(MAXPLAYERS,5).

start() ->
    openConnection(?PORT).

openConnection(Port) ->
    {ok,LSock}=gen_tcp:listen(Port, [list, {packet, 0}, {active, false}]),
    ASock=startAccepting(LSock,self()),
    waitForPlayers(0,0,{LSock,ASock},[],dict:new()).

waitForPlayers(3,_,{L,A},Ps,D) ->
    A ! stop,
    startGame(L,Ps,D);
waitForPlayers(LoggedIn,Connected,L,PPids,D) ->
    io:format("waiting ~w ~w~n",[LoggedIn,Connected]),
    receive
	{connect,PPid} ->
	    waitForPlayers(LoggedIn,Connected+1,L,[PPid|PPids],D);
	{login,PPid,Name} ->
	    PDict=dict:store(name,Name,dict:new()),
	    sendJoinMessage(PPid,D,Name),
	    waitForPlayers(LoggedIn+1,Connected,L,PPids,dict:store(PPid,PDict,D));
	{disconnect,PPid} ->
	    waitForPlayers(LoggedIn,Connected-1,L,lists:subtract(PPids,[PPid]),D);
	{logout,PPid,Reason} ->
	    sendLeaveMessage(PPid,D,Reason),
	    waitForPlayers(LoggedIn-1,Connected-1,L,
			   lists:subtract(PPids,[PPid]),dict:erase(PPid,D));
	{message,PPid,Msg} ->
	    sendMessage(Msg,PPid,D),
	    waitForPlayers(LoggedIn,Connected,L,PPids,D);
	Other ->
	    io:format("strange message ~p~n",[Other]),
	    waitForPlayers(LoggedIn,Connected,L,PPids,D)
    end.

startAccepting(Sock,Ctl) ->
    Pid=spawn_link(?MODULE,acceptPlayers,[Sock,Ctl]),
    ok=gen_tcp:controlling_process(Sock,Pid),
    Pid.

acceptPlayers(Sock,Ctl) ->
    receive
	stop ->
	    io:put_chars("terminating acceptor\n")
    after
	0 ->
	    io:put_chars("accepting another player\n"),
	    {ok,PSock}=gen_tcp:accept(Sock),
	    Pid=playerProc(PSock,Ctl),
	    Ctl ! {connect,Pid},
	    acceptPlayers(Sock,Ctl)
    end.

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
		    ok=gen_tcp:send(Sock,"ACK\r\n"),
		    Ctl ! {login,self(),Name},
		    inLobby(Sock,Ctl);
		false ->
		    ok=gen_tcp:close(Sock),
		    Ctl ! {disconnect,self()}
	    end;
	{tcp_closed} ->
	    Ctl ! {disconnect,self()};
	{start,M} ->
	    io:format("got start message ~p, exiting~n",[M]),
	    ok=gen_tcp:close(Sock)
    end.

inLobby(Sock,Ctl) ->
    inet:setopts(Sock,[{active,once}]),
    receive
	{tcp,Sock,Data} ->
	    lobbyCmd(Sock,lists:subtract(Data,"\r\n"),Ctl);
	{tcp_closed} ->
	    ok=gen_tcp:close(Sock),
	    Ctl ! {logout,self(),"lost connection"};
	{message,Str} ->
	    ok=gen_tcp:send(Sock,Str),
	    inLobby(Sock,Ctl);
	{start,Str} ->
	    ok=gen_tcp:send(Sock,Str),
	    inGame(Sock,Ctl)
    end.

lobbyCmd(Sock,[],Ctl) ->
    inLobby(Sock,Ctl);
lobbyCmd(Sock,Str,Ctl) ->
    Toks=string:tokens(Str," "),
    case hd(Toks) of
	"MSG" ->
	    Ctl ! {message,self(),afterN(Str,5)},
	    inLobby(Sock,Ctl);
	"LOGOUT" ->
	    gen_tcp:send(Sock,"TERMINATE logged out\r\n"),
	    Ctl ! {logout,self(),afterN(Str,8)};
	"START" ->
	    Ctl ! {start_req,self()},
	    inLobby(Sock,Ctl);
	Other ->
	    io:format("inv cmd ~p/~w~n",[Other,Toks]),
	    gen_tcp:send(Sock,"FAIL invalid command\r\n"),
	    inLobby(Sock,Ctl)
    end.

afterN(_Str,I) when I < 0 ->
	[];
afterN(Str,I) when I >= 0 ->
	L=length(Str),
	if
	    I =< L ->
		string:substr(Str,I);
	    I > L ->
		[]
	end.

sendMessage(Msg,Sender,Dict) ->
    Name=dict:fetch(name,dict:fetch(Sender,Dict)),
    Str=io_lib:format("MSGFROM ~s ~s\r\n",[Name,Msg]),
    io:format("sending ~p to all~n",[Str]),
    sendStr(Str,Sender,Dict).

sendLeaveMessage(Pid,Dict,Reason) ->
    Name=dict:fetch(name,dict:fetch(Pid,Dict)),
    Str=io_lib:format("LEAVE ~s ~s\r\n",[Name,Reason]),
    sendStr(Str,Pid,Dict).

sendJoinMessage(Pid,Dict,Name) ->
    Str=io_lib:format("JOIN ~s\r\n",[Name]),
    sendStr(Str,Pid,Dict).

sendStr(Str,Pid,Dict) ->
    Send=fun(PPid,_PDict,_Acc) ->
		 if
		     PPid == Pid ->
			 io:put_chars("not sending msg back to sender\n");
		     PPid /= Pid ->
			 PPid ! {message,Str}
		 end
	 end,
    dict:fold(Send,void,Dict).

startGame(LSock,Ps,D) ->
    N=broadcastStart(Ps,D),
    io:put_chars("game started\n"),
    Decks=genDecks(N),
    playRound(N,Decks,LSock,touchPlayers(D)).

playRound(0,_De,_L,_Di) ->
    io:put_chars("game ended\n");
playRound(N,De,L,Di) ->
    CurrentDecks=De,%rotate(De),
    D1=replaceDecks(CurrentDecks,Di),
    io:format("playing ~w more rounds~n",[N]),
    playRound(N-1,De,L,playTurns(D1,?DECKSIZE)).

broadcastStart(Ps,D) ->
    Namer=fun(_K,V,A) ->
		  [[$ |dict:fetch(name,V)]|A]
	  end,
    Names=dict:fold(Namer,[],D),
    Str=io_lib:format("START ~w~s\r\n",[N=length(Names),Names]),
    broadcast(start,Str,Ps),
    N.

broadcast(Type,Str,Ps) ->
    lists:foreach(fun(P) ->
			  P ! {Type,Str}
		  end,Ps).

genDecks(N) when N >= ?MINPLAYERS, N =< ?MAXPLAYERS ->
    Cs=lists:seq(1,?CARDCOUNT),
    T=erlang:list_to_tuple(Cs),
    Deck=erlang:tuple_to_list(shuffle(T,2*?CARDCOUNT)),
    genDecks([],N,Deck).

genDecks(Acc,0,_) ->
    Acc;
genDecks(Acc,N,Deck) ->
    genDecks([string:substr(Deck,1,?DECKSIZE)|Acc],N-1,
	     string:substr(Deck,1+?DECKSIZE)).

shuffle(T,0) ->
    T;
shuffle(T,K) when is_tuple(T), is_integer(K), K > 0 ->
    A=random:uniform(?CARDCOUNT-1),
    B=random:uniform(?CARDCOUNT-1),
    Ae=element(A,T),
    Be=element(B,T),
    Ta=setelement(A,T,Be),
    Tb=setelement(B,Ta,Ae),
    shuffle(Tb,K-1).

rotate([D|Ds]) ->
    lists:append(Ds,[D]).

replaceDecks(Decks,D) ->
    % FIXME: this is wrong, because evaluation order is undefined
    Replacer=fun(P,PD,{A,RD}) ->
		     B=dict:store(deck,hd(A),PD),
		     {tl(A),dict:store(P,B,RD)}
	     end,
    {[],R}=dict:fold(Replacer,{Decks,dict:new()},D),
    R.

touchPlayers(D) ->
    Toucher=fun(_PPid,PDict) ->
		    dict:store(deck,[],PDict)
	    end,
    dict:map(Toucher,D).

playTurns(D,0) ->
    D;
playTurns(D,N) ->
    io:format("playing ~w more turns~n",[N]),
    sendDecks(D),
    playTurns(D,N-1).

sendDecks(D) ->
    Printer=fun(N) ->
		    io_lib:format(" ~w",[N])
	    end,
    Sender=fun(P,PD,void) ->
		   Deck=dict:fetch(deck,PD),
		   Strs=lists:map(Printer,Deck),
		   P ! {message,io_lib:format("DECK~s\r\n",[Strs])},
		   void
	   end,
    dict:fold(Sender,void,D).

inGame(S,Ctl) ->
    receive
	{message,M} ->
	    ok=gen_tcp:send(S,M),
	    inGame(S,Ctl)
    end.

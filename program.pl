ev(neg(X),sub(1,RX)):-ev(X,RX).
ev(or(X,Y),sub(add(RX,RY),mul(RX,RY))):-ev(X,RX),ev(Y,RY).
ev(and(X,Y),mul(RX,RY)):-ev(X,RX),ev(Y,RY).
ev(X,X).

eva(neg(X),S):-
	eva(X,SX),
	append_strings("(1-",SX,AS),
	append_strings(AS,")",S).
	
%	string_concat("(1-",SX,AS),
%	string_concat(AS,")",S).
	
eva(and(X,Y), S):-
	eva(X,SX),
	eva(Y,SY),
	append_strings("(", SX, SXX),
	append_strings(SXX, "*", HS),
	append_strings(HS, SY, FS),
	append_strings(FS, ")", S).
		
%	string_concat("(", SX, SXX),
%	string_concat(SXX, "*", HS),
%	string_concat(HS, SY, FS),
%	string_concat(FS, ")", S).


eva(or(X,Y), S):-
	eva(X,SX),
	eva(Y,SY),
	append_strings("((", SX, S1),
	append_strings(S1, "+", S2),
	append_strings(S2, SY, S3),
	append_strings(S3, ")-(", S4),
	append_strings(S4, SX, S5),
	append_strings(S5, "*", S6),
	append_strings(S6, SY, S7),
	append_strings(S7, "))", S).
	
%	string_concat("((", SX, S1),
%	string_concat(S1, "+", S2),
%	string_concat(S2, SY, S3),
%	string_concat(S3, ")-(", S4),
%	string_concat(S4, SX, S5),
%	string_concat(S5, "*", S6),
%	string_concat(S6, SY, S7),
%	string_concat(S7, "))", S).
%eva(X,S):-string_to_atom(S,X).
eva(X,S):-atom_string(X,S).

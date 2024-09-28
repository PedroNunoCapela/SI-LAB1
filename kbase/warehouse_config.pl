:-ensure_loaded('RTXengine/RTXutil').

% dll/foreign predicates, to supress the warning
:- dynamic mqtt_connect/2.
:- dynamic mqtt_create_client/3.
:- dynamic mqtt_subscribe/4.
:- dynamic mqtt_publish/5.

:-dynamic state/2. % to represent state(x_is_at(2), 133223).
:-dynamic before/3. % represent before(x_is_at(2),  133223,  133250).
:-dynamic command/2. % to represent command(move_x_left, 213123).
:-dynamic goal/2.  % to represent goal(x_is_at(2), 33133).

assert_state(NewState):-
     get_time(TimeStamp),
     assert_before_state(NewState),
     assert(state(NewState, TimeStamp)).

state(State):-     state(State, _TimeStamp).

assert_before_state(NewState):-
     make_most_generic_term(NewState, GenericState),
     state(GenericState, _),
     retract_state(GenericState),
     !;
     true.

retract_state(State):-
     state(State, TimeStamp_ini),
     make_most_generic_term(State, GenericState),
     retractall(state(GenericState, _)),
     retractall(before(GenericState, _,_)),
     get_time(TimeStamp_end),
     assert(before(State, TimeStamp_ini, TimeStamp_end)),
     !
     ; % or
     true.

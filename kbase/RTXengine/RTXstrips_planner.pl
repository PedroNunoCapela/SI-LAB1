
% https://www.cs.ubc.ca/~poole/ci/lectures/ch8/lect5.pdf

:-ensure_loaded('RTXutil').

:-dynamic asserted_state/1.
:-dynamic strips/1.
:-dynamic world/2.
:-dynamic pre_conditions/1.
:-dynamic heap/1.
:-dynamic goals/1.
:-dynamic plan/1.

:-op(900, fy, act).
:-op(900, fy, pre).
:-op(900, fy, add).
:-op(900, fy, domain).
% :- op(900, fy, add1).
% :- op(900, fy, add2).
:-op(900, fy, del).
:-op(900, fy, effect).
:-op(900, fy, iff).
:-op(900, fy, partial).
%:-op(900, fy, condition).
%:-op(900, fy, sequence).
%

:-discontiguous strips/1.

solve_sequential([], Wi, Wi, []):-
  solve_sequential([], Wi, Wi, [], _Domain).

solve_sequential([Goal|Goals], W1, Wf, Plan):-
  solve_sequential([Goal|Goals], W1, Wf, Plan, _Domain).

solve_sequential([], Wi, Wi, [], _Domain).


solve_sequential([Goal|Goals], W1, Wf, Plan, Domain):-
  solve_with_domain([Goal], W1, W2, PlanOfGoal, Domain),
  solve_sequential(Goals, W2, Wf, PlanOfRemainingGoals, Domain),
  flatten(
   [
      %wait_for_all(W1),
      PlanOfGoal,
      PlanOfRemainingGoals
  ], Plan).


solve(Goals, W0, W1, Plan, Domain):-
  solve_with_domain(Goals, W0, W1, Plan,Domain).


solve(Goals, W0, W1, Plan):-
    solve_with_domain(Goals, W0, W1, Plan, _Domain).


solve_with_domain([SeqTerm], W0, W1, Plan, Domain):-
  SeqTerm =.. [seq|Goals],
  solve_sequential(Goals, W0, W1, Plan, Domain),
  !.

solve_with_domain(Goals, W0, W1, Plan, Domain):-
    retractall(goals(_)),
    assert(goals(Goals)),
    solve(Goals, W0, W1, [], Plan_nested, Domain),
    % ISTO NÂO DEVERIA SER NECESSARIO
    holds(Goals, W1),
    flatten(Plan_nested, Plan).



solve([], W0, W0, _,[], _):-
    !.

solve(Goals, W0, W0, _,[], _):-
    %ground_list(W0,Goals, Goals2),
    holds(Goals, W0),
    %forall(member(G, Goals2), member(G, W0)),
    !.

/*
% solve sequential with seq(List)
solve([seq(Goals)],W0, W3, _Forbidden, Plan, Domain):-
  is_list(Goals),
  solve_sequential(Goals, W0, W3, Plan, Domain),
  writeln(done).
*/


% solve sequential with seq(Term) or seq(List)
solve([SequenceGoalsTerm],W0, W3, _Forbidden, Plan, Domain):-
  SequenceGoalsTerm =.. [seq | _],
  findall( Goal, arg(_, SequenceGoalsTerm, Goal), List),
  flatten(List, Goals),
  solve_sequential(Goals, W0, W3, Plan, Domain).
  %writeln(done).



% solve unsatisfied
solve(Goals,W0, W3, Forbidden, Plan, Domain):-
    % get_a_goal(Goal, Goals, Remaining_goals),
    get_next_goal(Goal, Goals, Remaining_goals),

    \+ holds( [Goal], W0),   % passar à frente dum estado ja satisfeito

    %writeln(handling_goal(Goal, W0)),

    \+ member(Goal, W0),

    %writeln(next____goal(Goal)),

    achieves(Action, Goal),

    % se o Domain não estiver instanciada, então são unificadas.
    % Em caso disso, Domain assume o valor que está nessa accao
    same_domain(Action, Domain),
    %writeln(domain(Domain)),

    strips(Spec_list)=Action,
    member(act Operator, Spec_list),


    member( pre PrecList, Spec_list),
    member( add AddList, Spec_list),

    %(   member( condition [CondList], Spec_list), CondList, ! ; true),

        retractall(world(_,_)),
        assert(world(W0, AddList)),
        retractall(pre_conditions(_)),
        assert(pre_conditions(PrecList)),
        %unify_Pre_with_World(PrecList, W0), % !!! magics here
        Action,                             % CHAMADA AO CORPO

     (   \+ member(Operator, Forbidden); member(Operator, Forbidden), Operator== []),

    solve(PrecList, W0, W1,[Operator|Forbidden], Plan_for_Precedences, Domain),

    %test if Action can still be done after getting plan for preconditions.
    (
      subset(PrecList, W1)
      ;
/*
      writeln(lllllllllllllll),
      [seq(List)]=PrecList,
      writeln(seq(List)),
      is_list(List),
      */
       [SequenceTermOrList] = PrecList,
       SequenceTermOrList   =.. [seq | List], % send sequencia, as precedences ja foram verificadas antes, no solve normal
       filter_contradictory_states(List, List_Clean),
       subset(List_Clean, W1)
    ),


    do(Action, W1, W2, Plan_for_Precedences),

    (   not(member(  partial(_), Spec_list)),
        solve(Remaining_goals, W2, W3,Forbidden, Plan_for_remaining_goals,Domain)
        ;
        member(  partial(_), Spec_list),
        solve(Goals, W2, W3,Forbidden, Plan_for_remaining_goals, Domain)
    ),

    append([Plan_for_Precedences, Operator, Plan_for_remaining_goals ], Plan).
    %append([Plan_for_Precedences,[wait_for_all(PrecList), Operator], Plan_for_remaining_goals ], Plan).


% se o Domain não estiver instanciada, então são unificadas.
same_domain(Action, Domain):-
   strips(Spec_list)=Action,
   member(domain Domain2,Spec_list),
   %format('d1=~w  d2=~w~n',[Domain, Domain2]),
   Domain = Domain2,
   !.

% a accao não tem domain, então pode ser usada
same_domain(Action, _Domain):-
   strips(Spec_list)=Action,
   \+ member(domain _Domain2,Spec_list).


% there is magic in this operator...
% It does the job... wish understand why!
unify_Pre_with_World([], _) => true.
unify_Pre_with_World([E|R], Set) =>
    (   memberchk(E, Set),!;true),
    unify_Pre_with_World(R, Set).




test([]).
%test([seq]).
%test(simula:seq):-!.

test(seq(L)):-
    is_list(L),
    test(L),
    !.

test(G):-
    \+ is_list(G),
    %\+ (G == seq(_)),    % not a sequence with a list
    \+ (G =.. [seq|_] ),  % not a sequence
    test_with_catch(planner, simula:G, false).
    % catch(simula:G, _, false).

test([G|Goals]):-
    test(G),
    test(Goals).


achieves(strips(List_spec), G):-
    clause(strips(List_spec), _BODY),
    member(add AddList, List_spec),
    holds(G, AddList).


% se são sequencias, ja houve verificação do hold nas chamadas
% anteriores.

/*
holds([seq([])], _).
holds([seq([_|_])], _).
holds([Seq], W):-
  Seq =.. [seq|List],
  filter_contradictory_states(List, List_Filtered),
  holds(List_Filtered, W),!.
*/

holds(G, W):-
    retractall(asserted_state(_)), % to be sure
    assert_states(W),
    test(G), %
    ground_it(W, G, _),
    %member(G, W),       % member is used to unify and instantiate arguments of action
    retract_states,
    !.

holds(_G, _W):-
    retract_states,
    fail.


testa([]).

testa(Goals):-
    get_next_goal(G, Goals, RemainingGoals),
    %format('g=~w    L=~w~n',[G,RemainingGoals]),
    log_nl(G),
    testa(RemainingGoals).


% is not a sequence
get_next_goal(NextGoal, Goals, RemainingGoals):-
    nth1(1,Goals, NotASequence),
    not(   NotASequence =.. [seq|_]       ),
    member(NextGoal, Goals),
    delete(Goals, NextGoal, RemainingGoals).
    %writeln(1).

% is a sequence with a list
get_next_goal(NextGoal,[seq([NextGoal|RemainingGoals])], [seq(RemainingGoals)]):-
  seq(RemainingGoals) \= seq([]).
  %writeln(2.1).
get_next_goal(NextGoal,[seq([NextGoal|RemainingGoals])], []):-
  seq(RemainingGoals) == seq([]).
  %writeln(2.2).

% is a term sequence
% S= sequence(a,b,c), S =.. [sequence, NextGoal|RemainingGoals].
get_next_goal(NextGoal,[Sequence], [RemainingGoals]):-
  Sequence =.. [seq, NextGoal| RemainingList],
  not( is_list(NextGoal)),
  RemainingGoals =.. [seq| RemainingList],
  RemainingGoals \= seq.
  %writeln(3.1).

get_next_goal(NextGoal,[Sequence], []):-
  Sequence =.. [seq, NextGoal| RemainingList],
  not( is_list(NextGoal)),
  RemainingGoals =.. [seq| RemainingList],
  RemainingGoals == seq.
  %writeln(3.2).




/*
get_next_goal(NextGoal, Goals, RemainingGoals):-
    member(G, Goals),
    extract_goal(G, Goals, RemainingGoals, NextGoal).


extract_goal(sequence [NextGoal|List], Goals, RemainingGoals,       NextGoal):-
    !,
    findall( Element,
             (

                 member(El, Goals),
                 (
                     El = (sequence X),
                     X \= [NextGoal], % it is not the last of the sequence
                     Element = (sequence List),
                     !;
                     El \= (sequence X),
                     Element = El
                 )
             )
           ,RemainingGoals).




extract_goal(sequence [], Goals, RemainingGoals,       NextGoal):-
    !,
    select( sequence [], Goals, RemainingGoalsTemp),
    get_next_goal(NextGoal, RemainingGoalsTemp,RemainingGoals),!.


extract_goal(Goal, Goals, RemainingGoals,       Goal):-
    Goal \= (sequence _L),
    select( Goal, Goals, RemainingGoals).
*/

/*
get_a_goal(Goal, Goals, Remaining_goals):-
    member(Goal, Goals),
    delete(Goals, Goal, Remaining_goals).
*/


do(Action, W1, W3, _PreviousPlan):-
   %writeln(do(Action)),
   strips(Spec_List) = Action,
   not(  member(partial(_List), Spec_List)),
   member(add AddList, Spec_List),
   member(del DelList, Spec_List),
   subtract_set(W1, DelList, W2),
   append(W2,AddList, W3).

do(Action, W1, W3, _PreviousPlan):-
   strips(Spec_List) = Action,
   member(partial(ListPartial), Spec_List),
   %writeln(partial(ListPartial)),
   member(del DelList, Spec_List),
   subtract_set(W1, DelList, W2),
   append(W2,ListPartial, W3).



% just to provide values to no grounded arguments of clauses

ground_it(W, Term, Term):-
    \+ is_list(Term),
    member(Term, W),
    !.

ground_it(W, List, List2):-
    ground_list(W, List, List2).



ground_list(_W, [], []).
ground_list(W,[E|Tail], [E|Result]):-
    member(E, W),
    ground_list(W, Tail, Result),
    !.

ground_list(W,[E|Tail], [E|Result]):-
    ground_list(W, Tail, Result).



subtract_set(Set1, Set2, Result):-
   findall(Element,
           (
               member(Element, Set1),
               \+ member(Element, Set2)
           ), Result).



/*
holds(G,W):-
    member(G, W).
*/


% quando as goals são duma sequencia, podem haver estados contraditórios
% nesse caso, considerar só os ultimos, por exemplo
% seq(x_is_at(2),x_is_at(1)), so devendo considerar x_is_at(1)
filter_contradictory_states(List_i, List_f):-
  reverse(List_i, List_Reversed),
  filter_contradictory_states(List_Reversed, [], List_f).

filter_contradictory_states([], Result, Result).

filter_contradictory_states([Element|Tail], Acumulated, Result):-
  make_most_generic_term(Element, Generic),
  \+member( Generic, Acumulated),
  filter_contradictory_states(Tail, [Element|Acumulated], Result).


filter_contradictory_states([Element|Tail], Acumulated, Result):-
  make_most_generic_term(Element, Generic),
  member( Generic, Acumulated),
  filter_contradictory_states(Tail, Acumulated, Result).





assert_states([]).
assert_states([State|States]):-
    functor(State, Name, Arity),
    current_predicate(State, Name/Arity),
    assert_states(States),
    !.

assert_states([State|States]):-
    assert(simula:State),
    assert(asserted_state(State)),
    assert_states(States).

retract_states:-
    findall( _, ( asserted_state(S), retract(simula:S)), _),
    retractall(asserted_state(_)).






















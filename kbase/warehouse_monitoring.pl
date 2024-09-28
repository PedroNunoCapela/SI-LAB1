:-ensure_loaded('RTXengine/RTXengine').
:-ensure_loaded(mqtt_prolog_messages).


:-dynamic mqtt_monitoring_handler/1.
create_monitoring_client:-
    load_mqtt_library,
    not(mqtt_monitoring_handler(_)),
    mqtt_broker(Broker_URL), % it is defined in the previous file (right click to see it)
    mqtt_create_client(client_monitoring, Broker_URL,   Handler),
    % the Handler is the C/C++ void *pointer inside the DLL.
    assert(mqtt_monitoring_handler(Handler)),
    mqtt_connect(Handler, _Result),
    !;
    true.

client_monitoring_on_connect_success(Handler):-
   format('success connection of ~w~n', [client_monitoring]),
   %mqtt_monitoring_handler(Handler2), % get the void * pointer
   mqtt_subscribe(Handler, sensor  ,1, _Result1). % QoS= 1

  % subscribe to the other topics related to all the warehouse events.


client_monitoring_on_message_arrived(Topic, Message, _Handler):-
    % format('received MQTT topic: ~w, Payload: ~w~n', [Topic, Message]),
    % store the sensors as facts in the knowledge base
     atom_json_dict(Message, JsonDict, []),
    StringName        = JsonDict.name,
    StringValue = JsonDict.value,
    number_string(NumberValue, StringValue),
    atom_string(AtomName, StringName),
    assert_fact(Topic,AtomName, NumberValue).

assert_fact(sensor, x_is_at, -1):-
    retract_state( x_is_at(_)),
    !.

assert_fact(sensor, x_is_at, Position):-
    assert_state( x_is_at(Position)).


assert_fact(sensor, x_moving, Direction):-
    assert_state(x_moving(Direction)).

% remaining facts...

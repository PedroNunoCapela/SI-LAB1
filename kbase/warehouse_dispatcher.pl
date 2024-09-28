:-ensure_loaded(mqtt_prolog_messages).
:-ensure_loaded('RTXengine/RTXengine').
:-ensure_loaded(warehouse_config).
:-use_module(library(http/json)).


:-dynamic mqtt_dispatcher_handler/1.
create_dispatcher_client:-
    load_mqtt_library,
    not(mqtt_dispatcher_handler(_)),
    mqtt_broker(Broker_URL),
    mqtt_create_client(client_dispatcher, Broker_URL,   Handler),
    % the Handler is the C/C++ void *pointer inside the DLL.
    assert(mqtt_dispatcher_handler(Handler)),
    mqtt_connect(Handler, _Result),
    !;
    true.

assert_command(Command):-
    create_dispatcher_client,
    get_time(TimeStamp),
    assert(command(Command, TimeStamp)),
    send_pending_commands.

assert_command(Command, TS):-
    create_dispatcher_client,
    % get_time(TimeStamp),
    assert(command(Command, TS)),
    send_pending_commands.

client_dispatcher_on_connect_success(_Handle):-
   writeln(dispatcher_on_connect_success),
   send_pending_commands.

client_dispatcher_on_publish_success(_Topic, JsonMessageSent, _Handler ):-
    % writeln(on_publish_success(JsonMessageSent)),
    atom_json_dict(JsonMessageSent, JsonDict, []),
    StringName        = JsonDict.name,
    StringValue = JsonDict.value,
    number_string(NumberValue, StringValue),
    atom_string(AtomName, StringName),
    % writeln( mqtt(AtomName, NumberValue)  ),
    retract(command((AtomName, NumberValue), _TS)),
    send_pending_commands.

send_pending_commands:-
    command( (Name, Value), _TS), %e.g. (Command, Value)=(motor_x, 1)
    number_string(Value, StringValue),
    JSONTerm = json([name=Name, value=StringValue]),
    atom_json_term(JSONcommand, JSONTerm, []), % {"name":"motor_x", "value":"1"}
    mqtt_dispatcher_handler(Handler),
    mqtt_publish(Handler, 'actuator', JSONcommand, 1, _Result),
    !;
    true.
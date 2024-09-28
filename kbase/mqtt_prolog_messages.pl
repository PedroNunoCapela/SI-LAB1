mqtt_broker('tcp://localhost:1883').


:-dynamic mqtt_library_loaded/0.
load_mqtt_library:-
   not(mqtt_library_loaded),
   % loads the library mqtt_prolog.dll
   use_foreign_library(foreign(mqtt_prolog)),
   assert(mqtt_library_loaded),
   !;
   true.

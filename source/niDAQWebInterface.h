#pragma once


void terminar_ligacao(void);


void sighandle(int signum);


/*****************SIMULATOR ACCESS CODE *******************/
void start_mg_server(void);

void configure_simulator_server();


void sim_create_DI_channel(uInt32 port);


void sim_create_DO_channel(uInt32 port);


uInt8 sim_ReadDigitalU8(uInt32 port);



void sim_WriteDigitalU8(uInt32 port, uInt8 data);


void sim_close_channels();


/*****************REAL KIT CODE *******************/

void createDigitalInput(uInt32 port);


void createDigitalOutput(uInt32 port);


uInt8 readDigitalU8(uInt32 port);


void writeDigitalU8(uInt32 port, uInt8 data);


void closeChannels();

void stop_mg_server(void);

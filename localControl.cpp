#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <interface.h>

// uncomment later:
#include <localControl.h>

using namespace std;

void initializeHardwarePorts() {
	// INPUT PORTS
	createDigitalInput(0);
	createDigitalInput(1);
	createDigitalInput(2);
	createDigitalInput(3);

	// OUTPUT PORTS
	createDigitalOutput(4);
	createDigitalOutput(5);

	writeDigitalU8(4, 0);		// porta, dados a escrever
	writeDigitalU8(5, 0);
}

void setBitValue(uInt8& variable, int n_bit, int new_value_bit) {
	// given a byte value, set the n bit to value
	uInt8 mask_on = (uInt8)(1 << n_bit);
	uInt8 mask_off = ~mask_on;

	if (new_value_bit)
		variable |= mask_on;
	else
		variable &= mask_off;
}

int getBitValue(uInt8 value, uInt8 n_bit) {
	// given a byte value, returns the value of bit n
	return (value & (1 << n_bit));
}

void moveXRight() {
	uInt8 p = readDigitalU8(4);
	setBitValue(p, 1, 0);
	setBitValue(p, 0, 1);
	writeDigitalU8(4, p);
}

// bit map: 7 6 5 4 3 2 1 0
void moveXLeft() {
	uInt8 p = readDigitalU8(4);
	setBitValue(p, 0, 0);
	setBitValue(p, 1, 1);
	writeDigitalU8(4, p);
}

void stopX() {
	uInt8 p = readDigitalU8(4);
	setBitValue(p, 0, 0);
	setBitValue(p, 1, 0);
	writeDigitalU8(4, p);
}




// ----------------------- Y controls -----------------------
//	bit organization logic: XRightXLeftXX YYZZ
void moveYInside()
{
	uInt8 p = readDigitalU8(4);
	setBitValue(p, 3, 0);
	setBitValue(p, 4, 1);
	writeDigitalU8(4, p);
}

void moveYOutside()
{
	uInt8 p = readDigitalU8(4);
	setBitValue(p, 3, 1);
	setBitValue(p, 4, 0);
	writeDigitalU8(4, p);
}
void stopY() {
	uInt8 p = readDigitalU8(4);
	setBitValue(p, 3, 0);
	setBitValue(p, 4, 0);
	writeDigitalU8(4, p);
}

// ----------------------- Z controls -----------------------
void moveZUp()
{
	uInt8 p = readDigitalU8(4);
	setBitValue(p, 5, 1);
	setBitValue(p, 6, 0);
	writeDigitalU8(4, p);
}

void moveZDown()
{
	uInt8 p = readDigitalU8(4);
	setBitValue(p, 5, 0);
	setBitValue(p, 6, 1);
	writeDigitalU8(4, p);
}
void stopZ() {
	uInt8 p = readDigitalU8(4);
	setBitValue(p, 5, 0);
	setBitValue(p, 6, 0);
	writeDigitalU8(4, p);
}

int getXPosition() {
	int pp[10] = { 0,0,0,0,0,0,0,0,1,1 };
	int bb[10] = { 0,1,2,3,4,5,6,7,0,1 };
	int ports[2];

	ports[0] = readDigitalU8(0);
	ports[1] = readDigitalU8(1);

	for (int i = 0; i < 10; i++) {
		if (!getBitValue(ports[pp[i]], bb[i]))
			return i + 1;
	}

	return (-1);
}

int getYPosition()
{

	return 0;
}

int getXDirection() {
	uInt8 p4 = readDigitalU8(4);
	if (getBitValue(p4, 0)) // moving right?
		return 1;
	if (getBitValue(p4, 1)) // moving left?
		return -1;
	return 0; // not moving.
}

int getYDirection()
{
	uInt8 p4 = readDigitalU8(4);
	if (getBitValue(p4, 4)) // moving inside the storage?
		return 1;
	if (getBitValue(p4, 3)) // moving outside the storage?
		return -1;
	return 0; // not moving.
}

int getZDirection()
{
	uInt8 p4 = readDigitalU8(4);
	if (getBitValue(p4, 5)) // moving up the storage?
		return 1;
	if (getBitValue(p4, 6)) // moving down the storage?
		return -1;
	return 0; // not moving.
}

float getZPosition() {
	
	return 0;
}


int getLeftStationDirection()
{
	uInt8 p4 = readDigitalU8(4);
	uInt8 p5 = readDigitalU8(5);
	if (getBitValue(p4, 7)) // moving inside the storage?
		return 1;
	if (getBitValue(p5, 0)) // moving outside the storage?
		return -1;
	return 0; // not moving.
}

int getRightStationDirection()
{
	uInt8 p5 = readDigitalU8(5);
	if (getBitValue(p5, 1)) // moving inside the storage?
		return 1;
	if (getBitValue(p5, 2)) // moving outside the storage?
		return -1;
	return 0; // not moving.
}

void showPortsInformation() {
	for (int portNumber = 0; portNumber < 6; portNumber++) {
		uInt8 portValue = readDigitalU8(portNumber);
		printf("\nport(%d)=%02X : ", portNumber, portValue);

		for (int bitNumber = 0; bitNumber < 8; bitNumber++) {
			bool bitValue = portValue & (1 << (7 - bitNumber));
			if ((bitNumber % 4) == 0)
				putchar(' ');
			printf("%d", (int)bitValue);
		}
	}
	printf("\n");
}

void moveLeftStationInside()
{
	uInt8 p4 = readDigitalU8(4);
	uInt8 p5 = readDigitalU8(5);
	setBitValue(p4, 7, 1);
	setBitValue(p5, 0, 0);
	writeDigitalU8(4, p4);
	writeDigitalU8(5, p5);
}

void moveLeftStationOutside()
{
	uInt8 p4 = readDigitalU8(4);
	uInt8 p5 = readDigitalU8(5);
	setBitValue(p4, 7, 0);
	setBitValue(p5, 0, 1);
	writeDigitalU8(4, p4);
	writeDigitalU8(5, p5);
}

void stopLeftStation()
{
	uInt8 p4 = readDigitalU8(4);
	uInt8 p5 = readDigitalU8(5);
	setBitValue(p4, 7, 0);
	setBitValue(p5, 0, 0);
	writeDigitalU8(4, p4);
	writeDigitalU8(5, p5);
}

void moveRightStationInside()
{
	uInt8 p5 = readDigitalU8(5);
	setBitValue(p5, 1, 1);
	setBitValue(p5, 2, 0);
	writeDigitalU8(5, p5);
}

void moveRightStationOutside()
{
	uInt8 p5 = readDigitalU8(5);
	setBitValue(p5, 1, 0);
	setBitValue(p5, 2, 1);
	writeDigitalU8(5, p5);
}

void stopRightStation()
{
	uInt8 p5 = readDigitalU8(5);
	setBitValue(p5, 1, 0);
	setBitValue(p5, 2, 0);
	writeDigitalU8(5, p5);
}


// P3 bit 7 changes whenever a package passes the sensor
int isPartOnLeftStation()
{
	uInt8 p3 = readDigitalU8(3);		// reading port 3
	if (getBitValue(p3, 0))				// bit active?
		return 1;
	return 0;
}

// P3 bit 6 changes whenever a package passes the sensor
int isPartOnRightStation()
{
	uInt8 p3 = readDigitalU8(3);		// reading port 3
	int x = getBitValue(p3, 1);
	if (getBitValue(p3, 1))				// bit active?
		return 1;
	return 0;
}

// P1 bit 3 changes whenever a package passes the sensor
bool isPartInCage()
{
	uInt8 p1 = readDigitalU8(3);		// reading port 1
	if (!getBitValue(p1, 3))				// bit active?
		return true;
	return false;
}


void showStates() {
	printf("\nX:\t\t position(%d), direction(%d) ", getXPosition(), getXDirection());
	
	printf("\nZ:\t\t position(%d), direction(%d) ", getZPosition(), getZDirection());
	printf("\nY:\t\t position(%d), direction(%d) ", getYPosition(), getYDirection());

	printf("\nleftStation..:\t has_part(%d), direction(%d) ", isPartOnLeftStation(), getLeftStationDirection());
	printf("\nrightStation.:\t has_part(%d), direction(%d) ", isPartOnRightStation(), getRightStationDirection());
	printf("\nCAGE:\t\t 1(%d)", isPartInCage());
}

void showLocalMenu() {
	cout << endl;
	cout << "x_movement.....: q, a, z" << endl;
	cout << "z_movement.....: w, s, x" << endl;
	cout << "y_movement.....: z, x, c" << endl;
	cout << "left_station...: r, f, v" << endl;
	cout << "right_station..: t, g, b" << endl;
	cout << "show Port bits.: p" << endl;
	cout << "show states....: k" << endl;
	cout << "show menu......: m" << endl;
	cout << "END ...........: ESC" << endl;
}

void executeLocalControl(int keyboard) {
	switch (keyboard) {
		// X axis control
	case 'q': moveXRight(); break;
	case 'z': moveXLeft(); break;
	case 'a': stopX(); break;

		// Y axis control
	case 'e': moveYInside(); break;
	case 'c': moveYOutside(); break;
	case 'd': stopY(); break;

		// Z axis control
	case 'w': moveZUp(); break;
	case 'x': moveZDown(); break;
	case 's': stopZ(); break;

	case 'r': moveLeftStationInside(); break;
	case 'v': moveLeftStationOutside(); break;
	case 'f': stopLeftStation(); break;
	case 't': moveRightStationInside(); break;
	case 'b': moveRightStationOutside(); break;
	case 'g': stopRightStation(); break;

	case 'p': showPortsInformation(); break;
	case 'm': showLocalMenu(); break;
	case 'k': showStates(); break;
	default: break;
	}
}




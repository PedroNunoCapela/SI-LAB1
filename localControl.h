#pragma once

#include <interface.h>

// Function declarations
void setBitValue(uInt8* variable, int n_bit, int new_value_bit);
int getBitValue(uInt8 value, uInt8 n_bit);

int getXPosition();  // returns: 1, 2, 3, ... 10, -1 (not in a position)
int getXDirection(); // returns: -1 (moving left), 0 (stopped), or 1 (moving right)

void stopX();
void moveXLeft();
void moveXRight();

int getYPosition();  // returns: 1, 2, 3 or -1 (not in a position)
void moveYInside();
void moveYOutside();
void stopY();
int getYDirection(); // returns: -1 (moving outside the storage), 0 (stopped), or 1 (moving inside the storage)

float getZPosition();  // returns: 1, 1.5, 2, 2.5, ..., 5.5, or -1 (not in a position)
void moveZUp();
void moveZDown();
void stopZ();
int getZDirection(); // returns: -1 (moving down), 0 (stopped), or 1 (moving up)

void moveLeftStationInside();
void moveLeftStationOutside();
void stopLeftStation();
int getLeftStationDirection(); // returns: -1 (moving outside), 0 (stopped), or 1 (moving inside)

void moveRightStationInside();
void moveRightStationOutside();
void stopRightStation();
int getRightStationDirection(); // returns: -1 (moving outside), 0 (stopped), or 1 (moving inside)

int isPartOnLeftStation();
int isPartOnRightStation();
bool isAtZUp();
bool isAtZDown();
bool isPartInCage();

void executeLocalControl(int keyboard);
void showLocalMenu();

void initializeHardwarePorts();

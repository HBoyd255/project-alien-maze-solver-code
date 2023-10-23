
#include <Arduino.h>

#include "bleControl.h"
#include "bumperControl.h"

#define GPIO1 A7
#define GPIO2 A6
#define GPIO3 A3
#define GPIO4 A2

#define DATA GPIO1
#define LOAD GPIO2
#define CLOCK GPIO3

Bumper bumper1 = Bumper(DATA, LOAD, CLOCK);

void setup() {
    Serial.begin(9600);    

    setupBle();
}

void loop() {    
    bleUpdateBumper(bumper1.getInstant());
    blePoll();
}

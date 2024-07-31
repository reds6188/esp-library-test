#ifndef SENSORS_H_
#define SENSORS_H_

#include <Wire.h>
#include <Adafruit_BME280.h>
#include <console.h>
#include <timer.h>
#include "def.h"
#include "iot.h"

#define SENS_T	"SENSOR"

typedef struct {
	float current;
	float min;
	float max;
}weather_t;

double roundFloat(double value);
void initSensors(void);
void readSensors(void);

#endif  /* SENSORS_H_ */
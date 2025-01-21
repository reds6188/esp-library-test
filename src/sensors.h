#ifndef SENSORS_H_
#define SENSORS_H_

#include <Wire.h>
#include <Adafruit_BME280.h>
#include <console.h>
#include <timer.h>
#include "def.h"
#include "iot.h"
#include "t5_variables.h"

#define SENS_T	"SENSOR"

#define TEMP_BASE		24
#define TEMP_OFFSET		10
#define HUM_BASE		60
#define HUM_OFFSET		4
#define PRESS_BASE		1015
#define PRESS_OFFSET	15

typedef struct {
	float current;
	float min;
	float max;
}weather_t;

double roundFloat(double value);
void fakeSensor(void);
void checkMinMax(weather_t * value);
void initSensors(void);
void readSensors(void);
void sendSensorData(void);

#endif  /* SENSORS_H_ */
#include "sensors.h"

TwoWire Wire0(0);
Adafruit_BME280 bme;
Timer TimerSensor;
weather_t temperature, pressure, humidity;
bool sens_enabled;

double roundFloat(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}

void fakeSensor(void) {
	// Read temperature
	temperature.current = roundFloat(TEMP_BASE + (rand() % (TEMP_OFFSET + 1))/10);
	checkMinMax(&temperature);

	// Read humidity
	humidity.current = roundFloat(HUM_BASE + (rand() % (HUM_OFFSET + 1)));
	checkMinMax(&humidity);

	// Read pressure
	pressure.current = roundFloat(PRESS_BASE + (rand() % (PRESS_OFFSET + 1)));
	checkMinMax(&pressure);
}

void checkMinMax(weather_t * value) {
	if(value->current < value->min)
		value->min = value->current;
	else if(value->current > value->max)
		value->max = value->current;
}

void initSensors(void) {
	Wire0.setPins(I2C_SDA, I2C_SCL);
	if(bme.begin(0x76, &Wire0)) {
		console.success(SENS_T, "BME280 sensor was initialized");

		// Initialize values
		temperature.current = bme.readTemperature();
		temperature.min = temperature.current;
		temperature.max = temperature.current;

		pressure.current = bme.readPressure() / 100.0F;
		pressure.min = pressure.current;
		pressure.max = pressure.current;

		humidity.current = bme.readHumidity();
		humidity.min = humidity.current;
		humidity.max = humidity.current;

		sens_enabled = true;
	}
	else
		console.error(SENS_T, "Error on BME280 sensor initialization");

	// Initialize Things5 variables
	Thing.defMetric("temperature", METRICS_FLT);
	Thing.defMetric("humidity", METRICS_FLT);
	Thing.defMetric("pressure", METRICS_FLT);
}

void readSensors(void) {
	if(TimerSensor.elapsedX1s(5)) {
		TimerSensor.trigger();

		String sens_str;
		
		if(sens_enabled) {
			// Read temperature
			temperature.current = roundFloat(bme.readTemperature());
			checkMinMax(&temperature);
			sens_str += "Temp. : " + String(temperature.current) + " °C ; ";

			// Read humidity
			humidity.current = roundFloat(bme.readHumidity());
			checkMinMax(&humidity);
			sens_str += "Humidity : " + String(humidity.current) + " % ; ";

			// Read pressure
			pressure.current = roundFloat(bme.readPressure() / 100.0F);
			checkMinMax(&pressure);
			sens_str += "Pressure : " + String(pressure.current) + " hPa";

			#ifdef LOG_SENSOR
			console.info(SENS_T, sens_str);
			#endif
		}
		else {
			fakeSensor();
		}

		if(mqtt_enabled)
			sendSensorData();
	}
}

void sendSensorData(void) {
	// Update Things5
	unsigned long long timestamp = (unsigned long long)getTimestampNtp() * 1000;
	#ifdef LOG_SENSOR
	console.info(SENS_T, "Timestamp = " + String(timestamp));
	#endif
	Thing.createMessage(timestamp);
	Thing.updateMetric(T5_TEMPERATURE, temperature.current);
	Thing.updateMetric(T5_HUMIDITY, humidity.current);
	Thing.updateMetric(T5_PRESSURE, pressure.current);
	if(Thing.isEmptyMessage())
		Thing.deleteMessage();
	else {
		char new_topic[TOPIC_LENGTH];
		getFullT5Topic(new_topic, DATA_INGESTION_REQ);
		publishMqtt(new_topic, Thing.getPayload());
	}
	//time_t now = getTimestampNtp();
}
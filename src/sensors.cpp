#include "sensors.h"

TwoWire Wire0(0);
Adafruit_BME280 bme;
Timer TimerSensor;
weather_t temperature, pressure, humidity;

double roundFloat(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}


void initSensors(void) {
	Wire0.setPins(I2C_SDA, I2C_SCL);
	if(bme.begin(0x76, &Wire0))
		console.success(SENS_T, "BME280 sensor was initialized");
	else
		console.error(SENS_T, "Error on BME280 sensor initialization");

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

	// Initialize Things5 variables
	Thing.defMetric("temperature", METRICS_FLT);
	Thing.defMetric("humidity", METRICS_FLT);
	Thing.defMetric("pressure", METRICS_FLT);
}

void readSensors(void) {
	if(TimerSensor.elapsedX1s(5)) {
		TimerSensor.trigger();

		String sens_str;
		
		// Read temperature
		temperature.current = roundFloat(bme.readTemperature());
		if(temperature.current < temperature.min)
			temperature.min = temperature.current;
		else if(temperature.current > temperature.max)
			temperature.max = temperature.current;
		sens_str += "Temp. : " + String(temperature.current) + " °C ; ";

		// Read humidity
		humidity.current = roundFloat(bme.readHumidity());
		if(humidity.current < humidity.min)
			humidity.min = humidity.current;
		else if(humidity.current > humidity.max)
			humidity.max = humidity.current;
		sens_str += "Humidity : " + String(humidity.current) + " % ; ";

		// Read pressure
		pressure.current = roundFloat(bme.readPressure() / 100.0F);
		if(pressure.current < pressure.min)
			pressure.min = pressure.current;
		else if(pressure.current > pressure.max)
			pressure.max = pressure.current;
		sens_str += "Pressure : " + String(pressure.current) + " hPa";

		//console.info(SENS_T, sens_str);

		// Update Things5
		unsigned long long timestamp = (unsigned long long)getTimestampNtp() * 1000;
		//console.info(SENS_T, "TImestamp = " + String(timestamp));
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
}
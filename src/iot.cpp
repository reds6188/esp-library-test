#include "iot.h"

char thing5_url[31] = "things5-production/v1/devices/";
char * device_id;

char* getFileAsString(fs::FS &fs, String path) {
	if(fs.exists(path)) {
		File file = fs.open(path, FILE_READ);
		if(!file) {
			console.error(T5_T, "Failed to open " + path);
			return NULL;
		}
		char* buffer = (char*)calloc(file.size() + 1, sizeof(char));
		file.readBytes(buffer, file.size());
		file.close();
		return buffer;
	}

	console.error(T5_T, "File \"" + path + "\" does not exists!");
	return NULL;
}

void getFullT5Topic(char * topic, const char * short_topic) {
	char * url = (char*)calloc(strlen(thing5_url) + strlen(device_id) + strlen(short_topic) + 1, sizeof(char));
	strcpy(url, thing5_url);
	strcat(url, device_id);
	strcat(url, short_topic);
	console.log(T5_T, "Str = \"" + String(url) + "\"");
	strcpy(topic, url);
	free(url);
}

void initMqttT5(void) {
	fs::SPIFFSFS CERT;

	// Mount CERT partition -----------------------------------------------------------------------
	if(!CERT.begin(false, "/spiffs", 4, "spiffs"))
        console.error(MQTT_T, "An Error has occurred while mounting CERT partition");
	else
		console.success(MQTT_T, "CERT partition has mounted");

	// Read "/device.ca.pem" file ---------------------------------------------
	device_id = getFileAsString(CERT, PATH_DEVICE_ID);
	const char * cacert_str = getFileAsString(CERT, PATH_DEVICE_CA);
	const char * cert_str = getFileAsString(CERT, PATH_DEVICE_CERTIFICATE);
	const char * private_str = getFileAsString(CERT, PATH_DEVICE_PRV_KEY);
	CERT.end();

	char client_id[64] = "things5-production-machine-";
	strncat(client_id, device_id, strlen(device_id));

	console.log(T5_T, "Device ID = \"" + String(device_id) + "\"");
	console.log(T5_T, "Client ID = \"" + String(client_id) + "\"");

	const char * topic[6] = {
		READ_VAR_REQ,
		WRITE_VAR_REQ,
		LOG_INGESTION_RES,
		UPDATE_FIRMWARE_REQ,
		FIRMWARE_LIST_RES,
		FIRMWARE_DETAILS_RES,
	};
	for(uint8_t i=0 ; i<6 ; i++) {
		char new_topic[TOPIC_LENGTH];
		getFullT5Topic(new_topic, topic[i]);
		mqttAddTopic((const char *)new_topic);
	}

	initMqttClient(T5_BROKER_URL, client_id, cacert_str, cert_str, private_str);
}
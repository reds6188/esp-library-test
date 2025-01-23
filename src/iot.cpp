#include "iot.h"

Things5 Thing(false);
Timer TimerMqtt;

bool mqtt_enabled;

char thing5_url[31] = "things5-production/v1/devices/";
char * device_id;

void ntp_callback(timeval *tv) {
	console.log(T5_T, "Got it");
	Thing.enableTimestamp(true);
};

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
	//console.log(T5_T, "Str = \"" + String(url) + "\"");
	strcpy(topic, url);
	free(url);
}

bool initMqttT5(void) {
	fs::SPIFFSFS CERT;

	initNtpClient();
	onNtpTimeAvailable(ntp_callback);

	// Mount CERT partition -----------------------------------------------------------------------
	if(!CERT.begin(false, "/cert", 4, "cert"))
        console.error(MQTT_T, "An Error has occurred while mounting CERT partition");
	else
		console.success(MQTT_T, "CERT partition has mounted");

	// Read "/device.ca.pem" file ---------------------------------------------
	device_id = getFileAsString(CERT, PATH_DEVICE_ID);
	const char * cacert_str = getFileAsString(CERT, PATH_DEVICE_CA);
	const char * cert_str = getFileAsString(CERT, PATH_DEVICE_CERTIFICATE);
	const char * private_str = getFileAsString(CERT, PATH_DEVICE_PRV_KEY);
	CERT.end();

	if(!device_id || !cacert_str || !cert_str || !private_str) {
		console.error(T5_T, "Failed to setup MQTT: missing device ID or certificates");
		return false;
	}

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

	onMqttConnect(mqtt_on_connect);
	initMqttClient(T5_BROKER_URL, client_id, cacert_str, cert_str, private_str);

	return true;
}

bool restartFlag = true;

String getResetReason(void) {
    esp_reset_reason_t rst_reason = esp_reset_reason();
    String reason;

    switch(rst_reason) {
        case ESP_RST_UNKNOWN:   reason = "Reset reason can not be determined";                     break;
        case ESP_RST_POWERON:   reason = "Reset due to power-on event";                            break;
        case ESP_RST_EXT:       reason = "Reset by external pin (not applicable for ESP32)";       break;
        case ESP_RST_SW:        reason = "Software reset via esp_restart";                         break;
        case ESP_RST_PANIC:     reason = "Software reset due to exception/panic";                  break;
        case ESP_RST_INT_WDT:   reason = "Reset (software or hardware) due to interrupt watchdog"; break;
        case ESP_RST_TASK_WDT:  reason = "Reset due to task watchdog";                             break;
        case ESP_RST_WDT:       reason = "Reset due to other watchdogs";                           break;
        case ESP_RST_DEEPSLEEP: reason = "Reset after exiting deep sleep mode";                    break;
        case ESP_RST_BROWNOUT:  reason = "Brownout reset (software or hardware)";                  break;
        case ESP_RST_SDIO:      reason = "Reset over SDIO";                                        break;
    }

    return reason;
}

void printResetReason(void) {
	console.info(IOT_T, getResetReason());
}

void mqtt_on_connect(void) {
	char new_topic[TOPIC_LENGTH];
	if(restartFlag) {
		restartFlag = false;

		// Send Firmware Version ----------------------------------------------
		Thing.setUUID();
		Thing.setProperty("version", String(VERSION));
		getFullT5Topic(new_topic, FIRMWARE_VERSION_REQ);
		publishMqtt(new_topic, Thing.getPayload());

		// Send event "Info Gateway Restart" ----------------------------------
		Thing.createMessage();
		esp_reset_reason_t rst_reason = esp_reset_reason();
		if(rst_reason == ESP_RST_POWERON)
			Thing.addEvent(T5_INFO_POWER_ON);
		else if(rst_reason == ESP_RST_SW)
			Thing.addEvent(T5_INFO_SW_RESET);
		else
			Thing.addEvent(T5_WRN_RESET, getResetReason());

		// Send all data ---------------------
		//Thing.writeAllMetrics();
		getFullT5Topic(new_topic, DATA_INGESTION_REQ);
		publishMqtt(new_topic, Thing.getPayload());
		console.info(IOT_T, "System was restarted");
		printResetReason();
	}
	else {
		unsigned long long timestamp = (unsigned long long)getTimestampNtp() * 1000;
		Thing.createMessage(timestamp);
		Thing.addLog(timestamp, "MQTT reconnection");
		getFullT5Topic(new_topic, LOG_INGESTION_REQ);
		publishMqtt(new_topic, Thing.getPayload());
	}
}
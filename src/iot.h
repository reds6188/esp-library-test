#ifndef IOT_H_
#define IOT_H_

#include <console.h>
#include <timer.h>
#include <mqtt-client.h>
#include <things5.h>
#include "SPIFFS.h"

#define PATH_DEVICE_ID				"/device_id.txt"
#define PATH_DEVICE_CA				"/device.ca.pem"
#define PATH_DEVICE_CERTIFICATE		"/device.certificate.pem"
#define PATH_DEVICE_PRV_KEY			"/device.private.pem"
#define T5_BROKER_URL				"mqtts://mqtt.things5.digital:8883"

#define DATA_INGESTION_REQ			"/data_req"
#define DATA_INGESTION_RES			"/data_res"
#define READ_VAR_REQ				"/cmd/read_variables_req"
#define READ_VAR_RES				"/cmd/read_variables_res"
#define WRITE_VAR_REQ				"/cmd/write_variables_req"
#define WRITE_VAR_RES				"/cmd/write_variables_res"
#define UPDATE_FIRMWARE_REQ			"/cmd/update_firmware_req"
#define UPDATE_FIRMWARE_RES			"/cmd/update_firmware_res"
#define UPDATE_FIRMWARE_PROGRESS	"/cmd/update_firmware_progress"
#define FIRMWARE_VERSION_REQ		"/event/firmware_version_req"
#define FIRMWARE_LIST_REQ			"/req/firmwares_list_req"
#define FIRMWARE_LIST_RES			"/req/firmwares_list_res"
#define FIRMWARE_DETAILS_REQ		"/req/firmware_details_req"
#define FIRMWARE_DETAILS_RES		"/req/firmware_details_res"
#define LOG_INGESTION_REQ			"/logs/ingest_req"
#define LOG_INGESTION_RES			"/logs/ingest_res"

void initMqttT5(void);
void initVarT5(void);
void refreshT5(void);

#endif  /* IOT_H_ */
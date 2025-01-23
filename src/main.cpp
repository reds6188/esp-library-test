#include "main.h"
#include "credentials.h"

Timer TimerBlink2, TimerText;
Uart uart_232(UART_232);
#ifdef DEMOBOARD_V1
Led Led1(GRN_LED);
Led Led2(YLW_LED);
Led Led3(RED_LED, true);
Button Btn1(BUTTON_1);
Button Btn2(BUTTON_2);
#else
Led Led1(LED_1);
Led Led2(LED_2);
Led Led3(LED_3, true);
Button Btn1(P1);
Button Btn2(P2);
#endif

WiFiHandler wifi_handler("esp-test");

uint8_t text_counter;
const char * str = "Hello World!";

uart_pin_t UART_232_pin {
	.tx_pin = UART_232_TX,
	.rx_pin = UART_232_RX,
	.rts_pin = UART_PIN_NO_CHANGE,
	.cts_pin = UART_PIN_NO_CHANGE,
	.en_pin = UART_PIN_NO_CHANGE,
};

uart_config_t UART_232_config = {
	.baud_rate = 9600,
	.data_bits = UART_DATA_8_BITS,
	.parity = UART_PARITY_EVEN,
	.stop_bits = UART_STOP_BITS_1,
	.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	.rx_flow_ctrl_thresh = 122,
};

void uart_rx_callback (uint8_t *data, int data_size) {
	digitalWrite(LED_3, !digitalRead(LED_3));
}

void setLed3(void) {
	Led3.set();
}

void resetLed3(void) {
	Led3.reset();
}

uint8_t wifi_flag;

void setCredentials(void) {
	if(wifi_flag < 2)
		wifi_flag++;
	else
		wifi_flag = 0;
	
	if(wifi_flag == 0) {
		wifi_handler.setCredentials(test_wifi_ssid, "1234567");
		console.info(MAIN_T, ">>> TEST <<< Set credentials of existing network with WRONG password");
	}
	else if(wifi_flag == 1) {
		wifi_handler.setCredentials(test_wifi_ssid, test_wifi_password);
		console.info(MAIN_T, ">>> TEST <<< Set credentials of existing network with CORRECT PASSWORD");
	}
	else if(wifi_flag == 2) {
		//wifi_handler.setCredentials(default_wifi_ssid, default_wifi_password);
		//console.info(MAIN_T, ">>> TEST <<< Set credentials of NOT existing network");
		wifi_handler.setCredentials(my_ssid, my_password);
	}
		
	console.log(MAIN_T, "Credentials was set");
	wifi_handler.reconnect();
	console.info(MAIN_T, "SSID = \"" + wifi_handler.getSSID() + "\"");
}

void espRestart(void) {
	console.warning(MAIN_T, "Restarting...");
	wifi_handler.disconnect();
	delay(1000);
	ESP.restart();
}

void testPanicError(void) {
	console.println("PANIC!!!");
	char * pntr;
	console.log(MAIN_T, String(strlen(pntr)));
}

String api_restart_cb(uint8_t * payload) {
	DynamicJsonDocument doc(16);
	doc["status"] = "done";
	String msg;
	serializeJson(doc, msg);
	return msg;
}

String api_version_cb(uint8_t * payload) {
	DynamicJsonDocument doc(256);
	doc["version"] = String(VERSION);
	doc["date"] = String(__DATE__);
	doc["ssid"] = wifi_handler.getSSID();
	doc["rssi"] = wifi_handler.getRSSI();
	doc["mac_ap"] = wifi_handler.getMacAddress(WIFI_IF_AP);
	doc["mac_sta"] = wifi_handler.getMacAddress(WIFI_IF_STA);
	doc["id"] = device_id;
	String msg;
	serializeJson(doc, msg);
	return msg;
}

String api_ping_cb(uint8_t * payload) {
	DynamicJsonDocument doc(64);
	doc["status"] = "ok";
	doc["rssi"] = wifi_handler.getRSSI();
	String msg;
	serializeJson(doc, msg);
	return msg;
}

String api_scan_start_cb(uint8_t * payload) {
	DynamicJsonDocument doc(64);
		wifi_handler.startScanNetworks();
		/*
		if(!wifi_handler.connected()) {
			wifi_handler.end();
			console.warning(WIFI_T, "Forcing disconnection to avoid scan failure");
		}
		*/
	doc["status"] = "progress";
	String msg;
	serializeJson(doc, msg);
	return msg;
}

String api_scan_status_cb(uint8_t * payload) {
	String msg;
	return msg;
}


String api_rest_callback(String uri, uint8_t * payload) {
	DynamicJsonDocument doc(1536);
	DynamicJsonDocument rxDoc(384);

	if(uri.equals("/status")) {
		doc["version"] = String(VERSION);
		doc["date"] = String(__DATE__);
	}
	else {
		doc["info"] = "Command not recognized";
	}

	String msg;
	serializeJson(doc, msg);
	return msg;
}

void setup() {
	console.header(DOUBLE_DASHED, LOG_BLUE, 80, "START INITIALIZATION");
	console.info(MAIN_T, "Firwmare version: " + String(VERSION));
	Led1.startBlink(500,500);
	Led3.reset();
	//Btn1.onPress(espRestart);
	//Btn1.onPress(setCredentials);
	Btn1.onPress(testPanicError);
	Btn2.onPress([]() { wifi_handler.startScanNetworks(); });
	pinMode(UART_232_EN, OUTPUT);
	digitalWrite(UART_232_EN, HIGH);
	initSensors();
	wifi_handler.begin(WIFI_STA);
	//wifi_handler.setCredentials(my_ssid, my_password);
	wifi_handler.setCredentials(test_wifi_ssid, test_wifi_password);
	mqtt_enabled = initMqttT5();
	initWebServer(api_rest_callback);
	addGetCallback("/version", api_version_cb);
	addGetCallback("/ping", api_ping_cb);
	addGetCallback("/scan-start", api_scan_start_cb);
	addGetCallback("/scan-status", api_scan_status_cb);
	//wifi_handler.onConnect(initMqttT5);
	uart_232.begin(UART_232_pin, UART_232_config);
	uart_232.setHandler(uart_rx_callback);
	console.header(DASHED, LOG_GREN, 80, "END INITIALIZATION");
}

void loop() {
	Led1.loop();
	Btn1.loop();
	Btn2.loop();
	readSensors();

	/*
	if(TimerBlink2.elapsedX100ms(2)) {
		TimerBlink2.trigger();
		uart_232.writeData((uint8_t *)str, strlen(str));
	}
	*/

	if(TimerText.elapsedX1s(2)) {
		TimerText.trigger();
	}
}
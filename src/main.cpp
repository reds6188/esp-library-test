#include "main.h"
#include "credentials.h"

Timer TimerBlink2, TimerText;
Uart uart_232(UART_232);
Led Led1(LED_1);
Led Led2(LED_2);
Led Led3(LED_3, true);

WiFiHandler wifi_handler("esp-test");

Button Btn1(P1);
Button Btn2(P2);

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
		wifi_handler.setCredentials(default_wifi_ssid, default_wifi_password);
		console.info(MAIN_T, ">>> TEST <<< Set credentials of NOT existing network");
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

void setup() {
	console.header(DOUBLE_DASHED, LOG_BLUE, 80, "START INITIALIZATION");
	Led1.startBlink(500,500);
	Led3.reset();
	Btn1.onPress(setCredentials);
	Btn2.onPress(
		[]() {
			wifi_handler.startScanNetworks();
		}
	);
	pinMode(UART_232_EN, OUTPUT);
	digitalWrite(UART_232_EN, HIGH);
	wifi_handler.begin(WIFI_STA);
	uart_232.begin(UART_232_pin, UART_232_config);
	uart_232.setHandler(uart_rx_callback);
	console.header(DASHED, LOG_GREN, 80, "END INITIALIZATION");
}

void loop() {
	Led1.loop();
	Btn1.loop();
	Btn2.loop();

	if(TimerBlink2.elapsedX100ms(2)) {
		TimerBlink2.trigger();
		Led2.toggle();
		uart_232.writeData((uint8_t *)str, strlen(str));
	}

	if(TimerText.elapsedX1s(2)) {
		TimerText.trigger();
		if(text_counter < 4)
			text_counter++;
		else
			text_counter = 0;
		/*
		console.line(DOUBLE_DASHED, 80);
		switch(text_counter) {
			case 0: console.log(MAIN_T, "This text is normal");			break;
			case 1: console.info(MAIN_T, "Information message");		break;
			case 2: console.success(MAIN_T, "OK messagge");				break;
			case 3: console.warning(MAIN_T, "Warning messagge");		break;
			case 4: console.error(MAIN_T, "Error message");				break;
		}
		*/
	}
}
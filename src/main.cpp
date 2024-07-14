#include "main.h"

Timer TimerBlink1, TimerBlink2;
Uart uart_232(UART_232);
Led Led1(LED_1);
Led Led2(LED_2);

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

void setup() {
	Serial.begin(115200);
	TimerBlink1.trigger();
	Led1.startBlink(100,900);
	pinMode(LED_3, OUTPUT);
	pinMode(UART_232_EN, OUTPUT);
	digitalWrite(UART_232_EN, HIGH);
	uart_232.begin(UART_232_pin, UART_232_config);
	uart_232.setHandler(uart_rx_callback);
}

void loop() {
	Led1.loop();

	if(TimerBlink2.elapsedX100ms(2)) {
		TimerBlink2.trigger();
		Led2.toggle();
		uart_232.writeData((uint8_t *)str, strlen(str));
	}
}
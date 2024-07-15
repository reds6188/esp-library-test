#include "main.h"

Timer TimerBlink2, TimerText;
Uart uart_232(UART_232);
Led Led1(LED_1);
Led Led2(LED_2);
Led Led3(LED_3, true);

Button Btn1(P1);

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

void setup() {
	console.header(DOUBLE_DASHED, LOG_BLUE, 80, "START INITIALIZATION");
	Led1.startBlink(100,900);
	Led3.reset();
	Btn1.onPress(setLed3);
	Btn1.onRelease(resetLed3);
	pinMode(UART_232_EN, OUTPUT);
	digitalWrite(UART_232_EN, HIGH);
	uart_232.begin(UART_232_pin, UART_232_config);
	uart_232.setHandler(uart_rx_callback);
	console.header(DASHED, LOG_GREN, 80, "END INITIALIZATION");
}

void loop() {
	Led1.loop();
	Btn1.loop();

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

		console.line(DOUBLE_DASHED, 80);
		switch(text_counter) {
			case 0: console.log(MAIN_T, "This text is normal");			break;
			case 1: console.info(MAIN_T, "Information message");		break;
			case 2: console.success(MAIN_T, "OK messagge");				break;
			case 3: console.warning(MAIN_T, "Warning messagge");		break;
			case 4: console.error(MAIN_T, "Error message");				break;
		}
	}
}
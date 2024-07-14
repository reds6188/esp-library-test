#ifndef DEF_H_
#define DEF_H_

// ================================================================================================
// Pin definitions for ESP32 Proto BOard V2 (created by Daniele Rossi) ============================
// ================================================================================================

#define BUTTON_1    34
#define BUTTON_2    35

#define RED_LED     27
#define GRN_LED     14
#define BLU_LED     26

// ================================================================================================
// Pin definitions for CS005350 ===================================================================
// ================================================================================================

// Pushbutton -------------------------
#define BTN_1       35
#define BTN_2       34

// LED --------------------------------
#define LED_1		14
#define LED_2		15
#define LED_3		2

// ADC --------------------------------
#define ADC2_PIN    36	// Sens_VP
#define ADC1_PIN    39	// Sens_VN

// Dipswitch / Jumper -----------------
#define DIP1		13
#define DIP2		12
#define DIP3		27
#define DIP4		26
#define JUMPER      23

// RS-232 -----------------------------
#define UART_232		UART_NUM_2
#define UART_232_TX     17
#define UART_232_RX     16
#define UART_232_EN     4

// RS-485 -----------------------------
#define UART_485		UART_NUM_1
#define UART_485_TX     18
#define UART_485_RX     5
#define UART_485_EN     19

#endif  /* DEF_H_ */
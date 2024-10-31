#ifndef T5_VARIABLES_H_
#define T5_VARIABLES_H_

// WARNING: Limit variable name to 20 characters 

// METRICS ------------------------------------------------
static const char * T5_TEMPERATURE =    "temperature";      // integer
static const char * T5_HUMIDITY =       "humidity";         // integer
static const char * T5_PRESSURE =       "pressure";         // integer
static const char * T5_FREE_HEAP =      "dbg_free_heap";    // integer (only for debug)

// EVENTS -------------------------------------------------
static const char * T5_INFO_POWER_ON =  "info_power_on";
static const char * T5_INFO_SW_RESET =  "info_sw_reset";
static const char * T5_WRN_RESET =      "wrn_reset";

#endif  /* T5_VARIABLES_H_ */
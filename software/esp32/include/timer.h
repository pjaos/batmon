#ifndef SRC_INCLUDE_TIMERS_H_
#define SRC_INCLUDE_TIMERS_H_

#define TIMER1_PERIOD_MS 5000
#define STARTUP_INIT_TIMER1_PERIOD_MS 1500

#define VOLTAGE_CNV_FACTOR  140.25
#define MCP9700_CODES_PER_VOLT 16022.0
#define MCP9700_VOUT_0C        0.5
#define MCP9700_TC             0.01

#define MIN_AMPS 0.005

void  init_timers(void);
float get_temp(void);
float get_voltage(void);
float get_current(void);
char  *get_warning_message(void);
float get_last_on_charge_voltage(void);

#endif /* SRC_INCLUDE_TIMERS_H_ */

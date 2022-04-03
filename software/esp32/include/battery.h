#ifndef SRC_INCLUDE_BATTERY_H_
#define SRC_INCLUDE_BATTERY_H_

#define MIN_ID 1
#define MAX_ID 5

// 10S/36V LION battery
//#define LION_SERIES_CELL_COUNT          10
#define LION_SERIES_CELL_COUNT          7

// The maximum voltage that for a single LION CELL
#define MAX_LION_CELL_CHARGE_VOLTAGE    4.2
// The maximum charge voltage for the battery pack
#define MAX_BATTERY_PACK_VOLTAGE        LION_SERIES_CELL_COUNT * MAX_LION_CELL_CHARGE_VOLTAGE
// The charge voltage ID steps in increments of this value
// 5 = MAX_BATTERY_PACK_VOLTAGE - 4*CHARGE_LEVEL_ID_VOLTAGE_STEP_SIZE
// 4 = MAX_BATTERY_PACK_VOLTAGE - 3*CHARGE_LEVEL_ID_VOLTAGE_STEP_SIZE
// 3 = MAX_BATTERY_PACK_VOLTAGE - 2*CHARGE_LEVEL_ID_VOLTAGE_STEP_SIZE
// 2 = MAX_BATTERY_PACK_VOLTAGE - 1*CHARGE_LEVEL_ID_VOLTAGE_STEP_SIZE
// 1 = MAX_BATTERY_PACK_VOLTAGE - 0*CHARGE_LEVEL_ID_VOLTAGE_STEP_SIZE
#define CHARGE_LEVEL_ID_VOLTAGE_STEP_SIZE 0.5

int get_max_battery_charge_voltage_id(void);
void set_max_battery_charge_voltage_id(int _max_bat_charge_voltage_id);
float get_max_charge_voltage(void);
void set_battery_fully_charged(bool fully_charged);
bool is_battery_fully_charged(void);
int get_battery_cell_count(void);

#endif /* SRC_INCLUDE_BATTERY_H_ */

#include <mgos.h>

#include "logger.h"
#include "battery.h"

static bool battery_fully_charged = false;

/**
 * @return The ID number denoting the battery selected max battery charge voltage (5 - 1).
 *
 */
int get_max_battery_charge_voltage_id(void) {
    return mgos_sys_config_get_batmon_charge_level();
}

/**
 * @brief Set the maximum charge voltage that the battery should reach.
 * @param _max_bat_charge_voltage_id An ID number that defines the maximum
 *        voltage that the battery should reach when charged.
 *        5 = max battery life, min battery charge.
 *        1 = min battery life, max battery charge.
 */
void set_max_battery_charge_voltage_id(int max_bat_charge_voltage_id) {
    if ( max_bat_charge_voltage_id >= MIN_ID && max_bat_charge_voltage_id <= MAX_ID ) {
        mgos_sys_config_set_batmon_charge_level(max_bat_charge_voltage_id);
        save_cfg(&mgos_sys_config, NULL);
    }
}

/**
 * @brief Determine the maximum charge voltage for the battery.
 * @return The maximum charge voltage.
 */
float get_max_charge_voltage(void) {
    char *logger_buffer = get_logger_buffer();
    int charge_level_id = mgos_sys_config_get_batmon_charge_level();
    charge_level_id--;
    float max_charge_voltage = MAX_BATTERY_PACK_VOLTAGE - (charge_level_id*CHARGE_LEVEL_ID_VOLTAGE_STEP_SIZE);
#ifdef SHOW_MAX_CHARGE_VOLTAGE
    snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "max_charge_voltage=%.1f", max_charge_voltage);
    logger(__FUNCTION__, logger_buffer);
#endif
    return max_charge_voltage;
}

/**
 * @brief Set the state of the battery as fully charged.
 */
void set_battery_fully_charged(bool fully_charged) {
    battery_fully_charged = fully_charged;
}

/**
 * @brief Determine if the battery is fully charged.
 */
bool is_battery_fully_charged(void) {
    return battery_fully_charged;
}

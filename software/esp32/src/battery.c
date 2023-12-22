#include <mgos.h>

#include "logger.h"
#include "battery.h"

static bool battery_fully_charged = false;

#define SHOW_MAX_CHARGE_VOLTAGE 1

/**
 * @return The ID number denoting the battery selected max battery charge voltage (5 - 1).
 *
 */
int get_max_battery_charge_voltage_id(void) {
    return mgos_sys_config_get_batmon_charge_level();
}

/*
 * @brief Determine if the user has selected the charge level for storage charge.
 */
bool is_storage_charge_selected(void) {
    bool storage_charge_selected = false;
    char *logger_buffer = get_logger_buffer();

    if( mgos_sys_config_get_batmon_charge_level() <= 0 ) {
        storage_charge_selected = true;
    }
    snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "storage_charge_selected=%d", storage_charge_selected);
    logger(__FUNCTION__, logger_buffer);
    return storage_charge_selected;
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
    float max_charge_voltage = 0.0;
    char *logger_buffer = get_logger_buffer();
    int charge_level_id = mgos_sys_config_get_batmon_charge_level();
    // If set to -1 then we aim to charge the cells to a level that
    if( is_storage_charge_selected() ) {
        max_charge_voltage = CELL_STORAGE_VOLTAGE * LION_SERIES_CELL_COUNT;
    }
    else {
        charge_level_id--;
        max_charge_voltage = MAX_BATTERY_PACK_VOLTAGE - (charge_level_id*CHARGE_LEVEL_ID_VOLTAGE_STEP_SIZE);
    }
#ifdef SHOW_MAX_CHARGE_VOLTAGE
    snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "charge_level_id   =%d", charge_level_id);
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

/**
 * @brief Get the number of cells in the battery.
 */
int get_battery_cell_count(void) {
    return LION_SERIES_CELL_COUNT;
}

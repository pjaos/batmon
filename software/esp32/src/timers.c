#include <mgos.h>
#include <ads111x.h>

#include "timer.h"
#include "logger.h"
#include "gpio.h"
#include "esp_sleep.h"
#include "battery.h"

#define SHOW_TEMP_DEBUG 1
#define SHOW_VOLTS_DEBUG 1
#define SHOW_AMPS_DEBUG 1

#define READ_ADC_PERIOD_MS 1000
#define ADS111X_I2C_ADDRESS 72
#define ADC_READING_COUNT 3

#define MAX_LOGGER_BUFFER_LEN 129

#define STATUS_MSG_LENGTH 256

#define MIN_BATTERY_TEMP 0.0
#define MAX_BATTERY_TEMP 45.0

#define CHECK_CHARGE_PERIOD_SECS 3600

#define BAT_CHARGE_EVAL_SECS 10

static float tempC      = 1000000;
static float voltage    = 1000000;
static float amps       = 1000000;
static float lastOnChargeVoltage = 1000000;

static char warning_message[STATUS_MSG_LENGTH];

/**
 * @brief Get the status message.
 * @return The status message text.
 */
char *get_warning_message(void) {
    return warning_message;
}

/**
 * @brief Get the temperature in °C.
 * @return The temperature.
 */
float get_temp(void) {
    return tempC;
}

/**
 * @brief Get the voltage in volts.
 * @return The voltage.
 */
float get_voltage(void) {
    return voltage;
}
/**
 * @brief Get the battery voltage the last time it was read while charging.
 * @return The voltage.
 */
float get_last_on_charge_voltage(void) {
    return lastOnChargeVoltage;
}

/**
 * @brief Get the currunet in amps.
 * @return The current.
 */
float get_current(void) {
    return amps;
}

/**
 * @brief Callback to send periodic updates of the memory and file system state.
 **/
static void timer1_cb(void *arg) {
    char *logger_buffer = get_logger_buffer();

    if( mgos_sys_config_get_ydev_enable_syslog() ) {
      size_t heap_zize          = mgos_get_heap_size();
      size_t free_heap_size     = mgos_get_free_heap_size();
      size_t min_free_heap_size = mgos_get_min_free_heap_size();
      size_t fs_size            = mgos_get_fs_size();
      size_t fs_free_size       = mgos_get_free_fs_size();

      snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "uptime: %.2lf, heap (size/free/min) %d/%d/%d, fs (size/free) %d/%d",  mgos_uptime(), heap_zize, free_heap_size, min_free_heap_size, fs_size, fs_free_size);
      logger(__FUNCTION__, logger_buffer);
    }

    if( mgos_uptime() > CHECK_CHARGE_PERIOD_SECS ) {
        snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "Restarting to check the battery charge voltage.");
        logger(__FUNCTION__, logger_buffer);
        mgos_system_restart_after(250);
    }

    (void) arg;
}

/**
 * @brief Read ADC value. Read the ADC a number of times and return the average value.
 * @param adc The ADC to read.
 * @param fs_voltage_id The ID of the max ADC voltage.
 * @param single_ended If 1 then read the ADC in single ended mode.
 * @return
 */
static uint16_t read_adc(uint8_t adc, uint8_t fs_voltage_id, bool single_ended) {

    uint32_t adc_value_accum = 0;
    uint8_t adc_value_index;

    for( adc_value_index=0 ; adc_value_index < ADC_READING_COUNT ; adc_value_index++ ) {
        adc_value_accum += (uint32_t)get_adc_value(ADS111X_I2C_ADDRESS, adc, fs_voltage_id, SAMPLES_PER_SECOND_32, single_ended);
    }

    return (uint16_t)(adc_value_accum/ADC_READING_COUNT);
}

/**
 * @brief Callback to periodically read amps adc.
 **/
static void read_adc_amps_cb(void *arg) {
    static uint16_t amps_adc_load_off = 0;
    char *logger_buffer = get_logger_buffer();
    uint16_t amps_adc = read_adc(ADC0, FS_VOLTAGE_4_096, false);
    if ( is_load_on() ) {
        //If amps_adc_load_off value is not 0 then ww have a no current adc value.
        if ( amps_adc_load_off && amps_adc > amps_adc_load_off ) {
            uint16_t adc_delta_codes = amps_adc-amps_adc_load_off;
            float codes_per_amp = (float)mgos_sys_config_get_batmon_codes_per_amp();
            if( codes_per_amp >= 0.0 ) {
                amps = (float)adc_delta_codes / codes_per_amp;
            }
            else {
                amps = 0.0;
            }
            //Apply min amps limit as no load will result in some ADC noise.
            if( amps < MIN_AMPS) {
                amps = 0.0;
            }
    #ifdef SHOW_AMPS_DEBUG
            snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "amps_adc=0x%04x %d, amps_adc_load_off=%d, adc_delta_codes=%d, amps=%.3f", amps_adc,
                                                                                                                                      amps_adc,
                                                                                                                                      amps_adc_load_off,
                                                                                                                                      adc_delta_codes,
                                                                                                                                      amps);
            logger(__FUNCTION__, logger_buffer);
    #endif
        }
    }
    else {
        amps_adc_load_off = amps_adc;
        amps = 0.0;
        logger(__FUNCTION__, "Load OFF");
    }

    (void) arg;
}

/**
 * @brief Callback to periodically read volts adc.
 **/
static void read_adc_volts_cb(void *arg) {
    char *logger_buffer = get_logger_buffer();
    uint16_t volts_adc = read_adc(ADC2, FS_VOLTAGE_2_048, true);
    float codes_per_volt = (float)mgos_sys_config_get_batmon_codes_per_volt();
    if( codes_per_volt >= 0.0 ) {
        voltage = (float)volts_adc/codes_per_volt;
        // If still charging store the battery voltage
        if( is_load_on() ) {
            lastOnChargeVoltage = voltage;
        }
    }
    else {
        voltage = 0.0;
    }
    float max_charge_voltage = get_max_charge_voltage();
    bool max_battery_charge_voltage_id = get_max_battery_charge_voltage_id();

    snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "max_battery_charge_voltage_id=%d", max_battery_charge_voltage_id);
    logger(__FUNCTION__, logger_buffer);

#ifdef SHOW_VOLTS_DEBUG
    snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "volts_adc=0x%04x %d, voltage=%.3f, max charge voltage =%.3f, last_on_charge_voltage=%.3f", volts_adc, volts_adc, voltage, max_charge_voltage, get_last_on_charge_voltage());
    logger(__FUNCTION__, logger_buffer);
#endif

    // Ensure the charger is disconnected when the maximum battery pack voltage is reached.
    if( voltage >= get_max_charge_voltage() ) {
        if( mgos_uptime() > BAT_CHARGE_EVAL_SECS ) {
            set_load_on(false);
            set_battery_fully_charged(true);
            logger(__FUNCTION__, "!!! Reached max charge voltage. Battery is now fully charged.");
        }
        else {
            logger(__FUNCTION__, "!!! Waiting for 30 seconds to elapse before turning load off.");
        }
    }

    (void) arg;
}

/**
 * @brief Callback to periodically read temp adc.
 **/
static void read_adc_temp_cb(void *arg) {
    char *logger_buffer = get_logger_buffer();
    uint16_t temp_adc = read_adc(ADC3, FS_VOLTAGE_2_048, 1);
    float mcp9700_volts = temp_adc / MCP9700_CODES_PER_VOLT;
    tempC = ( mcp9700_volts - MCP9700_VOUT_0C ) / MCP9700_TC;

    // If the battery voltage drops below 0 then ensure battery charging is off as
    // it's dangerous to charge a LION battery at these temperatures.
    if( tempC < MIN_BATTERY_TEMP ) {
        set_load_on(false);
        snprintf(warning_message, STATUS_MSG_LENGTH, "Battery temperature is %.1f °C.<BR>Battery charging disabled as temperature is below %.1f °C which is not safe and will damage the battery.", tempC, MIN_BATTERY_TEMP);
    }
    // Its also dangerous to charge the battery if it's to hot.
    else if( tempC > MAX_BATTERY_TEMP ) {
            set_load_on(false);
            snprintf(warning_message, STATUS_MSG_LENGTH, "Battery temperature is %.1f °C.<BR>Battery charging disabled as temperature is above %.1f °C which is not safe.<BR>Cool it down and then try charging it.", tempC, MIN_BATTERY_TEMP);
    }
    // A message in the initial battery evaluation period
    else if( mgos_uptime() <= BAT_CHARGE_EVAL_SECS ) {
        int secs_left = BAT_CHARGE_EVAL_SECS - mgos_uptime();
        snprintf(warning_message, STATUS_MSG_LENGTH, "%d seconds left evaluating the battery charge.", secs_left);
    }
    else {
        strcpy(warning_message, "");
    }


#ifdef SHOW_TEMP_DEBUG
    snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "temp_adc=0x%04x, mcp9700_volts=%.3f, tempC=%.1f °C", temp_adc, mcp9700_volts, tempC);
    logger(__FUNCTION__, logger_buffer);
#endif

    (void) arg;
}

/**
 * @brief Callback to perform startup initialisation after an startup delay.
 **/
static void startup_init_cb(void *arg) {
    memset(warning_message, 0 ,  STATUS_MSG_LENGTH);
//    if( voltage < get_max_charge_voltage() ) {
        set_load_on(true);
//    }
    logger(__FUNCTION__, "Turned on the Load.");
}

/***
 * @brief Init all timer functions.
 */
void init_timers(void) {
    //Setup timer callbacks
    mgos_set_timer(TIMER1_PERIOD_MS, MGOS_TIMER_REPEAT,   timer1_cb, NULL);
    mgos_set_timer(READ_ADC_PERIOD_MS, MGOS_TIMER_REPEAT, read_adc_amps_cb, NULL);
    mgos_set_timer(READ_ADC_PERIOD_MS, MGOS_TIMER_REPEAT, read_adc_volts_cb, NULL);
    mgos_set_timer(READ_ADC_PERIOD_MS, MGOS_TIMER_REPEAT, read_adc_temp_cb, NULL);
    //run once timers
    mgos_set_timer(STARTUP_INIT_TIMER1_PERIOD_MS, 0,      startup_init_cb, NULL);
}

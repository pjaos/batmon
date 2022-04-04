#include <mgos.h>
#include <mgos_rpc.h>
#include <mgos_syslog.h>

#include "ayt_tx_handler.h"
#include "rpc_handler.h"
#include "logger.h"
#include "gpio.h"
#include "timer.h"
#include "restart.h"
#include "battery.h"

/*
 * @brief Callback handler to set the device to factory defaults. This will
 *        cause the device to reboot.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_ydev_factorydefault(struct mg_rpc_request_info *ri,
                                         void *cb_arg,
                                         struct mg_rpc_frame_info *fi,
                                         struct mg_str args) {
        logger(__FUNCTION__, "Reset to factory default configuration and reboot.");
        mgos_config_reset(MGOS_CONFIG_LEVEL_DEFAULTS);
        mg_rpc_send_responsef(ri, NULL);
        logger(__FUNCTION__, "Reboot now.");
        mgos_system_restart_after(250);

        (void) ri;
        (void) cb_arg;
        (void) fi;
        (void) args;
}

/*
 * @brief Callback handler to set update the syslog state (enabled/disabled).
 *        If enabled syslog data is sent to the ICONS_GW host address.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_ydev_update_syslog(struct mg_rpc_request_info *ri,
                                         void *cb_arg,
                                         struct mg_rpc_frame_info *fi,
                                         struct mg_str args) {
        char *logger_buffer = get_logger_buffer();
        logger(__FUNCTION__, "mgos_rpc_ydev_update_syslog()");
        if ( logger_buffer ) {
            char *icons_gw_ip_addr = get_icons_gw_ip_address();
            snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "icons_gw_ip_addr = %p", icons_gw_ip_addr);
            logger(__FUNCTION__, logger_buffer);
            //If syslog is enabled and we have the ICONS GW IP address setup syslog
            if( mgos_sys_config_get_ydev_enable_syslog() && icons_gw_ip_addr ) {
                snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "icons_gw_ip_addr = %s", icons_gw_ip_addr);
                logger(__FUNCTION__, logger_buffer);
                char *hostname = (char *)mgos_sys_config_get_ydev_unit_name();
                if( hostname ) {
                    snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "hostname = %s", hostname);
                    logger(__FUNCTION__, logger_buffer);
                    reinit_syslog(icons_gw_ip_addr, hostname);
                    logger(__FUNCTION__, "Enabled syslog");
                }
            }
            //disable syslog.
            else {
                logger(__FUNCTION__, "Disabled syslog");
                reinit_syslog(NULL, NULL);
            }
        }

        mg_rpc_send_responsef(ri, NULL);

        (void) ri;
        (void) cb_arg;
        (void) fi;
        (void) args;
}

/*
 * @brief Callback handler to set set the debug level.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_sys_set_debug_handler(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    char *logger_buffer = get_logger_buffer();

    int level = _LL_MIN;
    if (json_scanf(args.p, args.len, ri->args_fmt, &level) == 1) {
        if (level > _LL_MIN && level < _LL_MAX) {
            mg_rpc_send_responsef(ri, "%d", level);
            snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "level=%d", level);
            logger(__FUNCTION__, logger_buffer);
            cs_log_set_level((enum cs_log_level) level);
        }
        else {
            mg_rpc_send_errorf(ri, -1, "Invalid debug level (-1 to 4 are valid).");
        }
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"level\":value}");
    }

    deep_sleep_for(5000);

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to set set the load on/battery charging..
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_sys_set_load_on_handler(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    char *logger_buffer = get_logger_buffer();

    int load_on = 0;
    if (json_scanf(args.p, args.len, ri->args_fmt, &load_on) == 1) {
        if ( load_on == 0 || load_on == 1 ) {
            mg_rpc_send_responsef(ri, "%d", load_on);
            snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "load on=%d", load_on);
            logger(__FUNCTION__, logger_buffer);
            if( load_on ) {
                set_load_on(true);
            }
            else {
                set_load_on(false);
            }
        }
        else {
            mg_rpc_send_errorf(ri, -1, "Invalid load state (0 or 1 are valid).");
        }
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"load_on\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Set the max battery charge level.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void set_max_battery_charge(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    char *logger_buffer = get_logger_buffer();

    int level = 0;
    if (json_scanf(args.p, args.len, ri->args_fmt, &level) == 1) {
        if (level >= MIN_LEVEL_ID && level <=MAX_LEVEL_ID) {
            mg_rpc_send_responsef(ri, "%d", level);
            snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "set_max_battery_charge level=%d", level);
            logger(__FUNCTION__, logger_buffer);
            set_max_battery_charge_voltage_id(level);
        }
        else {
            snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "Invalid max_battery_charge level (%d - %d are valid).", MIN_LEVEL_ID, MAX_LEVEL_ID);
            mg_rpc_send_errorf(ri, -1, logger_buffer);
        }
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"level\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Get the state of the unit.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void get_state(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {
    float tempC = get_temp();
    float amps = get_current();
    float battery_voltage = get_battery_voltage();
    bool charging = is_load_on();
    bool battery_fully_charged = is_battery_fully_charged();
    float fullyChargedVoltage = get_max_charge_voltage();
    char *warning_message = get_warning_message();
    int battery_cell_count = get_battery_cell_count();
    float last_on_charge_voltage = get_last_on_charge_voltage();

    mg_rpc_send_responsef(ri, "{"
                              "amps:%.3f,"
                              "battery_voltage:%.3f,"
                              "last_on_charge_voltage:%.3f,"
                              "tempC:%.1f,"
                              "charging:%d,"
                              "fully_charged:%d,"
                              "fully_charged_voltage:%.1f,"
                              "warning_message:%Q,"
                              "battery_cell_count:%d"
                              "}"
                              ,
                              amps,
                              battery_voltage,
                              last_on_charge_voltage,
                              tempC,
                              charging,
                              battery_fully_charged,
                              fullyChargedVoltage,
                              warning_message,
                              battery_cell_count
                              );

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Set the calibration of the amps value in ADC codes per amp.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void set_amps_cal(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    char *logger_buffer = get_logger_buffer();

    int codes_per_amp = 0;
    if (json_scanf(args.p, args.len, ri->args_fmt, &codes_per_amp) == 1) {
            mg_rpc_send_responsef(ri, "%d", codes_per_amp);
            snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "set_amps_cal: codes_per_amp=%d", codes_per_amp);
            logger(__FUNCTION__, logger_buffer);
            mgos_sys_config_set_batmon_codes_per_amp(codes_per_amp);
            save_cfg(&mgos_sys_config, NULL);
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"level\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Set the calibration of the positive battery terminal voltage measurement value in ADC codes per volt.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void set_pvolts_cal(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    char *logger_buffer = get_logger_buffer();

    int codes_per_volt = 0;
    if (json_scanf(args.p, args.len, ri->args_fmt, &codes_per_volt) == 1) {
            mg_rpc_send_responsef(ri, "%d", codes_per_volt);
            snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "set_pvolts_cal: codes_per_volt=%d", codes_per_volt);
            logger(__FUNCTION__, logger_buffer);
            mgos_sys_config_set_batmon_pbat_codes_per_volt(codes_per_volt);
            save_cfg(&mgos_sys_config, NULL);
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"level\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Set the calibration of the negative battery terminal voltage measurement value in ADC codes per volt.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void set_nvolts_cal(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    char *logger_buffer = get_logger_buffer();

    int codes_per_volt = 0;
    if (json_scanf(args.p, args.len, ri->args_fmt, &codes_per_volt) == 1) {
            mg_rpc_send_responsef(ri, "%d", codes_per_volt);
            snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "set_nvolts_cal: codes_per_volt=%d", codes_per_volt);
            logger(__FUNCTION__, logger_buffer);
            mgos_sys_config_set_batmon_nbat_codes_per_volt(codes_per_volt);
            save_cfg(&mgos_sys_config, NULL);
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"level\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Init all the RPC handlers.
 */
void rpc_init(void) {

        struct mg_rpc *con = mgos_rpc_get_global();

        mg_rpc_add_handler(con, "factorydefault", NULL, mgos_rpc_ydev_factorydefault, NULL);
        mg_rpc_add_handler(con, "update_syslog", NULL,  mgos_rpc_ydev_update_syslog, NULL);
        mg_rpc_add_handler(con, "debug", "{level: %d}", mgos_sys_set_debug_handler, NULL);
        mg_rpc_add_handler(con, "load", "{on: %d}",  mgos_sys_set_load_on_handler, NULL);
        mg_rpc_add_handler(con, "set_max_battery_charge", "{level: %d}",  set_max_battery_charge, NULL);
        mg_rpc_add_handler(con, "set_amps_cal", "{codes_per_amp: %d}",  set_amps_cal, NULL);
        mg_rpc_add_handler(con, "set_pvolts_cal", "{codes_per_volt: %d}",  set_pvolts_cal, NULL);
        mg_rpc_add_handler(con, "set_nvolts_cal", "{codes_per_volt: %d}",  set_nvolts_cal, NULL);
        mg_rpc_add_handler(con, "get_state", NULL,  get_state, NULL);

}

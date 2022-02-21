#include <mgos.h>

#include "logger.h"

#include "esp32/rom/rtc.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"

static int restart_reason = 0; //Holds the reason for the last restart

/**
 * @brief Get the reason for the restart as a string.
 * @return The restart reason text.
 */
static char *get_restart_reason_text(int restart_reason) {
    char *restart_reason_text;
    switch(restart_reason) {
    case 1  : restart_reason_text = (char *)"Vbat power on reset";break;
    case 3  : restart_reason_text = (char *)"Software reset digital core";break;
    case 4  : restart_reason_text = (char *)"Legacy watch dog reset digital core";break;
    case 5  : restart_reason_text = (char *)"Deep Sleep reset digital core";break;
    case 6  : restart_reason_text = (char *)"Reset by SLC module, reset digital core";break;
    case 7  : restart_reason_text = (char *)"Timer Group0 Watch dog reset digital core";break;
    case 8  : restart_reason_text = (char *)"Timer Group1 Watch dog reset digital core";break;
    case 9  : restart_reason_text = (char *)"RTC Watch dog Reset digital core";break;
    case 10 : restart_reason_text = (char *)"Instrusion tested to reset CPU";break;
    case 11 : restart_reason_text = (char *)"Time Group reset CPU";break;
    case 12 : restart_reason_text = (char *)"Software reset CPU";break;
    case 13 : restart_reason_text = (char *)"RTC Watch dog Reset CPU";break;
    case 14 : restart_reason_text = (char *)"for APP CPU, reseted by PRO CPU";break;
    case 15 : restart_reason_text = (char *)"Reset when the vdd voltage is not stable";break;
    case 16 : restart_reason_text = (char *)"RTC Watch dog reset digital core and rtc module";break;
    default : restart_reason_text = (char *)"UNKNOWN";
    }
    return restart_reason_text;
}
/**
 * @brief Get the reason for the last reset.
 * @return The reset reason.
 *         1 = Vbat power on reset                              (power up)
 *         3 = Software reset digital core
 *         4 = Legacy watch dog reset digital core
 *         5 = Deep Sleep reset digital core                    (RTC wake up from deep sleep)
 *         6 = Reset by SLC module, reset digital core
 *         7 = Timer Group0 Watch dog reset digital core
 *         8 = Timer Group1 Watch dog reset digital core
 *         9 = RTC Watch dog Reset digital core
 *        10 = Instrusion tested to reset CPU
 *        11 = Time Group reset CPU
 *        12 = Software reset CPU                               (reboot called, E.G SW update)
 *        13 = RTC Watch dog Reset CPU
 *        14 = For APP CPU, reset by PRO CPU
 *        15 = Reset when the vdd voltage is not stable
 *        16 = RTC Watch dog reset digital core and rtc module
 */
static int get_reset_reason() {
  int restart_reason = rtc_get_reset_reason(0);

  char *restart_reason_text = get_restart_reason_text(restart_reason);
  char *logger_buffer = get_logger_buffer();
  snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "RESTART REASON = %d (%s)\n", restart_reason, restart_reason_text);
  logger(__FUNCTION__, logger_buffer);

  return restart_reason;
}


/**
 * @brief Go into deep sleep for a number of milli seconds.
 * @param milli_seconds The number of milli seconds to sleep for.
 */
void deep_sleep_for(int milli_seconds) {
    char *logger_buffer = get_logger_buffer();
    snprintf(logger_buffer, MAX_LOGGER_BUFFER_LEN, "Deep sleep for %d milli seconds.", milli_seconds);
    logger(__FUNCTION__, logger_buffer);
    esp_sleep_enable_timer_wakeup(milli_seconds * 1000);
    esp_deep_sleep_start();
}

/**
 * @return The reason for the restart. See get_restart_reason() doc for details.
 */
int get_restart_reason() {
    return restart_reason;
}

/**
 * @brief Called to init the restart reason.
 */
void restart_init(void) {
    restart_reason = get_reset_reason();
}

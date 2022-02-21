#include <mgos.h>

#include "mgos_syslog.h"
#include "logger.h"

char logger_buffer[MAX_LOGGER_BUFFER_LEN];

char *get_logger_buffer(void) {
    return (char *)logger_buffer;
}

/**
 * @brief log all messages using this function.
 *        log messages are sent to the serial port (if enabled) and to syslog (if enabled).
 * @param function The name of the parent function called.
 * @param msg The log message text.
 */
void logger(const char *function, char *msg) {
    //If we have a non null pointer.
    if( msg ) {
        mgos_syslog_log_info(function, msg);
        LOG(LL_INFO, (msg) );
    }
}

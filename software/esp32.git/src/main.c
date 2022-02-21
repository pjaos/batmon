#include <mgos.h>
#include <ydev_ayt_handler.h>
#include "mgos_syslog.h"
#include "ayt_tx_handler.h"
#include "rpc_handler.h"
#include "timer.h"
#include "logger.h"
#include "gpio.h"
#include "restart.h"

/**
 * @brief Initialises the application.
 **/
enum mgos_app_init_result mgos_app_init(void) {
    restart_init();

    gpio_init();

    rpc_init();

    add_ayt_response_handler(send_ayt_response);

    init_timers();

    logger("AYT HASHTAG: %s", (char *)mgos_sys_config_get_ydevayth_hashtag() );

    return MGOS_APP_INIT_SUCCESS;
}

#include <mgos.h>

#include "logger.h"
#include "gpio.h"


bool load_on = false;

/**
 * @brief Init the GPIO lines used by the unit.
 */
void gpio_init(void) {
    //Set the GPIO line used to control the Load (battery charging)
    mgos_gpio_set_mode(LOAD_ON_GPIO, MGOS_GPIO_MODE_OUTPUT);
    //By default load off to measure no load ADC amps.
    set_load_on(false);
}

/**
 * @brief Set the load on/off.
 * @param on If true turn the load on. False turn the load off.
 */
void set_load_on(bool on) {
    char *msg;

    if( on ) {
        mgos_gpio_write(LOAD_ON_GPIO, false);
        msg = "Battery charging enabled.";
        load_on = true;
    }
    else {
        msg = "Battery charging disabled.";
        mgos_gpio_write(LOAD_ON_GPIO, true);
        load_on = false;
    }

    logger(__FUNCTION__, msg);
}

/**
 * @brief Determine if the load is on.
 * @return true if the load is on, false if the load is off.
 */
bool is_load_on(void) {
    return load_on;
}


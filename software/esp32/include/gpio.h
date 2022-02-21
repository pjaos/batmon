#ifndef SRC_INCLUDE_GPIO_H_
#define SRC_INCLUDE_GPIO_H_

#define LOAD_ON_GPIO 12

void gpio_init(void);
void set_load_on(bool on);
bool is_load_on(void);

#endif /* SRC_INCLUDE_GPIO_H_ */

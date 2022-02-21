#ifndef SRC_INCLUDE_RESTART_H_
#define SRC_INCLUDE_RESTART_H_

void restart_init(void);
int get_restart_reason();
void deep_sleep_for(int milli_seconds);

#endif /* SRC_INCLUDE_RESTART_H_ */

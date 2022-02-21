#ifndef SRC_INCLUDE_LOGS_H_
#define SRC_INCLUDE_LOGS_H_

#define MAX_LOGGER_BUFFER_LEN 129

void logger(const char *function, char *msg);
char *get_logger_buffer(void);

#endif /* SRC_INCLUDE_LOGS_H_ */

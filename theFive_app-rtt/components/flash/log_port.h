#ifndef LOG_PORT_H_
#define LOG_PORT_H_

EfErrCode ef_log_read(size_t index, rt_uint32_t *log, size_t size);
EfErrCode ef_log_write(const rt_uint32_t *log, size_t size);
EfErrCode ef_log_clean(void);
size_t log_get_used_size(void);


#endif /* LOG_PORT_H_ */

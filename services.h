#ifndef SERVICES_H
#define SERVICES_H
struct service_desc_t {
    uint8_t name[STR_LEN]; 
    int (*start_fn_t)(void);
    int (*stop_fn_t)(void);
    int (*status_fn_t)(void);
};

#endif

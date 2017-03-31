#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "iopcdefine.h"
#include "iopcops_cfg.h"
#include "iopccmd_service.h"
#include "iopccmd_db.h"

int8_t hn_db_restore2default()
{
    GET_INSTANCE(ops_cfg_bdb)->restore_cfgsdb2default();
    return 0;
}

int8_t hn_db_save2persist()
{
    GET_INSTANCE(ops_cfg_bdb)->save_cfgsdb2persist();
    return 0;
}

static uint32_t db_setbool(uint8_t media, uint8_t* preq, uint8_t* pres)
{
    struct req_dbsetbool_t* req = (struct req_dbsetbool_t*)preq;
    struct res_dbsetbool_t* res = (struct res_dbsetbool_t*)pres;
    uint8_t key_len = req->key_len;
    uint8_t key[STR_LEN];
    uint8_t val = req->val;

    res->status = 0;
    memset(key, 0, STR_LEN);
    memcpy(key, req->key, key_len);

    if(media) 
        GET_INSTANCE(ops_cfg_bdb)->set_boolean_flash(key, val);
    else
        GET_INSTANCE(ops_cfg_bdb)->set_boolean_ram(key, val);

    return sizeof(struct res_dbsetbool_t);
}

uint32_t hn_db_setbool_f(uint8_t* preq, uint8_t* pres)
{
    return db_setbool(1, preq, pres);
}

uint32_t hn_db_setbool_r(uint8_t* preq, uint8_t* pres)
{
    return db_setbool(0, preq, pres);
}

static uint8_t get_boolean(uint8_t media, uint8_t* preq, uint8_t* pres)
{
    struct req_dbgetbool_t* req = (struct req_dbgetbool_t*)preq;
    struct res_dbgetbool_t* res = (struct res_dbgetbool_t*)pres;
    uint8_t key_len = req->key_len;
    uint8_t key[STR_LEN];

    res->status = 0;
    memset(key, 0, STR_LEN);
    memcpy(key, req->key, key_len);

    if(media){
        res->val = GET_INSTANCE(ops_cfg_bdb)->get_boolean_flash(key);
    } else {
        res->val = GET_INSTANCE(ops_cfg_bdb)->get_boolean_ram(key);
    }
    printf("get_boolean %s:%d\n", key, res->val);

    return sizeof(struct res_dbgetbool_t);
}

uint32_t hn_db_getbool_f(uint8_t* preq, uint8_t* pres)
{
    return get_boolean(1, preq, pres);
}

uint32_t hn_db_getbool_r(uint8_t* preq, uint8_t* pres)
{
    return get_boolean(0, preq, pres);
}

static uint32_t db_setuint32(uint8_t media, uint8_t* preq, uint8_t* pres)
{
    struct req_dbsetuint32_t* req = (struct req_dbsetuint32_t*)preq;
    struct res_dbsetuint32_t* res = (struct res_dbsetuint32_t*)pres;
    uint8_t key_len = req->key_len;
    uint8_t key[STR_LEN];
    uint32_t val = req->val;

    res->status = 0;
    memset(key, 0, STR_LEN);
    memcpy(key, req->key, key_len);

    if(media) 
        GET_INSTANCE(ops_cfg_bdb)->set_uint32_flash(key, val);
    else
        GET_INSTANCE(ops_cfg_bdb)->set_uint32_ram(key, val);

    return sizeof(struct res_dbsetuint32_t);
}

uint32_t hn_db_setuint32_f(uint8_t* preq, uint8_t* pres)
{
    return db_setuint32(1, preq, pres);
}

uint32_t hn_db_setuint32_r(uint8_t* preq, uint8_t* pres)
{
    return db_setuint32(0, preq, pres);
}

uint32_t db_getuint32(uint8_t media, uint8_t* preq, uint8_t* pres)
{
    struct req_dbgetuint32_t* req = (struct req_dbgetuint32_t*)preq;
    struct res_dbgetuint32_t* res = (struct res_dbgetuint32_t*)pres;
    uint8_t key_len = req->key_len;
    uint8_t key[STR_LEN];

    res->status = 0;
    memset(key, 0, STR_LEN);
    memcpy(key, req->key, key_len);

    if(media){
        res->val = GET_INSTANCE(ops_cfg_bdb)->get_uint32_flash(key);
    } else {
        res->val = GET_INSTANCE(ops_cfg_bdb)->get_uint32_ram(key);
    }

    return sizeof(struct res_dbgetuint32_t);
}

uint32_t hn_db_getuint32_f(uint8_t* preq, uint8_t* pres)
{
    return db_getuint32(1, preq, pres);
}

uint32_t hn_db_getuint32_r(uint8_t* preq, uint8_t* pres)
{
    return db_getuint32(0, preq, pres);
}

static uint32_t db_setstring(uint8_t media, uint8_t* preq, uint8_t* pres)
{
    struct req_dbsetstring_t* req = (struct req_dbsetstring_t*)preq;
    struct res_dbsetstring_t* res = (struct res_dbsetstring_t*)pres;
    uint8_t key_len = req->key_len;
    uint8_t key[STR_LEN];
    uint16_t val_len = req->val_len;
    uint8_t val[MAX_STR_VAL];

    printf("%s():%d, %s, %d, %s\n", __func__, req->key_len, req->key, req->val_len, req->val);
    res->status = 0;
    memset(key, 0, STR_LEN);
    memcpy(key, req->key, key_len);
    memset(val, 0, MAX_STR_VAL);
    memcpy(val, req->val, val_len);

    printf("%s():%d, %s, %d, %s\n", __func__, key_len, key, val_len, val);
    if(media) 
        GET_INSTANCE(ops_cfg_bdb)->set_string_flash(key, &val[0]);
    else
        GET_INSTANCE(ops_cfg_bdb)->set_string_ram(key, &val[0]);

    return sizeof(struct res_dbsetstring_t);
}

uint32_t hn_db_setstring_f(uint8_t* preq, uint8_t* pres)
{
    return db_setstring(1, preq, pres);
}

uint32_t hn_db_setstring_r(uint8_t* preq, uint8_t* pres)
{
    return db_setstring(0, preq, pres);
}

static uint32_t db_getstring(uint8_t media, uint8_t* preq, uint8_t* pres)
{
    struct req_dbgetstring_t* req = (struct req_dbgetstring_t*)preq;
    struct res_dbgetstring_t* res = (struct res_dbgetstring_t*)pres;
    uint8_t key_len = req->key_len;
    uint8_t key[STR_LEN];
    uint16_t val_len = 0;
    uint8_t val[MAX_STR_VAL];

    res->status = 0;
    memset(key, 0, STR_LEN);
    memcpy(key, req->key, key_len);

    if(media){
        val_len = GET_INSTANCE(ops_cfg_bdb)->get_string_flash(key, &val[0]);
    } else {
        val_len = GET_INSTANCE(ops_cfg_bdb)->get_string_ram(key, &val[0]);
    }

    res->val_len = val_len;
    memcpy(res->val, val, val_len);

    return sizeof(struct res_dbgetstring_t) - MAX_STR_VAL + val_len;
}

uint32_t hn_db_getstring_f(uint8_t* preq, uint8_t* pres)
{
    return db_getstring(1, preq, pres);
}

uint32_t hn_db_getstring_r(uint8_t* preq, uint8_t* pres)
{
    return db_getstring(0, preq, pres);
}


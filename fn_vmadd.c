#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <lxccontainer.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_bdb_status.h"
#include "iopcops_cfg_bdb_vm.h"
#include "iopccmd_vmadd.h"

uint32_t hn_vmadd(uint8_t* preq, uint8_t* pres)
{
    struct req_vmadd_t* req = (struct req_vmadd_t*)preq;
    struct res_vmadd_t* res = (struct res_vmadd_t*)pres;
    uint8_t vm_idx = 0;
    res->status = 0;
/*
    printf("add_vm begin(%d):\n", sizeof(req->nethwaddr));
    printf("autostart:%d\n", req->auto_start);
    printf("name:%s\n", req->name);
    printf("base_path:%s\n", req->base_path);
    printf("nettype:%s\n", req->nettype);
    printf("nethwlink:%s\n", req->nethwlink);
    printf("nethwaddr:%s\n", req->nethwaddr);
    printf("add_vm end\n");
*/
    vm_idx = GET_INSTANCE_CFG_VM()->add_vm(req->auto_start, req->name, req->base_path, req->nettype, req->nethwlink, req->nethwaddr);
    res->index = vm_idx;


    return sizeof(struct res_vmadd_t);
}


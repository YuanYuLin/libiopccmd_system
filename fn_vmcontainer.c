#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
//#include <lxccontainer.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_status.h"
#include "iopcops_cfg_vm.h"
#include "iopccmd_service.h"

#define SERVICE_PRE_VM	"prevm"
#define SERVICE_NAME	"vmcontainer"
#define SERVICE_WAIT	"netdev"

#define VM_BASE_DIR	"/persist/vms/"
#define VM_OVERLAYFS	"/persist/vms/%s/%s"

static struct root_t init_list[] = {
    INIT_NODE(ROOT_TYPE_MOUNT_CGROUP, "/sys/fs/cgroup", 0, 0 ),

    INIT_NODE(ROOT_TYPE_CREATE_DIR, "/usr/lib", DEFAULT_MODE, 0 ),
    INIT_NODE(ROOT_TYPE_CREATE_DIR, "/usr/lib/lxc", DEFAULT_MODE, 0 ),
    INIT_NODE(ROOT_TYPE_CREATE_DIR, "/usr/lib/lxc/rootfs", DEFAULT_MODE, 0 ),

    INIT_NODE(ROOT_TYPE_CREATE_DIR, "/usr/share/lxc", DEFAULT_MODE, 0),

    INIT_NODE(ROOT_TYPE_CREATE_DIR, "/var/lib", DEFAULT_MODE, 0 ),
    INIT_NODE(ROOT_TYPE_CREATE_DIR, "/var/lib/lxc", DEFAULT_MODE, 0 ),

    INIT_NODE(ROOT_TYPE_CREATE_DIR, VM_BASE_DIR, DEFAULT_MODE, 0 ),
};

static void startByIndex(uint8_t cfg_idx)
{
//TODO
#if 0
    struct lxc_container *container;
    uint8_t vm_autostart = 0;
    uint8_t vm_name[STR_LEN];
    uint8_t vm_nettype[STR_LEN];
    uint8_t vm_nethwlink[STR_LEN];
    uint8_t vm_nethwaddr[STR_LEN];
    uint8_t vm_opts[STR_LEN];
    uint8_t vm_rootfs[STR_LEN];
    uint8_t vm_lower[STR_LEN];
    uint8_t vm_upper[STR_LEN];
    uint8_t vm_work[STR_LEN];
    uint8_t vm_fstab[STR_LEN];
    uint8_t vm_ipaddress[STR_LEN];
    uint8_t vm_gateway[STR_LEN];
    uint8_t cmd_str[STR_LEN];

        memset(vm_name, 0, STR_LEN);
	GET_INSTANCE_CFG_VM()->get_name(cfg_idx, &vm_name[0]);

	memset(vm_nettype, 0, STR_LEN);
	GET_INSTANCE_CFG_VM()->get_nettype(cfg_idx, &vm_nettype[0]);

	memset(vm_nethwlink, 0, STR_LEN);
	GET_INSTANCE_CFG_VM()->get_nethwlink(cfg_idx, &vm_nethwlink[0]);

	memset(vm_nethwaddr, 0, STR_LEN);
	GET_INSTANCE_CFG_VM()->get_nethwaddr(cfg_idx, &vm_nethwaddr[0]);

	memset(vm_ipaddress, 0, STR_LEN);
	sprintf(vm_ipaddress, "192.168.200.%d/24", (1 + cfg_idx));

	memset(vm_gateway, 0, STR_LEN);
	sprintf(vm_gateway, "192.168.200.254");

        vm_autostart = GET_INSTANCE_CFG_VM()->get_autostart(cfg_idx);

        memset(vm_rootfs, 0, STR_LEN);
        sprintf(vm_rootfs, VM_OVERLAYFS, vm_name, "rootfs");
        GET_INSTANCE_MISC_OBJ()->create_dir_recursive(vm_rootfs, DEFAULT_MODE);

	memset(vm_fstab, 0, STR_LEN);
	sprintf(vm_fstab, "%s%s", vm_rootfs, "/etc/fstab");

        memset(vm_lower, 0, STR_LEN);
	GET_INSTANCE_CFG_VM()->get_base_path(cfg_idx, &vm_lower[0]);

        memset(vm_upper, 0, STR_LEN);
        sprintf(vm_upper, VM_OVERLAYFS, vm_name, "upper");
        GET_INSTANCE_MISC_OBJ()->create_dir_recursive(vm_upper, DEFAULT_MODE);

        memset(vm_work, 0, STR_LEN);
        sprintf(vm_work, VM_OVERLAYFS, vm_name, "work");
        GET_INSTANCE_MISC_OBJ()->create_dir_recursive(vm_work, DEFAULT_MODE);

        memset(vm_opts, 0, STR_LEN);
        sprintf(vm_opts, "lowerdir=%s,upperdir=%s,workdir=%s", vm_lower, vm_upper, vm_work);
//	printf("vm_opts=%s\n", vm_opts);
//        GET_INSTANCE(ops_misc)->mountfs(vm_name, vm_rootfs, "overlay", 0, vm_opts);


        container = (struct lxc_container*)lxc_container_new(vm_name, "/var/lib/lxc");
  
	/*
        if(container->is_running(container)) {
            printf("%s is running...\n", vm_name);
	    continue;
        }
	*/

        if(container) {
            if(container->is_defined(container)) {
		printf("1.vm%d thought it was defined\n", cfg_idx);
                fprintf(stderr, "1.vm%d thought it was defined\n", cfg_idx);
	    }

            container->clear_config(container);
//	    container->load_config(container, NULL);

            container->set_config_item(container, "lxc.utsname", vm_name);
            container->set_config_item(container, "lxc.rootfs", vm_rootfs);
            container->set_config_item(container, "lxc.pts", "256");
            container->set_config_item(container, "lxc.autodev", "1");
            container->set_config_item(container, "lxc.init_cmd", "/init");
	    container->set_config_item(container, "lxc.mount", vm_fstab);

    	//network interface eth0
            container->set_config_item(container, "lxc.network.type", vm_nettype);
            container->set_config_item(container, "lxc.network.flags", "up");
            container->set_config_item(container, "lxc.network.link", vm_nethwlink);
            container->set_config_item(container, "lxc.network.name", "eth0");
            container->set_config_item(container, "lxc.network.hwaddr", vm_nethwaddr);
	//network interface eth1
            container->set_config_item(container, "lxc.network.type", "veth");
            container->set_config_item(container, "lxc.network.flags", "up");
            container->set_config_item(container, "lxc.network.link", "br1");
            container->set_config_item(container, "lxc.network.name", "eth1");
            container->set_config_item(container, "lxc.network.ipv4", vm_ipaddress);
            container->set_config_item(container, "lxc.network.ipv4.gateway", vm_gateway);

            container->save_config(container, NULL);

	    printf("vm fstab cfg %s\n", vm_fstab);
	    printf("save config[%s]\n", vm_name);

/*            if(container->create(container, NULL, NULL, NULL, 0, NULL)) {
                printf("[%s] created\n", vm_name);
            } else {
                fprintf(stderr, "1.Failed to create container rootfs\n");
                if(!container->destroy(container)) {
                    fprintf(stderr, "1.Failed to destroy the container.\n");
                }
	    }
*/
        } else {
            fprintf(stderr, "1.Failed to setup lxc_container struct\n");
        }

        if(vm_autostart) {
            if(container->is_running(container)) {
                printf("%s is running...\n", vm_name);
		return ;
            }
            printf("AutoStart [%s]\n", vm_name);
	    if(GET_INSTANCE_MISC_OBJ()->is_mountfs_by_src_dst_type(vm_name, vm_rootfs, "overlay") == 0) {
	        GET_INSTANCE_MISC_OBJ()->mountfs(vm_name, vm_rootfs, "overlay", 0, vm_opts);
            }
	    memset(cmd_str, 0, STR_LEN);
	    sprintf(cmd_str, "iopcvm %s", vm_name);
	    GET_INSTANCE_MISC_OBJ()->execute_cmd(cmd_str, NULL);
        } else {
            printf("Not AutoStart [%s]\n", vm_name);
	}
#endif
}

#define TASK_ARG(type, src, dst) \
	type dst; \
	if(src != NULL) \
		memcpy((void*)&dst, (void*)src, (size_t)sizeof(type));

static void* action_start(void* arg)
{
    int init_list_size = (sizeof(init_list) / sizeof(init_list[0]));
    pthread_t thread_id;
    uint32_t cfg_size = 0;
    uint32_t cfg_idx = 0;
//    TASK_ARG(struct arg_t, arg, parm);
    struct arg_t *parm = (struct arg_t*)arg;
    //struct arg_t *parm = (struct arg_t*)arg;

    thread_id = pthread_self();
    printf("%s(%lu)\n", __func__, thread_id);

//    GET_INSTANCE(ops_cfg_bdb_status)->set_service_starting(SERVICE_NAME);

    if(arg == NULL) {
        GET_INSTANCE_CFG_STATUS()->wait_service_started(SERVICE_WAIT);

        if(GET_INSTANCE_CFG_STATUS()->is_service_started(SERVICE_PRE_VM) == 0) {
            GET_INSTANCE_CFG_STATUS()->set_service_starting(SERVICE_PRE_VM);

            GET_INSTANCE_MISC_OBJ()->create_system_by_list(init_list_size, &init_list[0]);

            GET_INSTANCE_CFG_STATUS()->set_service_started(SERVICE_PRE_VM);
        }

        cfg_size = GET_INSTANCE_CFG_VM()->get_cfg_size();

        printf("VM count %d\n", cfg_size);
        for(cfg_idx = 0;cfg_idx < cfg_size;cfg_idx++) {
            startByIndex(cfg_idx);
        }
    } else {
	printf("SPECIFIC:%d\n", parm->index);
	startByIndex(parm->index);
    }

    GET_INSTANCE_CFG_STATUS()->set_service_started(SERVICE_NAME);
    if(arg != NULL)
        free(arg);
    pthread_exit((void*)0);
    return 0;
}

static void* action_stop(void* arg)
{
    return 0;
}

static void* action_restart(void* arg)
{
    return 0;
}

uint32_t hn_vmcontainer(uint8_t* preq, uint8_t* pres)
{
    struct req_service_t* req = (struct req_service_t*)preq;
    struct res_service_t* res = (struct res_service_t*)pres;
    struct arg_t *arg = (struct arg_t*)(preq + sizeof(struct req_service_t));
    //int i = 0;
    uint8_t action = req->action;

    printf("%s running\n", __func__);
    switch(action) {
    case SERVICE_ACTION_STATUS:
    break;
    case SERVICE_ACTION_START:
	GET_INSTANCE_MISC_OBJ()->create_task(action_start);
    break;
    case SERVICE_ACTION_STOP:
        action_stop(NULL);
    break;
    case SERVICE_ACTION_RESTART:
        action_restart(NULL);
    break;
    case SERVICE_ACTION_STATUS_SPECIFIC:
    break;
    case SERVICE_ACTION_START_SPECIFIC:
        GET_INSTANCE_CFG_STATUS()->set_service_starting(SERVICE_NAME);
	//arg = (void*)req;
	//for(i=0;i<=sizeof(struct req_service_t);i++) {
	//	printf("%x,", preq[i]);
	//}
	//printf("\n");
        GET_INSTANCE_MISC_OBJ()->create_task_with_arg(action_start, (void*)arg, sizeof(struct arg_t));
    break;
    case SERVICE_ACTION_STOP_SPECIFIC:
    break;
    default:
        res->status = 0x01;
    break;
    }

    res->service_status = GET_INSTANCE_CFG_STATUS()->get_service_status(SERVICE_NAME);
    res->action = req->action;

    return sizeof(struct res_service_t);
}


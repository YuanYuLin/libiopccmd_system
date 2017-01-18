#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "iopcdefine.h"
#include "iopcops_cfg_bdb_status.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_bdb_network.h"
#include "iopccmd_service.h"

#define SERVICE_NAME		"netdev"
#define SERVICE_WAIT		"mntbase"

static struct root_t dev_init_list[] = {
    INIT_NODE(ROOT_TYPE_CREATE_DIR, "/run/network", DEFAULT_MODE, 0 ),
};

static void split_vlan_ifcname(uint8_t** array, uint8_t* ifcname, const uint8_t* sep)
{
    char *strs = strtok(ifcname, sep);
    while(strs != NULL) {
	    *array++ = strs;
	    strs = strtok(NULL, sep);
    }
}

static void* action_start(void* arg)
{
    uint8_t cmd_str[STR_LEN] = {0};
    int i = 0;
    int j = 0;
    int dev_init_list_size = (sizeof(dev_init_list) / sizeof(dev_init_list[0]));
    uint8_t no_interfaces = 0;
    uint16_t str_len = 0;
    uint8_t ifc_name[STR_LEN];
    int is_up = 0;
    int is_setup_hwaddress = 0;
    uint8_t ifc_type = NETWORK_UNKNOW;
    uint8_t ifc_hwaddress[STR_LEN];
    uint8_t ifc_ipaddress[STR_LEN];
    uint8_t ifc_ipnetmask[STR_LEN];
    uint8_t no_of_consist_interfaces = 0;
    uint8_t consist_ifc[STR_LEN];
    uint8_t is_dhcp = 0;
    uint8_t is_setup_ipaddress = 0;
    uint8_t* vlan_ifc[2];
    pthread_t thread_id = pthread_self();
    printf("%s(%lu)\n", __func__, thread_id);

    printf("Starting Network Dev...\n");

    GET_INSTANCE(ops_cfg_bdb_status)->wait_service_started(SERVICE_WAIT);
    //printf("%s started\n", SERVICE_MOUNT_BASE);

    GET_INSTANCE(ops_misc)->create_system_by_list(dev_init_list_size, &dev_init_list[0]);

    GET_INSTANCE(ops_cfg_bdb_network)->loadall();

    no_interfaces = GET_INSTANCE(ops_cfg_bdb_network)->get_no_networks();
    printf("no ifcs:%d\n", no_interfaces);

    for(i=0;i<no_interfaces;i++) {
        memset(ifc_name, 0, STR_LEN);
        str_len = GET_INSTANCE(ops_cfg_bdb_network)->get_interface_name(i, &ifc_name[0]);
        
	is_up = GET_INSTANCE(ops_cfg_bdb_network)->is_up(i);
        
	ifc_type = GET_INSTANCE(ops_cfg_bdb_network)->get_type(i);

        is_setup_hwaddress = GET_INSTANCE(ops_cfg_bdb_network)->is_setup_hwaddress(i);

	memset(ifc_hwaddress, 0, STR_LEN);
        str_len = GET_INSTANCE(ops_cfg_bdb_network)->get_hwaddress(i, &ifc_hwaddress[0]);

        is_dhcp = GET_INSTANCE(ops_cfg_bdb_network)->is_dhcp(i);

        is_setup_ipaddress = GET_INSTANCE(ops_cfg_bdb_network)->is_setup_ipaddress(i);

	memset(ifc_ipaddress, 0, STR_LEN);
	str_len = GET_INSTANCE(ops_cfg_bdb_network)->get_ipaddress(i, &ifc_ipaddress[0]);

	memset(ifc_ipnetmask, 0, STR_LEN);
	str_len = GET_INSTANCE(ops_cfg_bdb_network)->get_ipnetmask(i, &ifc_ipnetmask[0]);

	printf("*************************\n");
	printf("%s,%d,%d,%d,%s,%d,%d,%s,%s\n", ifc_name, is_up, ifc_type, is_setup_hwaddress, ifc_hwaddress, is_dhcp, is_setup_ipaddress, ifc_ipaddress, ifc_ipnetmask);
	printf("-------------------------\n");
        if(is_up) {
            switch(ifc_type){
            case NETWORK_GENERIC:
            break;
	    case NETWORK_VLAN:
	    	memset(cmd_str, 0, STR_LEN);
		split_vlan_ifcname(vlan_ifc, ifc_name, ".");
		sprintf(cmd_str, "vconfig add %s %s", vlan_ifc[0], vlan_ifc[1]);
		GET_INSTANCE(ops_misc)->execute_cmd(cmd_str, NULL);
		printf("%s\n", cmd_str);
	    break;
            case NETWORK_BRIDGE:
                GET_INSTANCE(ops_misc)->execute_cmd("echo 1 > /proc/sys/net/ipv4/ip_forward", NULL);
                memset(cmd_str, 0, STR_LEN);
                sprintf(cmd_str, "brctl addbr %s", ifc_name);
                GET_INSTANCE(ops_misc)->execute_cmd(cmd_str, NULL);
                no_of_consist_interfaces = GET_INSTANCE(ops_cfg_bdb_network)->get_no_consist_interfaces(i);
                for(j=0;j<no_of_consist_interfaces;j++) {
                    memset(consist_ifc, 0, STR_LEN);
                    str_len = GET_INSTANCE(ops_cfg_bdb_network)->get_consist_interfaces(i, j, &consist_ifc[0]);
                    memset(cmd_str, 0, STR_LEN);
	            sprintf(cmd_str, "brctl addif %s %s", ifc_name, consist_ifc);
	            GET_INSTANCE(ops_misc)->execute_cmd(cmd_str, NULL);
	        }
            break;
            default:
            break;
            }

            if(is_setup_hwaddress) {
                memset(cmd_str, 0, STR_LEN);
		printf("%s\n", cmd_str);
                sprintf(cmd_str, "ifconfig %s hw ether %s", ifc_name, ifc_hwaddress);
                GET_INSTANCE(ops_misc)->execute_cmd(cmd_str, NULL);
            }

            memset(cmd_str, 0, STR_LEN);
            sprintf(cmd_str, "ip link set up dev %s", ifc_name);
            GET_INSTANCE(ops_misc)->execute_cmd(cmd_str, NULL);
	    printf("%s\n", cmd_str);

            while(1) {
		printf("%s is up:%d?\n", ifc_name, GET_INSTANCE(ops_misc)->is_netdev_up(ifc_name));

                if(GET_INSTANCE(ops_misc)->is_netdev_up(ifc_name) < 0) {
                    sleep(1);
                    printf("IFC: %s is not ready\n", ifc_name);
                    continue;
                }
                printf("IFC: %s is ready\n", ifc_name);
                break;
            } 

            if(is_dhcp) {
                memset(cmd_str, 0, STR_LEN);
                sprintf(cmd_str, "udhcpc -i %s -b -p /tmp/udhcpc.pid", ifc_name);
		printf("cmd:%s\n", cmd_str);
                GET_INSTANCE(ops_misc)->execute_cmd(cmd_str, NULL);
            } else {
                if(is_setup_ipaddress) {
			memset(cmd_str, 0, STR_LEN);
			sprintf(cmd_str, "ifconfig %s %s netmask %s", ifc_name, ifc_ipaddress, ifc_ipnetmask);
			GET_INSTANCE(ops_misc)->execute_cmd(cmd_str, NULL);
                }
            }
        }
    }
    sleep(1);

    if(0)
	    printf("%d\n", str_len);

    GET_INSTANCE(ops_cfg_bdb_status)->set_service_started(SERVICE_NAME);
    GET_INSTANCE(ops_cfg_bdb_status)->wait_service_stoped(SERVICE_NAME);

    pthread_exit((void*)0);
    return 0;
}

static void* action_stop(void* arg)
{

    GET_INSTANCE(ops_cfg_bdb_status)->set_service_stoped(SERVICE_NAME);
    return 0;
}

static void* action_restart(void* arg)
{
    return 0;
}

uint32_t hn_netdev(uint8_t* preq, uint8_t* pres)
{
    struct req_service_t* req = (struct req_service_t*)preq;
    struct res_service_t* res = (struct res_service_t*)pres;
    uint8_t action = req->action;

    switch(action) {
    case SERVICE_ACTION_STATUS:
    break;
    case SERVICE_ACTION_START:
        GET_INSTANCE(ops_cfg_bdb_status)->set_service_starting(SERVICE_NAME);
        GET_INSTANCE(ops_misc)->create_task(action_start);
    break;
    case SERVICE_ACTION_STOP:
        GET_INSTANCE(ops_cfg_bdb_status)->set_service_stoping(SERVICE_NAME);
        action_stop(NULL);
    break;
    case SERVICE_ACTION_RESTART:
        action_restart(NULL);
    break;
    default:
        res->status = 0x01;
    break;
    }

    res->service_status = GET_INSTANCE(ops_cfg_bdb_status)->get_service_status(SERVICE_NAME);
    res->action = req->action;

    return sizeof(struct res_service_t);
}


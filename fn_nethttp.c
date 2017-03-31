#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>

#include "iopcdefine.h"
#include "iopcops_misc.h"
#include "iopcops_cfg_status.h"
#include "iopccmd_service.h"

#define SERVICE_NAME		"nethttp"
#define SERVICE_WAIT		"netdev"

static struct MHD_Daemon *service = NULL;
#define HTTPD_FLAGS		MHD_USE_EPOLL_INTERNALLY_LINUX_ONLY | MHD_USE_EPOLL_LINUX_ONLY | MHD_USE_DEBUG
#define HTTPD_PORT		80
//#define HTTPD_IP		"192.168.200.254"
#define HTTPD_IP		"0.0.0.0"

#define FROM_UNKNOW		0
#define FROM_CALLBACK		1
#define FROM_BUFFER		2
#define METHOD_GET		"GET"
#define METHOD_PORT		"POST"
#define METHOD_ALL		"ALL"

struct www_desc_t {
    uint8_t response_action;
    uint8_t url[STR_LEN];
    MHD_ContentReaderCallback callback;
    MHD_ContentReaderFreeCallback callback_free;
};

struct www_kv_t {
    uint8_t* key;
    uint8_t* val;
};

struct www_obj_t {
    struct MHD_Connection* con;
    uint8_t* url;
    uint8_t* method;
//    struct www_kv_t* header;
//    struct www_kv_t* args;
};
#if 0
static int print_out_key (void *cls, enum MHD_ValueKind kind, const char *key, const char *value)
{
//    struct www_kv_t* head = (struct www_kv_t*)cls;
//    struct www_kv_t* ptr = 
#if 0
    while(1) {
        if(ptr == NULL) {
            ptr = malloc(sizeof(struct www_kv_t));
	    ptr->key = (uint8_t*)key;
	    ptr->val = (uint8_t*)value;
	    ptr->next = NULL;
	    break;
	}
        ptr = ptr->next;
    }
#endif
    printf ("%s: %s\n", key, value);
    return MHD_YES;
}
#endif
static void callback_free (void *ptr)
{
    if(ptr != NULL) 
        free(ptr);
}
#if 0
static ssize_t callback_info(void *cls, uint64_t pos, char *buf, size_t max)
{
    struct www_obj_t *www_obj = (struct www_obj_t *)cls;
    const char* val;
    int len = 0;
    if(pos == 0) {
//        memset(buf, 0, max);
	val = MHD_lookup_connection_value (www_obj->con, MHD_GET_ARGUMENT_KIND, "a");
        sprintf(buf, "<html><body>Debug Info:</br>url:%s</br>method:%s</br>a:%s</body></html>", www_obj->url, www_obj->method, val);
        printf("====================\n");
	len = strlen(buf);
        printf("len %d, %ld, %ld\n", len, pos, max);
        printf("--------------------\n");
        return len;
    } else {
        return MHD_CONTENT_READER_END_OF_STREAM;
    }
}
#endif
static ssize_t callback_vmlist(void* cls, uint64_t pos, char *buf, size_t max)
{
    struct www_obj_t *www_obj = (struct www_obj_t *)cls;
    const char* val;
    int len = 0;
    if(pos == 0) {
	    val = MHD_lookup_connection_value (www_obj->con, MHD_GET_ARGUMENT_KIND, "a");
	    sprintf(buf, "<html><body>Debug Info:</br>url:%s</br>method:%s</br>a:%s</body></html>", www_obj->url, www_obj->method, val);
	    len = strlen(buf);
        return len;
    } else {
        return MHD_CONTENT_READER_END_OF_STREAM;
    }
}

#define CALLBACK_HTML(CALLBACK_FN, HTML_PATH)					\
static ssize_t CALLBACK_FN(void *cls, uint64_t pos, char *buf, size_t max)	\
{										\
	ssize_t size = 0;							\
	size = GET_INSTANCE_MISC_OBJ()->get_text(HTML_PATH, buf, pos, max);	\
	if(size == 0) {								\
		return MHD_CONTENT_READER_END_OF_STREAM;			\
	}									\
	return size;								\
}
#if 0
#define WWW_FILE(PATH)		"/usr/local/www" PATH

CALLBACK_HTML(callback_js_jquery,	WWW_FILE("/js/common/jquery.js"))
CALLBACK_HTML(callback_js_underscore,	WWW_FILE("/js/common/underscore.js"))
CALLBACK_HTML(callback_js_backbone,	WWW_FILE("/js/common/backbone.js"))
CALLBACK_HTML(callback_js_index,	WWW_FILE("/js/index.js"))
CALLBACK_HTML(callback_js_com_menu,	WWW_FILE("/js/lib/com_menu.js"))
CALLBACK_HTML(callback_js_helloworld,	WWW_FILE("/js/lib/com_helloworld.js"))
CALLBACK_HTML(callback_js_routes,	WWW_FILE("/js/lib/routes.js"))
CALLBACK_HTML(callback_js_util_debug,	WWW_FILE("/js/lib/util_debug.js"))
CALLBACK_HTML(callback_js_route_about,	WWW_FILE("/js/lib/route_about.js"))
CALLBACK_HTML(callback_js_com_titato,	WWW_FILE("/js/plugins/titato/com_titato.js"))
CALLBACK_HTML(callback_js_route_home,	WWW_FILE("/js/plugins/titato/route_home.js"))
CALLBACK_HTML(callback_js_model_titato,	WWW_FILE("/js/plugins/titato/model_titato.js"))
CALLBACK_HTML(callback_js_require,	WWW_FILE("/js/require.js"))
CALLBACK_HTML(callback_js_app,		WWW_FILE("/app.js"))
CALLBACK_HTML(callback_html_index,	WWW_FILE("/index.html"))

struct www_desc_t www_desc[] = {
    { FROM_CALLBACK, "/info", 			callback_info,		callback_free},
    { FROM_CALLBACK, "/js/common/jquery.js",	callback_js_jquery,	callback_free},
    { FROM_CALLBACK, "/js/common/underscore.js",callback_js_underscore,	callback_free},
    { FROM_CALLBACK, "/js/common/backbone.js", 	callback_js_backbone,	callback_free},
    { FROM_CALLBACK, "/js/index.js", 		callback_js_index,	callback_free},
    { FROM_CALLBACK, "/js/lib/com_menu.js",	callback_js_com_menu,	callback_free},
    { FROM_CALLBACK, "/js/lib/com_helloworld.js",callback_js_helloworld,callback_free},
    { FROM_CALLBACK, "/js/lib/routes.js",	callback_js_routes,	callback_free},
    { FROM_CALLBACK, "/js/lib/util_debug.js",	callback_js_util_debug,	callback_free},
    { FROM_CALLBACK, "/js/lib/route_about.js",	callback_js_route_about,callback_free},
    { FROM_CALLBACK, "/js/plugins/titato/com_titato.js",callback_js_com_titato,callback_free},
    { FROM_CALLBACK, "/js/plugins/titato/route_home.js",callback_js_route_home,callback_free},
    { FROM_CALLBACK, "/js/plugins/titato/model_titato.js",callback_js_model_titato,callback_free},
    { FROM_CALLBACK, "/js/require.js",		callback_js_require,	callback_free},
    { FROM_CALLBACK, "/app.js",			callback_js_app,	callback_free},
    { FROM_CALLBACK, "/",			callback_html_index,	callback_free},
    { FROM_UNKNOW, "", NULL, NULL },
};
#else
struct www_desc_t www_desc[] = {
    { FROM_CALLBACK, "/api/vmlist",		callback_vmlist,	callback_free },
};
#endif
static int httpd_handler(void *cls, struct MHD_Connection *con, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **ptr)
{
    static int aptr;
    struct MHD_Response *response;
    int ret;
    struct www_desc_t* www_ptr = NULL;
    struct www_obj_t* www_obj = NULL;
    int www_desc_size = sizeof(www_desc)/sizeof(www_desc[0]);
    int i = 0;
    ssize_t buf_len = 0;
    uint8_t buf[MAX_STR_VAL];

    if (&aptr != *ptr) {
        *ptr = &aptr;
        return MHD_YES;
    }

    *ptr = NULL;

    www_obj = malloc(sizeof(struct www_obj_t));
    www_obj->con = con;
    www_obj->url = (uint8_t*)url;
    www_obj->method = (uint8_t*)method;

    for(i=0;i<www_desc_size;i++) {
        www_ptr = &www_desc[i];
        if((strcmp(www_ptr->url, url) == 0)) {

	    switch(www_ptr->response_action) {
            case FROM_CALLBACK:
                response = MHD_create_response_from_callback (MHD_SIZE_UNKNOWN, 1024, www_ptr->callback, www_obj, www_ptr->callback_free);
            break;
	    case FROM_BUFFER:
	        memset(buf, 0, MAX_STR_VAL);
	        buf_len = www_ptr->callback(NULL, 0, &buf[0], MAX_STR_VAL);
		//response = MHD_create_response_from_buffer(buf_len, (void*)buf, MHD_RESPMEM_PERSISTENT);
		response = MHD_create_response_from_buffer(buf_len, (void*)buf, MHD_RESPMEM_MUST_FREE);
	    break;
	    default:
                response = NULL;
		return MHD_NO;
	    break;
	    }

	    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "text/html; charset=utf-8");
            ret = MHD_queue_response (con, MHD_HTTP_OK, response);
            MHD_destroy_response (response);
	    return ret;
	}
    }

    return MHD_NO;
}

static void* action_start(void* arg)
{
    GET_INSTANCE_CFG_STATUS()->wait_service_started(SERVICE_WAIT);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(HTTPD_PORT);
    inet_pton(AF_INET, HTTPD_IP, &(address.sin_addr.s_addr));

    printf("Starting network HTTP...[%s:%d]\n", HTTPD_IP, HTTPD_PORT);
    service = MHD_start_daemon(HTTPD_FLAGS, 
		    htons(HTTPD_PORT), 
		    NULL, NULL, 
		    &httpd_handler, NULL, 
		    MHD_OPTION_SOCK_ADDR, (struct sockaddr*) &address,
		    MHD_OPTION_END);

    GET_INSTANCE_CFG_STATUS()->set_service_started(SERVICE_NAME);
    GET_INSTANCE_CFG_STATUS()->wait_service_stoped(SERVICE_NAME);

    pthread_exit((void*)0);
    return 0;
}

static void* action_stop(void* arg)
{

    GET_INSTANCE_CFG_STATUS()->set_service_stoped(SERVICE_NAME);
    return 0;
}


static void* action_restart(void* arg)
{
}

uint32_t hn_nethttp(uint8_t* preq, uint8_t* pres)
{
    struct req_service_t* req = (struct req_service_t*)preq;
    struct res_service_t* res = (struct res_service_t*)pres;
    uint8_t action = req->action;

    switch(action) {
    case SERVICE_ACTION_STATUS:
    break;
    case SERVICE_ACTION_START:
        GET_INSTANCE_CFG_STATUS()->set_service_starting(SERVICE_NAME);
        GET_INSTANCE_MISC_OBJ()->create_task(action_start);
        //action_start(NULL);
    break;
    case SERVICE_ACTION_STOP:
        GET_INSTANCE_CFG_STATUS()->set_service_stoping(SERVICE_NAME);
        action_stop(NULL);
    break;
    case SERVICE_ACTION_RESTART:
        action_restart(NULL);
    break;
    default:
        res->status = 0x01;
    break;
    }

    res->action = req->action;
    res->service_status = GET_INSTANCE_CFG_STATUS()->get_service_status(SERVICE_NAME);

    return sizeof(struct res_service_t);
}


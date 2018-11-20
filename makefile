all: main.c update.o JSON_action.o JSON_request_handle.o config.o update_local.o JSON_action_local.o config_local.o JSON_request_handle_local.o
	mipsel-openwrt-linux-gcc -std=c99 -I./include main.c update.o JSON_action.o JSON_request_handle.o config.o -o ap_client
	gcc -std=c99 -I./include main.c update_local.o JSON_action_local.o JSON_request_handle_local.o config_local.o -o ap_client_local
	gcc -std=c99 -I./include test.c JSON_request_handle_local.o -o test_local
	rm *.o
update.o: update.c
	mipsel-openwrt-linux-gcc -std=c99 -I./include -c update.c -o update.o
	gcc -std=c99 -I./include -c update.c -o update_local.o
JSON_action.o: JSON_action.c
	mipsel-openwrt-linux-gcc -std=c99 -I./include -c JSON_action.c -o JSON_action.o
	gcc -std=c99 -I./include -c JSON_action.c -o JSON_action_local.o
JSON_request_handle.o: JSON_request_handle.c
	mipsel-openwrt-linux-gcc -std=c99 -I./include -c JSON_request_handle.c -o JSON_request_handle.o
	gcc -std=c99 -I./include -c JSON_request_handle.c -o JSON_request_handle_local.o
config.o: config.c
	mipsel-openwrt-linux-gcc -std=c99 -I./include -c config.c -o config.o
	gcc -std=c99 -I./include -c config.c -o config_local.o

SELF_DIR := ${CURDIR}

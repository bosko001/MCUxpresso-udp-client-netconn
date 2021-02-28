/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/netifapi.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include <string.h>
#include <stdio.h>

#include "dnet.h"

#define BUFLEN 128

static void
udp_client_thread(void *arg)
{
	struct netconn* conn = netconn_new(NETCONN_UDP);

	struct netbuf* buf = netbuf_new();

	ip_addr_t address;
	ipaddr_aton("192.168.0.132", &address);

	netbuf_set_fromaddr(buf, &address);
	netbuf_fromport(buf) = 12345;

	char msg[32];

	for (int i = 1; ; ++i) {
		sprintf(msg, "hello(%d)", i);

		netbuf_ref(buf, (void*)msg, strlen(msg));

		PRINTF("Sending: %s\n\r", msg);
		netconn_send(conn, buf);

		vTaskDelay(2000);
	}

	netbuf_delete(buf);
	netconn_close(conn);
}
/*-----------------------------------------------------------------------------------*/

/*!
 * @brief Main function
 */
int main(void)
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    PRINTF("Start\n\r");

    DnetConfig net_config = dnet_init("192.168.0.145", NULL, NULL, dnet_get_uid_location());

    dnet_start_new_thread("udp_client_thread", udp_client_thread, NULL);

    vTaskStartScheduler();

    return 0;
}
#endif

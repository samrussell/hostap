/*
 * Example application showing how EAP peer and server code from
 * wpa_supplicant/hostapd can be used as a library. This example program
 * initializes both an EAP server and an EAP peer entities and then runs
 * through an EAP-PEAP/MSCHAPv2 authentication.
 * Copyright (c) 2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"

#include "czmq.h"


int eap_example_peer_init(void);
void eap_example_peer_deinit(void);
int eap_example_peer_step(void);

int eap_example_server_init(void);
void eap_example_server_deinit(void);
int eap_example_server_step_gary(zsock_t*);

void eap_example_server_rx(const u8 *data, size_t data_len);

int main(int argc, char *argv[])
{
	int res_s, res_p;
	int i;
	char text[100]; // terrible way to do this
	char message[100];
	char* instr = NULL;
	zsock_t *sock;

	wpa_debug_level = 0;
	sock = zsock_new_pair("@tcp://127.0.0.1:4567");
	zsock_set_rcvtimeo(sock, 1000);

	if (eap_example_peer_init() < 0 ||
	    eap_example_server_init() < 0)
		return -1;

	do {
		printf("---[ server ]--------------------------------\n");
		res_s = eap_example_server_step_gary(sock);
		printf("---[ peer ]----------------------------------\n");
		//res_p = eap_example_peer_step();
		//scanf("%s", text);
		instr = zstr_recv(sock);
		if (instr == NULL){
			printf("Timeout, doing step anyway\n");
		}
		else{
			printf("received data %s\n", instr);
			// convert to data and pass
			for(i=0; i<strlen(instr)/2; i++){
				sscanf(&instr[i*2], "%2X", &message[i]);
			}
			zstr_free(instr);
			eap_example_server_rx(message, i);
		}


	} while (1);
	//} while (res_s || res_p);

	eap_example_peer_deinit();
	eap_example_server_deinit();

	return 0;
}

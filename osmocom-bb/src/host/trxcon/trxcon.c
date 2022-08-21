/*
 * OsmocomBB <-> SDR connection bridge
 *
 * (C) 2016-2019 by Vadim Yanitskiy <axilirator@gmail.com>
 *
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include <arpa/inet.h>

#include <osmocom/core/fsm.h>
#include <osmocom/core/msgb.h>
#include <osmocom/core/talloc.h>
#include <osmocom/core/signal.h>
#include <osmocom/core/select.h>
#include <osmocom/core/application.h>

#include <osmocom/gsm/gsm_utils.h>

#include "trxcon.h"
#include "trx_if.h"
#include "logging.h"
#include "l1ctl.h"
#include "l1ctl_link.h"
#include "l1ctl_proto.h"
#include "scheduler.h"
#include "sched_trx.h"

#define COPYRIGHT \
	"Copyright (C) 2016-2017 by Vadim Yanitskiy <axilirator@gmail.com>\n" \
	"License GPLv2+: GNU GPL version 2 or later " \
	"<http://gnu.org/licenses/gpl.html>\n" \
	"This is free software: you are free to change and redistribute it.\n" \
	"There is NO WARRANTY, to the extent permitted by law.\n\n"

static struct {
	const char *debug_mask;
	int daemonize;
	int quit;

	/* L1CTL specific */
	const char *bind_socket;

	/* TRX specific */
	const char *trx_bind_ip;
	const char *trx_remote_ip;
	uint16_t trx_base_port;
	uint32_t trx_fn_advance;
} app_data;

static void *tall_trxcon_ctx = NULL;

static void print_usage(const char *app)
{
	printf("Usage: %s\n", app);
}

static void print_help(void)
{
	printf(" Some help...\n");
	printf("  -h --help         this text\n");
	printf("  -d --debug        Change debug flags. Default: %s\n", DEBUG_DEFAULT);
	printf("  -b --trx-bind     TRX bind IP address (default 0.0.0.0)\n");
	printf("  -i --trx-remote   TRX remote IP address (default 127.0.0.1)\n");
	printf("  -p --trx-port     Base port of TRX instance (default 6700)\n");
	printf("  -f --trx-advance  Scheduler clock advance (default 20)\n");
	printf("  -s --socket       Listening socket for layer23 (default /tmp/osmocom_l2)\n");
	printf("  -D --daemonize    Run as daemon\n");
}

static void handle_options(int argc, char **argv)
{
	while (1) {
		int option_index = 0, c;
		static struct option long_options[] = {
			{"help", 0, 0, 'h'},
			{"debug", 1, 0, 'd'},
			{"socket", 1, 0, 's'},
			{"trx-bind", 1, 0, 'b'},
			/* NOTE: 'trx-ip' is now an alias for 'trx-remote'
			 * due to backward compatibility reasons! */
			{"trx-ip", 1, 0, 'i'},
			{"trx-remote", 1, 0, 'i'},
			{"trx-port", 1, 0, 'p'},
			{"trx-advance", 1, 0, 'f'},
			{"daemonize", 0, 0, 'D'},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "d:b:i:p:f:s:Dh",
				long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			print_usage(argv[0]);
			print_help();
			exit(0);
			break;
		case 'd':
			app_data.debug_mask = optarg;
			break;
		case 'b':
			app_data.trx_bind_ip = optarg;
			break;
		case 'i':
			app_data.trx_remote_ip = optarg;
			break;
		case 'p':
			app_data.trx_base_port = atoi(optarg);
			break;
		case 'f':
			app_data.trx_fn_advance = atoi(optarg);
			break;
		case 's':
			app_data.bind_socket = optarg;
			break;
		case 'D':
			app_data.daemonize = 1;
			break;
		default:
			break;
		}
	}
}

static void init_defaults(void)
{
	app_data.bind_socket = "/tmp/osmocom_l2";
	app_data.trx_remote_ip = "127.0.0.1";
	app_data.trx_bind_ip = "0.0.0.0";
	app_data.trx_base_port = 6700;
	app_data.trx_fn_advance = 20;

	app_data.debug_mask = NULL;
	app_data.daemonize = 0;
	app_data.quit = 0;
}

static void signal_handler(int signal)
{
	fprintf(stderr, "signal %u received\n", signal);

	switch (signal) {
	case SIGINT:
		app_data.quit++;
		break;
	case SIGABRT:
	case SIGUSR1:
	case SIGUSR2:
		talloc_report_full(tall_trxcon_ctx, stderr);
		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
	struct trxcon_inst *trxcon = NULL;
	int rc = 0;

	printf("%s", COPYRIGHT);
	init_defaults();
	handle_options(argc, argv);

	/* Track the use of talloc NULL memory contexts */
	talloc_enable_null_tracking();

	/* Init talloc memory management system */
	tall_trxcon_ctx = talloc_init("trxcon context");
	msgb_talloc_ctx_init(tall_trxcon_ctx, 0);

	/* Setup signal handlers */
	signal(SIGINT, &signal_handler);
	signal(SIGUSR1, &signal_handler);
	signal(SIGUSR2, &signal_handler);
	osmo_init_ignore_signals();

	/* Init logging system */
	trx_log_init(tall_trxcon_ctx, app_data.debug_mask);

	/* Allocate a single trxcon instance */
	trxcon = talloc_zero(tall_trxcon_ctx, struct trxcon_inst);
	if (trxcon == NULL) {
		LOGP(DAPP, LOGL_ERROR, "Failed to allocate a trxcon instance\n");
		goto exit;
	}

	/* Allocate an associated trxcon state machine */
	trxcon->fi = osmo_fsm_inst_alloc(&trxcon_fsm_def,
		tall_trxcon_ctx, trxcon, LOGL_DEBUG, "main");
	if (trxcon->fi == NULL) {
		LOGP(DAPP, LOGL_ERROR, "Failed to allocate trxcon FSM\n");
		goto exit;
	}

	/* Init L1CTL server */
	trxcon->l1l = l1ctl_link_init(trxcon->fi,
		app_data.bind_socket);
	if (trxcon->l1l == NULL)
		goto exit;

	/* Init transceiver interface */
	trxcon->trx = trx_if_open(trxcon->fi,
		app_data.trx_bind_ip, app_data.trx_remote_ip,
		app_data.trx_base_port);
	if (trxcon->trx == NULL)
		goto exit;

	/* Bind L1CTL with TRX and vice versa
	 * TODO: get rid of this, they should be abstracted */
	trxcon->trx->l1l = trxcon->l1l;

	/* Init scheduler */
	rc = sched_trx_init(trxcon->trx, app_data.trx_fn_advance);
	if (rc)
		goto exit;

	LOGP(DAPP, LOGL_NOTICE, "Init complete\n");

	if (app_data.daemonize) {
		rc = osmo_daemonize();
		if (rc < 0) {
			perror("Error during daemonize");
			goto exit;
		}
	}

	/* Initialize pseudo-random generator */
	srand(time(NULL));

	while (!app_data.quit)
		osmo_select_main(0);

exit:
	if (trxcon != NULL) {
		/* Shutdown scheduler */
		sched_trx_shutdown(trxcon->trx);

		/* Close active connections */
		l1ctl_link_shutdown(trxcon->l1l);
		trx_if_close(trxcon->trx);

		/* Shutdown main state machine */
		if (trxcon->fi != NULL)
			osmo_fsm_inst_free(trxcon->fi);

		/* Release trxcon instance */
		talloc_free(trxcon);
	}

	/* Deinitialize logging */
	log_fini();

	/**
	 * Print report for the root talloc context in order
	 * to be able to find and fix potential memory leaks.
	 */
	talloc_report_full(tall_trxcon_ctx, stderr);
	talloc_free(tall_trxcon_ctx);

	/* Make both Valgrind and ASAN happy */
	talloc_report_full(NULL, stderr);
	talloc_disable_null_tracking();

	return rc;
}

/*
 * OsmocomBB <-> SDR connection bridge
 * trxcon state machine definition
 *
 * (C) 2019 by Vadim Yanitskiy <axilirator@gmail.com>
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

#include <osmocom/core/utils.h>
#include <osmocom/core/fsm.h>

#include <logging.h>
#include <trxcon.h>
#include <l1ctl.h>

static void trxcon_fsm_idle_action(struct osmo_fsm_inst *fi,
	uint32_t event, void *data)
{
	switch ((enum trxcon_event_type) event) {
	case TRXCON_EV_L1CTL_CONNECT:
		osmo_fsm_inst_state_chg(fi, TRXCON_ST_MANAGED, 0, 0);
		break;
	default:
		LOGPFSML(fi, LOGL_ERROR, "Unhandled event '%s'\n",
			osmo_fsm_event_name(&trxcon_fsm_def, event));
	}
}

static void trxcon_fsm_managed_action(struct osmo_fsm_inst *fi,
	uint32_t event, void *data)
{
	struct trxcon_inst *trxcon = (struct trxcon_inst *) fi->priv;

	switch ((enum trxcon_event_type) event) {
	case TRXCON_EV_L1CTL_DISCONNECT:
		/* TODO: reset TRX interface */
		osmo_fsm_inst_state_chg(fi, TRXCON_ST_IDLE, 0, 0);
		break;
	case TRXCON_EV_TRX_DISCONNECT:
		/* TODO: notify L1CTL interface */
		osmo_fsm_inst_state_chg(fi, TRXCON_ST_IDLE, 0, 0);
		break;
	case TRXCON_EV_L1CTL_REQ:
		OSMO_ASSERT(data != NULL);
		l1ctl_rx_cb(trxcon, (struct msgb *) data);
		break;
	default:
		LOGPFSML(fi, LOGL_ERROR, "Unhandled event '%s'\n",
			osmo_fsm_event_name(&trxcon_fsm_def, event));
	}
}

/* Generates mask for a single state or event */
#define S(x) (1 << x)

static const struct osmo_fsm_state trxcon_fsm_states[] = {
	[TRXCON_ST_IDLE] = {
		.name = "IDLE",
		.out_state_mask = S(TRXCON_ST_MANAGED),
		.in_event_mask  = S(TRXCON_EV_L1CTL_CONNECT),
		.action = &trxcon_fsm_idle_action,
	},
	[TRXCON_ST_MANAGED] = {
		.name = "MANAGED",
		.out_state_mask = S(TRXCON_ST_IDLE),
		.in_event_mask  = S(TRXCON_EV_L1CTL_DISCONNECT)
				| S(TRXCON_EV_L1CTL_CONNECT)
				| S(TRXCON_EV_L1CTL_REQ)
				| S(TRXCON_EV_TRX_DISCONNECT)
				| S(TRXCON_EV_TRX_CTRL_ERROR)
				| S(TRXCON_EV_TRX_CTRL_RSP)
				| S(TRXCON_EV_TRX_DL_BURST)
				| S(TRXCON_EV_SCHED_CLCK_LOSS)
				| S(TRXCON_EV_SCHED_DL_DF_IND)
				| S(TRXCON_EV_SCHED_UL_DF_CONF)
				| S(TRXCON_EV_SCHED_DL_TF_IND)
				| S(TRXCON_EV_SCHED_UL_TF_CONF)
				,
		.action = &trxcon_fsm_managed_action,
	},
};

const struct value_string trxcon_fsm_event_names[] = {
	OSMO_VALUE_STRING(TRXCON_EV_L1CTL_DISCONNECT),
	OSMO_VALUE_STRING(TRXCON_EV_L1CTL_CONNECT),
	OSMO_VALUE_STRING(TRXCON_EV_L1CTL_REQ),

	OSMO_VALUE_STRING(TRXCON_EV_TRX_DISCONNECT),
	OSMO_VALUE_STRING(TRXCON_EV_TRX_CTRL_ERROR),
	OSMO_VALUE_STRING(TRXCON_EV_TRX_CTRL_RSP),
	OSMO_VALUE_STRING(TRXCON_EV_TRX_DL_BURST),

	OSMO_VALUE_STRING(TRXCON_EV_SCHED_CLCK_LOSS),
	OSMO_VALUE_STRING(TRXCON_EV_SCHED_DL_DF_IND),
	OSMO_VALUE_STRING(TRXCON_EV_SCHED_UL_DF_CONF),
	OSMO_VALUE_STRING(TRXCON_EV_SCHED_DL_TF_IND),
	OSMO_VALUE_STRING(TRXCON_EV_SCHED_UL_TF_CONF),

	{ 0, NULL }
};

struct osmo_fsm trxcon_fsm_def = {
	.name = "trxcon_fsm",
	.states = trxcon_fsm_states,
	.num_states = ARRAY_SIZE(trxcon_fsm_states),
	.event_names = trxcon_fsm_event_names,
	.log_subsys = DAPP,
};

static __attribute__((constructor)) void on_dso_load(void)
{
	/* Register our FSM */
	OSMO_ASSERT(osmo_fsm_register(&trxcon_fsm_def) == 0);
}

#pragma once

#include <osmocom/core/utils.h>
#include <osmocom/core/fsm.h>

#define GEN_MASK(state) (0x01 << state)

extern const struct value_string trxcon_fsm_event_names[];
extern struct osmo_fsm trxcon_fsm_def;

enum trxcon_fsm_state_type {
	TRXCON_ST_IDLE = 0,
	TRXCON_ST_MANAGED,
};

enum trxcon_event_type {
	/* Events coming from L1CTL interface */
	TRXCON_EV_L1CTL_DISCONNECT,	/*!< L1CTL interface has lost a connection */
	TRXCON_EV_L1CTL_CONNECT,	/*!< L1CTL interface has got a connection */
	TRXCON_EV_L1CTL_REQ,		/*!< L1CTL request message from a client */

	/* Events coming from TRX interface */
	TRXCON_EV_TRX_DISCONNECT,	/*!< TRX interface has lost connection */
	TRXCON_EV_TRX_CTRL_ERROR,	/*!< TRXC (CTRL) error from transceiver */
	TRXCON_EV_TRX_CTRL_RSP,		/*!< TRXC (CTRL) response from transceiver */
	TRXCON_EV_TRX_DL_BURST,		/*!< TRXD (DATA) DL burst from transceiver */

	/* Events coming from scheduler */
	TRXCON_EV_SCHED_CLCK_LOSS,	/*!< No more clock from transceiver */
	TRXCON_EV_SCHED_DL_DF_IND,	/*!< DL Data Frame INDication */
	TRXCON_EV_SCHED_UL_DF_CONF,	/*!< UL Data Frame CONFirmation */
	TRXCON_EV_SCHED_DL_TF_IND,	/*!< DL Traffic Frame INDication */
	TRXCON_EV_SCHED_UL_TF_CONF,	/*!< UL Traffic Frame CONFirmation */
};

/* Represents a single L1CTL <-> TRX connection */
struct trxcon_inst {
	/* Associated instance of trxcon_fsm */
	struct osmo_fsm_inst *fi;

	/* Connection with transceiver */
	struct trx_instance *trx;
	/* L1CTL server */
	struct l1ctl_link *l1l;
};

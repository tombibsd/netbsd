/*
 * Copyright (c) 1997, 2000 Hellmuth Michaelis. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *---------------------------------------------------------------------------
 *
 *	i4b_l2if.c - Layer 3 interface to Layer 2
 *	-------------------------------------------
 *
 *	$Id$
 *
 * $FreeBSD$
 *
 *      last edit-date: [Fri Jan  5 11:33:47 2001]
 *
 *---------------------------------------------------------------------------*/

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#ifdef __FreeBSD__
#include "i4bq931.h"
#else
#define	NI4BQ931	1
#endif
#if NI4BQ931 > 0

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#if defined(__NetBSD__) && __NetBSD_Version__ >= 104230000
#include <sys/callout.h>
#endif

#ifdef __FreeBSD__
#include <machine/i4b_debug.h>
#include <machine/i4b_ioctl.h>
#include <machine/i4b_cause.h>
#else
#include <netisdn/i4b_debug.h>
#include <netisdn/i4b_ioctl.h>
#include <netisdn/i4b_cause.h>
#endif

#include <netisdn/i4b_isdnq931.h>
#include <netisdn/i4b_l3l4.h>
#include <netisdn/i4b_mbuf.h>

#include <netisdn/i4b_l2.h>
#include <netisdn/i4b_l1l2.h>
#include <netisdn/i4b_l3.h>
#include <netisdn/i4b_l3fsm.h>
#include <netisdn/i4b_q931.h>

#include <netisdn/i4b_l4.h>

static unsigned char make_q931_cause(cause_t cause);

/*---------------------------------------------------------------------------*
 * this converts our internal state (number) to the number specified
 * in Q.931 and is used for reporting our state in STATUS messages.
 *---------------------------------------------------------------------------*/
int i4b_status_tab[] = {
	0,	/*	ST_U0,	*/
	1,	/*	ST_U1,	*/
	3,	/*	ST_U3,	*/
	4,	/*	ST_U4,	*/
	6,	/*	ST_U6,	*/
	7,	/*	ST_U7,	*/
	8,	/*	ST_U8,	*/
	9,	/*	ST_U9,	*/
	10,	/*	ST_U10,	*/
	11,	/*	ST_U11,	*/
	12,	/*	ST_U12,	*/
	19,	/*	ST_U19,	*/
	6,	/*	ST_IWA,	*/
	6,	/*	ST_IWR,	*/
	1,	/*	ST_OW,	*/
	6,	/*	ST_IWL,	*/
};

/*---------------------------------------------------------------------------*
 *	return a valid q.931/q.850 cause from any of the internal causes
 *---------------------------------------------------------------------------*/
static unsigned char
make_q931_cause(cause_t cause)
{
	register unsigned char ret;

	switch(GET_CAUSE_TYPE(cause))
	{
		case CAUSET_Q850:
			ret = GET_CAUSE_VAL(cause);
			break;
		case CAUSET_I4B:
			ret = cause_tab_q931[GET_CAUSE_VAL(cause)];
			break;
		default:
			panic("make_q931_cause: unknown cause type!");
			break;
	}
	ret |= EXT_LAST;
	return(ret);
}

/*---------------------------------------------------------------------------*
 *	return status of data link
 *---------------------------------------------------------------------------*/
int
i4b_get_dl_stat(call_desc_t *cd)
{
	const struct isdn_l3_driver * drv = cd->l3drv;
	return drv->dl_est;
}

/*---------------------------------------------------------------------------*
 *	DL ESTABLISH INDICATION from Layer 2
 *---------------------------------------------------------------------------*/
int
i4b_dl_establish_ind(struct isdn_l3_driver * drv)
{
	int i, found;
	NDBGL2(L2_PRIM, "DL-ESTABLISH-IND isdnif %d", drv->isdnif);

	/* first set DL up in controller descriptor */
	NDBGL3(L3_MSG, "isdnif %d DL established!", drv->isdnif);
	drv->dl_est = DL_UP;

	found = 0;

	/* second, inform all (!) active call of the event */

	for(i=0; i < num_call_desc; i++)
	{
		if( (call_desc[i].cdid != 0)
		    && call_desc[i].isdnif == drv->isdnif) {
			next_l3state(&call_desc[i], EV_DLESTIN);
			found++;
		}
	}

	if(found == 0)
	{
		NDBGL3(L3_ERR, "ERROR, no cdid for isdnif %d found!",
		    drv->isdnif);
		return(-1);
	}
	else
	{
		return(0);
	}
}

/*---------------------------------------------------------------------------*
 *	DL ESTABLISH CONFIRM from Layer 2
 *---------------------------------------------------------------------------*/
int
i4b_dl_establish_cnf(struct isdn_l3_driver * drv)
{
	int i;
	int found = 0;

	NDBGL2(L2_PRIM, "DL-ESTABLISH-CONF isdnif %d", drv->isdnif);

	for(i=0; i < num_call_desc; i++)
	{
		if(call_desc[i].cdid != 0
		    && call_desc[i].isdnif == drv->isdnif) {
			drv->dl_est = DL_UP;
			next_l3state(&call_desc[i], EV_DLESTCF);
			found++;
		}
	}

	if(found == 0)
	{
		NDBGL3(L3_ERR, "ERROR, no cdid for isdnif %d found!",
		    drv->isdnif);
		return(-1);
	}
	else
	{
		return(0);
	}
}

/*---------------------------------------------------------------------------*
 *	DL RELEASE INDICATION from Layer 2
 *---------------------------------------------------------------------------*/
int
i4b_dl_release_ind(struct isdn_l3_driver * drv)
{
	int i;
	int found = 0;

	NDBGL2(L2_PRIM, "DL-RELEASE-IND isdnif %d", drv->isdnif);

	/* first set controller to down */

	drv->dl_est = DL_DOWN;

	found = 0;

	/* second, inform all (!) active calls of the event */

	for(i=0; i < num_call_desc; i++)
	{
		if(call_desc[i].cdid != 0
		    && call_desc[i].isdnif == drv->isdnif) {
			next_l3state(&call_desc[i], EV_DLRELIN);
			found++;
		}
	}

	if(found == 0)
	{
		/* this is not an error since it might be a normal call end */
		NDBGL3(L3_MSG, "no cdid for isdnif %d found", drv->isdnif);
	}
	return(0);
}

/*---------------------------------------------------------------------------*
 *	DL RELEASE CONFIRM from Layer 2
 *---------------------------------------------------------------------------*/
int
i4b_dl_release_cnf(struct isdn_l3_driver * drv)
{
	NDBGL2(L2_PRIM, "DL-RELEASE-CONF isdnif %d", drv->isdnif);

	drv->dl_est = DL_DOWN;
	return(0);
}

/*---------------------------------------------------------------------------*
 *	i4b_dl_data_ind - process a rx'd I-frame got from layer 2
 *---------------------------------------------------------------------------*/
int
i4b_dl_data_ind(struct isdn_l3_driver *drv, struct mbuf *m)
{
	i4b_decode_q931(drv->isdnif, m->m_len, m->m_data);
	i4b_Dfreembuf(m);
	return(0);
}

/*---------------------------------------------------------------------------*
 *	dl_unit_data_ind - process a rx'd U-frame got from layer 2
 *---------------------------------------------------------------------------*/
int
i4b_dl_unit_data_ind(struct isdn_l3_driver *drv, struct mbuf *m)
{
	i4b_decode_q931(drv->isdnif, m->m_len, m->m_data);
	i4b_Dfreembuf(m);
	return(0);
}

/*---------------------------------------------------------------------------*
 *	send CONNECT message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_connect(call_desc_t *cd)
{
	struct l2_softc *l2sc = (l2_softc_t*)cd->l3drv->l1_token;
	struct mbuf *m;
	u_char *ptr;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if((m = i4b_Dgetmbuf(I_FRAME_HDRLEN + MSG_CONNECT_LEN)) == NULL)
		panic("i4b_l3_tx_connect: can't allocate mbuf");

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = CONNECT;		/* message type = connect */

	i4b_dl_data_req(l2sc, l2sc->drv, m);
}

/*---------------------------------------------------------------------------*
 *	send RELEASE COMPLETE message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_release_complete(call_desc_t *cd, int send_cause_flag)
{
	struct l2_softc *l2sc = (l2_softc_t*)cd->l3drv->l1_token;
	struct mbuf *m;
	u_char *ptr;
	int len = I_FRAME_HDRLEN + MSG_RELEASE_COMPLETE_LEN;

	if(send_cause_flag == 0)
	{
		len -= 4;
		NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x",
			cd->isdnif, cd->cr);
	}
	else
	{
		NDBGL3(L3_PRIM, "isdnif=%d, cr=0x%02x, cause=0x%x",
			cd->isdnif, cd->cr, cd->cause_out);
	}

	if((m = i4b_Dgetmbuf(len)) == NULL)
		panic("i4b_l3_tx_release_complete: can't allocate mbuf");

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = RELEASE_COMPLETE;	/* message type = release complete */

	if(send_cause_flag)
	{
		*ptr++ = IEI_CAUSE;		/* cause ie */
		*ptr++ = CAUSE_LEN;
		*ptr++ = CAUSE_STD_LOC_OUT;
		*ptr++ = make_q931_cause(cd->cause_out);
	}

	i4b_dl_data_req(l2sc, l2sc->drv, m);
}

/*---------------------------------------------------------------------------*
 *	send DISCONNECT message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_disconnect(call_desc_t *cd)
{
	struct l2_softc *l2sc = (l2_softc_t*)cd->l3drv->l1_token;
	struct mbuf *m;
	u_char *ptr;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if((m = i4b_Dgetmbuf(I_FRAME_HDRLEN + MSG_DISCONNECT_LEN)) == NULL)
		panic("i4b_l3_tx_disconnect: can't allocate mbuf");

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call ref length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = DISCONNECT;		/* message type = disconnect */

	*ptr++ = IEI_CAUSE;		/* cause ie */
	*ptr++ = CAUSE_LEN;
	*ptr++ = CAUSE_STD_LOC_OUT;
	*ptr++ = make_q931_cause(cd->cause_out);

	i4b_dl_data_req(l2sc, l2sc->drv, m);
}

/*---------------------------------------------------------------------------*
 *	send SETUP message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_setup(call_desc_t *cd)
{
	struct l2_softc *l2sc = (l2_softc_t*)cd->l3drv->l1_token;
	struct mbuf *m;
	u_char *ptr;
	int slen = strlen(cd->src_telno);
	int dlen = strlen(cd->dst_telno);
	int msglen = I_FRAME_HDRLEN + MSG_SETUP_LEN + slen + dlen +
			    (cd->bprot == BPROT_NONE ? 1 : 0);

	if (slen == 0)
		msglen -= IEI_CALLINGPN_LEN+2;	/* whole IE not send */

	/*
	 * there is one additional octet if cd->bprot == BPROT_NONE
	 * NOTE: the selection of a bearer capability by a B L1
	 *       protocol is highly questionable and a better
	 *       mechanism should be used in future. (-hm)
	 */

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if((m = i4b_Dgetmbuf(msglen)) == NULL)
	{
		panic("i4b_l3_tx_setup: can't allocate mbuf");
	}

	cd->crflag = CRF_ORIG;		/* we are the originating side */

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call ref length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = SETUP;			/* message type = setup */

	*ptr++ = IEI_SENDCOMPL;		/* sending complete */

	*ptr++ = IEI_BEARERCAP;		/* bearer capability */

	/* XXX
	 * currently i have no idea if this should be switched by
	 * the choosen B channel protocol or if there should be a
	 * separate configuration item for the bearer capability.
	 * For now, it is switched by the choosen b protocol (-hm)
	 */

	switch(cd->bprot)
	{
		case BPROT_NONE:	/* telephony */
			*ptr++ = IEI_BEARERCAP_LEN+1;
			*ptr++ = IT_CAP_SPEECH;
			*ptr++ = IT_RATE_64K;
			*ptr++ = IT_UL1_G711A;
			break;

		case BPROT_RHDLC:	/* raw HDLC */
			*ptr++ = IEI_BEARERCAP_LEN;
			*ptr++ = IT_CAP_UNR_DIG_INFO;
			*ptr++ = IT_RATE_64K;
			break;

		default:
			*ptr++ = IEI_BEARERCAP_LEN;
			*ptr++ = IT_CAP_UNR_DIG_INFO;
			*ptr++ = IT_RATE_64K;
			break;
	}

	*ptr++ = IEI_CHANNELID;		/* channel id */
	*ptr++ = IEI_CHANNELID_LEN;	/* channel id length */

	switch(cd->channelid)
	{
		case CHAN_B1:
			*ptr++ = CHANNELID_B1;
			break;
		case CHAN_B2:
			*ptr++ = CHANNELID_B2;
			break;
		default:
			*ptr++ = CHANNELID_ANY;
			break;
	}

	if (slen) {
		*ptr++ = IEI_CALLINGPN;		/* calling party no */
		*ptr++ = IEI_CALLINGPN_LEN+slen;/* calling party no length */
		*ptr++ = NUMBER_TYPEPLAN;	/* type of number, number plan id */
		strncpy(ptr, cd->src_telno, slen);
		ptr += slen;
	}

	*ptr++ = IEI_CALLEDPN;		/* called party no */
	*ptr++ = IEI_CALLEDPN_LEN+dlen;	/* called party no length */
	*ptr++ = NUMBER_TYPEPLAN;	/* type of number, number plan id */
	strncpy(ptr, cd->dst_telno, dlen);
	ptr += dlen;

	i4b_dl_data_req(l2sc, l2sc->drv, m);
}

/*---------------------------------------------------------------------------*
 *	send CONNECT ACKNOWLEDGE message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_connect_ack(call_desc_t *cd)
{
	struct l2_softc *l2sc = (l2_softc_t*)cd->l3drv->l1_token;
	struct mbuf *m;
	u_char *ptr;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if((m = i4b_Dgetmbuf(I_FRAME_HDRLEN + MSG_CONNECT_ACK_LEN)) == NULL)
		panic("i4b_l3_tx_connect_ack: can't allocate mbuf");

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = CONNECT_ACKNOWLEDGE;	/* message type = connect ack */

	i4b_dl_data_req(l2sc, l2sc->drv, m);
}

/*---------------------------------------------------------------------------*
 *	send STATUS message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_status(call_desc_t *cd, u_char q850cause)
{
	struct l2_softc *l2sc = (l2_softc_t*)cd->l3drv->l1_token;
	struct mbuf *m;
	u_char *ptr;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if((m = i4b_Dgetmbuf(I_FRAME_HDRLEN + MSG_STATUS_LEN)) == NULL)
		panic("i4b_l3_tx_status: can't allocate mbuf");

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = STATUS;	/* message type = connect ack */

	*ptr++ = IEI_CAUSE;		/* cause ie */
	*ptr++ = CAUSE_LEN;
	*ptr++ = CAUSE_STD_LOC_OUT;
	*ptr++ = q850cause | EXT_LAST;

	*ptr++ = IEI_CALLSTATE;		/* call state ie */
	*ptr++ = CALLSTATE_LEN;
	*ptr++ = i4b_status_tab[cd->Q931state];

	i4b_dl_data_req(l2sc, l2sc->drv, m);
}

/*---------------------------------------------------------------------------*
 *	send RELEASE message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_release(call_desc_t *cd, int send_cause_flag)
{
	struct l2_softc *l2sc = (l2_softc_t*)cd->l3drv->l1_token;
	struct mbuf *m;
	u_char *ptr;
	int len = I_FRAME_HDRLEN + MSG_RELEASE_LEN;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if(send_cause_flag == 0)
		len -= 4;

	if((m = i4b_Dgetmbuf(len)) == NULL)
		panic("i4b_l3_tx_release: can't allocate mbuf");

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = RELEASE;		/* message type = release complete */

	if(send_cause_flag)
	{
		*ptr++ = IEI_CAUSE;		/* cause ie */
		*ptr++ = CAUSE_LEN;
		*ptr++ = CAUSE_STD_LOC_OUT;
		*ptr++ = make_q931_cause(cd->cause_out);
	}

	i4b_dl_data_req(l2sc, l2sc->drv, m);
}

/*---------------------------------------------------------------------------*
 *	send ALERTING message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_alert(call_desc_t *cd)
{
	struct l2_softc *l2sc = (l2_softc_t*)cd->l3drv->l1_token;
	struct mbuf *m;
	u_char *ptr;

	if((m = i4b_Dgetmbuf(I_FRAME_HDRLEN + MSG_ALERT_LEN)) == NULL)
		panic("i4b_l3_tx_alert: can't allocate mbuf");

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = ALERT;			/* message type = alert */

	i4b_dl_data_req(l2sc, l2sc->drv, m);
}

#endif /* NI4BQ931 > 0 */

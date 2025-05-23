/*	$NetBSD$ */
/*	$OpenBSD: if_urndis.c,v 1.31 2011/07/03 15:47:17 matthew Exp $ */

/*
 * Copyright (c) 2010 Jonathan Armani <armani@openbsd.org>
 * Copyright (c) 2010 Fabien Romano <fabien@openbsd.org>
 * Copyright (c) 2010 Michael Knudsen <mk@openbsd.org>
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/sockio.h>
#include <sys/rwlock.h>
#include <sys/mbuf.h>
#include <sys/kmem.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/socket.h>
#include <sys/device.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_ether.h>

#include <net/bpf.h>

#include <sys/bus.h>
#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>
#include <dev/usb/usbdi_util.h>
#include <dev/usb/usbdivar.h>
#include <dev/usb/usbdevs.h>
#include <dev/usb/usbcdc.h>

#include <dev/usb/if_urndisreg.h>

#ifdef URNDIS_DEBUG
#define DPRINTF(x)      do { printf x; } while (0)
#else
#define DPRINTF(x)
#endif

#define DEVNAME(sc)	(device_xname(sc->sc_dev))

#define ETHER_ALIGN 2
#define URNDIS_RESPONSE_LEN 0x400


static int urndis_newbuf(struct urndis_softc *, struct urndis_chain *);

static int urndis_ioctl(struct ifnet *, unsigned long, void *);
#if 0
static void urndis_watchdog(struct ifnet *);
#endif

static void urndis_start(struct ifnet *);
static void urndis_rxeof(struct usbd_xfer *, void *, usbd_status);
static void urndis_txeof(struct usbd_xfer *, void *, usbd_status);
static int urndis_rx_list_init(struct urndis_softc *);
static int urndis_tx_list_init(struct urndis_softc *);

static int urndis_init(struct ifnet *);
static void urndis_stop(struct ifnet *);

static usbd_status urndis_ctrl_msg(struct urndis_softc *, uint8_t, uint8_t,
    uint16_t, uint16_t, void *, size_t);
static usbd_status urndis_ctrl_send(struct urndis_softc *, void *, size_t);
static struct urndis_comp_hdr *urndis_ctrl_recv(struct urndis_softc *);

static uint32_t urndis_ctrl_handle(struct urndis_softc *,
    struct urndis_comp_hdr *, void **, size_t *);
static uint32_t urndis_ctrl_handle_init(struct urndis_softc *,
    const struct urndis_comp_hdr *);
static uint32_t urndis_ctrl_handle_query(struct urndis_softc *,
    const struct urndis_comp_hdr *, void **, size_t *);
static uint32_t urndis_ctrl_handle_reset(struct urndis_softc *,
    const struct urndis_comp_hdr *);

static uint32_t urndis_ctrl_init(struct urndis_softc *);
#if 0
static uint32_t urndis_ctrl_halt(struct urndis_softc *);
#endif
static uint32_t urndis_ctrl_query(struct urndis_softc *, uint32_t, void *,
    size_t, void **, size_t *);
static uint32_t urndis_ctrl_set(struct urndis_softc *, uint32_t, void *, size_t);
#if 0
static uint32_t urndis_ctrl_set_param(struct urndis_softc *, const char *,
    uint32_t, void *, size_t);
static uint32_t urndis_ctrl_reset(struct urndis_softc *);
static uint32_t urndis_ctrl_keepalive(struct urndis_softc *);
#endif

static int urndis_encap(struct urndis_softc *, struct mbuf *, int);
static void urndis_decap(struct urndis_softc *, struct urndis_chain *, uint32_t);

static int urndis_match(device_t, cfdata_t, void *);
static void urndis_attach(device_t, device_t, void *);
static int urndis_detach(device_t, int);
static int urndis_activate(device_t, enum devact);

CFATTACH_DECL_NEW(urndis, sizeof(struct urndis_softc),
    urndis_match, urndis_attach, urndis_detach, urndis_activate);

/*
 * Supported devices that we can't match by class IDs.
 */
static const struct usb_devno urndis_devs[] = {
	{ USB_VENDOR_HTC,	USB_PRODUCT_HTC_ANDROID },
	{ USB_VENDOR_SAMSUNG,	USB_PRODUCT_SAMSUNG_ANDROID2 },
};

static usbd_status
urndis_ctrl_msg(struct urndis_softc *sc, uint8_t rt, uint8_t r,
    uint16_t index, uint16_t value, void *buf, size_t buflen)
{
	usb_device_request_t req;

	req.bmRequestType = rt;
	req.bRequest = r;
	USETW(req.wValue, value);
	USETW(req.wIndex, index);
	USETW(req.wLength, buflen);

	return usbd_do_request(sc->sc_udev, &req, buf);
}

static usbd_status
urndis_ctrl_send(struct urndis_softc *sc, void *buf, size_t len)
{
	usbd_status err;

	if (sc->sc_dying)
		return(0);

	err = urndis_ctrl_msg(sc, UT_WRITE_CLASS_INTERFACE, UR_GET_STATUS,
	    sc->sc_ifaceno_ctl, 0, buf, len);

	if (err != USBD_NORMAL_COMPLETION)
		printf("%s: %s\n", DEVNAME(sc), usbd_errstr(err));

	return err;
}

static struct urndis_comp_hdr *
urndis_ctrl_recv(struct urndis_softc *sc)
{
	struct urndis_comp_hdr	*hdr;
	char			*buf;
	usbd_status		 err;

	buf = kmem_alloc(URNDIS_RESPONSE_LEN, KM_SLEEP);
	if (buf == NULL) {
		printf("%s: out of memory\n", DEVNAME(sc));
		return NULL;
	}

	err = urndis_ctrl_msg(sc, UT_READ_CLASS_INTERFACE, UR_CLEAR_FEATURE,
	    sc->sc_ifaceno_ctl, 0, buf, URNDIS_RESPONSE_LEN);

	if (err != USBD_NORMAL_COMPLETION && err != USBD_SHORT_XFER) {
		printf("%s: %s\n", DEVNAME(sc), usbd_errstr(err));
		kmem_free(buf, URNDIS_RESPONSE_LEN);
		return NULL;
	}

	hdr = (struct urndis_comp_hdr *)buf;
	DPRINTF(("%s: urndis_ctrl_recv: type 0x%x len %u\n",
	    DEVNAME(sc),
	    le32toh(hdr->rm_type),
	    le32toh(hdr->rm_len)));

	if (le32toh(hdr->rm_len) > URNDIS_RESPONSE_LEN) {
		printf("%s: ctrl message error: wrong size %u > %u\n",
		    DEVNAME(sc),
		    le32toh(hdr->rm_len),
		    URNDIS_RESPONSE_LEN);
		kmem_free(buf, URNDIS_RESPONSE_LEN);
		return NULL;
	}

	return hdr;
}

static uint32_t
urndis_ctrl_handle(struct urndis_softc *sc, struct urndis_comp_hdr *hdr,
    void **buf, size_t *bufsz)
{
	uint32_t rval;

	DPRINTF(("%s: urndis_ctrl_handle\n", DEVNAME(sc)));

	if (buf && bufsz) {
		*buf = NULL;
		*bufsz = 0;
	}

	switch (le32toh(hdr->rm_type)) {
		case REMOTE_NDIS_INITIALIZE_CMPLT:
			rval = urndis_ctrl_handle_init(sc, hdr);
			break;

		case REMOTE_NDIS_QUERY_CMPLT:
			rval = urndis_ctrl_handle_query(sc, hdr, buf, bufsz);
			break;

		case REMOTE_NDIS_RESET_CMPLT:
			rval = urndis_ctrl_handle_reset(sc, hdr);
			break;

		case REMOTE_NDIS_KEEPALIVE_CMPLT:
		case REMOTE_NDIS_SET_CMPLT:
			rval = le32toh(hdr->rm_status);
			break;

		default:
			printf("%s: ctrl message error: unknown event 0x%x\n",
			    DEVNAME(sc), le32toh(hdr->rm_type));
			rval = RNDIS_STATUS_FAILURE;
	}

	kmem_free(hdr, URNDIS_RESPONSE_LEN);

	return rval;
}

static uint32_t
urndis_ctrl_handle_init(struct urndis_softc *sc,
    const struct urndis_comp_hdr *hdr)
{
	const struct urndis_init_comp	*msg;

	msg = (const struct urndis_init_comp *) hdr;

	DPRINTF(("%s: urndis_ctrl_handle_init: len %u rid %u status 0x%x "
	    "ver_major %u ver_minor %u devflags 0x%x medium 0x%x pktmaxcnt %u "
	    "pktmaxsz %u align %u aflistoffset %u aflistsz %u\n",
	    DEVNAME(sc),
	    le32toh(msg->rm_len),
	    le32toh(msg->rm_rid),
	    le32toh(msg->rm_status),
	    le32toh(msg->rm_ver_major),
	    le32toh(msg->rm_ver_minor),
	    le32toh(msg->rm_devflags),
	    le32toh(msg->rm_medium),
	    le32toh(msg->rm_pktmaxcnt),
	    le32toh(msg->rm_pktmaxsz),
	    le32toh(msg->rm_align),
	    le32toh(msg->rm_aflistoffset),
	    le32toh(msg->rm_aflistsz)));

	if (le32toh(msg->rm_status) != RNDIS_STATUS_SUCCESS) {
		printf("%s: init failed 0x%x\n",
		    DEVNAME(sc),
		    le32toh(msg->rm_status));

		return le32toh(msg->rm_status);
	}

	if (le32toh(msg->rm_devflags) != RNDIS_DF_CONNECTIONLESS) {
		printf("%s: wrong device type (current type: 0x%x)\n",
		    DEVNAME(sc),
		    le32toh(msg->rm_devflags));

		return RNDIS_STATUS_FAILURE;
	}

	if (le32toh(msg->rm_medium) != RNDIS_MEDIUM_802_3) {
		printf("%s: medium not 802.3 (current medium: 0x%x)\n",
		    DEVNAME(sc), le32toh(msg->rm_medium));

		return RNDIS_STATUS_FAILURE;
	}

	sc->sc_lim_pktsz = le32toh(msg->rm_pktmaxsz);

	return le32toh(msg->rm_status);
}

static uint32_t
urndis_ctrl_handle_query(struct urndis_softc *sc,
    const struct urndis_comp_hdr *hdr, void **buf, size_t *bufsz)
{
	const struct urndis_query_comp	*msg;

	msg = (const struct urndis_query_comp *) hdr;

	DPRINTF(("%s: urndis_ctrl_handle_query: len %u rid %u status 0x%x "
	    "buflen %u bufoff %u\n",
	    DEVNAME(sc),
	    le32toh(msg->rm_len),
	    le32toh(msg->rm_rid),
	    le32toh(msg->rm_status),
	    le32toh(msg->rm_infobuflen),
	    le32toh(msg->rm_infobufoffset)));

	if (buf && bufsz) {
		*buf = NULL;
		*bufsz = 0;
	}

	if (le32toh(msg->rm_status) != RNDIS_STATUS_SUCCESS) {
		printf("%s: query failed 0x%x\n",
		    DEVNAME(sc),
		    le32toh(msg->rm_status));

		return le32toh(msg->rm_status);
	}

	if (le32toh(msg->rm_infobuflen) + le32toh(msg->rm_infobufoffset) +
	    RNDIS_HEADER_OFFSET > le32toh(msg->rm_len)) {
		printf("%s: ctrl message error: invalid query info "
		    "len/offset/end_position(%u/%u/%u) -> "
		    "go out of buffer limit %u\n",
		    DEVNAME(sc),
		    le32toh(msg->rm_infobuflen),
		    le32toh(msg->rm_infobufoffset),
		    le32toh(msg->rm_infobuflen) +
		    le32toh(msg->rm_infobufoffset) + (uint32_t)RNDIS_HEADER_OFFSET,
		    le32toh(msg->rm_len));
		return RNDIS_STATUS_FAILURE;
	}

	if (buf && bufsz) {
		*buf = kmem_alloc(le32toh(msg->rm_infobuflen), KM_SLEEP);
		if (*buf == NULL) {
			printf("%s: out of memory\n", DEVNAME(sc));
			return RNDIS_STATUS_FAILURE;
		} else {
			const char *p;
			*bufsz = le32toh(msg->rm_infobuflen);

			p = (const char *)&msg->rm_rid;
			p += le32toh(msg->rm_infobufoffset);
			memcpy(*buf, p, le32toh(msg->rm_infobuflen));
		}
	}

	return le32toh(msg->rm_status);
}

static uint32_t
urndis_ctrl_handle_reset(struct urndis_softc *sc,
    const struct urndis_comp_hdr *hdr)
{
	const struct urndis_reset_comp	*msg;
	uint32_t			 rval;

	msg = (const struct urndis_reset_comp *) hdr;

	rval = le32toh(msg->rm_status);

	DPRINTF(("%s: urndis_ctrl_handle_reset: len %u status 0x%x "
	    "adrreset %u\n",
	    DEVNAME(sc),
	    le32toh(msg->rm_len),
	    rval,
	    le32toh(msg->rm_adrreset)));

	if (rval != RNDIS_STATUS_SUCCESS) {
		printf("%s: reset failed 0x%x\n", DEVNAME(sc), rval);
		return rval;
	}

	if (le32toh(msg->rm_adrreset) != 0) {
		uint32_t filter;

		filter = htole32(sc->sc_filter);
		rval = urndis_ctrl_set(sc, OID_GEN_CURRENT_PACKET_FILTER,
		    &filter, sizeof(filter));
		if (rval != RNDIS_STATUS_SUCCESS) {
			printf("%s: unable to reset data filters\n",
			    DEVNAME(sc));
			return rval;
		}
	}

	return rval;
}

static uint32_t
urndis_ctrl_init(struct urndis_softc *sc)
{
	struct urndis_init_req	*msg;
	uint32_t		 rval;
	struct urndis_comp_hdr	*hdr;

	msg = kmem_alloc(sizeof(*msg), KM_SLEEP);
	if (msg == NULL) {
		printf("%s: out of memory\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}

	msg->rm_type = htole32(REMOTE_NDIS_INITIALIZE_MSG);
	msg->rm_len = htole32(sizeof(*msg));
	msg->rm_rid = htole32(0);
	msg->rm_ver_major = htole32(1);
	msg->rm_ver_minor = htole32(1);
	msg->rm_max_xfersz = htole32(RNDIS_BUFSZ);

	DPRINTF(("%s: urndis_ctrl_init send: type %u len %u rid %u ver_major %u "
	    "ver_minor %u max_xfersz %u\n",
	    DEVNAME(sc),
	    le32toh(msg->rm_type),
	    le32toh(msg->rm_len),
	    le32toh(msg->rm_rid),
	    le32toh(msg->rm_ver_major),
	    le32toh(msg->rm_ver_minor),
	    le32toh(msg->rm_max_xfersz)));

	rval = urndis_ctrl_send(sc, msg, sizeof(*msg));
	kmem_free(msg, sizeof(*msg));

	if (rval != RNDIS_STATUS_SUCCESS) {
		printf("%s: init failed\n", DEVNAME(sc));
		return rval;
	}

	if ((hdr = urndis_ctrl_recv(sc)) == NULL) {
		printf("%s: unable to get init response\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}
	rval = urndis_ctrl_handle(sc, hdr, NULL, NULL);

	return rval;
}

#if 0
static uint32_t
urndis_ctrl_halt(struct urndis_softc *sc)
{
	struct urndis_halt_req	*msg;
	uint32_t		 rval;

	msg = kmem_alloc(sizeof(*msg), KM_SLEEP);
	if (msg == NULL) {
		printf("%s: out of memory\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}

	msg->rm_type = htole32(REMOTE_NDIS_HALT_MSG);
	msg->rm_len = htole32(sizeof(*msg));
	msg->rm_rid = 0;

	DPRINTF(("%s: urndis_ctrl_halt send: type %u len %u rid %u\n",
	    DEVNAME(sc),
	    le32toh(msg->rm_type),
	    le32toh(msg->rm_len),
	    le32toh(msg->rm_rid)));

	rval = urndis_ctrl_send(sc, msg, sizeof(*msg));
	kmem_free(msg, sizeof(*msg));

	if (rval != RNDIS_STATUS_SUCCESS)
		printf("%s: halt failed\n", DEVNAME(sc));

	return rval;
}
#endif

static uint32_t
urndis_ctrl_query(struct urndis_softc *sc, uint32_t oid,
    void *qbuf, size_t qlen,
    void **rbuf, size_t *rbufsz)
{
	struct urndis_query_req	*msg;
	uint32_t		 rval;
	struct urndis_comp_hdr	*hdr;

	msg = kmem_alloc(sizeof(*msg) + qlen, KM_SLEEP);
	if (msg == NULL) {
		printf("%s: out of memory\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}

	msg->rm_type = htole32(REMOTE_NDIS_QUERY_MSG);
	msg->rm_len = htole32(sizeof(*msg) + qlen);
	msg->rm_rid = 0; /* XXX */
	msg->rm_oid = htole32(oid);
	msg->rm_infobuflen = htole32(qlen);
	if (qlen != 0) {
		msg->rm_infobufoffset = htole32(20);
		memcpy((char*)msg + 20, qbuf, qlen);
	} else
		msg->rm_infobufoffset = 0;
	msg->rm_devicevchdl = 0;

	DPRINTF(("%s: urndis_ctrl_query send: type %u len %u rid %u oid 0x%x "
	    "infobuflen %u infobufoffset %u devicevchdl %u\n",
	    DEVNAME(sc),
	    le32toh(msg->rm_type),
	    le32toh(msg->rm_len),
	    le32toh(msg->rm_rid),
	    le32toh(msg->rm_oid),
	    le32toh(msg->rm_infobuflen),
	    le32toh(msg->rm_infobufoffset),
	    le32toh(msg->rm_devicevchdl)));

	rval = urndis_ctrl_send(sc, msg, sizeof(*msg));
	kmem_free(msg, sizeof(*msg) + qlen);

	if (rval != RNDIS_STATUS_SUCCESS) {
		printf("%s: query failed\n", DEVNAME(sc));
		return rval;
	}

	if ((hdr = urndis_ctrl_recv(sc)) == NULL) {
		printf("%s: unable to get query response\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}
	rval = urndis_ctrl_handle(sc, hdr, rbuf, rbufsz);

	return rval;
}

static uint32_t
urndis_ctrl_set(struct urndis_softc *sc, uint32_t oid, void *buf, size_t len)
{
	struct urndis_set_req	*msg;
	uint32_t		 rval;
	struct urndis_comp_hdr	*hdr;

	msg = kmem_alloc(sizeof(*msg) + len, KM_SLEEP);
	if (msg == NULL) {
		printf("%s: out of memory\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}

	msg->rm_type = htole32(REMOTE_NDIS_SET_MSG);
	msg->rm_len = htole32(sizeof(*msg) + len);
	msg->rm_rid = 0; /* XXX */
	msg->rm_oid = htole32(oid);
	msg->rm_infobuflen = htole32(len);
	if (len != 0) {
		msg->rm_infobufoffset = htole32(20);
		memcpy((char*)msg + 20, buf, len);
	} else
		msg->rm_infobufoffset = 0;
	msg->rm_devicevchdl = 0;

	DPRINTF(("%s: urndis_ctrl_set send: type %u len %u rid %u oid 0x%x "
	    "infobuflen %u infobufoffset %u devicevchdl %u\n",
	    DEVNAME(sc),
	    le32toh(msg->rm_type),
	    le32toh(msg->rm_len),
	    le32toh(msg->rm_rid),
	    le32toh(msg->rm_oid),
	    le32toh(msg->rm_infobuflen),
	    le32toh(msg->rm_infobufoffset),
	    le32toh(msg->rm_devicevchdl)));

	rval = urndis_ctrl_send(sc, msg, sizeof(*msg));
	kmem_free(msg, sizeof(*msg) + len);

	if (rval != RNDIS_STATUS_SUCCESS) {
		printf("%s: set failed\n", DEVNAME(sc));
		return rval;
	}

	if ((hdr = urndis_ctrl_recv(sc)) == NULL) {
		printf("%s: unable to get set response\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}
	rval = urndis_ctrl_handle(sc, hdr, NULL, NULL);
	if (rval != RNDIS_STATUS_SUCCESS)
		printf("%s: set failed 0x%x\n", DEVNAME(sc), rval);

	return rval;
}

#if 0
static uint32_t
urndis_ctrl_set_param(struct urndis_softc *sc,
    const char *name,
    uint32_t type,
    void *buf,
    size_t len)
{
	struct urndis_set_parameter	*param;
	uint32_t			 rval;
	size_t				 namelen, tlen;

	if (name)
		namelen = strlen(name);
	else
		namelen = 0;
	tlen = sizeof(*param) + len + namelen;
	param = kmem_alloc(tlen, KM_SLEEP);
	if (param == NULL) {
		printf("%s: out of memory\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}

	param->rm_namelen = htole32(namelen);
	param->rm_valuelen = htole32(len);
	param->rm_type = htole32(type);
	if (namelen != 0) {
		param->rm_nameoffset = htole32(20);
		memcpy(param + 20, name, namelen);
	} else
		param->rm_nameoffset = 0;
	if (len != 0) {
		param->rm_valueoffset = htole32(20 + namelen);
		memcpy(param + 20 + namelen, buf, len);
	} else
		param->rm_valueoffset = 0;

	DPRINTF(("%s: urndis_ctrl_set_param send: nameoffset %u namelen %u "
	    "type 0x%x valueoffset %u valuelen %u\n",
	    DEVNAME(sc),
	    le32toh(param->rm_nameoffset),
	    le32toh(param->rm_namelen),
	    le32toh(param->rm_type),
	    le32toh(param->rm_valueoffset),
	    le32toh(param->rm_valuelen)));

	rval = urndis_ctrl_set(sc, OID_GEN_RNDIS_CONFIG_PARAMETER, param, tlen);
	kmem_free(param, tlen);
	if (rval != RNDIS_STATUS_SUCCESS)
		printf("%s: set param failed 0x%x\n", DEVNAME(sc), rval);

	return rval;
}

/* XXX : adrreset, get it from response */
static uint32_t
urndis_ctrl_reset(struct urndis_softc *sc)
{
	struct urndis_reset_req		*reset;
	uint32_t			 rval;
	struct urndis_comp_hdr		*hdr;

	reset = kmem_alloc(sizeof(*reset), KM_SLEEP);
	if (reset == NULL) {
		printf("%s: out of memory\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}

	reset->rm_type = htole32(REMOTE_NDIS_RESET_MSG);
	reset->rm_len = htole32(sizeof(*reset));
	reset->rm_rid = 0; /* XXX rm_rid == reserved ... remove ? */

	DPRINTF(("%s: urndis_ctrl_reset send: type %u len %u rid %u\n",
	    DEVNAME(sc),
	    le32toh(reset->rm_type),
	    le32toh(reset->rm_len),
	    le32toh(reset->rm_rid)));

	rval = urndis_ctrl_send(sc, reset, sizeof(*reset));
	kmem_free(reset, sizeof(*reset));

	if (rval != RNDIS_STATUS_SUCCESS) {
		printf("%s: reset failed\n", DEVNAME(sc));
		return rval;
	}

	if ((hdr = urndis_ctrl_recv(sc)) == NULL) {
		printf("%s: unable to get reset response\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}
	rval = urndis_ctrl_handle(sc, hdr, NULL, NULL);

	return rval;
}

static uint32_t
urndis_ctrl_keepalive(struct urndis_softc *sc)
{
	struct urndis_keepalive_req	*keep;
	uint32_t			 rval;
	struct urndis_comp_hdr		*hdr;

	keep = kmem_alloc(sizeof(*keep), KM_SLEEP);
	if (keep == NULL) {
		printf("%s: out of memory\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}

	keep->rm_type = htole32(REMOTE_NDIS_KEEPALIVE_MSG);
	keep->rm_len = htole32(sizeof(*keep));
	keep->rm_rid = 0; /* XXX rm_rid == reserved ... remove ? */

	DPRINTF(("%s: urndis_ctrl_keepalive: type %u len %u rid %u\n",
	    DEVNAME(sc),
	    le32toh(keep->rm_type),
	    le32toh(keep->rm_len),
	    le32toh(keep->rm_rid)));

	rval = urndis_ctrl_send(sc, keep, sizeof(*keep));
	kmem_free(keep, sizeof(*keep));

	if (rval != RNDIS_STATUS_SUCCESS) {
		printf("%s: keepalive failed\n", DEVNAME(sc));
		return rval;
	}

	if ((hdr = urndis_ctrl_recv(sc)) == NULL) {
		printf("%s: unable to get keepalive response\n", DEVNAME(sc));
		return RNDIS_STATUS_FAILURE;
	}
	rval = urndis_ctrl_handle(sc, hdr, NULL, NULL);
	if (rval != RNDIS_STATUS_SUCCESS) {
		printf("%s: keepalive failed 0x%x\n", DEVNAME(sc), rval);
		urndis_ctrl_reset(sc);
	}

	return rval;
}
#endif

static int
urndis_encap(struct urndis_softc *sc, struct mbuf *m, int idx)
{
	struct urndis_chain		*c;
	usbd_status			 err;
	struct urndis_packet_msg	*msg;

	c = &sc->sc_data.sc_tx_chain[idx];

	msg = (struct urndis_packet_msg *)c->sc_buf;

	memset(msg, 0, sizeof(*msg));
	msg->rm_type = htole32(REMOTE_NDIS_PACKET_MSG);
	msg->rm_len = htole32(sizeof(*msg) + m->m_pkthdr.len);

	msg->rm_dataoffset = htole32(RNDIS_DATA_OFFSET);
	msg->rm_datalen = htole32(m->m_pkthdr.len);

	m_copydata(m, 0, m->m_pkthdr.len,
	    ((char*)msg + RNDIS_DATA_OFFSET + RNDIS_HEADER_OFFSET));

	DPRINTF(("%s: urndis_encap type 0x%x len %u data(off %u len %u)\n",
	    DEVNAME(sc),
	    le32toh(msg->rm_type),
	    le32toh(msg->rm_len),
	    le32toh(msg->rm_dataoffset),
	    le32toh(msg->rm_datalen)));

	c->sc_mbuf = m;

	usbd_setup_xfer(c->sc_xfer, c, c->sc_buf, le32toh(msg->rm_len),
	    USBD_FORCE_SHORT_XFER, 10000, urndis_txeof);

	/* Transmit */
	err = usbd_transfer(c->sc_xfer);
	if (err != USBD_IN_PROGRESS) {
		urndis_stop(GET_IFP(sc));
		return(EIO);
	}

	sc->sc_data.sc_tx_cnt++;

	return(0);
}

static void
urndis_decap(struct urndis_softc *sc, struct urndis_chain *c, uint32_t len)
{
	struct mbuf		*m;
	struct urndis_packet_msg	*msg;
	struct ifnet		*ifp;
	int			 s;
	int			 offset;

	ifp = GET_IFP(sc);
	offset = 0;

	while (len > 0) {
		msg = (struct urndis_packet_msg *)((char*)c->sc_buf + offset);
		m = c->sc_mbuf;

		DPRINTF(("%s: urndis_decap buffer size left %u\n", DEVNAME(sc),
		    len));

		if (len < sizeof(*msg)) {
			printf("%s: urndis_decap invalid buffer len %u < "
			    "minimum header %zu\n",
			    DEVNAME(sc),
			    len,
			    sizeof(*msg));
			return;
		}

		DPRINTF(("%s: urndis_decap len %u data(off:%u len:%u) "
		    "oobdata(off:%u len:%u nb:%u) perpacket(off:%u len:%u)\n",
		    DEVNAME(sc),
		    le32toh(msg->rm_len),
		    le32toh(msg->rm_dataoffset),
		    le32toh(msg->rm_datalen),
		    le32toh(msg->rm_oobdataoffset),
		    le32toh(msg->rm_oobdatalen),
		    le32toh(msg->rm_oobdataelements),
		    le32toh(msg->rm_pktinfooffset),
		    le32toh(msg->rm_pktinfooffset)));

		if (le32toh(msg->rm_type) != REMOTE_NDIS_PACKET_MSG) {
			printf("%s: urndis_decap invalid type 0x%x != 0x%x\n",
			    DEVNAME(sc),
			    le32toh(msg->rm_type),
			    REMOTE_NDIS_PACKET_MSG);
			return;
		}
		if (le32toh(msg->rm_len) < sizeof(*msg)) {
			printf("%s: urndis_decap invalid msg len %u < %zu\n",
			    DEVNAME(sc),
			    le32toh(msg->rm_len),
			    sizeof(*msg));
			return;
		}
		if (le32toh(msg->rm_len) > len) {
			printf("%s: urndis_decap invalid msg len %u > buffer "
			    "len %u\n",
			    DEVNAME(sc),
			    le32toh(msg->rm_len),
			    len);
			return;
		}

		if (le32toh(msg->rm_dataoffset) +
		    le32toh(msg->rm_datalen) + RNDIS_HEADER_OFFSET
		        > le32toh(msg->rm_len)) {
			printf("%s: urndis_decap invalid data "
			    "len/offset/end_position(%u/%u/%u) -> "
			    "go out of receive buffer limit %u\n",
			    DEVNAME(sc),
			    le32toh(msg->rm_datalen),
			    le32toh(msg->rm_dataoffset),
			    le32toh(msg->rm_dataoffset) +
			    le32toh(msg->rm_datalen) + (uint32_t)RNDIS_HEADER_OFFSET,
			    le32toh(msg->rm_len));
			return;
		}

		if (le32toh(msg->rm_datalen) < sizeof(struct ether_header)) {
			ifp->if_ierrors++;
			printf("%s: urndis_decap invalid ethernet size "
			    "%d < %zu\n",
			    DEVNAME(sc),
			    le32toh(msg->rm_datalen),
			    sizeof(struct ether_header));
			return;
		}

		memcpy(mtod(m, char*),
		    ((char*)&msg->rm_dataoffset + le32toh(msg->rm_dataoffset)),
		    le32toh(msg->rm_datalen));
		m->m_pkthdr.len = m->m_len = le32toh(msg->rm_datalen);

		ifp->if_ipackets++;
		m_set_rcvif(m, ifp);

		s = splnet();

		if (urndis_newbuf(sc, c) == ENOBUFS) {
			ifp->if_ierrors++;
		} else {

			bpf_mtap(ifp, m);

			if_percpuq_enqueue(ifp->if_percpuq, m);
		}
		splx(s);

		offset += le32toh(msg->rm_len);
		len -= le32toh(msg->rm_len);
	}
}

static int
urndis_newbuf(struct urndis_softc *sc, struct urndis_chain *c)
{
	struct mbuf *m_new = NULL;

	MGETHDR(m_new, M_DONTWAIT, MT_DATA);
	if (m_new == NULL) {
		printf("%s: no memory for rx list -- packet dropped!\n",
		    DEVNAME(sc));
		return ENOBUFS;
	}
	MCLGET(m_new, M_DONTWAIT);
	if (!(m_new->m_flags & M_EXT)) {
		printf("%s: no memory for rx list -- packet dropped!\n",
		    DEVNAME(sc));
		m_freem(m_new);
		return ENOBUFS;
	}
	m_new->m_len = m_new->m_pkthdr.len = MCLBYTES;

	m_adj(m_new, ETHER_ALIGN);
	c->sc_mbuf = m_new;
	return 0;
}

static int
urndis_rx_list_init(struct urndis_softc *sc)
{
	struct urndis_cdata	*cd;
	struct urndis_chain	*c;
	int			 i;

	cd = &sc->sc_data;
	for (i = 0; i < RNDIS_RX_LIST_CNT; i++) {
		c = &cd->sc_rx_chain[i];
		c->sc_softc = sc;
		c->sc_idx = i;

		if (urndis_newbuf(sc, c) == ENOBUFS)
			return ENOBUFS;

		if (c->sc_xfer == NULL) {
			int err = usbd_create_xfer(sc->sc_bulkin_pipe,
			    RNDIS_BUFSZ, USBD_SHORT_XFER_OK, 0, &c->sc_xfer);
			if (err)
				return err;
			c->sc_buf = usbd_get_buffer(c->sc_xfer);
		}
	}

	return 0;
}

static int
urndis_tx_list_init(struct urndis_softc *sc)
{
	struct urndis_cdata	*cd;
	struct urndis_chain	*c;
	int			 i;

	cd = &sc->sc_data;
	for (i = 0; i < RNDIS_TX_LIST_CNT; i++) {
		c = &cd->sc_tx_chain[i];
		c->sc_softc = sc;
		c->sc_idx = i;
		c->sc_mbuf = NULL;
		if (c->sc_xfer == NULL) {
			int err = usbd_create_xfer(sc->sc_bulkout_pipe,
			    RNDIS_BUFSZ, USBD_FORCE_SHORT_XFER, 0, &c->sc_xfer);
			if (err)
				return err;
			c->sc_buf = usbd_get_buffer(c->sc_xfer);
		}
	}
	return 0;
}

static int
urndis_ioctl(struct ifnet *ifp, unsigned long command, void *data)
{
	struct urndis_softc	*sc;
	int			 s, error;

	sc = ifp->if_softc;
	error = 0;

	if (sc->sc_dying)
		return EIO;

	s = splnet();

	switch(command) {
	case SIOCSIFFLAGS:
		if ((error = ifioctl_common(ifp, command, data)) != 0)
			break;
		if (ifp->if_flags & IFF_UP) {
			if (!(ifp->if_flags & IFF_RUNNING))
				urndis_init(ifp);
		} else {
			if (ifp->if_flags & IFF_RUNNING)
				urndis_stop(ifp);
		}
		error = 0;
		break;

	default:
		error = ether_ioctl(ifp, command, data);
		break;
	}

	if (error == ENETRESET)
		error = 0;

	splx(s);
	return error;
}

#if 0
static void
urndis_watchdog(struct ifnet *ifp)
{
	struct urndis_softc *sc;

	sc = ifp->if_softc;

	if (sc->sc_dying)
		return;

	ifp->if_oerrors++;
	printf("%s: watchdog timeout\n", DEVNAME(sc));

	urndis_ctrl_keepalive(sc);
}
#endif

static int
urndis_init(struct ifnet *ifp)
{
	struct urndis_softc	*sc;
	int			 i, s;
	int 			 err;
	usbd_status		 usberr;

	sc = ifp->if_softc;

	if (ifp->if_flags & IFF_RUNNING)
		return 0;

	err = urndis_ctrl_init(sc);
	if (err != RNDIS_STATUS_SUCCESS)
		return EIO;

	s = splnet();

	usberr = usbd_open_pipe(sc->sc_iface_data, sc->sc_bulkin_no,
	    USBD_EXCLUSIVE_USE, &sc->sc_bulkin_pipe);
	if (usberr) {
		printf("%s: open rx pipe failed: %s\n", DEVNAME(sc),
		    usbd_errstr(err));
		splx(s);
		return EIO;
	}

	usberr = usbd_open_pipe(sc->sc_iface_data, sc->sc_bulkout_no,
	    USBD_EXCLUSIVE_USE, &sc->sc_bulkout_pipe);
	if (usberr) {
		printf("%s: open tx pipe failed: %s\n", DEVNAME(sc),
		    usbd_errstr(err));
		splx(s);
		return EIO;
	}

	err = urndis_tx_list_init(sc);
	if (err) {
		printf("%s: tx list init failed\n",
		    DEVNAME(sc));
		splx(s);
		return err;
	}

	err = urndis_rx_list_init(sc);
	if (err) {
		printf("%s: rx list init failed\n",
		    DEVNAME(sc));
		splx(s);
		return err;
	}

	for (i = 0; i < RNDIS_RX_LIST_CNT; i++) {
		struct urndis_chain *c;

		c = &sc->sc_data.sc_rx_chain[i];

		usbd_setup_xfer(c->sc_xfer, c, c->sc_buf, RNDIS_BUFSZ,
		    USBD_SHORT_XFER_OK, USBD_NO_TIMEOUT, urndis_rxeof);
		usbd_transfer(c->sc_xfer);
	}

	ifp->if_flags |= IFF_RUNNING;
	ifp->if_flags &= ~IFF_OACTIVE;

	splx(s);
	return 0;
}

static void
urndis_stop(struct ifnet *ifp)
{
	struct urndis_softc	*sc;
	usbd_status	 err;
	int		 i;

	sc = ifp->if_softc;

	ifp->if_timer = 0;
	ifp->if_flags &= ~(IFF_RUNNING | IFF_OACTIVE);

	if (sc->sc_bulkin_pipe != NULL) {
		err = usbd_abort_pipe(sc->sc_bulkin_pipe);
		if (err)
			printf("%s: abort rx pipe failed: %s\n",
			    DEVNAME(sc), usbd_errstr(err));
	}

	if (sc->sc_bulkout_pipe != NULL) {
		err = usbd_abort_pipe(sc->sc_bulkout_pipe);
		if (err)
			printf("%s: abort tx pipe failed: %s\n",
			    DEVNAME(sc), usbd_errstr(err));
	}

	for (i = 0; i < RNDIS_RX_LIST_CNT; i++) {
		if (sc->sc_data.sc_rx_chain[i].sc_mbuf != NULL) {
			m_freem(sc->sc_data.sc_rx_chain[i].sc_mbuf);
			sc->sc_data.sc_rx_chain[i].sc_mbuf = NULL;
		}
		if (sc->sc_data.sc_rx_chain[i].sc_xfer != NULL) {
			usbd_destroy_xfer(sc->sc_data.sc_rx_chain[i].sc_xfer);
			sc->sc_data.sc_rx_chain[i].sc_xfer = NULL;
		}
	}

	for (i = 0; i < RNDIS_TX_LIST_CNT; i++) {
		if (sc->sc_data.sc_tx_chain[i].sc_mbuf != NULL) {
			m_freem(sc->sc_data.sc_tx_chain[i].sc_mbuf);
			sc->sc_data.sc_tx_chain[i].sc_mbuf = NULL;
		}
		if (sc->sc_data.sc_tx_chain[i].sc_xfer != NULL) {
			usbd_destroy_xfer(sc->sc_data.sc_tx_chain[i].sc_xfer);
			sc->sc_data.sc_tx_chain[i].sc_xfer = NULL;
		}
	}

	/* Close pipes. */
	if (sc->sc_bulkin_pipe != NULL) {
		err = usbd_close_pipe(sc->sc_bulkin_pipe);
		if (err)
			printf("%s: close rx pipe failed: %s\n",
			    DEVNAME(sc), usbd_errstr(err));
		sc->sc_bulkin_pipe = NULL;
	}

	if (sc->sc_bulkout_pipe != NULL) {
		err = usbd_close_pipe(sc->sc_bulkout_pipe);
		if (err)
			printf("%s: close tx pipe failed: %s\n",
			    DEVNAME(sc), usbd_errstr(err));
		sc->sc_bulkout_pipe = NULL;
	}
}

static void
urndis_start(struct ifnet *ifp)
{
	struct urndis_softc	*sc;
	struct mbuf		*m_head = NULL;

	sc = ifp->if_softc;

	if (sc->sc_dying || (ifp->if_flags & IFF_OACTIVE))
		return;

	IFQ_POLL(&ifp->if_snd, m_head);
	if (m_head == NULL)
		return;

	if (urndis_encap(sc, m_head, 0)) {
		ifp->if_flags |= IFF_OACTIVE;
		return;
	}
	IFQ_DEQUEUE(&ifp->if_snd, m_head);

	/*
	 * If there's a BPF listener, bounce a copy of this frame
	 * to him.
	 */
	bpf_mtap(ifp, m_head);

	ifp->if_flags |= IFF_OACTIVE;

	/*
	 * Set a timeout in case the chip goes out to lunch.
	 */
	ifp->if_timer = 5;

	return;
}

static void
urndis_rxeof(struct usbd_xfer *xfer,
    void *priv,
    usbd_status status)
{
	struct urndis_chain	*c;
	struct urndis_softc	*sc;
	struct ifnet		*ifp;
	uint32_t		 total_len;

	c = priv;
	sc = c->sc_softc;
	ifp = GET_IFP(sc);
	total_len = 0;

	if (sc->sc_dying || !(ifp->if_flags & IFF_RUNNING))
		return;

	if (status != USBD_NORMAL_COMPLETION) {
		if (status == USBD_NOT_STARTED || status == USBD_CANCELLED)
			return;
		if (usbd_ratecheck(&sc->sc_rx_notice)) {
			printf("%s: usb errors on rx: %s\n",
			    DEVNAME(sc), usbd_errstr(status));
		}
		if (status == USBD_STALLED)
			usbd_clear_endpoint_stall_async(sc->sc_bulkin_pipe);

		goto done;
	}

	usbd_get_xfer_status(xfer, NULL, NULL, &total_len, NULL);
	urndis_decap(sc, c, total_len);

done:
	/* Setup new transfer. */
	usbd_setup_xfer(c->sc_xfer, c, c->sc_buf, RNDIS_BUFSZ,
	    USBD_SHORT_XFER_OK, USBD_NO_TIMEOUT, urndis_rxeof);
	usbd_transfer(c->sc_xfer);
}

static void
urndis_txeof(struct usbd_xfer *xfer,
    void *priv,
    usbd_status status)
{
	struct urndis_chain	*c;
	struct urndis_softc	*sc;
	struct ifnet		*ifp;
	usbd_status		 err;
	int			 s;

	c = priv;
	sc = c->sc_softc;
	ifp = GET_IFP(sc);

	DPRINTF(("%s: urndis_txeof\n", DEVNAME(sc)));

	if (sc->sc_dying)
		return;

	s = splnet();

	ifp->if_timer = 0;
	ifp->if_flags &= ~IFF_OACTIVE;

	if (status != USBD_NORMAL_COMPLETION) {
		if (status == USBD_NOT_STARTED || status == USBD_CANCELLED) {
			splx(s);
			return;
		}
		ifp->if_oerrors++;
		printf("%s: usb error on tx: %s\n", DEVNAME(sc),
		    usbd_errstr(status));
		if (status == USBD_STALLED)
			usbd_clear_endpoint_stall_async(sc->sc_bulkout_pipe);
		splx(s);
		return;
	}

	usbd_get_xfer_status(c->sc_xfer, NULL, NULL, NULL, &err);

	if (c->sc_mbuf != NULL) {
		m_freem(c->sc_mbuf);
		c->sc_mbuf = NULL;
	}

	if (err)
		ifp->if_oerrors++;
	else
		ifp->if_opackets++;

	if (IFQ_IS_EMPTY(&ifp->if_snd) == 0)
		urndis_start(ifp);

	splx(s);
}

static int
urndis_match(device_t parent, cfdata_t match, void *aux)
{
	struct usbif_attach_arg		*uiaa = aux;
	usb_interface_descriptor_t	*id;

	if (!uiaa->uiaa_iface)
		return UMATCH_NONE;

	id = usbd_get_interface_descriptor(uiaa->uiaa_iface);
	if (id == NULL)
		return UMATCH_NONE;

	if (id->bInterfaceClass == UICLASS_WIRELESS &&
	    id->bInterfaceSubClass == UISUBCLASS_RF &&
	    id->bInterfaceProtocol == UIPROTO_RNDIS)
		return UMATCH_IFACECLASS_IFACESUBCLASS_IFACEPROTO;

	return usb_lookup(urndis_devs, uiaa->uiaa_vendor, uiaa->uiaa_product) != NULL ?
	    UMATCH_VENDOR_PRODUCT : UMATCH_NONE;
}

static void
urndis_attach(device_t parent, device_t self, void *aux)
{
	struct urndis_softc		*sc;
	struct usbif_attach_arg		*uiaa;
	struct ifnet			*ifp;
	usb_interface_descriptor_t	*id;
	usb_endpoint_descriptor_t	*ed;
	usb_config_descriptor_t		*cd;
	const usb_cdc_union_descriptor_t *ud;
	const usb_cdc_header_descriptor_t *desc;
	usbd_desc_iter_t		 iter;
	int				 if_ctl, if_data;
	int				 i, j, altcnt;
	int				 s;
	u_char				 eaddr[ETHER_ADDR_LEN];
	void				*buf;
	size_t				 bufsz;
	uint32_t			 filter;
	char				*devinfop;

	sc = device_private(self);
	uiaa = aux;
	sc->sc_dev = self;
	sc->sc_udev = uiaa->uiaa_device;

	aprint_naive("\n");
	aprint_normal("\n");

	devinfop = usbd_devinfo_alloc(uiaa->uiaa_device, 0);
	aprint_normal_dev(self, "%s\n", devinfop);
	usbd_devinfo_free(devinfop);

	sc->sc_iface_ctl = uiaa->uiaa_iface;
	id = usbd_get_interface_descriptor(sc->sc_iface_ctl);
	if_ctl = id->bInterfaceNumber;
	sc->sc_ifaceno_ctl = if_ctl;
	if_data = -1;

	usb_desc_iter_init(sc->sc_udev, &iter);
	while ((desc = (const void *)usb_desc_iter_next(&iter)) != NULL) {

		if (desc->bDescriptorType != UDESC_CS_INTERFACE) {
			continue;
		}
		switch (desc->bDescriptorSubtype) {
		case UDESCSUB_CDC_UNION:
			/* XXX bail out when found first? */
			ud = (const usb_cdc_union_descriptor_t *)desc;
			if (if_data == -1)
				if_data = ud->bSlaveInterface[0];
			break;
		}
	}

	if (if_data == -1) {
		DPRINTF(("urndis_attach: no union interface\n"));
		sc->sc_iface_data = sc->sc_iface_ctl;
	} else {
		DPRINTF(("urndis_attach: union interface: ctl %u, data %u\n",
		    if_ctl, if_data));
		for (i = 0; i < uiaa->uiaa_nifaces; i++) {
			if (uiaa->uiaa_ifaces[i] != NULL) {
				id = usbd_get_interface_descriptor(
				    uiaa->uiaa_ifaces[i]);
				if (id != NULL && id->bInterfaceNumber ==
				    if_data) {
					sc->sc_iface_data = uiaa->uiaa_ifaces[i];
					uiaa->uiaa_ifaces[i] = NULL;
				}
			}
		}
	}

	if (sc->sc_iface_data == NULL) {
		printf("%s: no data interface\n", DEVNAME(sc));
		return;
	}

	id = usbd_get_interface_descriptor(sc->sc_iface_data);
	cd = usbd_get_config_descriptor(sc->sc_udev);
	altcnt = usbd_get_no_alts(cd, id->bInterfaceNumber);

	for (j = 0; j < altcnt; j++) {
		if (usbd_set_interface(sc->sc_iface_data, j)) {
			printf("%s: interface alternate setting %u failed\n",
			    DEVNAME(sc), j);
			return;
		}
		/* Find endpoints. */
		id = usbd_get_interface_descriptor(sc->sc_iface_data);
		sc->sc_bulkin_no = sc->sc_bulkout_no = -1;
		for (i = 0; i < id->bNumEndpoints; i++) {
			ed = usbd_interface2endpoint_descriptor(
			    sc->sc_iface_data, i);
			if (!ed) {
				printf("%s: no descriptor for bulk endpoint "
				    "%u\n", DEVNAME(sc), i);
				return;
			}
			if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_IN &&
			    UE_GET_XFERTYPE(ed->bmAttributes) == UE_BULK) {
				sc->sc_bulkin_no = ed->bEndpointAddress;
			}
			else if (
			    UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_OUT &&
			    UE_GET_XFERTYPE(ed->bmAttributes) == UE_BULK) {
				sc->sc_bulkout_no = ed->bEndpointAddress;
			}
		}

		if (sc->sc_bulkin_no != -1 && sc->sc_bulkout_no != -1) {
			DPRINTF(("%s: in=0x%x, out=0x%x\n",
			    DEVNAME(sc),
			    sc->sc_bulkin_no,
			    sc->sc_bulkout_no));
			goto found;
		}
	}

	if (sc->sc_bulkin_no == -1)
		printf("%s: could not find data bulk in\n", DEVNAME(sc));
	if (sc->sc_bulkout_no == -1 )
		printf("%s: could not find data bulk out\n", DEVNAME(sc));
	return;

	found:

	ifp = GET_IFP(sc);
	ifp->if_softc = sc;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
	ifp->if_start = urndis_start;
	ifp->if_ioctl = urndis_ioctl;
	ifp->if_init = urndis_init;
#if 0
	ifp->if_watchdog = urndis_watchdog;
#endif

	strlcpy(ifp->if_xname, DEVNAME(sc), IFNAMSIZ);

	IFQ_SET_READY(&ifp->if_snd);

	urndis_init(ifp);

	s = splnet();

	if (urndis_ctrl_query(sc, OID_802_3_PERMANENT_ADDRESS, NULL, 0,
	    &buf, &bufsz) != RNDIS_STATUS_SUCCESS) {
		printf("%s: unable to get hardware address\n", DEVNAME(sc));
		urndis_stop(ifp);
		splx(s);
		return;
	}

	if (bufsz == ETHER_ADDR_LEN) {
		memcpy(eaddr, buf, ETHER_ADDR_LEN);
		printf("%s: address %s\n", DEVNAME(sc), ether_sprintf(eaddr));
		kmem_free(buf, bufsz);
	} else {
		printf("%s: invalid address\n", DEVNAME(sc));
		kmem_free(buf, bufsz);
		urndis_stop(ifp);
		splx(s);
		return;
	}

	/* Initialize packet filter */
	sc->sc_filter = RNDIS_PACKET_TYPE_BROADCAST;
	sc->sc_filter |= RNDIS_PACKET_TYPE_ALL_MULTICAST;
	filter = htole32(sc->sc_filter);
	if (urndis_ctrl_set(sc, OID_GEN_CURRENT_PACKET_FILTER, &filter,
	    sizeof(filter)) != RNDIS_STATUS_SUCCESS) {
		printf("%s: unable to set data filters\n", DEVNAME(sc));
		urndis_stop(ifp);
		splx(s);
		return;
	}

	if_attach(ifp);
	ether_ifattach(ifp, eaddr);
	sc->sc_attached = 1;

	splx(s);
}

static int
urndis_detach(device_t self, int flags)
{
	struct urndis_softc	*sc;
	struct ifnet		*ifp;
	int			 s;

	sc = device_private(self);

	DPRINTF(("urndis_detach: %s flags %u\n", DEVNAME(sc),
	    flags));

	if (!sc->sc_attached)
		return 0;

	s = splusb();

	ifp = GET_IFP(sc);

	if (ifp->if_softc != NULL) {
		ether_ifdetach(ifp);
		if_detach(ifp);
	}

	urndis_stop(ifp);
	sc->sc_attached = 0;

	splx(s);

	return 0;
}

static int
urndis_activate(device_t self, enum devact act)
{
	struct urndis_softc *sc;

	sc = device_private(self);

	switch (act) {
	case DVACT_DEACTIVATE:
		sc->sc_dying = 1;
		return 0;
	}

	return EOPNOTSUPP;
}


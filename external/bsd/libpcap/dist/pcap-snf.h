/*	$NetBSD$	*/

pcap_t *snf_create(const char *, char *, int *);
int snf_findalldevs(pcap_if_t **devlistp, char *errbuf);

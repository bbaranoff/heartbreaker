#ifndef DOS_H
#define DOS_H

struct {
	int camp;
	int rach;
	int attach;
	int detach;

	int t3211_sec;
	int t3211_msec;

	int max_retrans;
} dos;

struct {
	int pid;
	int dcs;
} silent_sms;

#endif


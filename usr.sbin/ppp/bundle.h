/*-
 * Copyright (c) 1998 Brian Somers <brian@Awfulhak.org>
 * All rights reserved.
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
 *	$Id: bundle.h,v 1.1.2.26 1998/04/05 22:48:02 brian Exp $
 */

#define	PHASE_DEAD		0	/* Link is dead */
#define	PHASE_ESTABLISH		1	/* Establishing link */
#define	PHASE_AUTHENTICATE	2	/* Being authenticated */
#define	PHASE_NETWORK		3	/* We're alive ! */
#define	PHASE_TERMINATE		4	/* Terminating link */


struct datalink;
struct physical;
struct link;
struct server;
struct prompt;

struct bundle {
  struct descriptor desc;     /* really all our datalinks */
  int unit;                   /* The tun number */
  int ifIndex;                /* The interface number */
  int tun_fd;                 /* The /dev/tunX descriptor */
  char dev[20];               /* The /dev/tunX name */
  char *ifname;               /* The interface name */
  int routing_seq;            /* The current routing sequence number */
  u_int phase;                /* Curent phase */
  int phys_type;              /* Union of all physical::type's */

  unsigned CleaningUp : 1;    /* Going to exit.... */

  struct fsm_parent fsm;      /* Our callback functions */
  struct datalink *links;     /* Our data links */

  struct {
    int idle_timeout;         /* NCP Idle timeout value */
    struct {
      char name[50];          /* PAP/CHAP system name */
      char key[50];           /* PAP/CHAP key */
    } auth;
  } cfg;

  struct {
    struct ipcp ipcp;         /* Our IPCP FSM */
    struct mp mp;             /* Our MP */
  } ncp;

  struct {
    struct filter in;		/* incoming packet filter */
    struct filter out;		/* outgoing packet filter */
    struct filter dial;		/* dial-out packet filter */
    struct filter alive;	/* keep-alive packet filter */
  } filter;

  struct {
    struct pppTimer timer;      /* timeout after cfg.idle_timeout */
    time_t done;
  } idle;

  struct {
    int fd;                     /* write status here */
  } notify;
};

#define descriptor2bundle(d) \
  ((d)->type == BUNDLE_DESCRIPTOR ? (struct bundle *)(d) : NULL)

extern struct bundle *bundle_Create(const char *, struct prompt *, int);
extern void bundle_Destroy(struct bundle *);
extern const char *bundle_PhaseName(struct bundle *);
#define bundle_Phase(b) ((b)->phase)
extern void bundle_NewPhase(struct bundle *, u_int);
extern int  bundle_LinkIsUp(const struct bundle *);
extern void bundle_SetRoute(struct bundle *, int, struct in_addr,
                            struct in_addr, struct in_addr, int);
extern void bundle_LinkLost(struct bundle *, struct physical *, int);
extern void bundle_Close(struct bundle *, const char *, int);
extern void bundle_Open(struct bundle *, const char *, int);
extern void bundle_LinkClosed(struct bundle *, struct datalink *);

extern int bundle_FillQueues(struct bundle *);
extern int bundle_ShowLinks(struct cmdargs const *);
extern void bundle_StartIdleTimer(struct bundle *);
extern void bundle_SetIdleTimer(struct bundle *, int);
extern void bundle_StopIdleTimer(struct bundle *);
extern int bundle_RemainingIdleTime(struct bundle *);
extern void bundle_LayerUp(struct bundle *, struct fsm *);
extern int bundle_IsDead(struct bundle *);
extern struct datalink *bundle2datalink(struct bundle *, const char *);

extern void bundle_RegisterDescriptor(struct bundle *, struct descriptor *);
extern void bundle_UnRegisterDescriptor(struct bundle *, struct descriptor *);

extern void bundle_DelPromptDescriptors(struct bundle *, struct server *);
extern void bundle_DisplayPrompt(struct bundle *);
extern void bundle_WriteTermPrompt(struct bundle *, struct datalink *,
                                   const char *, int);
extern void bundle_SetTtyCommandMode(struct bundle *, struct datalink *);

extern void bundle_DatalinkClone(struct bundle *, struct datalink *,
                                 const char *);
extern void bundle_DatalinkRemove(struct bundle *, struct datalink *);
extern void bundle_CleanDatalinks(struct bundle *);

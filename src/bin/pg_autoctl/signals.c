/*
 * src/bin/pg_autoctl/signals.c
 *   Signal handlers for pg_autoctl, used in loop.c and pgsetup.c
 *
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the PostgreSQL License.
 *
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "postgres_fe.h"        /* pqsignal, portable sigaction wrapper */

#include "defaults.h"
#include "log.h"
#include "signals.h"

/* This flag controls termination of the main loop. */
volatile sig_atomic_t asked_to_stop = 0;      /* SIGTERM */
volatile sig_atomic_t asked_to_stop_fast = 0; /* SIGINT */
volatile sig_atomic_t asked_to_reload = 0;    /* SIGHUP */

/*
 * set_signal_handlers sets our signal handlers for the 4 signals that we
 * specifically handle in pg_autoctl.
 */
void
set_signal_handlers(bool exitOnQuit)
{
	/* Establish a handler for signals. */
	log_trace("set_signal_handlers%s", exitOnQuit ? " (exit on quit)" : "");

	pqsignal(SIGHUP, catch_reload);
	pqsignal(SIGINT, catch_int);
	pqsignal(SIGTERM, catch_term);

	if (exitOnQuit)
	{
		pqsignal(SIGQUIT, catch_quit);
	}
	else
	{
		pqsignal(SIGQUIT, catch_int);
	}
}


/*
 * catch_reload receives the SIGHUP signal.
 */
void
catch_reload(int sig)
{
	asked_to_reload = 1;
	pqsignal(sig, catch_reload);
}


/*
 * catch_int receives the SIGINT signal.
 */
void
catch_int(int sig)
{
	asked_to_stop_fast = 1;
	pqsignal(sig, catch_int);
}


/*
 * catch_stop receives SIGTERM signal.
 */
void
catch_term(int sig)
{
	asked_to_stop = 1;
	pqsignal(sig, catch_term);
}


/*
 * catch_quit receives the SIGQUIT signal.
 */
void
catch_quit(int sig)
{
	/* default signal handler disposition is to core dump, we don't */
	exit(EXIT_CODE_QUIT);
}

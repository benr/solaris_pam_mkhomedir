/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 1995, by Sun Microsystems, Inc.
 * Copyright (c) 2011, by Ben Rockwood <benr@cuddletech.com>
 * All rights reserved.
 */

#pragma	ident	"@(#)sample_open_session.c	1.6	05/06/08 SMI"

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <syslog.h>
#include "homedir.h"

#include <pwd.h>
#include <sys/stat.h>

/*ARGSUSED*/
int pam_sm_open_session( pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	char *user;
	char *host;
	char *service;
	char *homedir;
	uid_t uid;
	gid_t gid;
	struct passwd *pass;
	struct stat   hdir;

	// Open SYSLOG auth.
	openlog("pam_mkhomedir", LOG_NDELAY, LOG_AUTH);
	
	// Grab some info from the PAM Handle
	pam_get_item(pamh, PAM_USER, (void**)&user); 
	pam_get_item(pamh, PAM_RHOST, (void**)&host);
	if ( host == NULL || *host == '\0')
		host = "localhost";
	pam_get_item(pamh, PAM_SERVICE, (void**)&service);

	/* Grab the Password structure for the PAM User & Get the home dir path */
	pass = getpwnam(user);
	homedir = pass->pw_dir;
	uid = pass->pw_uid;
	gid = pass->pw_gid;

	/* Determine whether or not the homedir exists */
	if ( stat(homedir, &hdir) < 0 ) {
		syslog(LOG_NOTICE, "User %s does not have local home directory %s\n", user, homedir);
		// Create it now.
		if ( create_home(homedir, "/etc/skel/", uid, gid) != 0 ) {
			syslog(LOG_NOTICE, "Creation of %s for %s failed.\n", homedir, user);
			return (PAM_IGNORE);
		} else {
			syslog(LOG_NOTICE, "Successfully created %s for %s!\n", homedir, user);
		}

	}   else {
		syslog(LOG_DEBUG, "User %s DOES have local home directory %s.  Nothing to do.\n", user, homedir);
	}
	
	// If we get here, life is good, just exit with success.
	
	/* Debugging Assist */
	//syslog(LOG_NOTICE, "User %s from %host using service %s is logging in: %s\n", user, host, service, homedir);



	closelog();
	return (PAM_SUCCESS);
}


/* We don't need to get in the way off a logout. */
int pam_sm_close_session( pam_handle_t *pamh, int     flags, int     argc, const char **argv) {
        return (PAM_SUCCESS);
}


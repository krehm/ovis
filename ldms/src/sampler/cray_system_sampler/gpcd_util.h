/*
 * Copyright (c) 2013-2015 Open Grid Computing, Inc. All rights reserved.
 * Copyright (c) 2013-2015 Sandia Corporation. All rights reserved.
 * Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 * license for use of this work by or on behalf of the U.S. Government.
 * Export of this program may require a license from the United States
 * Government.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the BSD-type
 * license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *      Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *      Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 *      Neither the name of Sandia nor the names of any contributors may
 *      be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 *      Neither the name of Open Grid Computing nor the names of any
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *      Modified source versions must be plainly marked as such, and
 *      must not be misrepresented as being the original software.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \file gpcd_util.h
 * \brief Utilities for processing and aggregating the gemini perf counters
 */

#ifndef __GPCD_UTIL_H_
#define __GPCD_UTIL_H_

#define _GNU_SOURCE

#include <inttypes.h>
#include <unistd.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <rs_id.h>
#include "gpcd_lib.h"
#include "gemini.h"
#include "ldms.h"
#include "ldmsd.h"


/* These are the NIC COUNTERS. The wrap is used to make an
   enum for the raw values. In the sampler are the metric
   names and the computation of the metrics from these. */

#define NUM_NIC_PERF_RAW 13

#define STR_WRAP(NAME) #NAME
#define PREFIX_ENUM_R(NAME) R_ ## NAME


#define NIC_PERF_RAW_NEWLIST(WRAP) \
	WRAP(GM_ORB_PERF_VC0_FLITS),		 \
		WRAP(GM_NPT_PERF_ACP_FLIT_CNTR), \
		WRAP(GM_NPT_PERF_NRP_FLIT_CNTR), \
		WRAP(GM_NPT_PERF_NPT_FLIT_CNTR), \
		WRAP(GM_ORB_PERF_VC0_PKTS),		   \
		WRAP(GM_NPT_PERF_NL_RSP_PKT_CNTR),	   \
		WRAP(GM_RAT_PERF_DATA_FLITS_VC0),	   \
		WRAP(GM_ORB_PERF_VC1_FLITS),	   \
		WRAP(GM_ORB_PERF_VC1_PKTS),	   \
		WRAP(GM_TARB_PERF_FMA_FLITS),	   \
		WRAP(GM_TARB_PERF_FMA_PKTS),	   \
		WRAP(GM_TARB_PERF_BTE_FLITS),	   \
		WRAP(GM_TARB_PERF_BTE_PKTS)


static char* nic_perf_raw_name[] = {
	NIC_PERF_RAW_NEWLIST(STR_WRAP)
};

typedef enum {
	NIC_PERF_RAW_NEWLIST(PREFIX_ENUM_R)
} nic_perf_raw_t;

gpcd_context_t *gem_link_perf_create_context(ldmsd_msg_log_f*);
gpcd_context_t *nic_perf_create_context(ldmsd_msg_log_f*);

#endif

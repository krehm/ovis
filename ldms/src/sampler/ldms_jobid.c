/*
 * Copyright (c) 2010-16 Open Grid Computing, Inc. All rights reserved.
 * Copyright (c) 2010-16 Sandia Corporation. All rights reserved.
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
 * \file ldms_jobid.c
 * \brief /var/run/ldms.jobid shared data provider.
 */
#define _GNU_SOURCE
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "ldms.h"
#include "ldmsd.h"
#include "ldms_jobid.h"

#ifdef ENABLE_JOBID

/* This sampler relies on the resource manager to
place notes in JOBID_FILE about the current job, or 0 if the
node is not reserved. If this file is absent, the sampler
will run, reporting 0 jobid.

The file checked is by default:
	/var/run/ldms.jobinfo
exists if the next lines are added to slurm-prolog or equivalent
	echo JOBID=$SLURM_JOBID > /var/run/ldms.slurm.jobinfo
	echo UID=$SLURM_UID >> /var/run/ldms.jobinfo
	echo USER=$SLURM_USER >> /var/run/ldms.jobinfo
and the next line is added to slurm-epilog or equivalent
	echo "JOBID=0" > /var/run/ldms.jobinfo
On some slurm 2.3.x systems, these are in /etc/nodestate/bin/.
On systems with non-slurm reservations or shared reservations,
this sampler may need to be cloned and modified.

The functionality is generic to files containing the
key=value pairs listed above.
*/
#define DEFAULT_FILE "/var/run/ldms.jobinfo"
#define SAMP "jobid"
static char *default_schema_name = SAMP;
static const char *JOBID_FILE = DEFAULT_FILE;
static const char *JOBID_COLNAME = LJI_JOBID_METRIC_NAME;

static char *procfile = NULL;

static char *metric_name = NULL;
static ldms_set_t set = NULL;
static ldmsd_msg_log_f msglog;
static char *producer_name;
static ldms_schema_t schema;
static uint64_t compid = 0;

static struct ldms_job_info ji;

static int32_t compid_pos = 0;
static int32_t jobid_pos = 0;
static int32_t uid_pos = 0;
static int32_t user_pos = 0;


#define JI_SIZE 32768
static char ji_buf[JI_SIZE];
static int parse_jobinfo(const char* file, struct ldms_job_info *ji, ldms_set_t js) {
	int rc = 0;
	ji->jobid = ji->uid = 0;
	memset(&(ji->user[0]), '\0', LJI_USER_NAME_MAX);
	strcpy(&(ji->user[0]), "root");
	FILE *mf = fopen(file, "r");
	struct attr_value_list *kvl = NULL;
	struct attr_value_list *avl = NULL;

	if (!mf) {
		msglog(LDMSD_LINFO,"Could not open the jobid file '%s'\n", procfile);
		rc = EINVAL;
		goto err;
	}
	rc = fseek(mf, 0, SEEK_END);
	if (rc)
		goto err;
	long flen = ftell(mf);
	if (flen >= JI_SIZE) {
		rc = E2BIG;
		goto err;
	}
	rewind(mf);

	{
		size_t nread = fread(ji_buf, sizeof(char), flen, mf);
		ji_buf[flen] = '\0';
		if (nread < flen) {
			ji_buf[nread] = '\0';
		}
		fclose(mf);

		int maxlist = 1;
		char *t = ji_buf;
		while (t[0] != '\0') {
			if (isspace(t[0])) maxlist++;
			t++;
		}
		kvl = av_new(maxlist);
		avl = av_new(maxlist);
		rc = tokenize(ji_buf,kvl,avl);
		if (rc) {
			goto err;
		}

		char *tmp = av_value(avl, "JOBID");
		if (tmp) {
			char *endp = NULL;
			errno = 0;
			uint64_t j = strtoull(tmp, &endp, 0);
			if (endp == tmp || errno) {
				msglog(LDMSD_LERROR,"Fail parsing JOBID '%s'\n", tmp);
				rc = EINVAL;
				goto err;
			}
			ji->jobid = j;

			tmp = av_value(avl, "UID");
			if (tmp) {
				endp = NULL;
				errno = 0;
				j = strtoull(tmp, &endp, 0);
				if (endp == tmp || errno) {
					msglog(LDMSD_LINFO,"Fail parsing UID '%s'\n",
						tmp);
					rc = EINVAL;
					goto err;
				}
				ji->uid = j;
			}

			tmp = av_value(avl, "USER");
			if (tmp) {
				if (strlen(tmp) >= LJI_USER_NAME_MAX) {
					msglog(LDMSD_LINFO,"Username too long '%s'\n",
						tmp);
					rc = EINVAL;
					goto err;
				} else {
					strcpy(ji->user,tmp);
				}
			}
		}


		ldms_transaction_begin(js);

		union ldms_value v;
		v.v_u64 = compid;
		ldms_metric_set(js, compid_pos, &v);

		v.v_u64 = ji->jobid;
		ldms_metric_set(js, jobid_pos, &v);

		v.v_u64 = ji->uid;
		ldms_metric_set(js, uid_pos, &v);

		ldms_mval_t mv = (ldms_mval_t) &(ji->user);
		ldms_metric_array_set(js, user_pos, mv, 0, LJI_USER_NAME_MAX);

		ldms_transaction_end(js);
		rc = 0;

	}
err:
	free(kvl);
	free(avl);
	return rc;
}

static int create_metric_set(const char *instance_name, char* schema_name)
{
	int rc = 0;

	schema = ldms_schema_new(schema_name);
	if (!schema) {
		rc = ENOMEM;
		msglog(LDMSD_LERROR, SAMP ": ldms_schema_new fail.\n");
		goto err;
	}

	rc = ldms_schema_meta_add(schema, "component_id", LDMS_V_U64);
	if (rc < 0) {
		msglog(LDMSD_LERROR, SAMP ": ldms_schema_meta_add component_id fail.\n");
		goto err;
	}
	compid_pos = rc;

	rc = ldms_schema_metric_add(schema, metric_name,
		LDMS_V_U64);
	if (rc < 0) {
		msglog(LDMSD_LERROR, SAMP ": ldms_schema_metric_add "
			"%s fail.\n", metric_name);
		goto err;
	}
	jobid_pos = rc;

	rc = ldms_schema_metric_add(schema, LJI_UID_METRIC_NAME,
		LDMS_V_U64);
	if (rc < 0) {
		msglog(LDMSD_LERROR, SAMP ": ldms_schema_metric_add "
			LJI_UID_METRIC_NAME " fail.\n");
		goto err;
	}
	uid_pos = rc;

	rc = ldms_schema_metric_array_add(schema, LJI_USER_METRIC_NAME,
		LDMS_V_CHAR_ARRAY, LJI_USER_NAME_MAX);
	if (rc < 0) {
		msglog(LDMSD_LERROR, SAMP ": ldms_schema_metric_array_add "
			LJI_USER_METRIC_NAME " fail.\n");
		goto err;
	}
	user_pos = rc;
	rc = 0;

	set = ldms_set_new(instance_name, schema);
	if (!set) {
		rc = errno;
		msglog(LDMSD_LERROR, SAMP ": ldms_set_new fail.\n");
		goto err;
	}

	/*
	 * Process the file to init jobid.
	 */
	rc = parse_jobinfo(procfile, &ji, set);
	if (rc) {
		msglog(LDMSD_LERROR, SAMP ": parse_jobinfo fail.\n");
		goto err;
	}
	return 0;

 err:
	if (set) {
		ldms_set_delete(set);
		set = NULL;
	}
	msglog(LDMSD_LDEBUG, SAMP ": rc=%d: %s.\n",rc,strerror(-rc));
	return -rc;
}

/**
 * check for invalid flags, with particular emphasis on warning the user about
 */
static int config_check(struct attr_value_list *kwl, struct attr_value_list *avl, void *arg)
{
	char *value;
	int i;

	char* deprecated[]={"set","colname"};

	for (i = 0; i < (sizeof(deprecated)/sizeof(deprecated[0])); i++){
		value = av_value(avl, deprecated[i]);
		if (value){
			msglog(LDMSD_LERROR, SAMP "config argument %s has been deprecated.\n",
			       deprecated[i]);
			return EINVAL;
		}
	}

	return 0;
}


/**
 * \brief Configuration
 *
 * config name=jobid component_id=<comp_id> set=<setname>
 *     comp_id     The component id value.
 *     file  The file to find job id in on 1st line in ascii.
 *     setname     The set name.
 */
static int config(struct ldmsd_plugin *self, struct attr_value_list *kwl, struct attr_value_list *avl)
{
	char *value;
	char *sname;
	char *iname;
	void * arg = NULL;
	int rc;

	if (set) {
		msglog(LDMSD_LERROR, SAMP ": Set already created.\n");
		return EINVAL;
	}

	rc = config_check(kwl, avl, arg);
	if (rc != 0){
		return rc;
	}

	producer_name = av_value(avl, "producer");
	if (!producer_name) {
		msglog(LDMSD_LERROR, SAMP ": missing producer.\n");
		return ENOENT;
	}

	iname = av_value(avl, "instance");
	if (!iname) {
		msglog(LDMSD_LERROR, SAMP ": missing instance.\n");
		return ENOENT;
	}

	sname = av_value(avl, "schema");
	if (!sname)
		sname = default_schema_name;
	if (strlen(sname) == 0) {
		msglog(LDMSD_LERROR, SAMP ": schema name invalid.\n");
		return EINVAL;
	}

	value = av_value(avl, "component_id");
	if (value) {
		char *endp = NULL;
		errno = 0;
		compid = strtoull(value, &endp, 0);
		if (endp == value || errno) {
			msglog(LDMSD_LERROR,"Fail parsing component_id '%s'\n",
				value);
			return EINVAL;
		}
	}

	value = av_value(avl, "file");
	if (value) {
		procfile = strdup(value);
		if (!procfile) {
			msglog(LDMSD_LERROR,SAMP " no memory\n");
			return ENOMEM;
		}
	} else {
		procfile = (char *)JOBID_FILE;
	}

	metric_name = (char *)JOBID_COLNAME;
	msglog(LDMSD_LDEBUG,"Jobid file is '%s'\n", procfile);



	rc = create_metric_set(iname, sname);
	if (rc) {
		msglog(LDMSD_LERROR, SAMP ": failed to create a metric set.\n");
		return rc;
	}
	ldms_set_producer_name_set(set, producer_name);
	return 0;
}

static ldms_set_t get_set(struct ldmsd_sampler *self)
{
	return set;
}

static int sample(struct ldmsd_sampler *self)
{
	if (!set) {
		msglog(LDMSD_LDEBUG,SAMP ": plugin not initialized\n");
		return EINVAL;
	}

	/* unlike device drivers, the file for jobid gets replaced
	   and must be reread every time. In production clusters, it's
	   almost certain in a local ram file system, so this is fast
	   enough compared to contacting a resource manager daemon.
	*/
	(void)parse_jobinfo(procfile, &ji, set);

	/* as a policy matter, missing file has the value 0, not an error. */
	return 0;
}

int ldms_job_info_get(struct ldms_job_info *ji, unsigned flags)
{
	if (!set) {
		return EAGAIN;
	}
	if (flags & LJI_jobid)
		ji->jobid = ldms_metric_get_u64(set, jobid_pos);
	if (flags & LJI_uid)
		ji->uid = ldms_metric_get_u64(set, uid_pos);
	if (flags & LJI_user) {
		const char * a = (char *)ldms_metric_array_get(set, user_pos);
		strncpy(ji->user, a, LJI_USER_NAME_MAX );
	}

	return 0;
}

static void term(struct ldmsd_plugin *self)
{
	if (schema)
		ldms_schema_delete(schema);
	schema = NULL;
	if (set)
		ldms_set_delete(set);
	set = NULL;

	if (metric_name && metric_name != JOBID_COLNAME ) {
		free(metric_name);
	}
	if ( procfile && procfile != JOBID_FILE ) {
		free(procfile);
	}
}

static const char *usage(struct ldmsd_plugin *self)
{
	return "config name=jobid producer=<prod_name> instance=<inst_name> "
		"[component_id=<compid> schema=<sname>] [file=<jobinfo>]"
		"    comp_id     The component id value.\n"
		"    <prod_name> The producer name\n"
		"    <inst_name> The instance name\n"
		"    <sname>     Optional schema name. Defaults to 'jobid'\n"
		"    <jobinfo>   Optional file to read.\n"
		"The default jobinfo is " DEFAULT_FILE ".\n"
		;
}

static struct ldmsd_sampler jobid_plugin = {
	.base = {
		.name = SAMP,
		.term = term,
		.config = config,
		.usage = usage,
	},
	.get_set = get_set,
	.sample = sample,
};

struct ldmsd_plugin *get_plugin(ldmsd_msg_log_f pf)
{
	msglog = pf;
	set = NULL;
	return &jobid_plugin.base;
}
#else

struct ldmsd_plugin *get_plugin(ldmsd_msg_log_f pf)
{
	pf(LDMSD_LERROR,"jobid plugin not enabled. (configured --disable-jobid)\n");
	return NULL;
}

#endif /* ifdef ENABLE_JOBID */

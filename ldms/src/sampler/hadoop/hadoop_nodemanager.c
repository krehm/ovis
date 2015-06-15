/* -*- c-basic-offset: 8 -*-
 * Copyright (c) 2013 Open Grid Computing, Inc. All rights reserved.
 * Copyright (c) 2013 Cray Inc. All rights reserved.
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

/*
 * hadoop_nodemanager.c
 *
 *  Created on: Mar 31, 2014
 *      Author: nichamon
 */

#include <stdio.h>
#include <inttypes.h>
#include <errno.h>
#include <pthread.h>
#include "sampler_hadoop.h"
#include "coll/str_map.h"

static char *producer_name;
static char *metric_name_file;
static int port;

static struct hadoop_set nodemanager_set;
static pthread_t thread;
static ldmsd_msg_log_f msglog;

static const char *usage(void)
{
	return  "config name=hadoop_nodemanager producer_name=<producer_name> instance_name=<instance_name>\n"
		"	port=<port> file=<file> \n"
		"    producer_name       The producer id value.\n"
		"    instance_name     The set name.\n"
		"    port	       The listener port which the LDMS sink sends data to.\n"
		"    file	       The file that contains metric names and their ldms_metric_types.\n"
		"		 For example, "
		"			jvm.metrics:memNonHeapUsedM	F\n"
		"			dfs.FSNamesystem:FileTotal	S32\n";
}

static ldms_set_t get_set()
{
	return nodemanager_set.set;
}

/**
 * \brief Configuration
 *
 * config name=hadoop_nodemanager producer_name=<producer_name> instance_name=<instance_name>
 *		port=<port> file=<file>
 *     producer_name       The producer id value.
 *     instance_name     The set name.
 *     port	         The listener port which the LDMS sink sends data to
 *     file	         The file that contains metric names and their ldms_metric_types.
 */
static int config(struct attr_value_list *kwl, struct attr_value_list *avl)
{
	char *value;
	char *attr;

	attr = "producer_name";
	producer_name = av_value(avl, attr);
	if (!producer_name)
		goto enoent;

	attr = "port";
	value = av_value(avl, attr);
	if (value)
		port = atoi(value);
	else
		goto enoent;

	attr = "file";
	value = av_value(avl, attr);
	if (!value) {
		msglog(LDMSD_LERROR, "hadoop_nodemanager: no file is given.\n");
		goto enoent;
	}
	metric_name_file = strdup(value);

	attr = "instance_name";
	value = av_value(avl, attr);
	if (value)
		nodemanager_set.setname = strdup(value);
	else
		goto enoent;

	nodemanager_set.daemon = "nodemanager";
	nodemanager_set.msglog = msglog;
	int rc = setup_datagram(port, &nodemanager_set.sockfd);
	if (rc) {
		msglog(LDMSD_LERROR, "hadoop_nodemanager: failed to setup "
				"datagram between ldmsd and hadoop.\n");
		goto err_1;
	}

	rc = create_hadoop_set(metric_name_file, &nodemanager_set, producer_name);
	if (rc)
		goto err_2;
	rc = pthread_create(&thread, NULL, recv_metrics, &nodemanager_set);
	if (rc) {
		destroy_hadoop_set(&nodemanager_set);
		goto err_0;
	}
	return 0;
err_2:
	close(nodemanager_set.sockfd);
err_1:
	free(nodemanager_set.setname);
err_0:
	return rc;
enoent:
	msglog(LDMSD_LERROR, "hadoop_nodemanager: need %s. Error %d\n", attr, ENOENT);
	return ENOENT;
}

static int sample(void)
{
	/* Do nothing */
	return 0;
}

static void term(void)
{
	destroy_hadoop_set(&nodemanager_set);
}

static struct ldmsd_sampler hadoop_nodemanager = {
	.base = {
		.name = "hadoop_nodemanager",
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
	return &hadoop_nodemanager.base;
}

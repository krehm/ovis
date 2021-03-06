/* -*- c-basic-offset: 8 -*-
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
 * \file rca_metrics.c
 * \brief Functions used in the cray_system_sampler that are particular to
 * rca (mesh coord).
 */


#include <rca_lib.h>
#include <rs_id.h>
#include <rs_meshcoord.h>
#include "rca_metrics.h"

int nettopo_setup(ldmsd_msg_log_f msglog)
{
	rs_node_t node;
	mesh_coord_t loc;
	uint16_t nid;

	rca_get_nodeid(&node);
	nid = (uint16_t)node.rs_node_s._node_id;
	rca_get_meshcoord(nid, &loc);
	nettopo_coord.x = loc.mesh_x;
	nettopo_coord.y = loc.mesh_y;
	nettopo_coord.z = loc.mesh_z;

	return 0;
}

int sample_metrics_nettopo(ldms_set_t set, ldmsd_msg_log_f msglog)
{

	union ldms_value v;

	/*  Fill in mesh coords (this is static and should be moved) */
	/* will want these 3 to be LDMS_V_U8 */
	v.v_u64 = (uint64_t) nettopo_coord.x;
	ldms_metric_set(set, nettopo_metric_table[0], &v);
	v.v_u64 = (uint64_t) nettopo_coord.y;
	ldms_metric_set(set, nettopo_metric_table[1], &v);
	v.v_u64 = (uint64_t) nettopo_coord.z;
	ldms_metric_set(set, nettopo_metric_table[2], &v);

	return 0;
}

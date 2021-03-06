/* -*- c-basic-offset: 8 -*-
 * Copyright (c) 2015-2016 Open Grid Computing, Inc. All rights reserved.
 * Copyright (c) 2015-2016 Sandia Corporation. All rights reserved.
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
 * \author Narate Taerat (narate at ogc dot us)
 * \file bhttpd_query.c
 */

/**
 * \page bhttpd_uri
 *
 * \section bhttpd_uri_query query
 *
 * <b>query</b> serves several baler information query requests. The 'type'
 * parameter determines query type. Each type has its own specific set of
 * parameters as described in the following sub sections.
 *
 * \subsection bhttpd_uri_query_ptn type=ptn
 * Get all available patterns from \c bhttpd.
 *
 * \par URI
 * <code>BHTTPD_LOCATION/query?type=ptn[&use_ts_fmt=<0|1>]</code>
 *
 * \par Response
 * If there is <b>no error</b>, pattern query returns JSON objects describing
 * each pattern. If <code>use_ts_fmt</code> is set to 0, the human-readable
 * date-time is used as follow:
 * \code{.json}
 * {
 *     "result": [
 *         {   // pattern object
 *             "type": "PTN",
 *             "ptn_id": <PTN_ID>,
 *             "count": <OCCURRENCE COUNT>,
 *             "first_seen": <yyyy-mm-dd HH:MM:SS.uuuuuu>,
 *             "last_seen": <yyyy-mm-dd HH:MM:SS.uuuuuu>,
 *             // pattern description is disguised in a message form --
 *             // a sequence of tokens.
 *             "msg": [
 *                 {
 *                     "tok_type": <TOK_TYPE>,
 *                     "text": <TEXT>
 *                 },
 *                 // more token objects describing the pattern messgae.
 *                 ...
 *             ]
 *         },
 *         ... // more pattern objects
 *     ]
 * }
 * \endcode
 *
 * \par
 * If <code>use_ts_fmt</code> is set to 1, the timestamp fields (such as
 * first_seen and last_seen) will be in Unix timestamp format (the number of
 * seconds since Epoch).
 *
 * \par
 * Please see msg2html() and tkn2html() methods in baler.coffee for the message
 * display utility functions.
 *
 * \par
 * The pattern objects returned in this query are only those derived from
 * messages. For metric patterns, please see query \ref
 * bhttpd_uri_query_metric_pattern.
 *
 * \par
 * <b>On error</b>, \b bhttpd response with an appropriate HTTP error code and a
 * message describing the error.
 *
 * \subsection bhttpd_uri_query_metric_pattern type=metric_pattern
 * Query metric patterns -- the patterns that represent metric-in-range events.
 *
 * \par URI
 * <code>BHTTPD_LOCATION/query?type=metric_pattern[&use_ts_fmt=<0|1>]</code>
 *
 * \par Response
 * This query returns objects similar to query \ref bhttpd_uri_query_ptn, except
 * that the returned objects are of metric pattern type. If there is no error,
 * and <code>use_ts_fmt</code> is 0, the following JSON objects are returned:
 * \code{.json}
 *     "result": [
 *         {   // pattern object
 *             "type": "PTN",
 *             "ptn_id": <PTN_ID>,
 *             "count": <OCCURRENCE COUNT>,
 *             "first_seen": <yyyy-mm-dd HH:MM:SS.uuuuuu>,
 *             "last_seen": <yyyy-mm-dd HH:MM:SS.uuuuuu>,
 *             // pattern description is disguised in a message form --
 *             // a sequence of tokens.
 *             "msg": [
 *                 {
 *                     "tok_type": <TOK_TYPE>,
 *                     "text": <TEXT>
 *                 },
 *                 // more token objects describing the pattern messgae.
 *                 ...
 *             ]
 *         },
 *         ... // more pattern objects
 *     ]
 * \endcode
 *
 * \par
 * If <code>use_ts_fmt</code> is set to 1, the timestamp fields (such as
 * first_seen and last_seen) will be in Unix timestamp format (the number of
 * seconds since Epoch).
 *
 * \par
 * Please see msg2html() and tkn2html() methods in baler.coffee for the message
 * display utility functions.
 *
 * \par
 * <b>On error</b>, \b bhttpd response with an appropriate HTTP error code and a
 * message describing the error.
 *
 * \subsection bhttpd_uri_query_msg type=msg
 * Query messages from the \c bhttpd. Because there can be a lot of messages
 * that matched the query, returning all messages in a single HTTP response is
 * impossible. So, message querying usage follows create-then-fetch scheme.
 *
 * \par usage
 *
 * -# request an initial query with some query criteria (\c ptn_ids, \c host_ids,
 *   \c ts0 and \c ts1).
 *   - the return objects will contain an initial result, along with \c
 *     session_id for the next data fetching. The number of returned messages
 *     can be controlled with parameter \c n. The direction parameter (\c dir)
 *     conrols the direction of the query.
 * -# Use \c session_id, \c n and \c dir to fetch the next, or previous, results.
 * -# Use URI \ref bhttpd_uri_query_destroy_session to destroy the session.
 *
 * \par URI
 * <code>BHTTPD_LOCATION/query?type=msg
 *                        [&n=<NUMBER>]
 *                        [&dir=DIRECTION]
 *                        [&session_id=SID]
 *                        [&host_ids=NUM_LIST]
 *                        [&ptn_ids=NUM_LIST]
 *                        [&ts0=UNIX_TS]
 *                        [&ts1=UNIX_TS]
 *                        [&use_ts_fmt=<0|1>]
 *                        </code>
 *
 * The parameters are described as follows:
 * - \b n: The maximum number of messages to retreive.
 * - \b dir: The direction of the fetch (\c fwd or \c bwd).
 * - \b session_id: The session ID (for fetching). Do not set this option for
 *   the first call of query.
 * - \b host_ids: The comma-separated numbers and ranges of numbers list of host
 *   IDs for the query. If this parameter is specified, only the results that
 *   match the hosts listed in the parameter are returned. If it is not
 *   specified, host IDs are not used in the result filtering.
 * - \b ptn_ids: The comma-separated list of numbers and ranges of numbers of
 *   pattern IDs for the query. If the parameter is specified, only the results
 *   that match the listed pattern IDs are returned. If not specified, pattern
 *   IDs are not used in the result filtering.
 * - \b ts0: The begin timestamp for the query. If specified, the records that
 *   have timestamp less than \c ts0 will be excluded.
 * - \b ts1: The end timestamp for the query. If specified, the records that
 *   have timestamp greater than \c ts1 will be excluded.
 * - \b use_ts_fmt: If set to 1, the Unix timestamp output format is used. If
 *   set to 0, the human readable date-time output format (yyyy-mm-dd
 *   HH:MM:SS.uuuuuu) is used.
 *
 * \subsection bhttpd_uri_query_destroy_session /destroy_session
 * This is the request to destroy unused message query (\ref
 * bhttpd_uri_query_msg) session.
 *
 * \par URI
 * <code>BHTTPD_LOCATION/query/destroy_session?session_id=SID</code>
 *
 * \par Response
 *
 * - If there is no error, \c bhttpd will reply an empty
 *   content with HTTP OK status (200).
 * - If there is an error, \c bhttpd response with an appropriate HTTP error
 *   code and a message describing the error.
 *
 * \subsection bhttpd_uri_query_meta type=meta
 * Meta pattern mapping query for the regular message patterns. For metric
 * patterns, please see \ref bhttpd_uri_query_metric_meta.
 *
 * \par URI
 * <code>BHTTPD_LOCATION/query/meta</code>
 *
 * \par Response
 * If there is no error, \c bhttpd response with HTTP status OK (200) with the
 * following contents:
 * \code{.json}
 * {
 *     "map": [
 *         [<PTN_ID>, <CLUSTER_ID>],
 *         ...
 *     ],
 *     "cluster_name": {
 *         <CLUSTER_ID>: <CLUSTER_NAME_TEXT>
 *     }
 * }
 * \endcode
 *
 * \par
 * If there is an error, \c bhttpd response with an appropriate HTTP error code
 * and a message describing the error.
 *
 * \subsection bhttpd_uri_query_metric_meta type=metric_meta
 * This is similar to \ref bhttpd_uri_query_meta, but for metric patterns.
 *
 * \par URI
 * <code>BHTTPD_LOCATION/query/metric_meta</code>
 *
 * \par Response
 * If there is no error, \c bhttpd response with HTTP status OK (200) with the
 * following contents:
 * \code{.json}
 * {
 *     "map": [
 *         [<PTN_ID>, <CLUSTER_ID>],
 *         ...
 *     ],
 *     "cluster_name": {
 *         <CLUSTER_ID>: <CLUSTER_NAME_TEXT>
 *     }
 * }
 * \endcode
 *
 * \par
 * If there is an error, \c bhttpd response with an appropriate HTTP error code
 * and a message describing the error.
 *
 * \subsection bhttpd_uri_query_img type=img
 * Query pixels according to the given query criteria. The criteria is similar
 * to \ref bhttpd_uri_query_msg.
 *
 * \par URI
 * <code>BHTTPD_LOCATION/query?type=img
 *                        &img_store=IMG_STORE
 *                        [&host_ids=NUM_LIST]
 *                        [&ptn_ids=NUM_LIST]
 *                        [&ts0=UNIX_TS]
 *                        [&ts1=UNIX_TS]
 *                        </code>
 *
 * \par
 * The following is the explanation of the parameters:
 * - \b img_store <b>(required)</b>: The image store to query against (see \ref
 *   bhttpd_uri_list_img_store for image store listing).
 * - \b host_ids: The comma-separated numbers and ranges of numbers list of host
 *   IDs for the query. If this parameter is specified, only the results that
 *   match the hosts listed in the parameter are returned. If it is not
 *   specified, host IDs are not used in the result filtering.
 * - \b ptn_ids: The comma-separated list of numbers and ranges of numbers of
 *   pattern IDs for the query. If the parameter is specified, only the results
 *   that match the listed pattern IDs are returned. If not specified, pattern
 *   IDs are not used in the result filtering.
 * - \b ts0: The begin timestamp for the query. If specified, the records that
 *   have timestamp less than \c ts0 will be excluded.
 * - \b ts1: The end timestamp for the query. If specified, the records that
 *   have timestamp greater than \c ts1 will be excluded.
 *
 * \par Response
 * If there is no error, \c bhttpd response with HTTP OK status (200). The
 * content of this query however is not in JSON format. It is a binary data
 * containing an array of pixels--each of which contai the number of occurrences
 * (COUNT) of the PTN_ID pattern at HOST_ID host in the time range TS ..
 * TS+DELTA. The DELTA is determined from the name of the \b img_store (e.g.
 * 3600-1 means DELTA = 3600). The format of the binary response is as the
 * following.
 * \code{.json}
 * [<TS(4byte)>, <HOST_ID(4byte)>, <PTN_ID(4byte)>, <COUNT(4byte)>]...
 * \endcode
 *
 * \par
 * If there is an error, \c bhttpd response with an appropriate HTTP error code
 * and a message describing the error.
 *
 * \subsection bhttpd_uri_query_img2 type=img2
 * This is similar to \ref bhttpd_uri_query_img, but \c bhttpd compose the image
 * in the given time-node ranges. The pixels returned to the requester are the
 * aggregated pixels, not pixel records by pattern IDs. The horizontal axis of
 * the image represents time (UNIX time - seconds since the Epoch), ascending
 * from left to right. The vertical axis ofthe image represents host IDs,
 * ascending from top to bottom.
 *
 * \see \ref bhttpd_uri_query_big_pic for the area in the time-node plane that
 * has some data.
 *
 * \par URI
 * <code>BHTTPD_LOCATION/query?type=img2
 *                        &img_store=IMG_STORE
 *                        &ts_begin=UNIX_TS
 *                        &host_begin=UNIX_TS
 *                        &spp=SEC_PER_PIXEL
 *                        &npp=NODE_PER_PIXEL
 *                        &width=IMAGE_WIDTH
 *                        &height=IMAGE_HEIGHT
 *                        [&ptn_ids=NUM_LIST]
 *                        </code>
 *
 * \par
 * The following is the explanatino for each parameter
 * - \b img_store <b>(required)</b>: The image store to query against (see \ref
 *   bhttpd_uri_list_img_store for image store listing).
 * - \b ts_begin <b>(required)</b>: The timestamp of the left edge of the image.
 * - \b host_begin <b>(required)</b>: The host ID of the top edge of the image.
 * - \b spp <b>(required)</b>: Seconds per pixel for image composition.
 * - \b npp <b>(required)</b>: Nodes per pixel for image composition.
 * - \b width <b>(required)</b>: The width of the composing image.
 * - \b height <b>(required)</b>: The height of hte composing image.
 * - \b ptn_ids: The comma-separated list of numbers and ranges of numbers of
 *   pattern IDs for the query. If the parameter is specified, only the
 *   specified patterns are used to compose the image. If not specified, all
 *   patterns will be used to compose the image.
 *
 * \par Response
 * If there is no error, \c bhttpd response with HTTP OK (200) and the binary
 * array containing the aggregated count by pixels (TS-HOST_ID).
 * \code{.json}
 * [AGGREGATED_COUNT(4byte)]...
 * \endcode
 * The ordering of the array is as follows:
 * \code{.json}
 * // For simplicity, assuming x and y start at 0 in this discussion.
 * [(x=0,y=0),(x=1,y=0),(x=2,y=0),...,(x=0,y=1),(x=1,y=1),...,(x=width-1,y=height-1)]
 * \endcode
 *
 * \par
 * If there is an \b error, \c bhttpd response with an appropriate HTTP error
 * code and a message describing the error.
 *
 * \subsection bhttpd_uri_query_host type=host
 * Get the mapping of <code>host_id :-> host_name</code>.
 *
 * \par URI
 * <code>BHTTPD_LOCATION/query/host</code>
 *
 * \par Response
 *
 * If there is no error, \c bhttpd response with HTTP OK (200) and the following
 * JSON objects in the content:
 * \code{.json}
 * {
 *     "host_ids": {
 *         <HOST_ID>: <HOST_NAME>,
 *         ...
 *     }
 * }
 * \endcode
 *
 * \par
 * If there is an \b error, \c bhttpd response with an appropriate HTTP error
 * code and a message describing the error.
 *
 * \subsection bhttpd_uri_query_big_pic type=big_pic
 * Big picture query returns the min/max of timestamp and component IDs for the
 * given pattern IDs. If no pattern ID is given, the min/max are from the entire
 * database.
 *
 * \note Due to internal database complication, currently the min/max of the
 * component IDs will always be min/max component IDs in the database. Min/max
 * timestamps is the real min/max timestamps for the given pattern ID(s).
 *
 * \par URI
 * <code>BHTTPD_LOCATION/query/big_pic</code>
 *
 * \par Response
 * If there is no error, \c bhttpd responses with HTTP OK (200) and the
 * following JSON objects in the content:
 * \code{.json}
 * {
 *     "min_ts": <MIN_TIMESTAMP>,
 *     "max_ts": <MAX_TIMESTAMP>,
 *     "min_comp_id": <MIN_COMPONENT_ID>,
 *     "max_comp_id": <MAX_COMPONENT_ID>
 * }
 * \endcode
 *
 * \par
 * If there is an \b error, \c bhttpd response with an appropriate HTTP error
 * code and a message describing the error.
 *
 * \tableofcontents
 */

#include <event2/event.h>
#include <assert.h>

#include "bhttpd.h"
#include "bq_fmt_json.h"

#include "baler/btypes.h"
#include "baler/bptn.h"
#include "baler/bhash.h"
#include "baler/bmeta.h"
#include "baler/bqueue.h"
#include "baler/butils.h"
#include "query/bquery.h"

pthread_mutex_t query_session_mutex = PTHREAD_MUTEX_INITIALIZER;
struct bhash *query_session_hash;
uint32_t query_session_gn;
struct timeval query_session_timeout = {.tv_sec = 600, .tv_usec = 0};

static
int __evbuffer_add_json_bstr(struct evbuffer *evb, const struct bstr *bstr)
{
	int rc = 0;
	int i;
	evbuffer_add_printf(evb, "\"");
	if (rc < 0)
		return errno;
	for (i = 0; i < bstr->blen; i++) {
		switch (bstr->cstr[i]) {
		case '"':
		case '\\':
		case '/':
			rc = evbuffer_add_printf(evb, "\\%c", bstr->cstr[i]);
			break;
		case '\b':
			rc = evbuffer_add_printf(evb, "\\b");
			break;
		case '\f':
			rc = evbuffer_add_printf(evb, "\\f");
			break;
		case '\n':
			rc = evbuffer_add_printf(evb, "\\n");
			break;
		case '\r':
			rc = evbuffer_add_printf(evb, "\\r");
			break;
		case '\t':
			rc = evbuffer_add_printf(evb, "\\t");
			break;
		default:
			rc = evbuffer_add_printf(evb, "%c", bstr->cstr[i]);
		}
		if (rc < 0)
			return errno;
	}
	rc = evbuffer_add_printf(evb, "\"");
	if (rc < 0)
		return errno;
	return 0;
}

static
void __bhttpd_handle_query_ptn(struct bhttpd_req_ctxt *ctxt, int is_metric)
{
	struct bptn_store *ptn_store = bq_get_ptn_store(bq_store);
	int n = bptn_store_last_id(ptn_store);
	int rc = 0;
	int i;
	int first = 1;
	struct bq_formatter *fmt = NULL;
	struct bdstr *bdstr = NULL;
	const char *use_ts_str = bpair_str_value(&ctxt->kvlist, "use_ts_fmt");
	int use_ts = 0;

	if (use_ts_str) {
		use_ts = atoi(use_ts_str);
	}

	fmt = bqfmt_json_new(bq_store, NULL);
	if (!fmt) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL, "Not enough memory");
		goto cleanup;
	}

	if (use_ts) {
		bqfmt_json_ts_use_ts(fmt);
	} else {
		bqfmt_json_ts_use_datetime(fmt);
	}

	bdstr = bdstr_new(1024);
	if (!bdstr) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL, "Not enough memory");
		goto cleanup;
	}

	evbuffer_add_printf(ctxt->evbuffer, "{\"result\": [");
	for (i=BMAP_ID_BEGIN; i<=n; i++) {

		const struct bstr *ptn = bptn_store_get_ptn(ptn_store, i);
		if (!ptn)
			/* It's OK to skip, slaves may not have all patterns */
			continue;

		if (bq_is_metric_pattern(bq_store, i) != is_metric)
			continue;
		rc = bq_print_ptn(bq_store, fmt, i, bdstr);
		if (rc) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL, "pattern query internal"
					" error, rc: %d", rc);
			goto cleanup;
		}
		if (first) {
			first = 0;
			evbuffer_add_printf(ctxt->evbuffer, "%s", bdstr->str);
		} else {
			evbuffer_add_printf(ctxt->evbuffer, ", %s", bdstr->str);
		}
	}
	evbuffer_add_printf(ctxt->evbuffer, "]}");

cleanup:
	if (fmt)
		bqfmt_json_free(fmt);
	if (bdstr)
		bdstr_free(bdstr);
}

static
void bhttpd_handle_query_ptn(struct bhttpd_req_ctxt *ctxt)
{
	__bhttpd_handle_query_ptn(ctxt, 0);
}

static
void bhttpd_handle_query_metric_ptn(struct bhttpd_req_ctxt *ctxt)
{
	__bhttpd_handle_query_ptn(ctxt, 1);
}

static
void bhttpd_handle_query_ptn_simple(struct bhttpd_req_ctxt *ctxt)
{
	/* COMPLETE ME */
	struct bptn_store *ptn_store = bq_get_ptn_store(bq_store);
	int n = bptn_store_last_id(ptn_store);
	int rc = 0;
	int i;
	int first = 1;
	struct bq_formatter *fmt = NULL;
	struct bdstr *bdstr = NULL;
	const char *use_ts_str = bpair_str_value(&ctxt->kvlist, "use_ts_fmt");
	int use_ts = 0;

	if (use_ts_str) {
		use_ts = atoi(use_ts_str);
	}

	fmt = bquery_default_formatter();

	bdstr = bdstr_new(4096);
	if (!bdstr) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL, "Not enough memory");
		goto cleanup;
	}

	for (i=BMAP_ID_BEGIN; i<=n; i++) {

		const struct bstr *ptn = bptn_store_get_ptn(ptn_store, i);
		if (!ptn)
			/* It's OK to skip, slaves may not have all patterns */
			continue;

		if (bq_is_metric_pattern(bq_store, i))
			/* skip metric patterns. */
			continue;
		evbuffer_add_printf(ctxt->evbuffer, "%d ", i);
		rc = bq_print_ptn(bq_store, fmt, i, bdstr);
		if (rc) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL, "pattern query internal"
					" error, rc: %d", rc);
			goto cleanup;
		}
		evbuffer_add_printf(ctxt->evbuffer, "%s\n", bdstr->str);
	}

cleanup:
	if (bdstr)
		bdstr_free(bdstr);
}

static
void bhttpd_msg_query_expire_cb(evutil_socket_t fd, short what, void *arg);

static
void bhttpd_msg_query_session_destroy(struct bhttpd_msg_query_session *qs)
{
	bdebug("destroying session: %lu", (uint64_t)qs);
	if (qs->event)
		event_free(qs->event);
	if (qs->q)
		bmsgquery_destroy((void*)qs->q);
	if (qs->fmt)
		bqfmt_json_free(qs->fmt);
	free(qs);
}

static
struct bhttpd_msg_query_session *bhttpd_msg_query_session_create(struct bhttpd_req_ctxt *ctxt, int simple)
{
	struct bhttpd_msg_query_session *qs;
	const char *host_ids, *ptn_ids, *ts0, *ts1;
	struct bpair_str *kv;
	int rc;
	qs = calloc(1, sizeof(*qs));
	if (!qs)
		return NULL;

	bdebug("creating session: %lu", (uint64_t)qs);

	qs->first = 1;
	qs->event = event_new(evbase, -1, EV_READ,
			bhttpd_msg_query_expire_cb, qs);
	if (!qs->event) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"Not enough memory.");
		goto err;
	}
	host_ids = bpair_str_value(&ctxt->kvlist, "host_ids");
	ptn_ids = bpair_str_value(&ctxt->kvlist, "ptn_ids");
	ts0 = bpair_str_value(&ctxt->kvlist, "ts0");
	ts1 = bpair_str_value(&ctxt->kvlist, "ts1");
	qs->q = (void*)bmsgquery_create(bq_store, host_ids, ptn_ids, ts0, ts1, 1, ' ', &rc);
	if (!qs->q) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"msg query creation failed, rc: %d.", rc);
		goto err;
	}

	if (simple) {
		bq_set_formatter(qs->q, bquery_default_formatter());
	} else {
		qs->fmt = bqfmt_json_new(bq_store, qs->q);
		if (!qs->fmt) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"Cannot create bqfmt_json, errno: %d.", errno);
			goto err;
		}
		bq_set_formatter(qs->q, qs->fmt);
	}

	return qs;
err:

	bhttpd_msg_query_session_destroy(qs);
	return NULL;
}

static
void bhttpd_msg_query_expire_cb(evutil_socket_t fd, short what, void *arg)
{
	struct bhttpd_msg_query_session *qs = arg;
	uint64_t key = (uint64_t)arg;
	struct timeval tv, dtv;
	struct bhash_entry *ent;
	pthread_mutex_lock(&query_session_mutex);
	gettimeofday(&tv, NULL);
	timersub(&tv, &qs->last_use, &dtv);
	dtv.tv_sec *= 2;
	if (timercmp(&dtv, &query_session_timeout, <)) {
		/* This is the case where the client access the query session
		 * at the very last second. Just do nothing and wait for the
		 * next timeout event. */
		pthread_mutex_unlock(&query_session_mutex);
		return;
	}
	ent = bhash_entry_get(query_session_hash, (void*)&key, sizeof(key));
	if (!ent) {
		bwarn("Cannot find hash entry %d, in function %s", key, __func__);
		pthread_mutex_unlock(&query_session_mutex);
		return;
	}
	bhash_entry_remove_free(query_session_hash, ent);
	pthread_mutex_unlock(&query_session_mutex);
	bdebug("session expired: %lu", (uint64_t) qs);
	bhttpd_msg_query_session_destroy(qs);
}

static
int bhttpd_msg_query_session_recover(struct bhttpd_msg_query_session *qs,
								int is_fwd)
{
	int rc = 0;
	bq_msg_ref_t ref;
	int (*begin)(struct bquery *q);
	int (*step)(struct bquery *q);

	if (is_fwd) {
		begin = bq_last_entry;
		step = bq_prev_entry;
	} else {
		begin = bq_first_entry;
		step = bq_next_entry;
	}

	rc = begin(qs->q);
	if (rc)
		return rc;

	while (rc == 0) {
		ref = bq_entry_get_ref(qs->q);
		if (bq_msg_ref_equal(&ref, &qs->ref))
			break;
		rc = step(qs->q);
	}

	return rc;
}

static
void bhttpd_handle_query_msg(struct bhttpd_req_ctxt *ctxt)
{
	struct bhttpd_msg_query_session *qs = NULL;
	struct bdstr *bdstr;
	struct bhash_entry *ent = NULL;
	uint64_t session_id = 0;
	const char *str;
	int is_fwd = 1;
	int i, n = 50;
	int rc;

	bdstr = bdstr_new(256);
	if (!bdstr) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL, "Out of memory");
		return;
	}
	str = bpair_str_value(&ctxt->kvlist, "n");
	if (str)
		n = atoi(str);
	str = bpair_str_value(&ctxt->kvlist, "dir");
	if (str && strcmp(str, "bwd") == 0)
		is_fwd = 0;

	str = bpair_str_value(&ctxt->kvlist, "session_id");
	if (str) {
		session_id = strtoull(str, NULL, 0);
		ent = bhash_entry_get(query_session_hash, (void*)&session_id,
				sizeof(session_id));
		if (!ent) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Session %lu not found.", session_id);
			goto out;
		}
		qs = (void*)ent->value;
	}
	if (!qs) {
		qs = bhttpd_msg_query_session_create(ctxt, 0);
		if (!qs) {
			/* bhttpd_msg_query_session_create() has already
			 * set the error message. */
			goto out;
		}
		session_id = (uint64_t)qs;
		ent = bhash_entry_set(query_session_hash, (void*)&session_id,
				sizeof(session_id), (uint64_t)(void*)qs);
		if (!ent) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Hash insert failed, errno: %d",
					errno);
			goto out;
		}
	}
	/* update last_use */
	gettimeofday(&qs->last_use, NULL);
	rc = event_add(qs->event, &query_session_timeout);
	if (rc) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"event_add() rc: %d, errno: %d", rc, errno);
		goto out;
	}

	evbuffer_add_printf(ctxt->evbuffer, "{");

	evbuffer_add_printf(ctxt->evbuffer, "\"session_id\": %lu", session_id);

	evbuffer_add_printf(ctxt->evbuffer, ", \"msgs\": [");
	for (i = 0; i < n; i++) {
		if (qs->first) {
			qs->first = 0;
			if (is_fwd)
				rc = bq_first_entry(qs->q);
			else
				rc = bq_last_entry(qs->q);
		} else {
			if (is_fwd)
				rc = bq_next_entry(qs->q);
			else
				rc = bq_prev_entry(qs->q);
		}
		if (rc) {
			rc = bhttpd_msg_query_session_recover(qs, is_fwd);
			break;
		}
		qs->ref = bq_entry_get_ref(qs->q);
		str = bq_entry_print(qs->q, bdstr);
		if (!str) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"bq_entry_print() errno: %d", errno);
			goto out;
		}
		if (i)
			evbuffer_add_printf(ctxt->evbuffer, ",%s", str);
		else
			evbuffer_add_printf(ctxt->evbuffer, "%s", str);
		bdstr_reset(bdstr);
	}
	evbuffer_add_printf(ctxt->evbuffer, "]");
	evbuffer_add_printf(ctxt->evbuffer, "}");

out:
	bdstr_free(bdstr);
}

static
void bhttpd_handle_query_msg_simple(struct bhttpd_req_ctxt *ctxt)
{
	struct bhttpd_msg_query_session *qs = NULL;
	struct bdstr *bdstr;
	struct bhash_entry *ent = NULL;
	uint64_t session_id = 0;
	const char *str;
	int is_fwd = 1;
	int i, n = 50;
	int rc;

	bdstr = bdstr_new(256);
	if (!bdstr) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL, "Out of memory");
		return;
	}
	str = bpair_str_value(&ctxt->kvlist, "n");
	if (str)
		n = atoi(str);
	str = bpair_str_value(&ctxt->kvlist, "dir");
	if (str && strcmp(str, "bwd") == 0)
		is_fwd = 0;

	str = bpair_str_value(&ctxt->kvlist, "session_id");
	if (str) {
		session_id = strtoull(str, NULL, 0);
		ent = bhash_entry_get(query_session_hash, (void*)&session_id,
				sizeof(session_id));
		if (!ent) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Session %lu not found.", session_id);
			goto out;
		}
		qs = (void*)ent->value;
	}
	if (!qs) {
		qs = bhttpd_msg_query_session_create(ctxt, 1);
		if (!qs) {
			/* bhttpd_msg_query_session_create() has already
			 * set the error message. */
			goto out;
		}
		session_id = (uint64_t)qs;
		ent = bhash_entry_set(query_session_hash, (void*)&session_id,
				sizeof(session_id), (uint64_t)(void*)qs);
		if (!ent) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Hash insert failed, errno: %d",
					errno);
			goto out;
		}
	}
	/* update last_use */
	gettimeofday(&qs->last_use, NULL);
	rc = event_add(qs->event, &query_session_timeout);
	if (rc) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"event_add() rc: %d, errno: %d", rc, errno);
		goto out;
	}

	evbuffer_add_printf(ctxt->evbuffer, "session_id: %lu\n", session_id);
	for (i = 0; i < n; i++) {
		if (qs->first) {
			qs->first = 0;
			if (is_fwd)
				rc = bq_first_entry(qs->q);
			else
				rc = bq_last_entry(qs->q);
		} else {
			if (is_fwd)
				rc = bq_next_entry(qs->q);
			else
				rc = bq_prev_entry(qs->q);
		}
		if (rc) {
			break;
		}
		str = bq_entry_print(qs->q, bdstr);
		if (!str) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"bq_entry_print() errno: %d", errno);
			goto out;
		}
		evbuffer_add_printf(ctxt->evbuffer, "%s\n", str);
		bdstr_reset(bdstr);
	}

out:
	bdstr_free(bdstr);
}

static
void bhttpd_handle_query_msg2(struct bhttpd_req_ctxt *ctxt)
{
	struct bhttpd_msg_query_session *qs = NULL;
	struct bdstr *bdstr;
	struct bhash_entry *ent = NULL;
	uint64_t session_id = 0;
	const char *str;
	BQUERY_POS(pos);
	int is_fwd = 1;
	int i, n = 50;
	int curr = 0;
	int rc;

	bdstr = bdstr_new(256);
	if (!bdstr) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL, "Out of memory");
		return;
	}
	str = bpair_str_value(&ctxt->kvlist, "n");
	if (str)
		n = atoi(str);
	str = bpair_str_value(&ctxt->kvlist, "dir");
	if (str && strcmp(str, "bwd") == 0)
		is_fwd = 0;

	qs = bhttpd_msg_query_session_create(ctxt, 0);
	if (!qs) {
		/* bhttpd_msg_query_session_create() has already
		 * set the error message. */
		goto out;
	}

	str = bpair_str_value(&ctxt->kvlist, "pos");
	if (str) {
		/* recover the position */
		rc = bquery_pos_from_str(pos, str);
		if (rc) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bquery_pos_from_str() error, %d", rc);
			goto out;
		}
		rc = bq_set_pos(qs->q, pos);
		if (rc) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bq_set_pos() error, %d", rc);
			goto out;
		}
		qs->first = 0;

		/* current (curr) flag can turn 1 only with pos */
		str = bpair_str_value(&ctxt->kvlist, "curr");
		if (str)
			curr = 1;
	}

	evbuffer_add_printf(ctxt->evbuffer, "{");

	evbuffer_add_printf(ctxt->evbuffer, "\"msgs\": [");
	if (curr) {
		rc = bq_get_pos(qs->q, pos);
		if (rc) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bq_get_pos() error, %d", rc);
			goto out;
		}
		str = bq_entry_print(qs->q, bdstr);
		if (!str) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"bq_entry_print() errno: %d", errno);
			goto out;
		}
		evbuffer_add_printf(ctxt->evbuffer, "%s", str);
		bdstr_reset(bdstr);
	}
	for (i = 0; i < n; i++) {
		if (qs->first) {
			qs->first = 0;
			if (is_fwd)
				rc = bq_first_entry(qs->q);
			else
				rc = bq_last_entry(qs->q);
		} else {
			if (is_fwd)
				rc = bq_next_entry(qs->q);
			else
				rc = bq_prev_entry(qs->q);
		}
		if (rc) {
			break;
		}
		rc = bq_get_pos(qs->q, pos);
		if (rc) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bq_get_pos() error, %d", rc);
			goto out;
		}
		str = bq_entry_print(qs->q, bdstr);
		if (!str) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"bq_entry_print() errno: %d", errno);
			goto out;
		}
		if (i || curr)
			evbuffer_add_printf(ctxt->evbuffer, ",%s", str);
		else
			evbuffer_add_printf(ctxt->evbuffer, "%s", str);
		bdstr_reset(bdstr);
	}
	evbuffer_add_printf(ctxt->evbuffer, "]");
	evbuffer_add_printf(ctxt->evbuffer, "}");

out:
	bdstr_free(bdstr);
	if (qs)
		bhttpd_msg_query_session_destroy(qs);
}

static
void bhttpd_handle_query_meta(struct bhttpd_req_ctxt *ctxt)
{
	int n = bptn_store_last_id(bq_get_ptn_store(bq_store));
	struct barray *array = barray_alloc(sizeof(uint32_t), n+1);
	int rc;
	int i, x, first;
	if (!array) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"Not enough memory");
		return;
	}
	rc = bmptn_store_get_class_id_array(mptn_store, array);
	if (rc) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bmptn_store_get_class_id_array(), rc: %d", rc);
		return;
	}
	n = barray_get_len(array);
	first = 1;
	evbuffer_add_printf(ctxt->evbuffer, "{\"map\": [");
	for (i = BMAP_ID_BEGIN; i < n; i++) {
		if (bq_is_metric_pattern(bq_store, i))
			continue;
		barray_get(array, i, &x);
		if (first) {
			evbuffer_add_printf(ctxt->evbuffer, "[%d, %d]", i, x);
			first = 0;
		} else {
			evbuffer_add_printf(ctxt->evbuffer, ", [%d, %d]", i, x);
		}
	}
	evbuffer_add_printf(ctxt->evbuffer, "], ");
	n = bmptn_store_get_last_cls_id(mptn_store);
	evbuffer_add_printf(ctxt->evbuffer, "\"cluster_names\": {");
	first = 1;
	for (i = 1; i <= n; i++) {
		const struct bstr *bstr = bmptn_get_cluster_name(mptn_store, i);
		if (!bstr)
			continue;
		if (first)
			first = 0;
		else
			evbuffer_add_printf(ctxt->evbuffer, ",");
		evbuffer_add_printf(ctxt->evbuffer, "\"%d\": ", i);
		__evbuffer_add_json_bstr(ctxt->evbuffer, bstr);
	}
	evbuffer_add_printf(ctxt->evbuffer, "}}");
}

static
void bhttpd_handle_query_metric_meta(struct bhttpd_req_ctxt *ctxt)
{
	int n = bptn_store_last_id(bq_get_ptn_store(bq_store));
	int rc;
	int i, x, first;
	const struct bstr *ptn;
	struct bhash *idhash = NULL;
	struct bhash_iter *itr = NULL;

	idhash = bhash_new(65521, 11, NULL);
	if (!idhash) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL, "Not enough memory");
		goto cleanup;
	}
	first = 1;
	evbuffer_add_printf(ctxt->evbuffer, "{\"map\": [");
	for (i = BMAP_ID_BEGIN; i <= n; i++) {
		if (!bq_is_metric_pattern(bq_store, i))
			continue;
		ptn = bptn_store_get_ptn(bq_get_ptn_store(bq_store), i);
		if (!ptn) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Cannot get pattern id: %d", i);
			goto cleanup;
		}
		if (first) {
			evbuffer_add_printf(ctxt->evbuffer, "[%d, %d]", i,
								ptn->u32str[1]);
			first = 0;
		} else {
			evbuffer_add_printf(ctxt->evbuffer, ", [%d, %d]", i,
								ptn->u32str[1]);
		}
		bhash_entry_set(idhash, (void*)&ptn->u32str[1],
					sizeof(uint32_t), ptn->u32str[1]);
	}

	evbuffer_add_printf(ctxt->evbuffer, "], ");

	first = 1;
	evbuffer_add_printf(ctxt->evbuffer, "\"cluster_names\": {");

	itr = bhash_iter_new(idhash);
	rc = bhash_iter_begin(itr);
	while (rc == 0) {
		struct bhash_entry *ent = bhash_iter_entry(itr);
		if (first)
			first = 0;
		else
			evbuffer_add_printf(ctxt->evbuffer, ",");
		evbuffer_add_printf(ctxt->evbuffer, "\"%d\": ", (uint32_t)ent->value);
		const struct bstr *name = btkn_store_get_bstr(bq_get_tkn_store(bq_store), ent->value);
		if (!name) {
			evbuffer_add_printf(ctxt->evbuffer, "\"!!!ERROR!!!\"");
		} else {
			__evbuffer_add_json_bstr(ctxt->evbuffer, name);
		}
		rc = bhash_iter_next(itr);
	}
	evbuffer_add_printf(ctxt->evbuffer, "}}");

cleanup:
	if (itr)
		bhash_iter_free(itr);
	if (idhash)
		bhash_free(idhash);
}

static
float __get_url_param_float(struct bhttpd_req_ctxt *ctxt, const char *key,
							float default_value)
{
	const char *str = bpair_str_value(&ctxt->kvlist, key);
	if (!str)
		return default_value;
	return strtof(str, NULL);
}

static
int __get_url_param_int(struct bhttpd_req_ctxt *ctxt, const char *key,
							int default_value)
{
	const char *str = bpair_str_value(&ctxt->kvlist, key);
	if (!str)
		return default_value;
	return atoi(str);
}

static
void get_closest_img_store_cb(const char *name, void *_ctxt)
{
	/* name format: spp-npp */
	/* looking for min spp that is greater than the given spp */
	struct bhttpd_req_ctxt *ctxt = _ctxt;
	uint32_t spp, npp;
	sscanf(name, "%u-%u", &spp, &npp);
	if (spp > ctxt->spp)
		return;
	if (spp <= ctxt->sppMax)
		return;
	ctxt->sppMax = spp;
	snprintf(ctxt->img_store, sizeof(ctxt->img_store), "%s", name);
}

static
void bhttpd_handle_query_img2(struct bhttpd_req_ctxt *ctxt)
{
	int rc;
	int first = 1;
	struct bpixel p;
	char ts0[16];
	char ts1[16];
	char host_ids[32];
	const char *ptn_ids = bpair_str_value(&ctxt->kvlist, "ptn_ids");
	const char *img_store = NULL;

	float spp = __get_url_param_float(ctxt, "spp", 0);
	float npp = __get_url_param_float(ctxt, "npp", 0);
	int width = __get_url_param_int(ctxt, "width", 0);
	int height = __get_url_param_int(ctxt, "height", 0);
	int ts_begin = __get_url_param_int(ctxt, "ts_begin", 0);
	int host_begin = __get_url_param_int(ctxt, "host_begin", 0);
	int ts_end = ts_begin + (int)(width*spp);
	int host_end = host_begin + (int)(height*npp);

	int *data = NULL;
	int i, len;

	struct bimgquery *q;

	if (!width || !height) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Please specify width and height");
		return;
	}

	if (spp < 0.000001 || npp < 0.000001) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Please specify spp and npp");
		return;
	}

	ctxt->spp = spp;
	ctxt->sppMax = 0;
	bq_imgstore_iterate(bq_store, get_closest_img_store_cb, ctxt);
	if (ctxt->sppMax == 0) {
		/* cannot find any image store */
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"No image store available.");
		return;
	}

	img_store = ctxt->img_store;
	bdebug("img_store: %s", img_store);

	snprintf(ts0, sizeof(ts0), "%d", ts_begin);
	snprintf(ts1, sizeof(ts0), "%d", ts_end);
	snprintf(host_ids, sizeof(host_ids), "%d-%d", host_begin, host_end);

	len = width * height;

	data = calloc(sizeof(int),  len);

	if (!data) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL, "Out of memory.");
		return;
	}

	q = bimgquery_create(bq_store, host_ids,
			ptn_ids, ts0, ts1, img_store, &rc);

	if (!q) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bimgquery_create() error, errno: %d", errno);
		return;
	}
	rc = bq_first_entry((void*)q);
	while (rc == 0) {
		rc = bq_img_entry_get_pixel(q, &p);
		if (rc)
			break;
		int x = (p.sec - ts_begin) / spp;
		int y = (p.comp_id - host_begin) / npp;
		if (x >= width || y >= height)
			goto next;
		int idx = y*width + x;
		data[idx] += p.count;
	next:
		rc = bq_next_entry((void*)q);
	}

	for (i = 0; i < len; i++) {
		data[i] = htonl(data[i]);
	}

	evbuffer_add(ctxt->evbuffer, data, sizeof(int)*width*height);
	bimgquery_destroy(q);
	bdebug("sending data, size: %d", sizeof(int)*width*height);
	free(data);
}

static
void bhttpd_handle_query_img(struct bhttpd_req_ctxt *ctxt)
{
	int rc;
	int first = 1;
	struct bpixel p;
	const char *ts0 = bpair_str_value(&ctxt->kvlist, "ts0");
	const char *ts1 = bpair_str_value(&ctxt->kvlist, "ts1");;
	const char *host_ids = bpair_str_value(&ctxt->kvlist, "host_ids");
	const char *ptn_ids = bpair_str_value(&ctxt->kvlist, "ptn_ids");
	const char *img_store = bpair_str_value(&ctxt->kvlist, "img_store");
	struct bimgquery *q;

	if (!img_store) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"Please specify 'img_store'"
				" (see /list_img_store)");
		return;
	}

	q = bimgquery_create(bq_store, host_ids,
			ptn_ids, ts0, ts1, img_store, &rc);

	if (!q) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bimgquery_create() error, errno: %d", errno);
		return;
	}
	// evbuffer_add_printf(ctxt->evbuffer, "{\"pixels\": [");
	rc = bq_first_entry((void*)q);
	while (rc == 0) {
		rc = bq_img_entry_get_pixel(q, &p);
		if (rc)
			break;
		hton_bpixel(&p);
		evbuffer_add(ctxt->evbuffer, &p, sizeof(p));
		/*
		if (first)
			first = 0;
		else
			evbuffer_add_printf(ctxt->evbuffer, ",");
		evbuffer_add_printf(ctxt->evbuffer, "[%d, %d, %d, %d]",
					p.sec, p.comp_id, p.ptn_id, p.count);
		*/
		rc = bq_next_entry((void*)q);
	}
	// evbuffer_add_printf(ctxt->evbuffer, "]}");
	bimgquery_destroy(q);
}

static
void bhttpd_handle_query_img_simple(struct bhttpd_req_ctxt *ctxt)
{
	int rc;
	int first = 1;
	struct bpixel p;
	const char *nStr = bpair_str_value(&ctxt->kvlist, "n");
	const char *ts0 = bpair_str_value(&ctxt->kvlist, "ts0");
	const char *ts1 = bpair_str_value(&ctxt->kvlist, "ts1");;
	const char *host_ids = bpair_str_value(&ctxt->kvlist, "host_ids");
	const char *ptn_ids = bpair_str_value(&ctxt->kvlist, "ptn_ids");
	const char *img_store = bpair_str_value(&ctxt->kvlist, "img_store");
	const char *dir = bpair_str_value(&ctxt->kvlist, "dir");
	int (*bq_step)(struct bquery *) = bq_next_entry;
	int (*bq_init)(struct bquery *) = bq_first_entry;
	int spp;
	int n = 0;
	struct bimgquery *q;

	if (!img_store) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"Please specify 'img_store'"
				" (see /list_img_store)");
		return;
	}

	sscanf(img_store, "%d", &spp);

	ctxt->spp = spp;
	ctxt->sppMax = 0;
	bq_imgstore_iterate(bq_store, get_closest_img_store_cb, ctxt);
	if (ctxt->sppMax == 0) {
		/* cannot find any image store */
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"No image store available.");
		return;
	}

	if (nStr)
		n = atoi(nStr);

	if (n == 0)
		n = 20;

	if (dir && strcasecmp(dir, "bwd")==0) {
		bq_step = bq_prev_entry;
		bq_init = bq_last_entry;
	}

	q = bimgquery_create(bq_store, host_ids,
			ptn_ids, ts0, ts1, ctxt->img_store, &rc);

	if (!q) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bimgquery_create() error, errno: %d", errno);
		return;
	}
	evbuffer_add_printf(ctxt->evbuffer, "{\"pixels\": [");
	rc = bq_init((void*)q);
	while (rc == 0 && n) {
		rc = bq_img_entry_get_pixel(q, &p);
		if (rc)
			break;
		if (first)
			first = 0;
		else
			evbuffer_add_printf(ctxt->evbuffer, ",");
		evbuffer_add_printf(ctxt->evbuffer, "[%d, %d, %d, %d]",
					p.sec, p.comp_id, p.ptn_id, p.count);
		n--;
		rc = bq_step((void*)q);
	}
	evbuffer_add_printf(ctxt->evbuffer, "]}");
	bimgquery_destroy(q);
}

static
void __bhttpd_handle_query_img_pan_store_cb(const char *name, void *_ctxt)
{
	struct pan_ctxt *pc = _ctxt;
	int len;
	if (pc->img_n >= 20)
		return;
	len = snprintf(pc->img_store[pc->img_n],
			sizeof(pc->img_store[pc->img_n]), "%s", name);
	if (len >= 64) {
		berr("img store name too long (%d): %s", len, name);
		pc->img_store[pc->img_n][0] = 0; /* reset to empty string */
		return;
	}
	pc->img_n++;
}

static
int __img_store_name_desc_cmp(const void *a, const void *b)
{
	int _a, _b;
	sscanf(a, "%d", &_a);
	sscanf(b, "%d", &_b);
	if (_a < _b)
		return 1;
	if (_a > _b)
		return -1;
	return 0;
}

static
void __dlog_pan_ctxt(struct pan_ctxt *pc)
{
	int i;
	for (i = 0; i < pc->img_n; i++) {
		bdebug("img_name[%d]: %s", i, pc->img_store[i]);
		bdebug("img_spp[%d]: %d", i, pc->img_spp[i]);
	}
	bdebug("qts0: %s", pc->qts0);
	bdebug("qts1: %s", pc->qts1);
	bdebug("qhost_ids: %s", pc->qhost_ids);
	bdebug("ptn_ids: %s", pc->ptn_ids);
}

static
struct bpixel *__img_pan(struct pan_ctxt *pc)
{
	int i, rc;
	struct bimgquery *q = NULL;
	struct bpixel *pxl = NULL;
	struct bpixel tmp;
	uint32_t max_node;
	max_node = bq_get_cmp_store(bq_store)->map->hdr->count - 1;
	for (i = 0; i < pc->img_n; i++) {
		if (pc->img_spp[i] <= pc->spp)
			break;
	}
	if (i == pc->img_n) {
		goto out;
	}
	switch (pc->dir) {
	case IMG_PAN_UP:
		pc->bq_init = bq_first_entry;
		pc->bq_step = (void*)bq_next_entry;
		snprintf(pc->qhost_ids, sizeof(pc->qhost_ids),
				"%d-%d", pc->host1, max_node);
		snprintf(pc->qts0, sizeof(pc->qts0), "%d", pc->ts0);
		snprintf(pc->qts1, sizeof(pc->qts1), "%d", pc->ts1);
		break;
	case IMG_PAN_DOWN:
		pc->bq_init = bq_last_entry;
		pc->bq_step = (void*)bq_prev_entry;
		snprintf(pc->qhost_ids, sizeof(pc->qhost_ids),
				"%d-%d", 0, pc->host0);
		snprintf(pc->qts0, sizeof(pc->qts0), "%d", pc->ts0);
		snprintf(pc->qts1, sizeof(pc->qts1), "%d", pc->ts1);
		break;
	case IMG_PAN_LEFT:
		pc->bq_init = bq_last_entry;
		pc->bq_step = (void*)bimgquery_prev_ptn;
		snprintf(pc->qhost_ids, sizeof(pc->qhost_ids),
				"%d-%d", pc->host0, pc->host1 - 1);
		pc->qts0[0] = 0;
		snprintf(pc->qts1, sizeof(pc->qts1), "%d", pc->ts0);
		break;
	case IMG_PAN_RIGHT:
		pc->bq_init = bq_first_entry;
		pc->bq_step = (void*)bimgquery_next_ptn;
		snprintf(pc->qhost_ids, sizeof(pc->qhost_ids),
				"%d-%d", pc->host0, pc->host1 - 1);
		snprintf(pc->qts0, sizeof(pc->qts0), "%d", pc->ts1);
		pc->qts0[1] = 0;
		break;
	default:
		assert(0);
		break;
	}

	bdebug("i: %d", i);
	q = bimgquery_create(bq_store, pc->qhost_ids, pc->ptn_ids,
				pc->qts0, pc->qts1, pc->img_store[i], &rc);
	if (!q)
		goto out;
	rc = pc->bq_init((void*)q);
	if (rc)
		goto cleanup;
	bq_img_entry_get_pixel(q, &tmp);
	pc->pxl = tmp;
	pxl = &pc->pxl;

	while (0 == (rc = pc->bq_step((void*)q))) {
		bq_img_entry_get_pixel(q, &tmp);
		switch (pc->dir) {
		case IMG_PAN_UP:
			pc->pxl.comp_id = BMIN(tmp.comp_id, pc->pxl.comp_id);
			break;
		case IMG_PAN_DOWN:
			pc->pxl.comp_id = BMAX(tmp.comp_id, pc->pxl.comp_id);
			break;
		case IMG_PAN_LEFT:
			pc->pxl.sec = BMAX(tmp.sec, pc->pxl.sec);
			break;
		case IMG_PAN_RIGHT:
			pc->pxl.sec = BMIN(tmp.sec, pc->pxl.sec);
			break;
		default: /* suppressing warning */
			break;
		}
	}

	switch (pc->dir) {
	case IMG_PAN_LEFT:
	case IMG_PAN_RIGHT:
		pxl->comp_id = pc->host0;
		pxl->sec -= pc->spp*pc->pxl_width/2;
		break;
	case IMG_PAN_UP:
	case IMG_PAN_DOWN:
		pxl->comp_id -= pc->npp*pc->pxl_height/2;
		if (((int)pxl->comp_id) < 0) {
			pxl->comp_id = 0;
		}
		pxl->sec = pc->ts0;
		break;
	default:
		break;
	}

cleanup:
	bimgquery_destroy(q);
out:
	return pxl;
}

static
void bhttpd_handle_query_img_pan(struct bhttpd_req_ctxt *ctxt)
{
	int rc, i;
	int first = 1;
	struct bpixel *pxl;
	const uint32_t ts_begin = bpair_u32_value(&ctxt->kvlist, "ts_begin");
	const uint32_t host_begin = bpair_u32_value(&ctxt->kvlist, "host_begin");
	const int pxl_width = bpair_int_value(&ctxt->kvlist, "pxl_width");
	const int pxl_height = bpair_int_value(&ctxt->kvlist, "pxl_height");
	const int spp = bpair_int_value(&ctxt->kvlist, "spp");
	const int npp = bpair_int_value(&ctxt->kvlist, "npp");
	const char *ptn_ids = bpair_str_value(&ctxt->kvlist, "ptn_ids");
	bimg_pan_dir_t dir = str2bimg_pan_dir(bpair_str_value(&ctxt->kvlist, "dir"));
	struct bimgquery *q;
	struct pan_ctxt *pc = NULL;

	if (!npp) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Please specify 'npp'");
		return;
	}
	if (!spp) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Please specify 'spp'");
		return;
	}
	if (!pxl_width) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Please specify 'pxl_width'");
		return;
	}
	if (!pxl_height) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Please specify 'pxl_height'");
		return;
	}
	if (dir == IMG_PAN_LAST) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
					"Please specify 'dir'");
		return;
	}

	pc = calloc(1, sizeof(*pc));
	if (!pc) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
						"Not enough memory.");
		return;
	}
	pc->npp = npp;
	pc->spp = spp;
	pc->dir = dir;
	pc->ctxt = ctxt;
	pc->pxl_width = pxl_width;
	pc->pxl_height = pxl_height;
	pc->ts_begin = ts_begin;
	pc->host_begin = host_begin;
	pc->host0 = host_begin;
	pc->host1 = host_begin + npp*pxl_height;
	pc->ts0 = ts_begin;
	pc->ts1 = ts_begin + spp*pxl_width;

	/* get all store names, order by spp */
	bq_imgstore_iterate(bq_store, __bhttpd_handle_query_img_pan_store_cb, pc);
	qsort(pc->img_store, pc->img_n,
		sizeof(pc->img_store[0]), __img_store_name_desc_cmp);

	for (i = 0; i < pc->img_n; i++) {
		sscanf(pc->img_store[i], "%d", &pc->img_spp[i]);
	}

	pxl = __img_pan(pc);

	if (!pxl) {
		evbuffer_add_printf(ctxt->evbuffer,
			"{\"host_begin\": -1, \"ts_begin\": -1}");
		return;
	}

	evbuffer_add_printf(ctxt->evbuffer,
		"{\"host_begin\": %d, \"ts_begin\": %d}", pxl->comp_id, pxl->sec
	);
}

static
void bhttpd_handle_query_destroy_session(struct bhttpd_req_ctxt *ctxt)
{
	const char *_session_id = bpair_str_value(&ctxt->kvlist, "session_id");
	uint64_t session_id;
	struct bhttpd_msg_query_session *qs;
	struct bhash_entry *ent;
	if (!session_id) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"session_id is not set");
		return;
	}
	session_id = strtoul(_session_id, NULL, 0);

	pthread_mutex_lock(&query_session_mutex);
	ent = bhash_entry_get(query_session_hash, (void*)&session_id,
			sizeof(session_id));
	if (!ent) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"session_id %ul not found", session_id);
		pthread_mutex_unlock(&query_session_mutex);
		return;
	}
	bhash_entry_remove_free(query_session_hash, ent);
	pthread_mutex_unlock(&query_session_mutex);
	qs = (void*)session_id;
	bhttpd_msg_query_session_destroy(qs);
}

static
void bhttpd_handle_query_host(struct bhttpd_req_ctxt *ctxt)
{
	const struct bstr *bstr;
	struct btkn_store *cmp_store = bq_get_cmp_store(bq_store);
	int id = 0;
	int first = 1;
	evbuffer_add_printf(ctxt->evbuffer, "{ \"host_ids\": {");
	while (1) {
		bstr = btkn_store_get_bstr(cmp_store, bcompid2mapid(id));
		if (!bstr)
			break;
		if (first)
			first = 0;
		else
			evbuffer_add_printf(ctxt->evbuffer, ", ");
		evbuffer_add_printf(ctxt->evbuffer, "\"%d\": \"%.*s\"",
						id, bstr->blen, bstr->cstr);
		id++;
	}
	evbuffer_add_printf(ctxt->evbuffer, "}}");
}

static
void bhttpd_handle_query_big_pic(struct bhttpd_req_ctxt *ctxt)
{
	struct btkn_store *cmp_store = bq_get_cmp_store(bq_store);
	const char *ptn_ids = bpair_str_value(&ctxt->kvlist, "ptn_ids");
	struct bquery *q;
	int rc;
	uint32_t min_ts, max_ts;
	uint32_t min_node, max_node;

	q = (void*)bmsgquery_create(bq_store, NULL, ptn_ids, NULL, NULL, 0, ',', &rc);
	if (!q) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bmsgquery_create() error, rc: %d", rc);
		goto cleanup;
	}

	rc = bq_first_entry(q);
	if (rc) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bq_first_entry() error, rc: %d", rc);
		goto cleanup;
	}
	min_ts = bq_entry_get_sec(q);

	rc = bq_last_entry(q);
	if (rc) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bq_first_entry() error, rc: %d", rc);
		goto cleanup;
	}
	max_ts = bq_entry_get_sec(q);

	min_node = 0;
	max_node = cmp_store->map->hdr->count - 1;

	evbuffer_add_printf(ctxt->evbuffer,
			"{"
				"\"min_ts\": %u,"
				"\"max_ts\": %u,"
				"\"min_comp_id\": %u,"
				"\"max_comp_id\": %u"
			"}",
			min_ts, max_ts, min_node, max_node
			);
cleanup:
	if (q)
		bmsgquery_destroy((void*)q);
}

struct bhttpd_handle_fn_entry {
	const char *key;
	const char *content_type;
	void (*fn)(struct bhttpd_req_ctxt*);
};

#define  HTTP_CONT_JSON    "application/json"
#define  HTTP_CONT_STREAM  "application/octet-stream"
#define  HTTP_CONT_TEXT    "text/plain"

struct bhttpd_handle_fn_entry query_handle_entry[] = {
{  "BIG_PIC",      HTTP_CONT_JSON,    bhttpd_handle_query_big_pic      },
{  "HOST",         HTTP_CONT_JSON,    bhttpd_handle_query_host         },
{  "IMG2",         HTTP_CONT_STREAM,  bhttpd_handle_query_img2         },
{  "IMG",          HTTP_CONT_STREAM,  bhttpd_handle_query_img          },
{  "IMG_SIMPLE",   HTTP_CONT_JSON,    bhttpd_handle_query_img_simple   },
{  "IMG_PAN",      HTTP_CONT_JSON,    bhttpd_handle_query_img_pan      },
{  "META",         HTTP_CONT_JSON,    bhttpd_handle_query_meta         },
{  "METRIC_META",  HTTP_CONT_JSON,    bhttpd_handle_query_metric_meta  },
{  "METRIC_PTN",   HTTP_CONT_JSON,    bhttpd_handle_query_metric_ptn   },
{  "MSG2",         HTTP_CONT_JSON,    bhttpd_handle_query_msg2         },
{  "MSG",          HTTP_CONT_JSON,    bhttpd_handle_query_msg          },
{  "MSG_SIMPLE",   HTTP_CONT_TEXT,    bhttpd_handle_query_msg_simple   },
{  "PTN",          HTTP_CONT_JSON,    bhttpd_handle_query_ptn          },
{  "PTN_SIMPLE",   HTTP_CONT_TEXT,    bhttpd_handle_query_ptn_simple   },
};

static
void bhttpd_handle_query(struct bhttpd_req_ctxt *ctxt)
{
	struct bpair_str *kv;
	int i, n, rc;
	struct timeval tv0, tv1, dtv;
	kv = bpair_str_search(&ctxt->kvlist, "type", NULL);
	if (!kv) {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
						"Query type not specified");
		return;
	}

	gettimeofday(&tv0, NULL);
	n = sizeof(query_handle_entry)/sizeof(query_handle_entry[0]);
	for (i = 0; i < n; i++) {
		if (strcasecmp(query_handle_entry[i].key, kv->s1) == 0)
			break;
	}
	if (i < n) {
		pthread_mutex_lock(&query_session_mutex);
		rc = bq_store_refresh(bq_store);
		if (rc) {
			bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"bq_store_refresh() error, rc: %d", rc);
		} else {
			evhttp_add_header(ctxt->hdr, "content-type",
					query_handle_entry[i].content_type);
			query_handle_entry[i].fn(ctxt);
		}
		pthread_mutex_unlock(&query_session_mutex);
	} else {
		bhttpd_req_ctxt_errprintf(ctxt, HTTP_INTERNAL,
				"Unknown query type: %s", kv->s1);
	}
	gettimeofday(&tv1, NULL);
	timersub(&tv1, &tv0, &dtv);
	const char *q = evhttp_uri_get_query(ctxt->uri);
	const char *p = evhttp_uri_get_path(ctxt->uri);
	binfo("handle_query(%s?%s) time: %ld.%06ld", p, q, dtv.tv_sec, dtv.tv_usec);
}

static __attribute__((constructor))
void __init()
{
	const char *s;
	int i, n;
	bdebug("Adding /query handler");
	set_uri_handle("/query", bhttpd_handle_query);
	set_uri_handle("/query/destroy_session",
			bhttpd_handle_query_destroy_session);
	query_session_hash = bhash_new(4099, 7, NULL);
	if (!query_session_hash) {
		berror("bhash_new()");
		exit(-1);
	}
	s = getenv("BHTTPD_QUERY_SESSION_TIMEOUT");
	if (s)
		query_session_timeout.tv_sec = atoi(s);
}

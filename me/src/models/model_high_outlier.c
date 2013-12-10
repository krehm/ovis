/*
 * model_high_outlier.c
 *
 *  Created on: Jun 4, 2013
 *      Author: nichamon
 */
/**
 * model_high_outlier.c
 * Model Type: univariate
 * Thresholds: number in standard deviation unit, e.g., 2.5 = 2.5sd
 * Detect: too high values
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include "me.h"

#define HIGH_OUTLIER_NUM_REQ_SAMPLES 100

static me_log_fn msglog;

typedef struct high_outlier_param {
	int num_req_samples;
} *high_outlier_param_t;

typedef struct high_outlier_ctxt {
	double mean;
	double avg_sum_square;
	int num_samples;
} *high_outlier_ctxt_t;

static int __update(high_outlier_ctxt_t ctxt, double val)
{
	ctxt->num_samples++;
	int num = ctxt->num_samples;
	double mean = ctxt->mean;
	double avg_ss = ctxt->avg_sum_square;
	if (num == 1) {
		mean = val;
		avg_ss = pow(val, 2);
	} else if (num == 2) {
		mean = (mean + val) / 2;
		avg_ss += pow(val, 2);
	} else {
		mean = ((mean * (num - 1)) + val) / num;
		avg_ss = ((avg_ss * (num - 2)) + pow(val, 2)) / (num - 1);
	}
	ctxt->mean = mean;
	ctxt->avg_sum_square = avg_ss;
	return 0;
}

static double get_sd(double mean, double avg_sum_square, int n)
{
	return sqrt(avg_sum_square - ((n / (n -1 )) * pow(mean, 2)));

}

static double __evaluate(high_outlier_ctxt_t ctxt, double val)
{
	double sd = get_sd(ctxt->mean, ctxt->avg_sum_square,
					ctxt->num_samples);
	return (val - ctxt->mean) / sd;
}

static int evaluate(me_model_engine_t m, me_model_cfg_t cfg,
				me_input_t input_val, me_output_t output)
{
	uint64_t value = me_get_input_value(input_val, msglog);
	high_outlier_param_t param = (high_outlier_param_t)me_get_params(cfg);
	struct high_outlier_ctxt *ctxt = m->mcontext;

	double z;
	if (ctxt->num_samples < param->num_req_samples) {
		output->level = ME_ONLY_UPDATE;
		__update(ctxt, value);
	} else {
		z = __evaluate(ctxt, value);

		double *thrs = me_get_thresholds(cfg);

		if (z >= thrs[ME_SEV_INFO])
			if (z >= thrs[ME_SEV_WARNING])
				if (z >= thrs[ME_SEV_CRITICAL])
					output->level = ME_SEV_CRITICAL;
				else
					output->level = ME_SEV_WARNING;
			else
				output->level = ME_SEV_INFO;
		else
			output->level = ME_SEV_NOMINAL;

		output->ts = me_get_timestamp(input_val);

		if (output->level <= ME_SEV_INFO)
			__update(ctxt, value);
	}
	return 0;
}

static int reset(me_model_engine_t m)
{
	return 0;
}

static void term()
{
}

static const char *usage()
{
	return  "   A univariate model detects too high values according to \n"
		"   the sample Mean and sample Standard Deviation\n"
		"   A too high value is greater than {mean + s.d. * a given threshold}.\n"
		"	create name=model_high_outlier model_id=<model_id> threshlds=<multipliers of s.d.>\n"
		"		params=<number>\n"
		"	   -  number	The number of required samples. If none is given,\n"
		"			the default is 100\n";
}

static void *parse_param(char *params)
{
	struct high_outlier_param *param = malloc(sizeof(*param));
	if (!params)
		param->num_req_samples = HIGH_OUTLIER_NUM_REQ_SAMPLES;
	else
		param->num_req_samples = atoi((char *)params);
	return param;
}

me_model_engine_t new_model_engine(me_model_cfg_t cfg)
{
	struct high_outlier_ctxt *ctxt = calloc(1, sizeof(*ctxt));
	if (!ctxt)
		return NULL;
	struct me_model_engine *eng = calloc(1, sizeof(*eng));
	if (!eng)
		return NULL;
	eng->evaluate = evaluate;
	eng->reset_state = reset;
	eng->mcontext = (void *)ctxt;
	return eng;
}

struct me_model_plugin model_high_outlier = {
	.base = {
			.name = "model_high_outlier",
			.term = term,
			.type = ME_MODEL_PLUGIN,
			.usage = usage
		},
	.new_model_engine = new_model_engine,
	.parse_param = parse_param
};

struct me_plugin *get_plugin(me_log_fn log_fn)
{
	msglog = log_fn;
	model_high_outlier.base.model_pi = &model_high_outlier;
	return &model_high_outlier.base;
}
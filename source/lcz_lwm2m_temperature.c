/**
 * @file lcz_lwm2m_temperature.c
 * @brief
 *
 * Copyright (c) 2022 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(lwm2m_temperature, CONFIG_LCZ_LWM2M_TEMPERATURE_LOG_LEVEL);

/**************************************************************************************************/
/* Includes                                                                                       */
/**************************************************************************************************/
#include <zephyr/zephyr.h>
#include <zephyr/init.h>
#include <zephyr/net/lwm2m.h>
#include <lwm2m_resource_ids.h>
#include <lcz_snprintk.h>
#include <lcz_lwm2m_util.h>

#include "lcz_lwm2m_temperature.h"

/**************************************************************************************************/
/* Local Data Definitions                                                                         */
/**************************************************************************************************/
static struct lwm2m_obj_agent temperature_create_agent;

/**************************************************************************************************/
/* Local Function Prototypes                                                                      */
/**************************************************************************************************/
static int create_temperature_sensor(int idx, uint16_t type, uint16_t instance, void *context);

/**************************************************************************************************/
/* SYS INIT                                                                                       */
/**************************************************************************************************/
static int lcz_lwm2m_temperature_init(const struct device *dev)
{
	ARG_UNUSED(dev);

	temperature_create_agent.type = IPSO_OBJECT_TEMP_SENSOR_ID;
	temperature_create_agent.create = create_temperature_sensor;
	lcz_lwm2m_util_register_agent(&temperature_create_agent);

	return 0;
}

SYS_INIT(lcz_lwm2m_temperature_init, APPLICATION, LCZ_LWM2M_UTIL_USER_INIT_PRIORITY);

/**************************************************************************************************/
/* Global Function Definitions                                                                    */
/**************************************************************************************************/
#if defined(CONFIG_LCZ_LWM2M_UTIL_MANAGE_OBJ_INST)
int lcz_lwm2m_managed_temperature_set(int idx, uint16_t offset, double value)
{
	uint16_t type = IPSO_OBJECT_TEMP_SENSOR_ID;
	int instance;
	int r;

	instance = lcz_lwm2m_util_manage_obj_instance(type, idx, offset);
	if (instance < 0) {
		return instance;
	}

	r = lcz_lwm2m_temperature_set(instance, value);
	lcz_lwm2m_util_manage_obj_deletion(r, type, idx, instance);
	return r;
}
#endif

int lcz_lwm2m_temperature_set(uint16_t instance, double value)
{
	char path[LWM2M_MAX_PATH_STR_LEN];

	LCZ_SNPRINTK(path, "%u/%u/%u", IPSO_OBJECT_TEMP_SENSOR_ID, instance, SENSOR_VALUE_RID);
	return lwm2m_engine_set_float(path, &value);
}

int lcz_lwm2m_temperature_create(uint16_t instance)
{
	return lcz_lwm2m_util_create_obj_inst(IPSO_OBJECT_TEMP_SENSOR_ID, instance);
}

/**************************************************************************************************/
/* Local Function Definitions                                                                     */
/**************************************************************************************************/
static int create_temperature_sensor(int idx, uint16_t type, uint16_t instance, void *context)
{
	ARG_UNUSED(idx);
	ARG_UNUSED(context);
	char units[] = "C";
	double min = -40.0;
	double max = 125.0;
	char path[LWM2M_MAX_PATH_STR_LEN];

	LCZ_SNPRINTK(path, "%u/%u/5701", type, instance);
	lwm2m_engine_set_string(path, units);

	/* 5603 and 5604 are the range of values supported by sensor. */
	LCZ_SNPRINTK(path, "%u/%u/5603", type, instance);
	lwm2m_engine_set_float(path, &min);

	LCZ_SNPRINTK(path, "%u/%u/5604", type, instance);
	lwm2m_engine_set_float(path, &max);

	return 0;
}

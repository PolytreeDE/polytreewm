#include "constraints.h"

#define MIN_MASTER_AREA_FACTOR 0.05
#define MAX_MASTER_AREA_FACTOR 0.95

float constraints_default_master_area_factor(const float default_master_area_factor)
{
	return constraints_master_area_factor(default_master_area_factor);
}

float constraints_master_area_factor(const float master_area_factor)
{
	if (master_area_factor < MIN_MASTER_AREA_FACTOR) return MIN_MASTER_AREA_FACTOR;
	if (master_area_factor > MAX_MASTER_AREA_FACTOR) return MAX_MASTER_AREA_FACTOR;
	return master_area_factor;
}

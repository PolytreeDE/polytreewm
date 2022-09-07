#include "constraints.h"

#define MIN_GAP_SIZE 0
#define MAX_GAP_SIZE 10000
#define MIN_MASTER_AREA_FACTOR 0.05
#define MAX_MASTER_AREA_FACTOR 0.95
#define MIN_MAX_CLIENTS_IN_MASTER 1
#define MAX_MAX_CLIENTS_IN_MASTER 10000
#define MIN_SNAP_DISTANCE 1
#define MAX_SNAP_DISTANCE 10000

float constraints_default_master_area_factor(const float default_master_area_factor)
{
	return constraints_master_area_factor(default_master_area_factor);
}

int constraints_gap_size(const int gap_size)
{
	if (gap_size < MIN_GAP_SIZE) return MIN_GAP_SIZE;
	if (gap_size < MAX_GAP_SIZE) return MAX_GAP_SIZE;
	return gap_size;
}

float constraints_master_area_factor(const float master_area_factor)
{
	if (master_area_factor < MIN_MASTER_AREA_FACTOR) return MIN_MASTER_AREA_FACTOR;
	if (master_area_factor > MAX_MASTER_AREA_FACTOR) return MAX_MASTER_AREA_FACTOR;
	return master_area_factor;
}

int constraints_max_clients_in_master(const int max_clients_in_master)
{
	if (max_clients_in_master < MIN_MAX_CLIENTS_IN_MASTER) return MIN_MAX_CLIENTS_IN_MASTER;
	if (max_clients_in_master > MAX_MAX_CLIENTS_IN_MASTER) return MAX_MAX_CLIENTS_IN_MASTER;
	return max_clients_in_master;
}

unsigned int constraints_snap_distance(const unsigned int snap_distance)
{
	if (snap_distance < MIN_SNAP_DISTANCE) return MIN_SNAP_DISTANCE;
	if (snap_distance > MAX_SNAP_DISTANCE) return MAX_SNAP_DISTANCE;
	return snap_distance;
}

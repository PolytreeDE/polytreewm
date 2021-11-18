#include "constraints.h"

#define MIN_BORDER_WIDTH 0
#define MAX_BORDER_WIDTH 10000
#define MIN_DEFAULT_CLIENTS_IN_MASTER 1
#define MAX_DEFAULT_CLIENTS_IN_MASTER 10000
#define MIN_DEFAULT_MASTER_AREA_FACTOR 0.05
#define MAX_DEFAULT_MASTER_AREA_FACTOR 0.95
#define MIN_GAP_SIZE 0
#define MAX_GAP_SIZE 10000
#define MIN_MASTER_AREA_FACTOR 0.05
#define MAX_MASTER_AREA_FACTOR 0.95
#define MIN_MAX_CLIENTS_IN_MASTER 1
#define MAX_MAX_CLIENTS_IN_MASTER 10000
#define MIN_SNAP_DISTANCE 1
#define MAX_SNAP_DISTANCE 10000

int constraints_border_width(const int border_width)
{
	if (border_width < MIN_BORDER_WIDTH) return MIN_BORDER_WIDTH;
	if (border_width > MAX_BORDER_WIDTH) return MAX_BORDER_WIDTH;
	return border_width;
}

int constraints_default_clients_in_master(const int default_clients_in_master)
{
	if (default_clients_in_master < MIN_DEFAULT_CLIENTS_IN_MASTER) return MIN_DEFAULT_CLIENTS_IN_MASTER;
	if (default_clients_in_master > MAX_DEFAULT_CLIENTS_IN_MASTER) return MAX_DEFAULT_CLIENTS_IN_MASTER;
	return default_clients_in_master;
}

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

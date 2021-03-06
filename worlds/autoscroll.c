#include <stdbool.h>
#include <stdint.h>

#include "../worlds.h"
#include "local.h"

#define abs(x)	(((x) < 0) ? -(x) : (x))

static void
mark (const struct world_state *state, int64_t now, struct mark *mark)
{
	mark->coords = state->center;
	mark->time = now;
}

void
autoscroll_measure_down (struct world_state *state, int64_t now)
{
	mark(state, now, &state->autoscroll.down);
}

void
autoscroll_measure_hold (struct world_state *state, int64_t now)
{
	mark(state, now, &state->autoscroll.hold);
}

void
autoscroll_measure_free (struct world_state *state, int64_t now)
{
	const struct mark *down = &state->autoscroll.down;
	const struct mark *hold = &state->autoscroll.hold;
	const struct mark *free = &state->autoscroll.free;

	mark(state, now, &state->autoscroll.free);

	// Check if the user has "moved the hold" sufficiently to start the
	// autoscroll. Not every click on the map should cause movement. Only
	// "significant" drags count.

	float dx = free->coords.tile.x - hold->coords.tile.x;
	float dy = free->coords.tile.y - hold->coords.tile.y;
	float dt = free->time - hold->time;

	// If the mouse has been stationary for a little while, and the last
	// mouse movement was insignificant, don't start:
	if (dt > 1e5f && abs(dx) < 12.0f && abs(dy) < 12.0f)
		return;

	// Speed and direction of autoscroll is measured between "down" point
	// and "up" point:
	dx = free->coords.tile.x - down->coords.tile.x;
	dy = free->coords.tile.y - down->coords.tile.y;
	dt = free->time - down->time;

	// The 2.0 coefficient is "friction":
	state->autoscroll.speed.tile.x = dx / dt / 2.0f;
	state->autoscroll.speed.tile.y = dy / dt / 2.0f;

	// Now for lat/lon reference:
	dx = free->coords.lon - down->coords.lon;
	dy = free->coords.lat - down->coords.lat;

	state->autoscroll.speed.lon = dx / dt / 2.0f;
	state->autoscroll.speed.lat = dy / dt / 2.0f;

	state->autoscroll.active = true;
}

// Returns true if autoscroll was actually stopped:
bool
autoscroll_stop (struct world_state *state)
{
	bool on = state->autoscroll.active;
	state->autoscroll.active = false;
	return on;
}

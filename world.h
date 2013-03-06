#ifndef WORLD_H
#define WORLD_H

unsigned int world_get_size (void);
unsigned int world_get_size_at (const int zoomlevel);
int world_get_zoom (void);
bool world_zoom_in (void);
bool world_zoom_out (void);
void world_zoom_to (const int zoomlevel);

#endif	/* WORLD_H */
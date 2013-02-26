#ifndef VIEWPORT_H
#define VIEWPORT_H

bool viewport_init (void);
void viewport_destroy (void);
void viewport_zoom_in (const int screen_x, const int screen_y);
void viewport_zoom_out (const int screen_x, const int screen_y);
void viewport_scroll (const int dx, const int dy);
void viewport_center_at (const int screen_x, const int screen_y);
void viewport_reshape (const unsigned int width, const unsigned int height);
void viewport_render (void);
bool viewport_within_world_bounds (void);

unsigned int viewport_get_ht (void);
unsigned int viewport_get_wd (void);
unsigned int viewport_get_center_x (void);
unsigned int viewport_get_center_y (void);
int viewport_get_tile_top (void);
int viewport_get_tile_left (void);
int viewport_get_tile_right (void);
int viewport_get_tile_bottom (void);

#endif

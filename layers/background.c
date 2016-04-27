#include <stdbool.h>
#include <stdlib.h>

#include <GL/gl.h>

#include "../png.h"
#include "../layers.h"
#include "../inlinebin.h"
#include "../programs.h"
#include "../programs/bkgd.h"

static struct {
	char		*rgb;
	const char	*png;
	size_t		 len;
	GLuint		 id;
	unsigned int	 width;
	unsigned int	 height;
} tex;

// Array of counterclockwise vertices:
//
//   3--2
//   |  |
//   0--1
//
static struct vertex {
	float x;
	float y;
	float u;
	float v;
} __attribute__((packed))
vertex[4] = {
	[0] = { .x = -1.0, .y = -1.0 },
	[1] = { .x =  1.0, .y = -1.0 },
	[2] = { .x =  1.0, .y =  1.0 },
	[3] = { .x = -1.0, .y =  1.0 },
};

// Array of indices. We define two counterclockwise triangles:
// 3-0-2 and 0-1-2
static GLubyte index[6] = {
	3, 0, 2,
	0, 1, 2,
};

static GLuint vao, vbo;

static void
texcoords (float screen_wd, float screen_ht)
{
	float wd = screen_wd / tex.width;
	float ht = screen_ht / tex.height;

	// Bottom left:
	vertex[0].u = 0;
	vertex[0].v = 0;

	// Bottom right:
	vertex[1].u = wd;
	vertex[1].v = 0;

	// Top right:
	vertex[2].u = wd;
	vertex[2].v = ht;

	// Top left:
	vertex[3].u = 0;
	vertex[3].v = ht;
}

static void
paint (void)
{
	// The background is infinitely far away:
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	// Use the background program:
	program_bkgd_use(&((struct program_bkgd) {
		.tex = 0,
	}));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex.id);

	// Copy vertices to buffer:
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	// Draw all triangles in the buffer:
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, sizeof(index) / sizeof(index[0]), GL_UNSIGNED_BYTE, index);

	program_none();
}

static void
resize (const unsigned int width, const unsigned int height)
{
	// Update texture coordinates:
	texcoords(width, height);
}

static bool
init (void)
{
	// Generate vertex buffer object:
	glGenBuffers(1, &vbo);

	// Generate vertex array object:
	glGenVertexArrays(1, &vao);

	// Bind buffer and vertex array:
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindVertexArray(vao);

	// Add pointer to 'vertex' attribute:
	glEnableVertexAttribArray(program_bkgd_loc_vertex());
	glVertexAttribPointer(program_bkgd_loc_vertex(), 2, GL_FLOAT, GL_FALSE,
		sizeof(struct vertex),
		(void *)(&((struct vertex *)0)->x));

	// Add pointer to 'texture' attribute:
	glEnableVertexAttribArray(program_bkgd_loc_texture());
	glVertexAttribPointer(program_bkgd_loc_texture(), 2, GL_FLOAT, GL_FALSE,
		sizeof(struct vertex),
		(void *)(&((struct vertex *)0)->u));

	// Generate texture:
	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);

	// Load texture:
	inlinebin_get(TEXTURE_BACKGROUND, &tex.png, &tex.len);
	png_load(tex.png, tex.len, &tex.height, &tex.width, &tex.rgb);

	// Upload texture:
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.width, tex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex.rgb);

	// Free memory:
	free(tex.rgb);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return true;
}

static void
destroy (void)
{
	// Delete texture:
	glDeleteTextures(1, &tex.id);

	// Delete vertex array object:
	glDeleteVertexArrays(1, &vao);

	// Delete vertex buffer:
	glDeleteBuffers(1, &vbo);
}

const struct layer *
layer_background (void)
{
	static struct layer layer = {
		.init     = &init,
		.paint    = &paint,
		.resize   = &resize,
		.destroy  = &destroy,
	};

	return &layer;
}

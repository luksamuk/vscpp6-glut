#include "scene.hpp"
#include <cmath>
#include <GL/glut.h>
#include <GL/gl.h>

#include "utils.hpp"
#include "render.hpp"
#include "keyboard.hpp"

// Rectangle with constant speed
static GLuint container_texture = 0;
static float x = 0.0f;
static float y = 0.0f;

// Ball with accelerated movement
static float bx  = 0.0f;
static float by  = 0.0f;
static float bsx = 0.0f;
static float bsy = 0.0f;

// Teapot with constant speed in Z axis
static float teapot_angle = 0.0f;
static float teapot_z = 0.0f;

void
scene_init(void)
{
	container_texture = load_texture("img/win98.png");
}

void
scene_dispose(void)
{
	glDeleteTextures(1, &container_texture);
	container_texture = 0;
}

void
scene_update(double dt)
{
	/* CONSTANT MOVEMENT */

	// Represents the amount of pixels walked for every
	// frame. Given that a frame is 16ms for a 60FPS
	// game, we'd be walking 1/2 px per frame -- or 30px
	// per "expected" second.
	const float pixelspeed = 0.5;
	
	// We're going to discover the distance that we should walk
	// on screen regardless of the FPS. So we need to take the
	// walked distance per second as base, and interpolate it
	// so that we walk the expected amount for a time difference
	// dt between frames -- s = vt

	float walkdist = pixelspeed * dt;

	if(kbdPressing(BTN_UP))
		y += walkdist;
	if(kbdPressing(BTN_DOWN))
		y -= walkdist;
	if(kbdPressing(BTN_LEFT))
		x -= walkdist;
	if(kbdPressing(BTN_RIGHT))
		x += walkdist;


	/* ACCELERATED MOVEMENT */

	// Acceleration and deceleration are also compensated
	// since the values are represented as if we were
	// running a 60FPS application.
	// Deceleration shouldn't be greater than acceleration.
	const float accel = 0.02f  * 60.0f;
	const float decel = 0.015f * 60.0f;
	float topspeed = 180.0f * dt;

	// Horizontal movement
	if(kbdPressing(BTN_RIGHT))
		bsx += accel * dt;
	if(kbdPressing(BTN_LEFT))
		bsx -= accel * dt;
	if(!kbdPressing(BTN_RIGHT) && !kbdPressing(BTN_LEFT)) {
		bsx -= decel * signbit(bsx) * dt;
		if(fabs(bsx) <= (accel * dt)) {
			bsx = 0.0f;
		}
	}

	// Vertical movement
	if(kbdPressing(BTN_UP))
		bsy += accel * dt;
	if(kbdPressing(BTN_DOWN))
		bsy -= accel * dt;
	if(!kbdPressing(BTN_UP) && !kbdPressing(BTN_DOWN)) {
		bsy -= decel * signbit(bsy) * dt;
		if(fabs(bsy) <= (accel * dt)) {
			bsy = 0.0f;
		}
	}

	// Speed limits
	bsx = clamp(bsx, -topspeed, topspeed);
	bsy = clamp(bsy, -topspeed, topspeed);

	// Apply ball speed to position wrt. dt
	bx += bsx * dt;
	by += bsy * dt;

	// Set light 0 position to ball
	float lightPos[] = {bx, by, -1.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	/* Teapot */
	teapot_angle += 45.0f * dt;
	teapot_angle -= floor(teapot_angle / 360.0f) * 360.0f;

	if(kbdPressing(BTN_ACTION2))
		teapot_z += walkdist;
	if(kbdPressing(BTN_ACTION1))
		teapot_z -= walkdist;
}

void
_draw_rectangle(void)
{
	// Rectangle
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, container_texture);
	glPushMatrix();
		glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
		glTranslatef(x, y, 0.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			//glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex2f(-0.5f, 0.5f);

			glTexCoord2f(1.0f, 0.0f);
			//glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertex2f(0.5f, 0.5f);

			glTexCoord2f(1.0f, 1.0f);
			//glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
			glVertex2f(0.5f, -0.5f);

			glTexCoord2f(0.0f, 1.0f);
			//glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertex2f(-0.5f, -0.5f);
		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void
_draw_ball(void)
{
	// Ball
	float angle = 0.0f;
	const float radius = 0.5f;

	const float colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.3f, 0.2f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.7f, 0.0f, 0.5f,
		0.5f, 0.2f, 0.0f,
		0.6f, 0.0f, 0.8f,
		0.0f, 0.4f, 0.3f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
	};

	const int num_colors = sizeof(colors) / (3 * sizeof(float));

	static int color_stride = 0;
	static int old_time = 0;

	int curr_time = glutGet(GLUT_ELAPSED_TIME);
	if(curr_time - old_time > 50) {
		old_time = curr_time;
		color_stride = (color_stride + 3) % (6 * 3);
	}

	glPushMatrix();
		glTranslatef(bx, by, 0.25f);
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glVertex2f(0.0f, 0.0f);

		int current_color = color_stride;
		for(angle = 0.0f; angle < 360.0f; angle += 0.25f) {
			glColor4f(
				colors[current_color],
				colors[current_color + 1],
				colors[current_color + 2],
				0.02f
			);
			glVertex2f(
				radius * cosf(angle),
				radius * sinf(angle)
			);
			current_color = (current_color + 3) % (6 * 3);
		}
		glEnd();
	glPopMatrix();
}

void
scene_draw(void)
{
	//_draw_rectangle();
	_draw_ball();

	// Teapot
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, teapot_z);
		glRotatef(teapot_angle, 0.0f, 1.0f, 0.0f);
		glutSolidTeapot(0.3f);
	glPopMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
}

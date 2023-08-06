#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <GL/glut.h>
#include <GL/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "fps.hpp"
#include "keyboard.hpp"

#define WINW 500
#define WINH 500

// Triangle with constant speed
static float x = 0.0f;
static float y = 0.0f;

// Ball with accelerated movement
static float bx  = 0.0f;
static float by  = 0.0f;
static float bsx = 0.0f;
static float bsy = 0.0f;

static std::string windowTitle;

#ifndef signbit
#define signbit(x) (x < 0 ? -1 : 1)
#endif

#define clamp(x, min, max) (x < min ? min : (x > max ? max : x))

static GLuint container_texture = 0;
static float teapot_angle = 0.0f;

GLuint
load_texture()
{
	int width, height, channels;
	unsigned char *data = stbi_load("img/win98.jpg", &width, &height, &channels, 0);
	if(data == NULL) {
		std::cerr << "Error loading texture" << std::endl;
		exit(1);
	}
	
	glEnable(GL_TEXTURE_2D);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	stbi_image_free(data);
	
	return texture;
}



void
update(void)
{
	static int oldTime = 0;

	fpsUpdate();

	double dt = getDeltaTime();

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


	teapot_angle += 45.0f * dt;
	teapot_angle -= floor(teapot_angle / 360.0f) * 360.0f;

	/* FPS information on title */
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	if(currTime - oldTime > 2000) { // Every 2s
		std::ostringstream oss;
		oss.clear();
		oss << "MyGame | FPS: "
			<< std::setw(2)
			<< getFps();
		windowTitle = oss.str();
		oldTime = currTime;

		glutSetWindowTitle(windowTitle.c_str());
	}

	glutPostRedisplay();
}

void
draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Rectangle
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, container_texture);
	glPushMatrix();
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		glTranslatef(x, y, 0.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			//glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex2f(-0.5f, 0.5f);

			glTexCoord2f(1.0f, 0.0f);
			//glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertex2f(0.5f, 0.5f);

			glTexCoord2f(1.0f, 1.0f);
			//glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertex2f(0.5f, -0.5f);

			glTexCoord2f(0.0f, 1.0f);
			//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glVertex2f(-0.5f, -0.5f);
		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	// Ball
	float angle = 0.0f;
	const float radius = 0.3f;

	const float colors[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.3f, 0.2f, 1.0f, 0.3f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.7f, 0.0f, 0.5f, 0.6f,
		0.5f, 0.2f, 0.0f, 1.0f,
		0.6f, 0.0f, 0.8f, 1.0f,
		0.0f, 0.4f, 0.3f, 1.0f,
	};

	const int num_colors = sizeof(colors) / (4 * sizeof(float));

	static int color_stride = 0;
	static int old_time = 0;

	int curr_time = glutGet(GLUT_ELAPSED_TIME);
	if(curr_time - old_time > 50) {
		old_time = curr_time;
		color_stride = (color_stride + 4) % (6 * 4);
	}

	glPushMatrix();
		glTranslatef(bx, by, 1.0f);
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);

		int current_color = color_stride;
		for(angle = 0.0f; angle < 360.0f; angle += 0.2f) {
			glColor4f(
				colors[current_color],
				colors[current_color + 1],
				colors[current_color + 2],
				colors[current_color + 3]
			);
			glVertex2f(
				radius * cosf(angle),
				radius * sinf(angle)
			);
			current_color = (current_color + 4) % (6 * 4);
		}
		glEnd();
	glPopMatrix();


	glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
	glPushMatrix();
	glRotatef(teapot_angle, 0.0f, 1.0f, 0.0f);
	glutSolidTeapot(0.3f);
	glPopMatrix();

	glutSwapBuffers();
}

void
display(void)
{
	static bool init = false;

	if(!init) {
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		container_texture = load_texture();
		init = true;
	}

	update();
	draw();
}

inline void
keyHandle(unsigned char key, bool pressed)
{
	if(pressed && key == 27) {
		exit(0);
	}

	unsigned int btn;

	switch(key) {
	case 'w': case 'W':
		btn = BTN_UP;
		break;
	case 'a': case 'A':
		btn = BTN_LEFT;
		break;
	case 's': case 'S':
		btn = BTN_DOWN;
		break;
	case 'd': case 'D':
		btn = BTN_RIGHT;
		break;
	case 'k': case 'K':
		btn = BTN_ACTION1;
		break;
	// TODO: Action 2
	case 10:
		btn = BTN_START;
		break;
	default:
		return; // Do not process further
	}

	kbdUpdateButton(btn, pressed);
}

void
keyDown(unsigned char key, int x, int y)
{
	keyHandle(key, true);
}

void
keyUp(unsigned char key, int x, int y)
{
	keyHandle(key, false);
}

int
main(int argc, char **argv)
{
	kbdInit();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - WINW) / 2,
        (glutGet(GLUT_SCREEN_HEIGHT) - WINH) / 2
	);

	glutInitWindowSize(WINW, WINH);
	
	glutCreateWindow("MyGame");

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);

	glutMainLoop();
	return 0;
}

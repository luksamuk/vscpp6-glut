#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <GL/glut.h>
#include <GL/gl.h>

#include "fps.hpp"
#include "keyboard.hpp"
#include "render.hpp"
#include "utils.hpp"
#include "scene.hpp"

// Window stuff
static std::string windowTitle;
#define WINW 500
#define WINH 500

void
update(void)
{
	static int oldTime = 0;
	fpsUpdate();
	double dt = getDeltaTime();

	scene_update(dt);

	/* FPS information on title */
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	if(currTime - oldTime > 2000) { // Every 2s
		std::ostringstream oss;

		double fps = getFps();

		oss.clear();
		oss << "MyGame | FPS: "
			<< std::setw(2)
			<< getFps();
		windowTitle = oss.str();
		oldTime = currTime;

		std::cout << "FPS: " << fps << std::endl;

		glutSetWindowTitle(windowTitle.c_str());
	}

	glutPostRedisplay();
}

void
draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene_draw();
	glutSwapBuffers();
}

void
display(void)
{
	update();
	draw();
}

inline void
keyHandle(unsigned char key, bool pressed)
{
	if(pressed && key == 27) {
		goto app_exit;
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
	case 'i': case 'I':
		btn = BTN_ACTION2;
		break;
	case 10:
		btn = BTN_START;
		break;
	default:
		return; // Do not process further
	}

	kbdUpdateButton(btn, pressed);
	return;

app_exit:
	scene_dispose();
	exit(0);
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

	render_init();
	scene_init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);

	glutMainLoop();

	return 0;
}

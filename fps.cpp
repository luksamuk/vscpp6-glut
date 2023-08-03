#include <GL/glut.h>
#include "fps.hpp"

static double fps        = 1.0f;
static double deltaTime  = 1.0f;
static int frame         = 0;
static int time          = 0;
static int lasttime      = 0;
static int timebase      = 0;

void
fpsUpdate(void)
{
	frame++;
	lasttime = time;
	time = glutGet(GLUT_ELAPSED_TIME);

	deltaTime = (double)(time - lasttime);

	if(time - timebase > 1000.0) {
		fps = frame * 1000.0 / (time - timebase);
		timebase = time;
		frame = 0;
	}
}

double
getFps(void)
{
	return fps;
}

double
getDeltaTime(void)
{
	return deltaTime / 1000.0;
}
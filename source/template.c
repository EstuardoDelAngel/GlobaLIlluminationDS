#include <nds.h>
#include <malloc.h>
#include <nds/arm9/image.h>

#include "lightmap_pcx.h"
#include "scene.h"

int	texture[1];

int rot = 12287;
int y_pos = 8192;


int main(void) {
	videoSetMode(MODE_0_3D);
	vramSetBankA(VRAM_A_TEXTURE);

	glInit();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ANTIALIAS);
	
	glClearColor(0,0,0,31);
	glClearPolyID(63);
	glClearDepth(0x7FFF);
	
	glViewport(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
	
	sImage pcx;
	loadPCX((u8 *)lightmap_pcx, &pcx);
	image8to16(&pcx);
	glGenTextures(1, &texture[0]);
	glBindTexture(0, texture[0]);
	glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_256, TEXTURE_SIZE_256, 0, TEXGEN_TEXCOORD, pcx.image.data8);
	imageDestroy(&pcx);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 100);
	
	glMatrixMode(GL_MODELVIEW);

	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

	while (1) {
		glColor3b(255, 255, 255);
		
		glLoadIdentity();

		glTranslatef32(0, -y_pos, -24576);
		
		glRotateYi(rot);

		glBindTexture(GL_TEXTURE_2D, texture[0]);

		glBegin(GL_TRIANGLES);

		for (int i = 0; i < sceneNumPointIndices; i++) {
			glTexCoord2t16(sceneUV[2*sceneUVIndices[i]], sceneUV[2*sceneUVIndices[i]+1]);
			glVertex3v16(scenePoints[3*scenePointIndices[i]], scenePoints[3*scenePointIndices[i]+1], scenePoints[3*scenePointIndices[i]+2]);
		}

		glFlush(0);
		
		swiWaitForVBlank();
		scanKeys();

		int pressed = keysDown();
		int held = keysHeld();

		if (pressed & KEY_START) break;	
		if (held & KEY_LEFT) rot -= 100;
		if (held & KEY_RIGHT) rot += 100;
		if (held & KEY_DOWN) y_pos -= 100;
		if (held & KEY_UP) y_pos += 100;
	}
	
	return 0;
}
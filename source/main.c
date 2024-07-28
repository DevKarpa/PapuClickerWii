#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <asndlib.h>
#include <mp3player.h>

// nombre de la canción a sonar de fondo + "_mp3.h"
#include "slow_mp3.h"
#include "fast_mp3.h"


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static u32 *xfb;
static GXRModeObj *rmode;


// Inicializa el video
void screen_setup()
{
	// Initialise the video system
	VIDEO_Init();

	// This function initialises the attached controllers
	WPAD_Init();
	
	// audio
	ASND_Init();
	MP3Player_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
}

int auto_increment(int click){

	click++;
	
	return click;
}


int main() {
 
	int click = 0;
	int frames = 0;
	bool mejora = false;
	
	screen_setup();
	
	// pone un temazo
	MP3Player_PlayBuffer(slow_mp3, slow_mp3_size, NULL);
 
	while(true) {
	
		frames++;

		printf(ANSI_COLOR_GREEN "PAPU CLICKER WII PORT 0.1! \n");
		printf(ANSI_COLOR_RED "%d",click);
		printf("\n");
		// Lee las lecturas del mando en cada ejecución del bucle
		WPAD_ScanPads();
		
		// Para que no salte hasta que se levanta el botón
		u32 pressed = WPAD_ButtonsDown(0);
		
		// Si se presiona el botón A
		if ( pressed & WPAD_BUTTON_A ){
			click = auto_increment(click);
		}
		
		// Si se tienen 100 puntos puedes comprar una mejora
		if(click > 50 && mejora == false){
			printf(ANSI_COLOR_CYAN "PULSA B PARA COMPRAR LA MEJORA !!! \n");
			if ( pressed & WPAD_BUTTON_B ){
				mejora = true;
				MP3Player_Stop();
				MP3Player_PlayBuffer(fast_mp3, fast_mp3_size, NULL);
			};
		}
		
		// Si está la mejora activada, se suma el valor +1 cada 30 frames.
		if(mejora == true){
			if(frames > 30){
				click = auto_increment(click);
				frames = 0;
			}
		}
		
		
		// Al pulsar home vuelve al homebrew launcher
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		

		// Espera al siguiente frame
		VIDEO_WaitVSync();
		printf("\e[1;1H\e[2J");	
		
	}
 
	return 0;
}

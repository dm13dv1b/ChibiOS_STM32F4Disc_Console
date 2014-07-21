/*
 ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "usbcfg.h"
#include "shell.h"
#include "ansi.h"
#include "status.h"
#include "test.h"

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)
#define TEST_WA_SIZE    THD_WA_SIZE(256)

/*
 * UART driver configuration structure.
 */

/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;

static const ShellCommand commands[] = {
	{"mem", cmd_mem},
	{"threads",cmd_threads},
	{"status",cmd_status},
	{"ansiTest",cmd_ansiColorTest},
	{"echo",cmd_echo},
	{"getKey",cmd_getKey},
	{"extended",cmd_ExtendedAscii},
	{"box",cmd_box},
	{"draw",cmd_draw},
	{NULL,NULL}
};

static const ShellConfig shell_cfg0 = {(BaseSequentialStream *)&SDU1, commands};

/* Added by myself */
/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_LED3);       /* Orange.  */
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, GPIOD_LED3);     /* Orange.  */
    chThdSleepMilliseconds(500);
  }
}
/* Added by myself */

/*
 * Application entry point.
 */
int main(void) {
	Thread *shelltp0 = NULL;
	Thread *shelltp2 = NULL;

	//unsigned int i = 0;
	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();

	  /*
	   * Activates the serial driver 2 using the driver default configuration.
	   * PA2(TX) and PA3(RX) are routed to USART2.
	   */
	  sdStart(&SD2, NULL);
	  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

	  /*
	   * If the user button is pressed after the reset then the test suite is
	   * executed immediately before activating the various device drivers in
	   * order to not alter the benchmark scores.
	   */
	  if (palReadPad(GPIOA, GPIOA_BUTTON))
	    TestThread(&SD2);


	/*
	 * Initializes a serial-over-USB CDC driver.
	 */
	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);

	/*
	 * Activates the USB driver and then the USB bus pull-up on D+.
	 * Note, a delay is inserted in order to not have to disconnect the cable
	 * after a reset.
	 */
	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1500);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);

	/* added by myself */
	chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
	/* added by myself */

	/*
	 * Normal main() thread activity, in this demo it just performs
	 * a shell respawn upon its termination.
	 */
	while (1) {
		if (!shelltp0) {
			if (SDU1.config->usbp->state == USB_ACTIVE) {
				/* Spawns a new shell.*/
				shelltp0 = shellCreate(&shell_cfg0, SHELL_WA_SIZE, NORMALPRIO);
			}
		} else {
			/* If the previous shell exited.*/
			if (chThdTerminated(shelltp0)) {
				/* Recovers memory of the previous shell.*/
				chThdRelease(shelltp0);
				shelltp0 = NULL;
			}
		}
		//added by myself
		if (palReadPad(GPIOA, GPIOA_BUTTON))
		      TestThread(&SD2);
		//added by myself
		chThdSleepMilliseconds(1000);
	}
}

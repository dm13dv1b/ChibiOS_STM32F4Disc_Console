/*
 * status.h
 *
 *  Created on: Nov 21, 2013
 *      Author: freyjed
 */

#ifndef STATUS_H_
#define STATUS_H_

void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_status(BaseSequentialStream *chp, int argc, char *argv[]);
msg_t statusRefresh(BaseSequentialStream *chp);
#endif /* STATUS_H_ */

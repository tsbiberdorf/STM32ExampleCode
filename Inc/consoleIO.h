/*
 * consoleIO.h
 *
 *  Created on: Oct 17, 2018
 *      Author: TBiberdorf
 */

#ifndef CONSOLEIO_H_
#define CONSOLEIO_H_

typedef enum _cliCmdType
{
	cliCmdType_np,
	cliCmdType_wp_str,
	cliCmdType_wp_int,
	cliCmdType_i,
	cliCmdType_ip_str,
	cliCmdType_ip_int,
};

void consoleInit();
void StartConsoleIO();


#endif /* CONSOLEIO_H_ */

/*******************************************************************************
 *
 * Project:
 *
 * Subsystem:
 *
 * Filename: types_general.h
 *
 * Description: Declaration of all general types and functions.
 *
 * Author: Dmitry Durnov (dmitry.durnov at gmail.com)
 *
 * History:
 * -----------------------------------------------------------------------------
 * Version  Date      Author            Comment
 * -----------------------------------------------------------------------------
 * 0.1.0    20100523  Dmitry Durnov     Created.
 * 0.2.0    20100606  Dmitry Durnov     LOOP_FOREVER has been added.
 *
 ******************************************************************************/
#ifndef TYPES_GENERAL_H
#define TYPES_GENERAL_H

typedef unsigned char Boolean_t;
#define TRUE  (1)
#define FALSE (0)

typedef unsigned short int Port_t;

#define MAIN_LOOP while(TRUE)
#define LOOP_FOREVER while(TRUE)

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

#endif /* TYPES_GENERAL_H */

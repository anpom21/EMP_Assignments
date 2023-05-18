/*
 * string.h
 *
 *  Created on: 26/12/2011
 *      Author: Morten
 */
#ifndef STRING_H_
#define STRING_H_
#include "file.h"
typedef INT8U *FILE;

void putc1(FILE, unsigned char);
const void gfprintf(FILE, const char *, ...);
const void gsprintf(INT8U *, const char *, ...);
const void gprintf(const char *, ...);
#endif

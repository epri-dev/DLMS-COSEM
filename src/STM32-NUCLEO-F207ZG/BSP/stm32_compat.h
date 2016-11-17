/*
	This file is a part of VisualGDB [http://visualgdb.com/].
	It prevents IntelliSense errors specific to STM32 by hiding them via empty preprocessor definitions.
	This file overrides some of the default definitions in gcc_compat.h in VisualGDB directory.
	NEVER INCLUDE THIS FILE IN YOUR ACTUAL SOURCE CODE.
*/

#ifndef __SYSPROGS_CODESENSE__
#define __asm
#endif
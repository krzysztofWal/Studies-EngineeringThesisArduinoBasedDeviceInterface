// typedefs.h

#ifndef _TYPEDEFS_h
#define _TYPEDEFS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/* Definicja typów */
enum rodzajPolecenia {
    r, /* read  */
    w, /* write */
//    e, /* error */
};

typedef struct {
    byte nrPinu;
    byte stanPinu;
} PinCyfrowy;

typedef struct {
    rodzajPolecenia rodzaj;
    byte nrPinu;
    byte nowyStan;
} PolecenieInfo;


#endif


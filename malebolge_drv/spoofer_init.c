#include "spoofer_globals.h"
#include "spoofer_init.h"
#include "driver_defs.h"

CHAR SERIAL[] = "VOLKOV_PIDARAS";

void InitSpoofer()
{
	RANDOM_SEED = KeQueryTimeIncrement();
	CHAR alphabet[] = "ABCDEF0123456789";
	for(DWORD i = 0; i < strlen(SERIAL); i++)
	{
		RtlRandomEx(&RANDOM_SEED);
		SERIAL[i] = alphabet[RtlRandomEx(&RANDOM_SEED) % (sizeof(alphabet) - 1)];
	}
}
#include <ntifs.h>
#include <stdarg.h>
#include "log.h"


/**
 * \brief Prints text to any driver that has registered
 * callback using DbgSetDebugPrintCallback (ex. DbgView)
 * \param text Text to print
 * \param ... printf() style arguments
 */
void Log::Print(const char* text, ...)
{
	va_list(args);
	va_start(args, text);

#ifdef DEBUG
	vDbgPrintExWithPrefix("[spoofer] ", 0, 0, text, args);
#endif
	va_end(args);
}
#ifndef UTIL_H
#define UTIL_H

#include <locale.h>
#include <libintl.h>

#define _(str) gettext(str)

void init_locale(void);
void loglvl(const char *fmt, ...);

#endif

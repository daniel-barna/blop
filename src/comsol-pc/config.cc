#include "config.h"

#include <cstdlib>

namespace blop
{
    const char *BLOP_INSDIR;
    const char *BLOP_VERSION;
    const char *BLOP_DOCDIR;
    const char *BLOP_LIBDIR;
    const char *BLOP_BINDIR;
    const char *BLOP_INCDIR;
    const char *BLOP_STYDIR;
#ifdef USE_CINTSYSDIR
    const char *BLOP_CINTSYSDIR;
#endif
    const char *BLOP_CPPOPTS;
    const char *BLOP_COMPILER;
    const char *BLOP_ARCH;

    void get_config_vars()
    {
	BLOP_VERSION= getenv("BLOP_VERSION");
	BLOP_INSDIR = getenv("BLOP_INSDIR");
	BLOP_DOCDIR = getenv("BLOP_DOCDIR");
	BLOP_LIBDIR = getenv("BLOP_LIBDIR");
	BLOP_BINDIR = getenv("BLOP_BINDIR");
	BLOP_INCDIR = getenv("BLOP_INCDIR");
	BLOP_STYDIR = getenv("BLOP_STYDIR");
#ifdef USE_CINTSYSDIR
	BLOP_CINTSYSDIR = getenv("BLOP_CINTSYSDIR");
#endif
	BLOP_CPPOPTS = getenv("BLOP_CPPOPTS");
	BLOP_COMPILER = getenv("BLOP_COMPILER");
	BLOP_ARCH     = getenv("BLOP_ARCH");
    }
}

/* src/config.h.  Generated from config.h.in by configure.  */
#ifndef __BLOP_CONFIG_H__
#define __BLOP_CONFIG_H__

#define HAVE_REGEX_H 1
#define HAVE_GSL 1
#define HAVE_GSL_FFT_FORWARD 1
#define HAVE_GTS_H 1
/* #undef USE_CINTSYSDIR */
/* #undef CINT_INCLUDE_PREFIX */
/* #undef BLOP_ENABLE_CINT */

namespace blop
{
    extern const char *BLOP_VERSION;
    extern const char *BLOP_ARCH;
    extern const char *BLOP_INSDIR;
    extern const char *BLOP_DOCDIR;
    extern const char *BLOP_LIBDIR;
    extern const char *BLOP_BINDIR;
    extern const char *BLOP_INCDIR;
    extern const char *BLOP_STYDIR;
#ifdef USE_CINTSYSDIR
    extern const char *BLOP_CINTSYSDIR;
#endif
    extern const char *BLOP_CPPOPTS;
    extern const char *BLOP_COMPILER;

    extern void get_config_vars();
}

#endif

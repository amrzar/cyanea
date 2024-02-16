/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ULIB_ULOG_H__
#define __ULIB_ULOG_H__

#define DEBUG

#ifndef __ASSEMBLY__

#define RESET       "\033[0m"
#define BLACK       "\033[30m"  /* Black */
#define RED         "\033[31m"  /* Red */
#define GREEN       "\033[32m"  /* Green */
#define YELLOW      "\033[33m"  /* Yellow */
#define BLUE        "\033[34m"  /* Blue */
#define MAGENTA     "\033[35m"  /* Magenta */
#define CYAN        "\033[36m"  /* Cyan */
#define WHITE       "\033[37m"  /* White */
#define BOLDBLACK   "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"   /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"   /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"   /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"   /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"   /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"   /* Bold White */

void ulog(const char *, ...);

#if (CONFIG_LOG_LEVEL < 1)
# define ulog_info(...) ulog(__VA_ARGS__)
#else
# define ulog_info(...)
#endif

#define ulog_err(...) do { \
        ulog(BOLDRED "[%s][%d]: " RESET, __FUNCTION__, __LINE__); \
        ulog(__VA_ARGS__); \
        ulog("\n"); \
    } while (0)

#ifdef DEBUG
# define ulog_debug(...) do { \
        ulog(BOLDGREEN "[%s][%d]: " RESET, __FUNCTION__, __LINE__); \
        ulog(__VA_ARGS__); \
        ulog("\n"); \
    } while (0)
#else
# define ulog_debug(...)
#endif

#define HALT do { \
        halt(); \
    } while (1)

#define assert(cond, ...) do { \
        if (!(cond)) { \
            ulog_err(__VA_ARGS__); \
            HALT; \
        } \
    } while(0)

#define warning_on(cond) ({ \
        if (cond) \
            ulog(BOLDYELLOW "[%s][%d]: warning on %s\n" RESET, \
                __FUNCTION__, __LINE__, #cond); \
        (cond); \
    })

#endif /*  __ASSEMBLY__ */

#endif /* __ULIB_ULOG_H__ */

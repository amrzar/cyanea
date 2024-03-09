/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ULOG_H__
#define __ULOG_H__

#define DEBUG

#ifndef __ASSEMBLY__

void ulog(const char *, ...);

#if (CONFIG_LOG_LEVEL < 1)
# define ulog_info(...) ulog(__VA_ARGS__)
#else
# define ulog_info(...)
#endif

#define ulog_err(...) do { \
        ulog("[%s][%d]: ", __FUNCTION__, __LINE__); \
        ulog(__VA_ARGS__); \
        ulog("\n"); \
    } while (0)

#ifdef DEBUG
# define ulog_debug(...) do { \
        ulog("[%s][%d]: ", __FUNCTION__, __LINE__); \
        ulog(__VA_ARGS__); \
        ulog("\n"); \
    } while (0)

#else
# define ulog_debug(...)
#endif

#define assert(cond, ...) do { \
        if (!(cond)) { \
            ulog_err(__VA_ARGS__); \
            while(1); \
        } \
    } while(0)

#define warning_on(cond) ({ \
        int ___cond = (cond); \
        if (__cond) \
            ulog("[%s][%d]: warning on %s\n", __FUNCTION__, __LINE__, #cond); \
        __cond; \
    })

#endif /*  __ASSEMBLY__ */

#endif /* __ULOG_H__ */

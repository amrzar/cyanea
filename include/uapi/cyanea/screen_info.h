/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __UAPI_CYANEA_SCREEN_INFO_H__
#define __UAPI_CYANEA_SCREEN_INFO_H__

#include <uapi/cyanea/types.h>

struct screen_info {
    __u64 framebuffer;
    __u64 framebuffer_size;
    __u64 screen_width;
    __u64 screen_height;
    __u32 pixels_per_scan_line;
} __attribute__((__packed__));

#endif /* __UAPI_CYANEA_SCREEN_INFO_H__ */

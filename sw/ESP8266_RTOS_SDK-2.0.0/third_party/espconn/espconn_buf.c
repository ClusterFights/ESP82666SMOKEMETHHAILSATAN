/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <string.h>

#include "lwip/def.h"
#include "esp_libc.h"

#include "espconn/espconn_buf.h"

#if (!defined(lwIP_unlikely))
#define lwIP_unlikely(Expression) !!(Expression)
#endif

#define lwIP_ASSERT(Expression) do{if(!(Expression)) {os_printf("%s %d\n", __func__, __LINE__);return;}}while(0)

ringbuf_t ringbuf_new(size_t capacity)
{
    ringbuf_t rb = (ringbuf_t)os_zalloc(sizeof(struct ringbuf_t));

    if (rb) {
        rb->size = capacity + 1;
        rb->buf = (uint8*)os_zalloc(rb->size);

        if (rb->buf) {
            ringbuf_reset(rb);
        } else {
            os_free(rb);
            return NULL;
        }
    }

    return rb;
}

size_t ringbuf_buffer_size(const struct ringbuf_t* rb)
{
    return rb->size;
}

void ringbuf_reset(ringbuf_t rb)
{
    rb ->head = rb->tail = rb->buf;
}

void ringbuf_free(ringbuf_t* rb)
{
    lwIP_ASSERT(rb && *rb);
    os_free((*rb)->buf);
    os_free(*rb);
    *rb = NULL;
}

size_t ringbuf_capacity(const struct ringbuf_t* rb)
{
    return ringbuf_buffer_size(rb) - 1;
}

static const uint8_t* ringbuf_end(const struct ringbuf_t* rb)
{
    return rb->buf + ringbuf_buffer_size(rb);
}

size_t ringbuf_bytes_free(const struct ringbuf_t* rb)
{
    if (rb->head >= rb->tail) {
        return ringbuf_capacity(rb) - (rb->head - rb->tail);
    } else {
        return rb->tail - rb->head - 1;
    }
}

size_t ringbuf_bytes_used(const struct ringbuf_t* rb)
{
    return ringbuf_capacity(rb) - ringbuf_bytes_free(rb);
}

int ringbuf_is_full(const struct ringbuf_t* rb)
{
    return ringbuf_bytes_free(rb) == 0;
}

int ringbuf_is_empty(const struct ringbuf_t* rb)
{
    return ringbuf_bytes_free(rb) == ringbuf_capacity(rb);
}

const void* ringbuf_tail(const struct ringbuf_t* rb)
{
    return rb->tail;
}
const void* ringbuf_head(const struct ringbuf_t* rb)
{
    return rb->head;
}

static uint8_t* ringbuf_nextp(ringbuf_t rb, const uint8_t* p)
{
    lwIP_ASSERT((p >= rb->buf) && (p < ringbuf_end(rb)));
    return rb->buf + ((++p - rb->buf) % ringbuf_buffer_size(rb));
}

size_t ringbuf_findchr(const struct ringbuf_t* rb, int c, size_t offset)
{
    const uint8_t* bufend = ringbuf_end(rb);
    size_t bytes_used = ringbuf_bytes_used(rb);

    if (offset >= bytes_used) {
        return bytes_used;
    }

    const uint8_t* start = rb ->buf + (((rb->tail - rb->buf) + offset) % ringbuf_buffer_size(rb));
    lwIP_ASSERT(bufend > start);
    size_t n = LWIP_MIN(bufend - start, bytes_used - offset);
    const uint8_t* found = (const uint8_t*)memchr(start, c, n);

    if (found) {
        return offset + (found - start);
    } else {
        return ringbuf_findchr(rb, c, offset + n);
    }
}

size_t ringbuf_memset(ringbuf_t dst, int c, size_t len)
{
    const uint8_t* bufend = ringbuf_end(dst);
    size_t nwritten = 0;
    size_t count = LWIP_MIN(len, ringbuf_buffer_size(dst));
    int overflow = count > ringbuf_bytes_free(dst);

    while (nwritten != count) {

        lwIP_ASSERT(bufend > dst->head);
        size_t n = LWIP_MIN(bufend - dst->head, count - nwritten);
        os_memset(dst->head, c, n);
        dst->head += n;
        nwritten += n;

        if (dst->head == bufend) {
            dst->head = dst->buf;
        }
    }

    if (overflow) {
        dst->tail = ringbuf_nextp(dst, dst->head);
        lwIP_ASSERT(ringbuf_is_full(dst));
    }

    return nwritten;
}

void* ringbuf_memcpy_into(ringbuf_t dst, const void* src, size_t count)
{
    const uint8_t* u8src = src;
    const uint8_t* bufend = ringbuf_end(dst);
    int overflow = count > ringbuf_bytes_free(dst);
    size_t nread = 0;

    while (nread != count) {
        lwIP_ASSERT(bufend > dst->head);
        size_t n = LWIP_MIN(bufend - dst->head, count - nread);
        memcpy(dst->head, u8src + nread, n);
        dst->head += n;
        nread += n;

        if (dst->head == bufend) {
            dst->head = dst->buf;
        }
    }

    if (overflow) {
        dst->tail = ringbuf_nextp(dst, dst->head);
        lwIP_ASSERT(ringbuf_is_full(dst));
    }

    return dst->head;
}

void* ringbuf_memcpy_from(void* dst, ringbuf_t src, size_t count)
{
    size_t bytes_used = ringbuf_bytes_used(src);

    if (count > bytes_used) {
        return NULL;
    }

    const uint8_t* u8dst = dst;
    const uint8_t* bufend = ringbuf_end(src);
    size_t nwritten = 0;

    while (nwritten != count) {
        lwIP_ASSERT(bufend > src->tail);
        size_t n = LWIP_MIN(bufend - src->tail, count - nwritten);
        memcpy((uint8_t*)u8dst + nwritten, src->tail, n);
        src->tail += n;
        nwritten += n;

        if (src->tail == bufend) {
            src->tail = src->buf;
        }
    }

    lwIP_ASSERT(count + ringbuf_bytes_used(src) == bytes_used);
    return src->tail;
}




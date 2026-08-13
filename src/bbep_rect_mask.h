// Pure C, no Arduino/FastEPD dependencies, so it can be exercised directly under a
// host unity build. Builds one packed 4bpp drive row (widthPx/4 bytes): 0 outside
// [startCol, endCol], val inside. High nibble is the leftmost pixel, matching the
// FastEPD 4bpp row layout every caller in FastEPD.inl already assumes.
//
// Shared by bbepClear (the constant clear-pass row) and bbepFullUpdate (the grey-pass
// AND mask), so the pixel set the two select is equal by construction rather than by
// two hand-maintained copies staying in sync.
#ifndef BBEP_RECT_MASK_H
#define BBEP_RECT_MASK_H

#include <stdint.h>
#include <string.h>

static inline void bbep_rect_mask_row(int startCol, int endCol, int widthPx, uint8_t val, uint8_t *out)
{
    int rowBytes = widthPx / 4;
    int startByte = startCol / 4;
    int endByte = endCol / 4;
    int b;

    memset(out, 0, rowBytes);
    for (b = startByte; b <= endByte; b++) {
        uint8_t mask = 0xff;
        // A rect narrower than 4 columns can land start and end in the same byte;
        // both edge masks apply to it, so they combine rather than one overwriting
        // the other.
        if (b == startByte && (startCol & 3) != 0) {
            mask &= (uint8_t)(0xff >> ((startCol & 3) * 2));
        }
        if (b == endByte && (endCol & 3) != 3) {
            mask &= (uint8_t)(0xff << ((3 - (endCol & 3)) * 2));
        }
        out[b] = val & mask;
    }
}

#endif // BBEP_RECT_MASK_H

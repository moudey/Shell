#pragma once
/*
 * stb_image_write.h
 *
 * Copyright (c) 2017 Sean Barrett
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

 //////////////////////////////////////////////////////////////////////////////
 //
 // PNG writer
 //

#ifndef STBIW_MALLOC
#define STBIW_MALLOC(sz)        malloc(sz)
#define STBIW_REALLOC(p,newsz)  realloc(p,newsz)
#define STBIW_FREE(p)           free(p)
#endif

#ifndef STBIW_REALLOC_SIZED
#define STBIW_REALLOC_SIZED(p,oldsz,newsz) STBIW_REALLOC(p,newsz)
#endif

#ifndef STBIW_MEMMOVE
#define STBIW_MEMMOVE(a,b,sz) memmove(a,b,sz)
#endif

#ifndef STBIW_ASSERT
#include <assert.h>
#define STBIW_ASSERT(x) assert(x)
#endif

#define STBIW_UCHAR(x) (unsigned char) ((x) & 0xff)

typedef unsigned int stbiw_uint32;

static int stbi__flip_vertically_on_write = 0;
static int stbi_write_force_png_filter = -1;
static int stbi_write_png_compression_level = 8;

#define stbiw__sbraw(a) ((int *) (void *) (a) - 2)
#define stbiw__sbm(a)   stbiw__sbraw(a)[0]
#define stbiw__sbn(a)   stbiw__sbraw(a)[1]

#define stbiw__sbneedgrow(a,n)  ((a)==0 || stbiw__sbn(a)+n >= stbiw__sbm(a))
#define stbiw__sbmaybegrow(a,n) (stbiw__sbneedgrow(a,(n)) ? stbiw__sbgrow(a,n) : 0)
#define stbiw__sbgrow(a,n)  stbiw__sbgrowf((void **) &(a), (n), sizeof(*(a)))

#define stbiw__sbpush(a, v)      (stbiw__sbmaybegrow(a,1), (a)[stbiw__sbn(a)++] = (v))
#define stbiw__sbcount(a)        ((a) ? stbiw__sbn(a) : 0)
#define stbiw__sbfree(a)         ((a) ? STBIW_FREE(stbiw__sbraw(a)),0 : 0)

static void *stbiw__sbgrowf(void **arr, int increment, int itemsize)
{
    int m = *arr ? 2 * stbiw__sbm(*arr) + increment : increment + 1;
    void *p = STBIW_REALLOC_SIZED(*arr ? stbiw__sbraw(*arr) : 0, *arr ? (stbiw__sbm(*arr) * itemsize + sizeof(int) * 2) : 0, itemsize * m + sizeof(int) * 2);
    STBIW_ASSERT(p);
    if(p) {
        if(!*arr) ((int *)p)[1] = 0;
        *arr = (void *)((int *)p + 2);
        stbiw__sbm(*arr) = m;
    }
    return *arr;
}

static unsigned char *stbiw__zlib_flushf(unsigned char *data, unsigned int *bitbuffer, int *bitcount)
{
    while(*bitcount >= 8) {
        stbiw__sbpush(data, STBIW_UCHAR(*bitbuffer));
        *bitbuffer >>= 8;
        *bitcount -= 8;
    }
    return data;
}

static int stbiw__zlib_bitrev(int code, int codebits)
{
    int res = 0;
    while(codebits--) {
        res = (res << 1) | (code & 1);
        code >>= 1;
    }
    return res;
}

static unsigned int stbiw__zlib_countm(unsigned char *a, unsigned char *b, int limit)
{
    int i;
    for(i = 0; i < limit && i < 258; ++i)
        if(a[i] != b[i]) break;
    return i;
}

static unsigned int stbiw__zhash(unsigned char *data)
{
    stbiw_uint32 hash = data[0] + (data[1] << 8) + (data[2] << 16);
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
    return hash;
}

#define stbiw__zlib_flush() (out = stbiw__zlib_flushf(out, &bitbuf, &bitcount))
#define stbiw__zlib_add(code,codebits) \
      (bitbuf |= (code) << bitcount, bitcount += (codebits), stbiw__zlib_flush())
#define stbiw__zlib_huffa(b,c)  stbiw__zlib_add(stbiw__zlib_bitrev(b,c),c)
// default huffman tables
#define stbiw__zlib_huff1(n)  stbiw__zlib_huffa(0x30 + (n), 8)
#define stbiw__zlib_huff2(n)  stbiw__zlib_huffa(0x190 + (n)-144, 9)
#define stbiw__zlib_huff3(n)  stbiw__zlib_huffa(0 + (n)-256,7)
#define stbiw__zlib_huff4(n)  stbiw__zlib_huffa(0xc0 + (n)-280,8)
#define stbiw__zlib_huff(n)  ((n) <= 143 ? stbiw__zlib_huff1(n) : (n) <= 255 ? stbiw__zlib_huff2(n) : (n) <= 279 ? stbiw__zlib_huff3(n) : stbiw__zlib_huff4(n))
#define stbiw__zlib_huffb(n) ((n) <= 143 ? stbiw__zlib_huff1(n) : stbiw__zlib_huff2(n))

#define stbiw__ZHASH   16384

static unsigned char *stbi_zlib_compress(unsigned char *data, int data_len, int *out_len, int quality)
{
    static unsigned short lengthc[] = { 3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258, 259 };
    static unsigned char  lengtheb[] = { 0,0,0,0,0,0,0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  5,  5,  5,  5,  0 };
    static unsigned short distc[] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577, 32768 };
    static unsigned char  disteb[] = { 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13 };
    unsigned int bitbuf = 0;
    int i, j, bitcount = 0;
    unsigned char *out = NULL;
    unsigned char ***hash_table = (unsigned char ***)STBIW_MALLOC(stbiw__ZHASH * sizeof(unsigned char **));
    if(hash_table == NULL)
        return NULL;
    if(quality < 5) quality = 5;

    stbiw__sbpush(out, 0x78);   // DEFLATE 32K window
    stbiw__sbpush(out, 0x5e);   // FLEVEL = 1
    stbiw__zlib_add(1, 1);  // BFINAL = 1
    stbiw__zlib_add(1, 2);  // BTYPE = 1 -- fixed huffman

    for(i = 0; i < stbiw__ZHASH; ++i)
        hash_table[i] = NULL;

    i = 0;
    while(i < data_len - 3) {
        // hash next 3 bytes of data to be compressed
        int h = stbiw__zhash(data + i) & (stbiw__ZHASH - 1), best = 3;
        unsigned char *bestloc = 0;
        unsigned char **hlist = hash_table[h];
        int n = stbiw__sbcount(hlist);
        for(j = 0; j < n; ++j) {
            if(hlist[j] - data > i - 32768) { // if entry lies within window
                int d = stbiw__zlib_countm(hlist[j], data + i, data_len - i);
                if(d >= best) { best = d; bestloc = hlist[j]; }
            }
        }
        // when hash table entry is too long, delete half the entries
        if(hash_table[h] && stbiw__sbn(hash_table[h]) == 2 * quality) {
            STBIW_MEMMOVE(hash_table[h], hash_table[h] + quality, sizeof(hash_table[h][0]) * quality);
            stbiw__sbn(hash_table[h]) = quality;
        }
        stbiw__sbpush(hash_table[h], data + i);

        if(bestloc) {
            // "lazy matching" - check match at *next* byte, and if it's better, do cur byte as literal
            h = stbiw__zhash(data + i + 1) & (stbiw__ZHASH - 1);
            hlist = hash_table[h];
            n = stbiw__sbcount(hlist);
            for(j = 0; j < n; ++j) {
                if(hlist[j] - data > i - 32767) {
                    int e = stbiw__zlib_countm(hlist[j], data + i + 1, data_len - i - 1);
                    if(e > best) { // if next match is better, bail on current match
                        bestloc = NULL;
                        break;
                    }
                }
            }
        }

        if(bestloc) {
            int d = (int)(data + i - bestloc); // distance back
            STBIW_ASSERT(d <= 32767 && best <= 258);
            for(j = 0; best > lengthc[j + 1] - 1; ++j);
            stbiw__zlib_huff(j + 257);
            if(lengtheb[j]) stbiw__zlib_add(best - lengthc[j], lengtheb[j]);
            for(j = 0; d > distc[j + 1] - 1; ++j);
            stbiw__zlib_add(stbiw__zlib_bitrev(j, 5), 5);
            if(disteb[j]) stbiw__zlib_add(d - distc[j], disteb[j]);
            i += best;
        }
        else {
            stbiw__zlib_huffb(data[i]);
            ++i;
        }
    }
    // write out final bytes
    for(; i < data_len; ++i)
        stbiw__zlib_huffb(data[i]);
    stbiw__zlib_huff(256); // end of block
    // pad with 0 bits to byte boundary
    while(bitcount)
        stbiw__zlib_add(0, 1);

    for(i = 0; i < stbiw__ZHASH; ++i)
        (void)stbiw__sbfree(hash_table[i]);
    STBIW_FREE(hash_table);

    // store uncompressed instead if compression was worse
    if(stbiw__sbn(out) > data_len + 2 + ((data_len + 32766) / 32767) * 5) {
        stbiw__sbn(out) = 2;  // truncate to DEFLATE 32K window and FLEVEL = 1
        for(j = 0; j < data_len;) {
            int blocklen = data_len - j;
            if(blocklen > 32767) blocklen = 32767;
            stbiw__sbpush(out, data_len - j == blocklen); // BFINAL = ?, BTYPE = 0 -- no compression
            stbiw__sbpush(out, STBIW_UCHAR(blocklen)); // LEN
            stbiw__sbpush(out, STBIW_UCHAR(blocklen >> 8));
            stbiw__sbpush(out, STBIW_UCHAR(~blocklen)); // NLEN
            stbiw__sbpush(out, STBIW_UCHAR(~blocklen >> 8));
            memcpy(out + stbiw__sbn(out), data + j, blocklen);
            stbiw__sbn(out) += blocklen;
            j += blocklen;
        }
    }

    {
        // compute adler32 on input
        unsigned int s1 = 1, s2 = 0;
        int blocklen = (int)(data_len % 5552);
        j = 0;
        while(j < data_len) {
            for(i = 0; i < blocklen; ++i) { s1 += data[j + i]; s2 += s1; }
            s1 %= 65521; s2 %= 65521;
            j += blocklen;
            blocklen = 5552;
        }
        stbiw__sbpush(out, STBIW_UCHAR(s2 >> 8));
        stbiw__sbpush(out, STBIW_UCHAR(s2));
        stbiw__sbpush(out, STBIW_UCHAR(s1 >> 8));
        stbiw__sbpush(out, STBIW_UCHAR(s1));
    }
    *out_len = stbiw__sbn(out);
    // make returned pointer freeable
    STBIW_MEMMOVE(stbiw__sbraw(out), out, *out_len);
    return (unsigned char *)stbiw__sbraw(out);
}

static unsigned int stbiw__crc32(unsigned char *buffer, int len)
{
    static unsigned int crc_table[256] =
    {
       0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
       0x0eDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
       0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
       0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
       0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
       0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
       0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
       0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
       0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
       0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
       0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
       0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
       0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
       0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
       0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
       0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
       0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
       0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
       0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
       0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
       0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
       0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
       0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
       0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
       0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
       0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
       0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
       0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
       0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
       0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
       0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
       0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };

    unsigned int crc = ~0u;
    int i;
    for(i = 0; i < len; ++i)
        crc = (crc >> 8) ^ crc_table[buffer[i] ^ (crc & 0xff)];
    return ~crc;
}

#define stbiw__wpng4(o,a,b,c,d) ((o)[0]=STBIW_UCHAR(a),(o)[1]=STBIW_UCHAR(b),(o)[2]=STBIW_UCHAR(c),(o)[3]=STBIW_UCHAR(d),(o)+=4)
#define stbiw__wp32(data,v) stbiw__wpng4(data, (v)>>24,(v)>>16,(v)>>8,(v));
#define stbiw__wptag(data,s) stbiw__wpng4(data, s[0],s[1],s[2],s[3])

static void stbiw__wpcrc(unsigned char **data, int len)
{
    unsigned int crc = stbiw__crc32(*data - len - 4, len + 4);
    stbiw__wp32(*data, crc);
}

static unsigned char stbiw__paeth(int a, int b, int c)
{
    int p = a + b - c, pa = abs(p - a), pb = abs(p - b), pc = abs(p - c);
    if(pa <= pb && pa <= pc) return STBIW_UCHAR(a);
    if(pb <= pc) return STBIW_UCHAR(b);
    return STBIW_UCHAR(c);
}

// @OPTIMIZE: provide an option that always forces left-predict or paeth predict
static void stbiw__encode_png_line(unsigned char *pixels, int stride_bytes, int width, int height, int y, int n, int filter_type, signed char *line_buffer)
{
    static int mapping[] = { 0,1,2,3,4 };
    static int firstmap[] = { 0,1,0,5,6 };
    int *mymap = (y != 0) ? mapping : firstmap;
    int i;
    int type = mymap[filter_type];
    unsigned char *z = pixels + stride_bytes * (stbi__flip_vertically_on_write ? height - 1 - y : y);
    int signed_stride = stbi__flip_vertically_on_write ? -stride_bytes : stride_bytes;

    if(type == 0) {
        memcpy(line_buffer, z, width * n);
        return;
    }

    // first loop isn't optimized since it's just one pixel
    for(i = 0; i < n; ++i) {
        switch(type) {
            case 1: line_buffer[i] = z[i]; break;
            case 2: line_buffer[i] = z[i] - z[i - signed_stride]; break;
            case 3: line_buffer[i] = z[i] - (z[i - signed_stride] >> 1); break;
            case 4: line_buffer[i] = (signed char)(z[i] - stbiw__paeth(0, z[i - signed_stride], 0)); break;
            case 5: line_buffer[i] = z[i]; break;
            case 6: line_buffer[i] = z[i]; break;
        }
    }
    switch(type) {
        case 1: for(i = n; i < width * n; ++i) line_buffer[i] = z[i] - z[i - n]; break;
        case 2: for(i = n; i < width * n; ++i) line_buffer[i] = z[i] - z[i - signed_stride]; break;
        case 3: for(i = n; i < width * n; ++i) line_buffer[i] = z[i] - ((z[i - n] + z[i - signed_stride]) >> 1); break;
        case 4: for(i = n; i < width * n; ++i) line_buffer[i] = z[i] - stbiw__paeth(z[i - n], z[i - signed_stride], z[i - signed_stride - n]); break;
        case 5: for(i = n; i < width * n; ++i) line_buffer[i] = z[i] - (z[i - n] >> 1); break;
        case 6: for(i = n; i < width * n; ++i) line_buffer[i] = z[i] - stbiw__paeth(z[i - n], 0, 0); break;
    }
}

static unsigned char *stbi_write_png_to_mem(const unsigned char *pixels, int stride_bytes, int x, int y, int n, int *out_len)
{
    int force_filter = stbi_write_force_png_filter;
    int ctype[5] = { -1, 0, 4, 2, 6 };
    unsigned char sig[8] = { 137,80,78,71,13,10,26,10 };
    unsigned char *out, *o, *filt, *zlib;
    signed char *line_buffer;
    int j, zlen;

    if(stride_bytes == 0)
        stride_bytes = x * n;

    if(force_filter >= 5) {
        force_filter = -1;
    }

    filt = (unsigned char *)STBIW_MALLOC((x * n + 1) * y); if(!filt) return 0;
    line_buffer = (signed char *)STBIW_MALLOC(x * n); if(!line_buffer) { STBIW_FREE(filt); return 0; }
    for(j = 0; j < y; ++j) {
        int filter_type;
        if(force_filter > -1) {
            filter_type = force_filter;
            stbiw__encode_png_line((unsigned char *)(pixels), stride_bytes, x, y, j, n, force_filter, line_buffer);
        }
        else { // Estimate the best filter by running through all of them:
            int best_filter = 0, best_filter_val = 0x7fffffff, est, i;
            for(filter_type = 0; filter_type < 5; filter_type++) {
                stbiw__encode_png_line((unsigned char *)(pixels), stride_bytes, x, y, j, n, filter_type, line_buffer);

                // Estimate the entropy of the line using this filter; the less, the better.
                est = 0;
                for(i = 0; i < x * n; ++i) {
                    est += abs((signed char)line_buffer[i]);
                }
                if(est < best_filter_val) {
                    best_filter_val = est;
                    best_filter = filter_type;
                }
            }
            if(filter_type != best_filter) {  // If the last iteration already got us the best filter, don't redo it
                stbiw__encode_png_line((unsigned char *)(pixels), stride_bytes, x, y, j, n, best_filter, line_buffer);
                filter_type = best_filter;
            }
        }
        // when we get here, filter_type contains the filter type, and line_buffer contains the data
        filt[j * (x * n + 1)] = (unsigned char)filter_type;
        STBIW_MEMMOVE(filt + j * (x * n + 1) + 1, line_buffer, x * n);
    }
    STBIW_FREE(line_buffer);
    zlib = stbi_zlib_compress(filt, y * (x * n + 1), &zlen, stbi_write_png_compression_level);
    STBIW_FREE(filt);
    if(!zlib) return 0;

    // each tag requires 12 bytes of overhead
    out = (unsigned char *)STBIW_MALLOC(8 + 12 + 13 + 12 + zlen + 12);
    if(!out) return 0;
    *out_len = 8 + 12 + 13 + 12 + zlen + 12;

    o = out;
    STBIW_MEMMOVE(o, sig, 8); o += 8;
    stbiw__wp32(o, 13); // header length
    stbiw__wptag(o, "IHDR");
    stbiw__wp32(o, x);
    stbiw__wp32(o, y);
    *o++ = 8;
    *o++ = STBIW_UCHAR(ctype[n]);
    *o++ = 0;
    *o++ = 0;
    *o++ = 0;
    stbiw__wpcrc(&o, 13);

    stbiw__wp32(o, zlen);
    stbiw__wptag(o, "IDAT");
    STBIW_MEMMOVE(o, zlib, zlen);
    o += zlen;
    STBIW_FREE(zlib);
    stbiw__wpcrc(&o, zlen);

    stbiw__wp32(o, 0);
    stbiw__wptag(o, "IEND");
    stbiw__wpcrc(&o, 0);

    STBIW_ASSERT(o == out + *out_len);

    return out;
}

int plutovg_stbi_write_png(const wchar_t *filename, int w, int h, const void *data)
{
    FILE *f{};
    int len;
    auto png = stbi_write_png_to_mem((const unsigned char *)data, 0, w, h, 4, &len);

    if(png == NULL) return 0;

    _wfopen_s(&f, filename, L"wb");
    if(!f) { STBIW_FREE(png); return 0; }
    fwrite(png, 1, len, f);
    fclose(f);
    STBIW_FREE(png);
    return 1;
}

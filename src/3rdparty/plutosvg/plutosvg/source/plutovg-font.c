#define STB_TRUETYPE_IMPLEMENTATION
#include "plutovg-private.h"
#include "stb_truetype.h"

#include <stdio.h>

struct plutovg_font_data_t {
    int ref;
    const unsigned char* data;
    int datasize;
    int owndata;
};

plutovg_font_data_t* plutovg_font_data_load_from_memory(const unsigned char* data, int datasize, int owndata)
{
    plutovg_font_data_t* fontdata = malloc(sizeof(plutovg_font_data_t));
    if(fontdata)
    {
        fontdata->ref = 1;
        fontdata->data = data;
        fontdata->datasize = datasize;
        fontdata->owndata = owndata;
    }
    return fontdata;
}

plutovg_font_data_t* plutovg_font_data_load_from_file(const char* filename)
{
    FILE* fp = NULL;
    fopen_s(&fp, filename, "rb");
    if(fp == NULL)
        return NULL;

    fseek(fp, 0, SEEK_END);
    size_t size = (size_t)ftell(fp);
    unsigned char* data = malloc(size);
    fseek(fp, 0, SEEK_SET);

    fread(data, 1, size, fp);
    fclose(fp);

    return plutovg_font_data_load_from_memory(data, (int)size, 1);
}

plutovg_font_data_t* plutovg_font_data_reference(plutovg_font_data_t* data)
{
    if(data == NULL)
        return NULL;

    ++data->ref;
    return data;
}

void plutovg_font_data_destroy(plutovg_font_data_t* data)
{
    if(data == NULL)
        return;

    if(--data->ref == 0)
    {
        if(data->owndata)
            free((void*)(data->data));
        free(data);
    }
}

int plutovg_font_data_get_reference_count(const plutovg_font_data_t* data)
{
    if(data == NULL)
        return 0;

    return data->ref;
}

int plutovg_font_data_face_count(const plutovg_font_data_t* data)
{
    return stbtt_GetNumberOfFonts(data->data);
}

#define CACHE_SIZE 256
typedef struct {
    int codepoint;
    int advance;
    int x1, y1, x2, y2;
    plutovg_path_t* path;
} plutovg_glyph_t;

struct plutovg_font_face {
    int ref;
    int ascent, descent, linegap;
    int x1, y1, x2, y2;
    stbtt_fontinfo info;
    plutovg_font_data_t* data;
    plutovg_glyph_t cache[CACHE_SIZE];
};

plutovg_font_face_t* plutovg_font_face_load_from_memory(const unsigned char* data, int datasize, int owndata)
{
    plutovg_font_data_t* fontdata = plutovg_font_data_load_from_memory(data, datasize, owndata);
    plutovg_font_face_t* fontface = plutovg_font_face_load_from_data(fontdata, 0);
    plutovg_font_data_destroy(fontdata);
    return fontface;
}

plutovg_font_face_t* plutovg_font_face_load_from_file(const char* filename)
{
    plutovg_font_data_t* fontdata = plutovg_font_data_load_from_file(filename);
    plutovg_font_face_t* fontface = plutovg_font_face_load_from_data(fontdata, 0);
    plutovg_font_data_destroy(fontdata);
    return fontface;
}

plutovg_font_face_t* plutovg_font_face_load_from_data(plutovg_font_data_t* data, int index)
{
    if(data == NULL)
        return NULL;

    stbtt_fontinfo info;
    if(!stbtt_InitFont(&info, data->data, index))
        return NULL;

    plutovg_font_face_t* fontface = malloc(sizeof(plutovg_font_face_t));
    fontface->ref = 1;
    fontface->data = plutovg_font_data_reference(data);
    fontface->info = info;
    stbtt_GetFontVMetrics(&fontface->info, &fontface->ascent, &fontface->descent, &fontface->linegap);
    stbtt_GetFontBoundingBox(&fontface->info, &fontface->x1, &fontface->y1, &fontface->x2, &fontface->y2);
    memset(fontface->cache, 0, sizeof(fontface->cache));
    return fontface;
}

plutovg_font_face_t* plutovg_font_face_reference(plutovg_font_face_t* face)
{
    if(face == NULL)
        return NULL;

    ++face->ref;
    return face;
}

void plutovg_font_face_destroy(plutovg_font_face_t* face)
{
    if(face == NULL)
        return;

    if(--face->ref == 0)
    {
        for(int i = 0;i < CACHE_SIZE;i++)
        {
            plutovg_glyph_t* glyph = &face->cache[i];
            plutovg_path_destroy(glyph->path);
        }

        plutovg_font_data_destroy(face->data);
        free(face);
    }
}

int plutovg_font_face_get_reference_count(const plutovg_font_face_t* face)
{
    if(face == NULL)
        return 0;

    return face->ref;
}

plutovg_font_style_t plutovg_font_face_get_style(const plutovg_font_face_t* face)
{
    (face);
    return plutovg_font_style_normal;
}

const char* plutovg_font_face_get_family(const plutovg_font_face_t* face)
{
    (face);
    return NULL;
}

static inline plutovg_glyph_t *find_glyph(const plutovg_font_face_t *face, int codepoint)
{
    plutovg_glyph_t *glyph = (void *)(&face->cache[codepoint % CACHE_SIZE]);
    if(glyph->codepoint == codepoint)
        return glyph;

    glyph->codepoint = codepoint;
    if(glyph->path == NULL)
        glyph->path = plutovg_path_create();
    else
        plutovg_path_clear(glyph->path);

    int index = stbtt_FindGlyphIndex(&face->info, codepoint);
    stbtt_vertex *v;
    int num_vertices = stbtt_GetGlyphShape(&face->info, index, &v);
    for(int i = 0; i < num_vertices; i++)
    {
        switch(v[i].type) {
            case STBTT_vmove:
                plutovg_path_move_to(glyph->path, v[i].x, v[i].y);
                break;
            case STBTT_vline:
                plutovg_path_line_to(glyph->path, v[i].x, v[i].y);
                break;
            case STBTT_vcurve:
                plutovg_path_quad_to(glyph->path, v[i].cx, v[i].cy, v[i].x, v[i].y);
                break;
            case STBTT_vcubic:
                plutovg_path_cubic_to(glyph->path, v[i].cx, v[i].cy, v[i].cx1, v[i].cy1, v[i].x, v[i].y);
                break;
        }
    }

    stbtt_FreeShape(&face->info, v);
    stbtt_GetGlyphHMetrics(&face->info, index, &glyph->advance, NULL);
    stbtt_GetGlyphBox(&face->info, index, &glyph->x1, &glyph->y1, &glyph->x2, &glyph->y2);
    return glyph;
}

plutovg_path_t* plutovg_font_face_get_char_path(const plutovg_font_face_t* face, int ch)
{
    plutovg_glyph_t* glyph = find_glyph(face, ch);
    return plutovg_path_reference(glyph->path);
}

void plutovg_font_face_get_char_extents(const plutovg_font_face_t* face, int ch, plutovg_rect_t* rect)
{
    plutovg_glyph_t* glyph = find_glyph(face, ch);
    plutovg_rect_init(rect, glyph->x1, glyph->y1, glyph->x2 - glyph->x1, glyph->y2 - glyph->y1);
}

double plutovg_font_face_get_char_advance(const plutovg_font_face_t* face, int ch)
{
    plutovg_glyph_t* glyph = find_glyph(face, ch);
    return glyph->advance;
}

void plutovg_font_face_get_matrix(const plutovg_font_face_t* face, double size, plutovg_matrix_t* matrix)
{
    double scale = plutovg_font_face_get_scale(face, size);
    plutovg_matrix_init_scale(matrix, scale, -scale);
}

double plutovg_font_face_get_scale(const plutovg_font_face_t* face, double size)
{
    return stbtt_ScaleForMappingEmToPixels(&face->info, (float)size);
}

double plutovg_font_face_get_ascent(const plutovg_font_face_t* face)
{
    return face->ascent;
}

double plutovg_font_face_get_descent(const plutovg_font_face_t* face)
{
    return face->descent;
}

double plutovg_font_face_get_line_gap(const plutovg_font_face_t* face)
{
    return face->linegap;
}

double plutovg_font_face_get_leading(const plutovg_font_face_t* face)
{
    return (face->ascent - face->descent + face->linegap);
}

double plutovg_font_face_get_kerning(const plutovg_font_face_t* face, int ch1, int ch2)
{
    return stbtt_GetCodepointKernAdvance(&face->info, ch1, ch2);
}

void plutovg_font_face_get_extents(const plutovg_font_face_t* face, plutovg_rect_t* rect)
{
    plutovg_rect_init(rect, face->x1, face->y1, face->x2 - face->x1, face->y2 - face->y1);
}

struct plutovg_font {
    int ref;
    double size;
    plutovg_font_face_t* face;
};

plutovg_font_t* plutovg_font_load_from_memory(const unsigned char* data, int datasize, int owndata, double size)
{
    plutovg_font_face_t* fontface = plutovg_font_face_load_from_memory(data, datasize, owndata);
    plutovg_font_t* font = plutovg_font_load_from_face(fontface, size);
    plutovg_font_face_destroy(fontface);
    return font;
}

plutovg_font_t* plutovg_font_load_from_file(const char* filename, double size)
{
    plutovg_font_face_t* fontface = plutovg_font_face_load_from_file(filename);
    plutovg_font_t* font = plutovg_font_load_from_face(fontface, size);
    plutovg_font_face_destroy(fontface);
    return font;
}

plutovg_font_t* plutovg_font_load_from_data(plutovg_font_data_t* data, int index, double size)
{
    plutovg_font_face_t* fontface = plutovg_font_face_load_from_data(data, index);
    plutovg_font_t* font = plutovg_font_load_from_face(fontface, size);
    plutovg_font_face_destroy(fontface);
    return font;
}

plutovg_font_t* plutovg_font_load_from_face(plutovg_font_face_t* face, double size)
{
    if(face == NULL)
        return NULL;

    plutovg_font_t* font = malloc(sizeof(plutovg_font_t));
    font->ref = 1;
    font->size = size;
    font->face = plutovg_font_face_reference(face);
    return font;
}

plutovg_font_t* plutovg_font_reference(plutovg_font_t* font)
{
    if(font == NULL)
        return NULL;

    ++font->ref;
    return font;
}

void plutovg_font_destroy(plutovg_font_t* font)
{
    if(font == NULL)
        return;

    if(--font->ref == 0)
    {
        plutovg_font_face_destroy(font->face);
        free(font);
    }
}

int plutovg_font_get_reference_count(const plutovg_font_t* font)
{
    if(font == NULL)
        return 0;

    return font->ref;
}

plutovg_font_face_t* plutovg_font_get_face(const plutovg_font_t* font)
{
    return font->face;
}

void plutovg_font_set_face(plutovg_font_t* font, plutovg_font_face_t* face)
{
    face = plutovg_font_face_reference(face);
    plutovg_font_face_destroy(font->face);
    font->face = face;
}

void plutovg_font_set_size(plutovg_font_t* font, double size)
{
    font->size = size;
}

double plutovg_font_get_size(const plutovg_font_t* font)
{
    return font->size;
}

plutovg_font_style_t plutovg_font_get_style(const plutovg_font_t* font)
{
    return plutovg_font_face_get_style(font->face);
}

const char* plutovg_font_get_family(const plutovg_font_t* font)
{
    return plutovg_font_face_get_family(font->face);
}

double plutovg_font_get_scale(const plutovg_font_t* font)
{
    return plutovg_font_face_get_scale(font->face, font->size);
}

double plutovg_font_get_ascent(const plutovg_font_t* font)
{
    double scale = plutovg_font_get_scale(font);
    return plutovg_font_face_get_ascent(font->face) * scale;
}

double plutovg_font_get_descent(const plutovg_font_t* font)
{
    double scale = plutovg_font_get_scale(font);
    return plutovg_font_face_get_descent(font->face) * scale;
}

double plutovg_font_get_line_gap(const plutovg_font_t* font)
{
    double scale = plutovg_font_get_scale(font);
    return plutovg_font_face_get_line_gap(font->face) * scale;
}

double plutovg_font_get_leading(const plutovg_font_t* font)
{
    double scale = plutovg_font_get_scale(font);
    return plutovg_font_face_get_leading(font->face) * scale;
}

double plutovg_font_get_kerning(const plutovg_font_t* font, int ch1, int ch2)
{
    double scale = plutovg_font_get_scale(font);
    return plutovg_font_face_get_kerning(font->face, ch1, ch2) * scale;
}

static inline int decode_utf8(const char** begin, const char* end, int* codepoint)
{
    static const int trailing[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
    };

    static const uint32_t offsets[6] = {
        0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080
    };

    const char* ptr = *begin;
    int trailing_bytes = trailing[(uint8_t)(*ptr)];
    if(ptr + trailing_bytes >= end)
        return 0;

    uint32_t output = 0;
    switch(trailing_bytes) {
    case 5: output += (uint8_t)(*ptr++); output <<= 6;
    case 4: output += (uint8_t)(*ptr++); output <<= 6;
    case 3: output += (uint8_t)(*ptr++); output <<= 6;
    case 2: output += (uint8_t)(*ptr++); output <<= 6;
    case 1: output += (uint8_t)(*ptr++); output <<= 6;
    case 0: output += (uint8_t)(*ptr++);
    }

    output -= offsets[trailing_bytes];
    *begin = ptr;
    *codepoint = output;
    return 1;
}

double plutovg_font_get_char_advance(const plutovg_font_t* font, int ch)
{
    double scale = plutovg_font_get_scale(font);
    return plutovg_font_face_get_char_advance(font->face, ch) * scale;
}

double plutovg_font_get_text_advance(const plutovg_font_t* font, const char* utf8)
{
    return plutovg_font_get_textn_advance(font, utf8, (int)strlen(utf8));
}

double plutovg_font_get_textn_advance(const plutovg_font_t* font, const char* utf8, int size)
{
    double advance = 0;
    const char* end = utf8 + size;
    while(utf8 < end)
    {
        int ch = 0;
        if(!decode_utf8(&utf8, end, &ch))
            break;

        advance += plutovg_font_get_char_advance(font, ch);
    }

    return advance;
}

void plutovg_font_get_char_extents(const plutovg_font_t* font, int ch, plutovg_rect_t* rect)
{
    plutovg_matrix_t matrix;
    plutovg_font_face_get_matrix(font->face, font->size, &matrix);
    plutovg_font_face_get_char_extents(font->face, ch, rect);
    plutovg_matrix_map_rect(&matrix, rect, rect);
}

void plutovg_font_get_text_extents(const plutovg_font_t* font, const char* utf8, plutovg_rect_t* rect)
{
    plutovg_font_get_textn_extents(font, utf8, (int)strlen(utf8), rect);
}

void plutovg_font_get_textn_extents(const plutovg_font_t* font, const char* utf8, int size, plutovg_rect_t* rect)
{
    plutovg_rect_init_invalid(rect);
    double advance = 0;
    double scale = plutovg_font_get_scale(font);
    const char* end = utf8 + size;
    while(utf8 < end)
    {
        int ch = 0;
        if(!decode_utf8(&utf8, end, &ch))
            break;

        plutovg_matrix_t matrix;
        plutovg_matrix_init_translate(&matrix, advance, 0);
        plutovg_matrix_scale(&matrix, scale, -scale);

        plutovg_rect_t box;
        plutovg_font_face_get_char_extents(font->face, ch, &box);
        plutovg_matrix_map_rect(&matrix, &box, &box);
        plutovg_rect_unite(rect, &box);
        advance += plutovg_font_get_char_advance(font, ch);
    }
}

void plutovg_font_get_extents(const plutovg_font_t* font, plutovg_rect_t* rect)
{
    plutovg_matrix_t matrix;
    plutovg_font_face_get_matrix(font->face, font->size, &matrix);
    plutovg_font_face_get_extents(font->face, rect);
    plutovg_matrix_map_rect(&matrix, rect, rect);
}

plutovg_path_t* plutovg_font_get_char_path(const plutovg_font_t* font, int ch)
{
    plutovg_matrix_t matrix;
    plutovg_font_face_get_matrix(font->face, font->size, &matrix);
    plutovg_path_t* result = plutovg_path_create();
    plutovg_path_t* path = plutovg_font_face_get_char_path(font->face, ch);
    plutovg_path_add_path(result, path, &matrix);
    plutovg_path_destroy(path);
    return result;
}

plutovg_path_t* plutovg_font_get_text_path(const plutovg_font_t* font, const char* utf8)
{
    return plutovg_font_get_textn_path(font, utf8, (int)strlen(utf8));
}

plutovg_path_t* plutovg_font_get_textn_path(const plutovg_font_t* font, const char* utf8, int size)
{
    plutovg_path_t* result = plutovg_path_create();
    double advance = 0;
    double scale = plutovg_font_get_scale(font);
    const char* end = utf8 + size;
    while(utf8 < end)
    {
        int ch = 0;
        if(!decode_utf8(&utf8, end, &ch))
            break;

        plutovg_matrix_t matrix;
        plutovg_matrix_init_translate(&matrix, advance, 0);
        plutovg_matrix_scale(&matrix, scale, -scale);

        plutovg_path_t* path = plutovg_font_face_get_char_path(font->face, ch);
        plutovg_path_add_path(result, path, &matrix);
        plutovg_path_destroy(path);
        advance += plutovg_font_get_char_advance(font, ch);
    }

    return result;
}

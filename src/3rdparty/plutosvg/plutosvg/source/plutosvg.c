#include "plutosvg.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <limits.h>

#define IS_ALPHA(c) (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
#define IS_NUM(c) (c >= '0' && c <= '9')
#define IS_WS(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r')

static inline int skip_string(const char** begin, const char* end, const char* str)
{
    const char* ptr = *begin;
    while(ptr < end && *str && *ptr == *str)
    {
        ++ptr;
        ++str;
    }

    if(*str)
        return 0;

    *begin = ptr;
    return 1;
}

static inline int string_eq(const char* ptr, const char* end, const char* str)
{
    if(!skip_string(&ptr, end, str))
        return 0;

    return ptr == end;
}

static const char* string_find(const char* ptr, const char* end, const char* str)
{
    while(ptr < end)
    {
        const char* start = ptr;
        if(skip_string(&ptr, end, str))
            return start;
        ++ptr;
    }

    return NULL;
}

static inline int skip_ws(const char** begin, const char* end)
{
    const char* ptr = *begin;
    while(ptr < end && IS_WS(*ptr))
        ++ptr;

    *begin = ptr;
    return ptr < end;
}

static inline const char* rtrim(const char* start, const char* end)
{
    while(end > start && IS_WS(end[-1]))
        --end;

    return end;
}

static inline const char* ltrim(const char* start, const char* end)
{
    while(start < end && IS_WS(*start))
        ++start;

    return start;
}

static inline int skip_ws_delimiter(const char** begin, const char* end, char delimiter)
{
    const char* ptr = *begin;
    if(ptr < end && !IS_WS(*ptr) && *ptr != delimiter)
        return 0;

    if(skip_ws(&ptr, end))
    {
        if(ptr < end && *ptr == delimiter)
        {
            ++ptr;
            skip_ws(&ptr, end);
        }
    }

    *begin = ptr;
    return ptr < end;
}

static inline int skip_ws_comma(const char** begin, const char* end)
{
    return skip_ws_delimiter(begin, end, ',');
}

enum {
    ID_UNKNOWN = 0,
    ID_COLOR,
    ID_DISPLAY,
    ID_FILL,
    ID_FILL_OPACITY,
    ID_FILL_RULE,
    ID_FONT_SIZE,
    ID_OPACITY,
    ID_SOLID_COLOR,
    ID_SOLID_OPACITY,
    ID_STOP_COLOR,
    ID_STOP_OPACITY,
    ID_STROKE,
    ID_STROKE_LINECAP,
    ID_STROKE_LINEJOIN,
    ID_STROKE_MITERLIMIT,
    ID_STROKE_OPACITY,
    ID_STROKE_WIDTH,
    ID_TEXT_ANCHOR,
    ID_VISIBILITY,
    MAX_STYLE,
    ID_CX,
    ID_CY,
    ID_D,
    ID_FX,
    ID_FY,
    ID_GRADIENT_TRANSFORM,
    ID_GRADIENT_UNITS,
    ID_HEIGHT,
    ID_ID,
    ID_OFFSET,
    ID_POINTS,
    ID_PRESERVE_ASPECT_RATIO,
    ID_R,
    ID_RX,
    ID_RY,
    ID_SPREAD_METHOD,
    ID_STYLE,
    ID_TRANSFORM,
    ID_VIEW_BOX,
    ID_WIDTH,
    ID_X,
    ID_X1,
    ID_X2,
    ID_XLINK_HREF,
    ID_Y,
    ID_Y1,
    ID_Y2,
    MAX_PROPERTY
};

static const struct {
    int id;
    const char* name;
} propertymap[] = {
    {ID_UNKNOWN, ""},
    {ID_COLOR, "color"},
    {ID_DISPLAY, "display"},
    {ID_FILL, "fill"},
    {ID_FILL_OPACITY, "fill-opacity"},
    {ID_FILL_RULE, "fill-rule"},
    {ID_FONT_SIZE, "font-size"},
    {ID_OPACITY, "opacity"},
    {ID_SOLID_COLOR, "solid-color"},
    {ID_SOLID_OPACITY, "solid-opacity"},
    {ID_STOP_COLOR, "stop-color"},
    {ID_STOP_OPACITY, "stop-opacity"},
    {ID_STROKE, "stroke"},
    {ID_STROKE_LINECAP, "stroke-linecap"},
    {ID_STROKE_LINEJOIN, "stroke-linejoin"},
    {ID_STROKE_MITERLIMIT, "stroke-miterlimit"},
    {ID_STROKE_OPACITY, "stroke-opacity"},
    {ID_STROKE_WIDTH, "stroke-width"},
    {ID_TEXT_ANCHOR, "text-anchor"},
    {ID_VISIBILITY, "visibility"},
    {MAX_STYLE, ""},
    {ID_CX, "cx"},
    {ID_CY, "cy"},
    {ID_D, "d"},
    {ID_FX, "fx"},
    {ID_FY, "fy"},
    {ID_GRADIENT_TRANSFORM, "gradientTransform"},
    {ID_GRADIENT_UNITS, "gradientUnits"},
    {ID_HEIGHT, "height"},
    {ID_ID, "id"},
    {ID_OFFSET, "offset"},
    {ID_POINTS, "points"},
    {ID_PRESERVE_ASPECT_RATIO, "preserveAspectRatio"},
    {ID_R, "r"},
    {ID_RX, "rx"},
    {ID_RY, "ry"},
    {ID_SPREAD_METHOD, "spreadMethod"},
    {ID_STYLE, "style"},
    {ID_TRANSFORM, "transform"},
    {ID_VIEW_BOX, "viewBox"},
    {ID_WIDTH, "width"},
    {ID_X, "x"},
    {ID_X1, "x1"},
    {ID_X2, "x2"},
    {ID_XLINK_HREF, "xlink:href"},
    {ID_Y, "y"},
    {ID_Y1, "y1"},
    {ID_Y2, "y2"}
};

static int csspropertyid(const char* ptr, const char* end)
{
    if(ptr >= end)
        return ID_UNKNOWN;

    for(int i = 0;i < MAX_STYLE;i++)
        if(string_eq(ptr, end, propertymap[i].name))
            return propertymap[i].id;
    return ID_UNKNOWN;
}

static int dompropertyid(const char* ptr, const char* end)
{
    if(ptr >= end)
        return ID_UNKNOWN;

    for(int i = MAX_STYLE;i < MAX_PROPERTY;i++)
        if(string_eq(ptr, end, propertymap[i].name))
            return propertymap[i].id;
    return csspropertyid(ptr, end);
}

enum {
    TAG_UNKNOWN = 0,
    TAG_CIRCLE,
    TAG_DEFS,
    TAG_ELLIPSE,
    TAG_G,
    TAG_LINE,
    TAG_LINEAR_GRADIENT,
    TAG_PATH,
    TAG_POLYGON,
    TAG_POLYLINE,
    TAG_RADIAL_GRADIENT,
    TAG_RECT,
    TAG_SOLID_COLOR,
    TAG_STOP,
    TAG_SVG,
    TAG_SYMBOL,
    TAG_TEXT,
    TAG_TSPAN,
    TAG_USE,
    MAX_ELEMENT
};

static const struct {
    int id;
    const char* name;
} elementmap[] = {
    {TAG_UNKNOWN, ""},
    {TAG_CIRCLE, "circle"},
    {TAG_DEFS, "defs"},
    {TAG_ELLIPSE, "ellipse"},
    {TAG_G, "g"},
    {TAG_LINE, "line"},
    {TAG_LINEAR_GRADIENT, "linearGradient"},
    {TAG_PATH, "path"},
    {TAG_POLYGON, "polygon"},
    {TAG_POLYLINE, "polyline"},
    {TAG_RADIAL_GRADIENT, "radialGradient"},
    {TAG_RECT, "rect"},
    {TAG_SOLID_COLOR, "solidColor"},
    {TAG_STOP, "stop"},
    {TAG_SVG, "svg"},
    {TAG_SYMBOL, "symbol"},
    {TAG_TEXT, "text"},
    {TAG_TSPAN, "tspan"},
    {TAG_USE, "use"}
};

static int domelementid(const char* ptr, const char* end)
{
    if(ptr >= end)
        return ID_UNKNOWN;

    for(int i = 0;i < MAX_ELEMENT;i++)
        if(string_eq(ptr, end, elementmap[i].name))
            return elementmap[i].id;
    return TAG_UNKNOWN;
}

typedef struct {
    const char* start;
    const char* end;
} string_t;

#define string_init(string, a, b) \
    do { \
    string.start = a; \
    string.end = b; \
    } while(0)

typedef struct property {
    int id;
    string_t value;
    struct property* next;
} property_t;

typedef struct element {
    int id;
    struct element* parent;
    struct element* next;
    struct element* last;
    struct element* first;
    struct property* property;
} element_t;

#define CHUNK_SIZE 4096
typedef struct _heap_chunk {
    struct _heap_chunk* next;
} heap_chunk_t;

typedef struct {
    heap_chunk_t* chunk;
    size_t size;
} heap_t;

static heap_t* heap_create(void)
{
    heap_t* heap = malloc(sizeof(heap_t));
    heap->chunk = NULL;
    heap->size = 0;
    return heap;
}

static void* heap_alloc(heap_t* heap, size_t size)
{
    size = (size + 7ul) & ~7ul;
    if(heap->chunk == NULL || heap->size + size > CHUNK_SIZE)
    {
        heap_chunk_t* chunk = malloc(sizeof(heap_chunk_t) + CHUNK_SIZE);
        chunk->next = heap->chunk;
        heap->chunk = chunk;
        heap->size = 0;
    }

    void* data = (char*)(heap->chunk) + sizeof(heap_chunk_t) + heap->size;
    heap->size += size;
    return data;
}

static void heap_destroy(heap_t* heap)
{
    heap_chunk_t* chunk = heap->chunk;
    while(chunk)
    {
        heap_chunk_t* n = chunk->next;
        free(chunk);
        chunk = n;
    }

    free(heap);
}

typedef struct hashmap_entry {
    int hash;
    string_t key;
    void* value;
    struct hashmap_entry* next;
} hashmap_entry_t;

typedef struct {
    hashmap_entry_t** buckets;
    int size;
    int capacity;
} hashmap_t;

static hashmap_t* hashmap_create(void)
{
    hashmap_t* map = malloc(sizeof(hashmap_t));
    map->buckets = calloc(16, sizeof(hashmap_entry_t*));
    map->size = 0;
    map->capacity = 16;
    return map;
}

static int hashmap_hash(const char* ptr, const char* end)
{
    int size = (int)(end - ptr);
    int h = size;
    for(int i = 0;i < size;i++)
    {
        h = h * 31 + *ptr;
        ++ptr;
    }

    return h;
}

static int hashmap_eq(const hashmap_entry_t* entry, const char* a, const char* a_end)
{
    const string_t* key = &entry->key;
    const char* b = key->start;
    const char* b_end = key->end;

    int a_size = (int)(a_end - a);
    int b_size = (int)(b_end - b);
    if(a_size != b_size)
        return 0;

    for(int i = 0;i < a_size;i++)
    {
        if(a[i] != b[i])
            return 0;
    }

    return 1;
}

static void hashmap_expand(hashmap_t* map)
{
    if(map->size > (map->capacity * 3 / 4))
    {
        int newcapacity = map->capacity << 1;
        hashmap_entry_t** newbuckets = calloc((size_t)newcapacity, sizeof(hashmap_entry_t*));

        for(int i = 0;i < map->capacity;i++)
        {
            hashmap_entry_t* entry = map->buckets[i];
            while(entry)
            {
                hashmap_entry_t* next = entry->next;
                size_t index = (size_t)(entry->hash) & (size_t)(newcapacity - 1);
                entry->next = newbuckets[index];
                newbuckets[index] = entry;
                entry = next;
            }
        }

        free(map->buckets);
        map->buckets = newbuckets;
        map->capacity = newcapacity;
    }
}

static void hashmap_put(hashmap_t* map, heap_t* heap, const char* ptr, const char* end, void* value)
{
    int hash = hashmap_hash(ptr, end);
    size_t index = (size_t)(hash) & (size_t)(map->capacity - 1);

    hashmap_entry_t** p = &map->buckets[index];
    while(1)
    {
        hashmap_entry_t* current = *p;
        if(current == NULL)
        {
            hashmap_entry_t* entry = heap_alloc(heap, sizeof(hashmap_entry_t));
            string_init(entry->key, ptr, end);
            entry->hash = hash;
            entry->value = value;
            entry->next = NULL;
            *p = entry;
            map->size += 1;
            hashmap_expand(map);
            break;
        }

        if(current->hash==hash && hashmap_eq(current, ptr, end))
        {
            current->value = value;
            break;
        }

        p = &current->next;
    }
}

static void* hashmap_get(hashmap_t* map, const char* ptr, const char* end)
{
    int hash = hashmap_hash(ptr, end);
    size_t index = (size_t)(hash) & (size_t)(map->capacity - 1);

    hashmap_entry_t* entry = map->buckets[index];
    while(entry)
    {
        if(entry->hash==hash && hashmap_eq(entry, ptr, end))
            return entry->value;
        entry = entry->next;
    }

    return NULL;
}

static void hashmap_destroy(hashmap_t* map)
{
    free(map->buckets);
    free(map);
}

typedef struct {
    element_t* root;
    hashmap_t* cache;
    heap_t* heap;
} document_t;

#define IS_CSS_STARTNAMECHAR(c) (IS_ALPHA(c) || c == '_')
#define IS_CSS_NAMECHAR(c) (IS_CSS_STARTNAMECHAR(c) || IS_NUM(c) || c == '-')
static void parse_property(element_t* element, heap_t* heap, int id, const char* ptr, const char* end)
{
    if(id < MAX_STYLE && string_eq(ptr, end, "inherit"))
        return;

    if(id == ID_STYLE)
    {
        while(ptr < end && IS_CSS_STARTNAMECHAR(*ptr))
        {
            const char* start = ptr;
            ++ptr;
            while(ptr < end && IS_CSS_NAMECHAR(*ptr))
                ++ptr;
            int id2 = csspropertyid(start, ptr);
            skip_ws(&ptr, end);
            if(ptr >= end || *ptr!=':')
                return;
            ++ptr;
            skip_ws(&ptr, end);
            start = ptr;
            while(ptr < end && *ptr!=';')
                ++ptr;
            if(id2 != ID_UNKNOWN)
                parse_property(element, heap, id2, start, ptr);
            skip_ws_delimiter(&ptr, end, ';');
        }
    }

    if(ptr >= end)
        return;

    property_t* property = heap_alloc(heap, sizeof(property_t));
    property->id = id;
    string_init(property->value, ptr, end);
    property->next = element->property;
    element->property = property;
}

#define IS_STARTNAMECHAR(c) (IS_ALPHA(c) ||  c == '_' || c == ':')
#define IS_NAMECHAR(c) (IS_STARTNAMECHAR(c) || IS_NUM(c) || c == '-' || c == '.')
static int parse_attributes(const char** begin, const char* end, element_t* element, hashmap_t* cache, heap_t* heap)
{
    const char* ptr = *begin;
    while(ptr < end && IS_STARTNAMECHAR(*ptr))
    {
        const char* start = ptr;
        ++ptr;
        while(ptr < end && IS_NAMECHAR(*ptr))
            ++ptr;

        int id = dompropertyid(start, ptr);
        skip_ws(&ptr, end);
        if(ptr >= end || *ptr!='=')
            return 0;
        ++ptr;

        skip_ws(&ptr, end);
        if(ptr >= end || (*ptr!='"' && *ptr!='\''))
            return 0;

        const char quote = *ptr;
        ++ptr;
        skip_ws(&ptr, end);
        start = ptr;
        while(ptr < end && *ptr!=quote)
            ++ptr;
        if(ptr >= end || *ptr!=quote)
            return 0;
        if(id && element)
        {
            if(id == ID_ID)
                hashmap_put(cache, heap, start, ptr, element);
            else
                parse_property(element, heap, id, start, ptr);
        }

        ++ptr;
        skip_ws(&ptr, end);
    }

    *begin = ptr;
    return 1;
}

static document_t* parse_document(const char* data, int size)
{
    const char* ptr = data;
    const char* end = ptr + size;

    heap_t* heap = heap_create();
    hashmap_t* cache = hashmap_create();
    element_t* root = NULL;
    element_t* current = NULL;
    int ignore = 0;
    while(ptr < end)
    {
        while(ptr < end && *ptr != '<')
            ++ptr;

        if(ptr >= end || *ptr != '<')
            break;

        ++ptr;
        if(ptr < end && *ptr == '/')
        {
            ++ptr;
            if(ptr >= end || !IS_STARTNAMECHAR(*ptr))
                break;

            ++ptr;
            while(ptr < end && IS_NAMECHAR(*ptr))
                ++ptr;

            skip_ws(&ptr, end);
            if(ptr >= end || *ptr != '>')
                break;

            if(ignore > 0)
                ignore--;
            else if(current && current->parent)
                current = current->parent;

            ++ptr;
            continue;
        }

        if(ptr < end && *ptr == '?')
        {
            ++ptr;
            if(!skip_string(&ptr, end, "xml"))
                break;

            skip_ws(&ptr, end);
            if(!parse_attributes(&ptr, end, NULL, NULL, NULL))
                break;

            if(!skip_string(&ptr, end, "?>"))
                break;

            skip_ws(&ptr, end);
            continue;
        }

        if(ptr < end && *ptr == '!')
        {
            ++ptr;
            if(skip_string(&ptr, end, "--"))
            {
                const char* sub = string_find(ptr, end, "-->");
                if(sub == NULL)
                    break;

                ptr += (sub - ptr) + 3;
                skip_ws(&ptr, end);
                continue;
            }

            if(skip_string(&ptr, end, "[CDATA["))
            {
                const char* sub = string_find(ptr, end, "]]>");
                if(sub == NULL)
                    break;

                ptr += (sub - ptr) + 3;
                skip_ws(&ptr, end);
                continue;
            }

            if(skip_string(&ptr, end, "DOCTYPE"))
            {
                while(ptr < end && *ptr != '>')
                {
                    if(*ptr=='[')
                    {
                        ++ptr;
                        int depth = 1;
                        while(ptr < end && depth > 0)
                        {
                            if(*ptr=='[') ++depth;
                            else if(*ptr==']') --depth;
                            ++ptr;
                        }
                    }
                    else
                    {
                        ++ptr;
                    }
                }

                if(ptr >= end || *ptr != '>')
                    break;

                ptr += 1;
                skip_ws(&ptr, end);
                continue;
            }

            break;
        }

        if(ptr >= end || !IS_STARTNAMECHAR(*ptr))
            break;

        const char* start = ptr;
        ++ptr;
        while(ptr < end && IS_NAMECHAR(*ptr))
            ++ptr;

        element_t* element = NULL;
        if(ignore > 0)
        {
            ignore++;
        }
        else
        {
            int id = domelementid(start, ptr);
            if(id == TAG_UNKNOWN)
            {
                ignore = 1;
            }
            else
            {
                if(root && current == NULL)
                    break;
                element = heap_alloc(heap, sizeof(element_t));
                element->id = id;
                element->parent = NULL;
                element->next = NULL;
                element->first = NULL;
                element->last = NULL;
                element->property = NULL;
                if(root == NULL)
                {
                    if(element->id != TAG_SVG)
                        break;
                    root = element;
                }
                else
                {
                    element->parent = current;
                    if(current->last)
                    {
                        current->last->next = element;
                        current->last = element;
                    }
                    else
                    {
                        current->last = element;
                        current->first = element;
                    }
                }
            }
        }

        skip_ws(&ptr, end);
        if(!parse_attributes(&ptr, end, element, cache, heap))
            break;

        if(ptr < end && *ptr == '>')
        {
            if(element)
                current = element;
            ++ptr;
            continue;
        }

        if(ptr < end && *ptr == '/')
        {
            ++ptr;
            if(ptr >= end || *ptr != '>')
                break;

            if(ignore > 0)
                ignore--;
            ++ptr;
            continue;
        }

        break;
    }

    skip_ws(&ptr, end);
    if(root == NULL || ptr != end || ignore != 0)
    {
        hashmap_destroy(cache);
        heap_destroy(heap);
        return NULL;
    }

    document_t* document = malloc(sizeof(document_t));
    document->root = root;
    document->cache = cache;
    document->heap = heap;
    return document;
}

static void document_destroy(document_t* document)
{
    hashmap_destroy(document->cache);
    heap_destroy(document->heap);
    free(document);
}

static const string_t* findvalue(const element_t* e, int id)
{
    const property_t* p = e->property;
    while(p != NULL)
    {
        if(p->id == id)
            return &p->value;
        p = p->next;
    }

    return NULL;
}

static inline int parse_double(const char** begin, const char* end, double* number)
{
    const char* ptr = *begin;
    double integer, fraction;
    int sign, expsign, exponent;

    fraction = 0;
    integer = 0;
    exponent = 0;
    sign = 1;
    expsign = 1;

    if(ptr < end && *ptr == '+')
        ++ptr;
    else if(ptr < end && *ptr == '-')
    {
        ++ptr;
        sign = -1;
    }

    if(ptr >= end || (!IS_NUM(*ptr) && *ptr != '.'))
        return 0;

    if(*ptr != '.')
    {
        while(*ptr && IS_NUM(*ptr))
            integer = 10.0 * integer + (*ptr++ - '0');
    }

    if(ptr < end && *ptr == '.')
    {
        ++ptr;
        if(ptr >= end || !IS_NUM(*ptr))
            return 0;
        double div = 1.0;
        while(ptr < end && IS_NUM(*ptr))
        {
            fraction = 10.0 * fraction + (*ptr++ - '0');
            div *= 10.0;
        }

        fraction /= div;
    }

    if(ptr < end && (*ptr == 'e' || *ptr == 'E')
       && (ptr[1] != 'x' && ptr[1] != 'm'))
    {
        ++ptr;
        if(ptr < end && *ptr == '+')
            ++ptr;
        else if(ptr < end && *ptr == '-')
        {
            ++ptr;
            expsign = -1;
        }

        if(ptr >= end || !IS_NUM(*ptr))
            return 0;

        while(ptr < end && IS_NUM(*ptr))
            exponent = 10 * exponent + (*ptr++ - '0');
    }

    *begin = ptr;
    *number = sign * (integer + fraction);
    if(exponent)
        *number *= pow(10.0, expsign*exponent);

    return *number >= -DBL_MAX && *number <= DBL_MAX;
}

static int parse_number(const element_t* e, int id, double* number, int percent)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;

    if(!parse_double(&ptr, end, number))
        return 0;

    if(percent)
    {
        if(skip_string(&ptr, end, "%"))
            *number /= 100.0;
        *number = (*number < 0.0) ? 0.0 : (*number > 1.0) ? 1.0 : *number;
    }

    return 1;
}

enum {
    length_unit_unknown,
    length_unit_number,
    length_unit_px,
    length_unit_pt,
    length_unit_pc,
    length_unit_in,
    length_unit_cm,
    length_unit_mm,
    length_unit_ex,
    length_unit_em,
    length_unit_percent
};

typedef struct {
    double value;
    int units;
} length_t;

static int parse_length(const element_t* e, int id, length_t* length, int negative)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;

    double v;
    if(!parse_double(&ptr, end, &v))
        return 0;

    if(negative == 0 && v < 0.0)
        return 0;

    length->value = v;
    length->units = length_unit_number;
    if(string_eq(ptr, end, "%"))
        length->units = length_unit_percent;
    else if(string_eq(ptr, end, "px"))
        length->units = length_unit_px;
    else if(string_eq(ptr, end, "pt"))
        length->units = length_unit_pt;
    else if(string_eq(ptr, end, "in"))
        length->units = length_unit_in;
    else if(string_eq(ptr, end, "cm"))
        length->units = length_unit_cm;
    else if(string_eq(ptr, end, "mm"))
        length->units = length_unit_mm;
    else if(string_eq(ptr, end, "em"))
        length->units = length_unit_em;
    else if(string_eq(ptr, end, "ex"))
        length->units = length_unit_ex;

    return 1;
}

static double length_value(const length_t* length, double max, double dpi)
{
    switch(length->units) {
    case length_unit_number:
    case length_unit_px:
        return length->value;
    case length_unit_in:
        return length->value * dpi;
    case length_unit_cm:
        return length->value * dpi / 2.54;
    case length_unit_mm:
        return length->value * dpi / 25.4;
    case length_unit_pt:
        return length->value * dpi / 72.0;
    case length_unit_pc:
        return length->value * dpi / 6.0;
    case length_unit_em:
    case length_unit_ex:
    case length_unit_percent:
        return length->value * max / 100.0;
    default:
        break;
    }

    return 0.0;
}

static int length_relative(const length_t* length)
{
    if(length->units==length_unit_em
        || length->units==length_unit_ex
        || length->units==length_unit_percent)
        return 1;

    return 0;
}

#define length_zero(length) (length.value == 0.0)
#define length_valid(length) (length.units != length_unit_unknown)

enum {
    paint_type_none,
    paint_type_color,
    paint_type_url,
    paint_type_current_color
};

typedef struct {
    int type;
    plutovg_color_t color;
    string_t url;
} paint_t;

static const struct {
    const char* name;
    unsigned long value;
} colormap[] = {
    {"aliceblue", 0xF0F8FF},
    {"antiquewhite", 0xFAEBD7},
    {"aqua", 0x00FFFF},
    {"aquamarine", 0x7FFFD4},
    {"azure", 0xF0FFFF},
    {"beige", 0xF5F5DC},
    {"bisque", 0xFFE4C4},
    {"black", 0x000000},
    {"blanchedalmond", 0xFFEBCD},
    {"blue", 0x0000FF},
    {"blueviolet", 0x8A2BE2},
    {"brown", 0xA52A2A},
    {"burlywood", 0xDEB887},
    {"cadetblue", 0x5F9EA0},
    {"chartreuse", 0x7FFF00},
    {"chocolate", 0xD2691E},
    {"coral", 0xFF7F50},
    {"cornflowerblue", 0x6495ED},
    {"cornsilk", 0xFFF8DC},
    {"crimson", 0xDC143C},
    {"cyan", 0x00FFFF},
    {"darkblue", 0x00008B},
    {"darkcyan", 0x008B8B},
    {"darkgoldenrod", 0xB8860B},
    {"darkgray", 0xA9A9A9},
    {"darkgreen", 0x006400},
    {"darkgrey", 0xA9A9A9},
    {"darkkhaki", 0xBDB76B},
    {"darkmagenta", 0x8B008B},
    {"darkolivegreen", 0x556B2F},
    {"darkorange", 0xFF8C00},
    {"darkorchid", 0x9932CC},
    {"darkred", 0x8B0000},
    {"darksalmon", 0xE9967A},
    {"darkseagreen", 0x8FBC8F},
    {"darkslateblue", 0x483D8B},
    {"darkslategray", 0x2F4F4F},
    {"darkslategrey", 0x2F4F4F},
    {"darkturquoise", 0x00CED1},
    {"darkviolet", 0x9400D3},
    {"deeppink", 0xFF1493},
    {"deepskyblue", 0x00BFFF},
    {"dimgray", 0x696969},
    {"dimgrey", 0x696969},
    {"dodgerblue", 0x1E90FF},
    {"firebrick", 0xB22222},
    {"floralwhite", 0xFFFAF0},
    {"forestgreen", 0x228B22},
    {"fuchsia", 0xFF00FF},
    {"gainsboro", 0xDCDCDC},
    {"ghostwhite", 0xF8F8FF},
    {"gold", 0xFFD700},
    {"goldenrod", 0xDAA520},
    {"gray", 0x808080},
    {"grey", 0x808080},
    {"green", 0x008000},
    {"greenyellow", 0xADFF2F},
    {"honeydew", 0xF0FFF0},
    {"hotpink", 0xFF69B4},
    {"indianred", 0xCD5C5C},
    {"indigo", 0x4B0082},
    {"ivory", 0xFFFFF0},
    {"khaki", 0xF0E68C},
    {"lavender", 0xE6E6FA},
    {"lavenderblush", 0xFFF0F5},
    {"lawngreen", 0x7CFC00},
    {"lemonchiffon", 0xFFFACD},
    {"lightblue", 0xADD8E6},
    {"lightcoral", 0xF08080},
    {"lightcyan", 0xE0FFFF},
    {"lightgoldenrodyellow", 0xFAFAD2},
    {"lightgray", 0xD3D3D3},
    {"lightgreen", 0x90EE90},
    {"lightgrey", 0xD3D3D3},
    {"lightpink", 0xFFB6C1},
    {"lightsalmon", 0xFFA07A},
    {"lightseagreen", 0x20B2AA},
    {"lightskyblue", 0x87CEFA},
    {"lightslategray", 0x778899},
    {"lightslategrey", 0x778899},
    {"lightsteelblue", 0xB0C4DE},
    {"lightyellow", 0xFFFFE0},
    {"lime", 0x00FF00},
    {"limegreen", 0x32CD32},
    {"linen", 0xFAF0E6},
    {"magenta", 0xFF00FF},
    {"maroon", 0x800000},
    {"mediumaquamarine", 0x66CDAA},
    {"mediumblue", 0x0000CD},
    {"mediumorchid", 0xBA55D3},
    {"mediumpurple", 0x9370DB},
    {"mediumseagreen", 0x3CB371},
    {"mediumslateblue", 0x7B68EE},
    {"mediumspringgreen", 0x00FA9A},
    {"mediumturquoise", 0x48D1CC},
    {"mediumvioletred", 0xC71585},
    {"midnightblue", 0x191970},
    {"mintcream", 0xF5FFFA},
    {"mistyrose", 0xFFE4E1},
    {"moccasin", 0xFFE4B5},
    {"navajowhite", 0xFFDEAD},
    {"navy", 0x000080},
    {"oldlace", 0xFDF5E6},
    {"olive", 0x808000},
    {"olivedrab", 0x6B8E23},
    {"orange", 0xFFA500},
    {"orangered", 0xFF4500},
    {"orchid", 0xDA70D6},
    {"palegoldenrod", 0xEEE8AA},
    {"palegreen", 0x98FB98},
    {"paleturquoise", 0xAFEEEE},
    {"palevioletred", 0xDB7093},
    {"papayawhip", 0xFFEFD5},
    {"peachpuff", 0xFFDAB9},
    {"peru", 0xCD853F},
    {"pink", 0xFFC0CB},
    {"plum", 0xDDA0DD},
    {"powderblue", 0xB0E0E6},
    {"purple", 0x800080},
    {"rebeccapurple", 0x663399},
    {"red", 0xFF0000},
    {"rosybrown", 0xBC8F8F},
    {"royalblue", 0x4169E1},
    {"saddlebrown", 0x8B4513},
    {"salmon", 0xFA8072},
    {"sandybrown", 0xF4A460},
    {"seagreen", 0x2E8B57},
    {"seashell", 0xFFF5EE},
    {"sienna", 0xA0522D},
    {"silver", 0xC0C0C0},
    {"skyblue", 0x87CEEB},
    {"slateblue", 0x6A5ACD},
    {"slategray", 0x708090},
    {"slategrey", 0x708090},
    {"snow", 0xFFFAFA},
    {"springgreen", 0x00FF7F},
    {"steelblue", 0x4682B4},
    {"tan", 0xD2B48C},
    {"teal", 0x008080},
    {"thistle", 0xD8BFD8},
    {"tomato", 0xFF6347},
    {"turquoise", 0x40E0D0},
    {"violet", 0xEE82EE},
    {"wheat", 0xF5DEB3},
    {"white", 0xFFFFFF},
    {"whitesmoke", 0xF5F5F5},
    {"yellow", 0xFFFF00},
    {"yellowgreen", 0x9ACD32}
};

static int parse_color_component(const char** ptr, const char* end, double* value)
{
    if(!parse_double(ptr, end, value))
        return 0;

    if(skip_string(ptr, end, "%"))
        *value *= 2.56;

    *value = (*value < 0.0) ? 0.0 : (*value > 255.0) ? 255.0 : round(*value);
    return 1;
}

static int parse_paint(const element_t* e, int id, paint_t* paint)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;

    if(skip_string(&ptr, end, "#"))
    {
        const char* start = ptr;
        while(ptr < end && isxdigit(*ptr))
            ++ptr;

        int n = (int)(ptr - start);
        if(n != 3 && n != 6)
            return 0;

        char* end_ptr;
        unsigned long value2 = strtoul(start, &end_ptr, 16);
        assert(ptr == end_ptr);
        if(n == 3)
        {
            value2 = ((value2&0xf00) << 8) | ((value2&0x0f0) << 4) | (value2&0x00f);
            value2 |= value2 << 4;
        }

        uint32_t r = (value2&0xff0000)>>16;
        uint32_t g = (value2&0x00ff00)>>8;
        uint32_t b = (value2&0x0000ff)>>0;
        paint->type = paint_type_color;
        plutovg_color_init_rgb(&paint->color, r / 255.0, g / 255.0, b / 255.0);
        return 1;
    }
    else if(skip_string(&ptr, end, "url("))
    {
        const char* start = ptr;
        while(ptr < end && *ptr != ')')
            ++ptr;

        if(ptr >= end || *ptr != ')')
            return 0;

        paint->type = paint_type_url;
        string_init(paint->url, start, ptr);
        return 1;
    }
    else if(skip_string(&ptr, end, "none"))
    {
        paint->type = paint_type_none;
        return 1;
    }
    else if(skip_string(&ptr, end, "currentColor"))
    {
        paint->type = paint_type_current_color;
        return 1;
    }
    else if(skip_string(&ptr, end, "rgb("))
    {
        double r, g, b;
        if(!skip_ws(&ptr, end)
            || !parse_color_component(&ptr, end, &r)
            || !skip_ws_comma(&ptr, end)
            || !parse_color_component(&ptr, end, &g)
            || !skip_ws_comma(&ptr, end)
            || !parse_color_component(&ptr, end, &b)
            || !skip_ws(&ptr, end)
            || !skip_string(&ptr, end, ")"))
            return 0;

        paint->type = paint_type_color;
        plutovg_color_init_rgb(&paint->color, r / 255.0, g / 255.0, b / 255.0);
        return 1;
    }
    else
    {
        for(size_t i = 0;i < sizeof(colormap) / sizeof(colormap[0]);i++)
        {
            if(!string_eq(ptr, end, colormap[i].name))
                continue;

            uintptr_t value2 = colormap[i].value;
            uint8_t r = (uint8_t)((value2 & 0xff0000) >> 16);
            uint8_t g = (uint8_t)((value2 & 0x00ff00) >> 8);
            uint8_t b = (uint8_t)((value2 & 0x0000ff) >> 0);
            paint->type = paint_type_color;
            plutovg_color_init_rgb(&paint->color, r / 255.0, g / 255.0, b / 255.0);
            return 1;
        }
    }

    return 0;
}

static int parse_view_box(const element_t* e, int id, plutovg_rect_t* viewbox)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;

    double x = 0;
    double y = 0;
    double w = 0;
    double h = 0;
    if(!parse_double(&ptr, end, &x)
            || !skip_ws_comma(&ptr, end)
            || !parse_double(&ptr, end, &y)
            || !skip_ws_comma(&ptr, end)
            || !parse_double(&ptr, end, &w)
            || !skip_ws_comma(&ptr, end)
            || !parse_double(&ptr, end, &h)
            || skip_ws(&ptr, end))
        return 0;

    if(w < 0.0 || h < 0.0)
        return 0;

    viewbox->x = x;
    viewbox->y = y;
    viewbox->w = w;
    viewbox->h = h;
    return 1;
}

enum {
    transform_type_matrix,
    transform_type_rotate,
    transform_type_scale,
    transform_type_skew_x,
    transform_type_skew_y,
    transform_type_translate
};

static int parse_transform(const char** begin, const char* end, int* type, double* values, int* count)
{
    const char* ptr = *begin;
    int required = 0;
    int optional = 0;
    if(skip_string(&ptr, end, "matrix"))
    {
        *type = transform_type_matrix;
        required = 6;
        optional = 0;
    }
    else if(skip_string(&ptr, end, "rotate"))
    {
        *type = transform_type_rotate;
        required = 1;
        optional = 2;
    }
    else if(skip_string(&ptr, end, "scale"))
    {
        *type = transform_type_scale;
        required = 1;
        optional = 1;
    }
    else if(skip_string(&ptr, end, "skewX"))
    {
        *type = transform_type_skew_x;
        required = 1;
        optional = 0;
    }
    else if(skip_string(&ptr, end, "skewY"))
    {
        *type = transform_type_skew_y;
        required = 1;
        optional = 0;
    }
    else if(skip_string(&ptr, end, "translate"))
    {
        *type = transform_type_translate;
        required = 1;
        optional = 1;
    }
    else
    {
        return 0;
    }

    skip_ws(&ptr, end);
    if(ptr >= end || *ptr != '(')
        return 0;
    ++ptr;

    int max_count = required + optional;
    int i = 0;
    skip_ws(&ptr, end);
    while(i < max_count)
    {
        if(!parse_double(&ptr, end, values + i))
            break;
        ++i;
        skip_ws_comma(&ptr, end);
    }

    if(ptr >= end || *ptr != ')' || !(i == required || i == max_count))
        return 0;
    ++ptr;

    *begin = ptr;
    *count = i;
    return 1;
}

#define PI 3.14159265358979323846
static int parse_transform_list(const element_t* e, int id, plutovg_matrix_t* matrix)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;

    int type, count;
    double values[6];
    plutovg_matrix_init_identity(matrix);
    while(ptr < end)
    {
        if(!parse_transform(&ptr, end, &type, values, &count))
            break;
        skip_ws_comma(&ptr, end);
        switch(type) {
        case transform_type_matrix:
            plutovg_matrix_multiply(matrix, (plutovg_matrix_t*)values, matrix);
            break;
        case transform_type_rotate:
            if(count == 1)
                plutovg_matrix_rotate(matrix, values[0]*PI/180.0);
            else
                plutovg_matrix_rotate_translate(matrix, values[0]*PI/180.0, values[1], values[2]);
            break;
        case transform_type_scale:
            if(count == 1)
                plutovg_matrix_scale(matrix, values[0], values[0]);
            else
                plutovg_matrix_scale(matrix, values[0], values[1]);
            break;
        case transform_type_skew_x:
            plutovg_matrix_shear(matrix, values[0]*PI/180.0, 0.0);
            break;
        case transform_type_skew_y:
            plutovg_matrix_shear(matrix, 0.0, values[0]*PI/180.0);
            break;
        case transform_type_translate:
            if(count == 1)
                plutovg_matrix_translate(matrix, values[0], 0.0);
            else
                plutovg_matrix_translate(matrix, values[0], values[1]);
            break;
        default:
            break;
        }
    }

    return 1;
}

static int parse_coordinate(const char** begin, const char* end, double* values, int length)
{
    const char* ptr = *begin;
    for(int i = 0;i < length;i++)
    {
        if(!parse_double(&ptr, end, values + i))
            return 0;
        skip_ws_comma(&ptr, end);
    }

    *begin = ptr;
    return 1;
}

static int parse_arc_flag(const char** begin, const char* end, int* flag)
{
    const char* ptr = *begin;
    if(ptr < end && *ptr == '0')
        *flag = 0;
    else if(ptr < end && *ptr == '1')
        *flag = 1;
    else
        return 0;

    ++ptr;
    skip_ws_comma(&ptr, end);
    *begin = ptr;
    return 1;
}

static void path_arc_segment(plutovg_path_t* path, double xc, double yc, double th0, double th1, double rx, double ry, double x_axis_rotation)
{
    double sin_th, cos_th;
    double a00, a01, a10, a11;
    double x1, y1, x2, y2, x3, y3;
    double t;
    double th_half;

    sin_th = sin(x_axis_rotation * PI / 180.0);
    cos_th = cos(x_axis_rotation * PI / 180.0);

    a00 =  cos_th * rx;
    a01 = -sin_th * ry;
    a10 =  sin_th * rx;
    a11 =  cos_th * ry;

    th_half = 0.5 * (th1 - th0);
    t = (8.0 / 3.0) * sin(th_half * 0.5) * sin(th_half * 0.5) / sin(th_half);
    x1 = xc + cos(th0) - t * sin(th0);
    y1 = yc + sin(th0) + t * cos(th0);
    x3 = xc + cos(th1);
    y3 = yc + sin(th1);
    x2 = x3 + t * sin(th1);
    y2 = y3 - t * cos(th1);

    plutovg_path_cubic_to(path, a00 * x1 + a01 * y1, a10 * x1 + a11 * y1, a00 * x2 + a01 * y2, a10 * x2 + a11 * y2, a00 * x3 + a01 * y3, a10 * x3 + a11 * y3);
}

static void path_arc_to(plutovg_path_t* path, double current_x, double current_y, double rx, double ry, double x_axis_rotation, int large_arc_flag, int sweep_flag, double x, double y)
{
    double sin_th, cos_th;
    double a00, a01, a10, a11;
    double x0, y0, x1, y1, xc, yc;
    double d, sfactor, sfactor_sq;
    double th0, th1, th_arc;
    int i, n_segs;
    double dx, dy, dx1, dy1, Pr1, Pr2, Px, Py, check;

    rx = fabs(rx);
    ry = fabs(ry);

    sin_th = sin(x_axis_rotation * PI / 180.0);
    cos_th = cos(x_axis_rotation * PI / 180.0);

    dx = (current_x - x) / 2.0;
    dy = (current_y - y) / 2.0;
    dx1 =  cos_th * dx + sin_th * dy;
    dy1 = -sin_th * dx + cos_th * dy;
    Pr1 = rx * rx;
    Pr2 = ry * ry;
    Px = dx1 * dx1;
    Py = dy1 * dy1;
    check = Px / Pr1 + Py / Pr2;
    if(check > 1)
    {
        rx = rx * sqrt(check);
        ry = ry * sqrt(check);
    }

    a00 =  cos_th / rx;
    a01 =  sin_th / rx;
    a10 = -sin_th / ry;
    a11 =  cos_th / ry;
    x0 = a00 * current_x + a01 * current_y;
    y0 = a10 * current_x + a11 * current_y;
    x1 = a00 * x + a01 * y;
    y1 = a10 * x + a11 * y;
    d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
    sfactor_sq = 1.0 / d - 0.25;
    if(sfactor_sq < 0) sfactor_sq = 0;
    sfactor = sqrt(sfactor_sq);
    if(sweep_flag == large_arc_flag) sfactor = -sfactor;
    xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
    yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);

    th0 = atan2(y0 - yc, x0 - xc);
    th1 = atan2(y1 - yc, x1 - xc);

    th_arc = th1 - th0;
    if(th_arc < 0 && sweep_flag)
        th_arc += 2.0 * PI;
    else if(th_arc > 0 && !sweep_flag)
        th_arc -= 2.0 * PI;

    n_segs = (int)(ceil(fabs(th_arc / (PI * 0.5 + 0.001))));
    for(i = 0;i < n_segs; i++)
        path_arc_segment(path, xc, yc, th0 + i * th_arc / n_segs, th0 + (i + 1) * th_arc / n_segs, rx, ry, x_axis_rotation);
}

static int parse_path(const element_t* e, int id, plutovg_path_t* path)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;
    if(ptr >= end || !(*ptr == 'M' || *ptr == 'm'))
        return 0;

    char command = *ptr++;
    double c[6];
    int f[2];

    double start_x, start_y;
    double current_x, current_y;
    double last_control_x, last_control_y;

    start_x = start_y = 0.0;
    current_x = current_y = 0.0;
    last_control_x = last_control_y = 0.0;

    plutovg_path_clear(path);
    while(1)
    {
        skip_ws(&ptr, end);
        switch(command) {
        case 'M':
        case 'm':
            if(!parse_coordinate(&ptr, end, c, 2))
                break;

            if(command == 'm')
            {
                c[0] += current_x;
                c[1] += current_y;
            }

            plutovg_path_move_to(path, c[0], c[1]);
            current_x = start_x = last_control_x = c[0];
            current_y = start_y = last_control_y = c[1];
            command = command == 'm' ? 'l' : 'L';
            break;
        case 'L':
        case 'l':
            if(!parse_coordinate(&ptr, end, c, 2))
                break;

            if(command == 'l')
            {
                c[0] += current_x;
                c[1] += current_y;
            }

            plutovg_path_line_to(path, c[0], c[1]);
            current_x = last_control_x = c[0];
            current_y = last_control_y = c[1];
            break;
        case 'Q':
        case 'q':
            if(!parse_coordinate(&ptr, end, c, 4))
                break;

            if(command == 'q')
            {
                c[0] += current_x;
                c[1] += current_y;
                c[2] += current_x;
                c[3] += current_y;
            }

            plutovg_path_quad_to(path, c[0], c[1], c[2], c[3]);
            last_control_x = c[0];
            last_control_y = c[1];
            current_x = c[2];
            current_y = c[3];
            break;
        case 'C':
        case 'c':
            if(!parse_coordinate(&ptr, end, c, 6))
                break;

            if(command == 'c')
            {
                c[0] += current_x;
                c[1] += current_y;
                c[2] += current_x;
                c[3] += current_y;
                c[4] += current_x;
                c[5] += current_y;
            }

            plutovg_path_cubic_to(path, c[0], c[1], c[2], c[3], c[4], c[5]);
            last_control_x = c[2];
            last_control_y = c[3];
            current_x = c[4];
            current_y = c[5];
            break;
        case 'T':
        case 't':
            c[0] = 2 * current_x - last_control_x;
            c[1] = 2 * current_y - last_control_y;
            if(!parse_coordinate(&ptr, end, c + 2, 2))
                break;

            if(command == 't')
            {
                c[2] += current_x;
                c[3] += current_y;
            }

            plutovg_path_quad_to(path, c[0], c[1], c[2], c[3]);
            last_control_x = c[0];
            last_control_y = c[1];
            current_x = c[2];
            current_y = c[3];
            break;
        case 'S':
        case 's':
            c[0] = 2 * current_x - last_control_x;
            c[1] = 2 * current_y - last_control_y;
            if(!parse_coordinate(&ptr, end, c + 2, 4))
                break;

            if(command == 's')
            {
                c[2] += current_x;
                c[3] += current_y;
                c[4] += current_x;
                c[5] += current_y;
            }

            plutovg_path_cubic_to(path, c[0], c[1], c[2], c[3], c[4], c[5]);
            last_control_x = c[2];
            last_control_y = c[3];
            current_x = c[4];
            current_y = c[5];
            break;
        case 'H':
        case 'h':
            if(!parse_coordinate(&ptr, end, c, 1))
                break;

            if(command == 'h')
               c[0] += current_x;

            plutovg_path_line_to(path, c[0], current_y);
            current_x = last_control_x = c[0];
            break;
        case 'V':
        case 'v':
            if(!parse_coordinate(&ptr, end, c + 1, 1))
                break;

            if(command == 'v')
               c[1] += current_y;

            plutovg_path_line_to(path, current_x, c[1]);
            current_y = last_control_y = c[1];
            break;
        case 'A':
        case 'a':
            if(!parse_coordinate(&ptr, end, c, 3)
                    || !parse_arc_flag(&ptr, end, f)
                    || !parse_arc_flag(&ptr, end, f + 1)
                    || !parse_coordinate(&ptr, end, c + 3, 2))
                break;

            if(command == 'a')
            {
               c[3] += current_x;
               c[4] += current_y;
            }

            path_arc_to(path, current_x, current_y, c[0], c[1], c[2], f[0], f[1], c[3], c[4]);
            current_x = last_control_x = c[3];
            current_y = last_control_y = c[4];
            break;
        case 'Z':
        case 'z':
            plutovg_path_close(path);
            current_x = last_control_x = start_x;
            current_y = last_control_y = start_y;
            break;
        default:
            break;
        }

        skip_ws_comma(&ptr, end);
        if(ptr >= end)
            break;

        if(IS_ALPHA(*ptr))
            command = *ptr++;
    }

    return 1;
}

static int parse_points(const element_t* e, int id, plutovg_path_t* path, int closed)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;

    double c[2];
    if(!parse_coordinate(&ptr, end, c, 2))
        return 0;

    plutovg_path_clear(path);
    plutovg_path_move_to(path, c[0], c[1]);
    skip_ws_comma(&ptr, end);
    while(ptr < end)
    {
        if(!parse_coordinate(&ptr, end, c, 2))
            break;
        plutovg_path_line_to(path, c[0], c[1]);
        skip_ws_comma(&ptr, end);
    }

    if(closed)
        plutovg_path_close(path);

    return 1;
}

static void path_bounding_box(const plutovg_path_t* path, plutovg_rect_t* dst)
{
    const plutovg_point_t* p = plutovg_path_get_points(path);
    int count = plutovg_path_get_point_count(path);
    if(count == 0)
    {
        dst->x = 0;
        dst->y = 0;
        dst->w = 0;
        dst->h = 0;
        return;
    }

    double l = p[0].x;
    double t = p[0].y;
    double r = p[0].x;
    double b = p[0].y;

    for(int i = 0;i < count;i++)
    {
        if(p[i].x < l) l = p[i].x;
        if(p[i].x > r) r = p[i].x;
        if(p[i].y < t) t = p[i].y;
        if(p[i].y > b) b = p[i].y;
    }

    dst->x = l;
    dst->y = t;
    dst->w = r - l;
    dst->h = b - t;
}

enum {
    positioning_scale_meet,
    positioning_scale_slice
};

enum {
    positioning_align_none,
    positioning_align_x_min_y_min,
    positioning_align_x_mid_y_min,
    positioning_align_x_max_y_min,
    positioning_align_x_min_y_mid,
    positioning_align_x_mid_y_mid,
    positioning_align_x_max_y_mid,
    positioning_align_x_min_y_max,
    positioning_align_x_mid_y_max,
    positioning_align_x_max_y_max
};

typedef struct {
    int align;
    int scale;
} positioning_t;

static int parse_positioning(const element_t* e, int id, positioning_t* position)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;

    int align;
    if(skip_string(&ptr, end, "none"))
        align = positioning_align_none;
    else if(skip_string(&ptr, end, "xMinYMin"))
        align = positioning_align_x_min_y_min;
    else if(skip_string(&ptr, end, "xMidYMin"))
        align = positioning_align_x_mid_y_min;
    else if(skip_string(&ptr, end, "xMaxYMin"))
        align = positioning_align_x_max_y_min;
    else if(skip_string(&ptr, end, "xMinYMid"))
        align = positioning_align_x_min_y_mid;
    else if(skip_string(&ptr, end, "xMidYMid"))
        align = positioning_align_x_mid_y_mid;
    else if(skip_string(&ptr, end, "xMaxYMid"))
        align = positioning_align_x_max_y_mid;
    else if(skip_string(&ptr, end, "xMinYMax"))
        align = positioning_align_x_min_y_max;
    else if(skip_string(&ptr, end, "xMidYMax"))
        align = positioning_align_x_mid_y_max;
    else if(skip_string(&ptr, end, "xMaxYMax"))
        align = positioning_align_x_max_y_max;
    else
        return 0;

    position->align = align;
    position->scale = positioning_scale_meet;
    if(position->align != positioning_align_none)
    {
        skip_ws(&ptr, end);
        if(skip_string(&ptr, end, "meet"))
            position->scale = positioning_scale_meet;
        else if(skip_string(&ptr, end, "slice"))
            position->scale = positioning_scale_slice;
    }

    return 1;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
static void positioning_get_matrix(const positioning_t* position, plutovg_matrix_t* matrix, const plutovg_rect_t* viewport, const plutovg_rect_t* viewbox)
{
    plutovg_matrix_init_identity(matrix);
    plutovg_matrix_translate(matrix, viewport->x, viewport->y);
    if(viewbox->w == 0.0 || viewbox->h == 0.0)
        return;

    double sx = viewport->w  / viewbox->w;
    double sy = viewport->h / viewbox->h;

    if(sx == 0.0 || sy == 0.0)
        return;

    double tx = -viewbox->x;
    double ty = -viewbox->y;

    if(position->align == positioning_align_none)
    {
        plutovg_matrix_scale(matrix, sx, sy);
        plutovg_matrix_translate(matrix, tx, ty);
        return;
    }

    double scale = (position->scale == positioning_scale_meet) ? MIN(sx, sy) : MAX(sx, sy);
    double vw = viewport->w / scale;
    double vh = viewport->h / scale;

    switch(position->align) {
    case positioning_align_x_mid_y_min:
    case positioning_align_x_mid_y_mid:
    case positioning_align_x_mid_y_max:
        tx -= (viewbox->w - vw) * 0.5;
        break;
    case positioning_align_x_max_y_min:
    case positioning_align_x_max_y_mid:
    case positioning_align_x_max_y_max:
        tx -= (viewbox->w - vw);
        break;
    default:
        break;
    }

    switch(position->align) {
    case positioning_align_x_min_y_mid:
    case positioning_align_x_mid_y_mid:
    case positioning_align_x_max_y_mid:
        ty -= (viewbox->h - vh) * 0.5;
        break;
    case positioning_align_x_min_y_max:
    case positioning_align_x_mid_y_max:
    case positioning_align_x_max_y_max:
        ty -= (viewbox->h - vh);
        break;
    default:
        break;
    }

    plutovg_matrix_scale(matrix, scale, scale);
    plutovg_matrix_translate(matrix, tx, ty);
}

static int parse_line_cap(const element_t* e, int id, plutovg_line_cap_t* cap)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;
    if(string_eq(ptr, end, "round"))
        *cap = plutovg_line_cap_round;
    else if(string_eq(ptr, end, "square"))
        *cap = plutovg_line_cap_square;
    else
        *cap = plutovg_line_cap_butt;
    return 1;
}

static int parse_line_join(const element_t* e, int id, plutovg_line_join_t* join)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;
    if(string_eq(ptr, end, "bevel"))
        *join = plutovg_line_join_bevel;
    else if(string_eq(ptr, end, "round"))
        *join = plutovg_line_join_round;
    else
        *join = plutovg_line_join_miter;
    return 1;
}

static int parse_fill_rule(const element_t* e, int id, plutovg_fill_rule_t* winding)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;
    if(string_eq(ptr, end, "evenodd"))
        *winding = plutovg_fill_rule_even_odd;
    else
        *winding = plutovg_fill_rule_non_zero;
    return 1;
}

static int parse_spread_method(const element_t* e, int id, plutovg_spread_method_t* spread)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;
    if(string_eq(ptr, end, "reflect"))
        *spread = plutovg_spread_method_reflect;
    else if(string_eq(ptr, end, "repeat"))
        *spread = plutovg_spread_method_repeat;
    else
        *spread = plutovg_spread_method_pad;
    return 1;
}

enum {
    display_inline,
    display_none
};

static int parse_display(const element_t* e, int id, int* display)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;
    if(string_eq(ptr, end, "none"))
        *display = display_none;
    else
        *display = display_inline;
    return 1;
}

enum {
    visibility_visible,
    visibility_hidden
};

static int parse_visibility(const element_t* e, int id, int* visibility)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;
    if(string_eq(ptr, end, "hidden"))
        *visibility = visibility_hidden;
    else
        *visibility = visibility_visible;
    return 1;
}

enum {
    units_type_object_bounding_box,
    units_type_user_space_on_use
};

static int parse_units_type(const element_t* e, int id, int* units)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;
    if(string_eq(ptr, end, "userSpaceOnUse"))
        *units = units_type_user_space_on_use;
    else
        *units = units_type_object_bounding_box;
    return 1;
}

enum {
    text_anchor_start,
    text_anchor_mid,
    text_anchor_end
};

static int parse_text_anchor(const element_t* e, int id, int* anchor)
{
    const string_t* value = findvalue(e, id);
    if(value == NULL)
        return 0;

    const char* ptr = value->start;
    const char* end = value->end;
    if(string_eq(ptr, end, "mid"))
        *anchor = text_anchor_mid;
    else if(string_eq(ptr, end, "end"))
        *anchor = text_anchor_end;
    else
        *anchor = text_anchor_start;
    return 1;
}

typedef struct {
    paint_t fill;
    paint_t stroke;

    length_t linewidth;
    length_t fontsize;

    double fillopacity;
    double strokeopacity;
    double opacity;
    double miterlimit;

    plutovg_line_cap_t linecap;
    plutovg_line_join_t linejoin;
    plutovg_fill_rule_t fillrule;

    int textanchor;
    int visibility;
} style_t;

static void style_init(style_t* style)
{
    style->fill.type = paint_type_color;
    style->stroke.type = paint_type_none;

    style->fill.color.r = 0;
    style->fill.color.g = 0;
    style->fill.color.b = 0;
    style->fill.color.a = 1;

    style->linewidth.value = 1.0;
    style->linewidth.units = length_unit_px;

    style->fontsize.value = 12.0;
    style->fontsize.units = length_unit_px;

    style->opacity = 1.0;
    style->fillopacity = 1.0;
    style->strokeopacity = 1.0;
    style->miterlimit = 4.0;

    style->linecap = plutovg_line_cap_butt;
    style->linejoin = plutovg_line_join_miter;
    style->fillrule = plutovg_fill_rule_non_zero;

    style->textanchor = text_anchor_start;
    style->visibility = visibility_visible;
}

static void parse_style(const element_t* e, style_t* style)
{
    parse_paint(e, ID_FILL, &style->fill);
    parse_paint(e, ID_STROKE, &style->stroke);

    parse_length(e, ID_STROKE_WIDTH, &style->linewidth, 0);
    parse_length(e, ID_FONT_SIZE, &style->fontsize, 0);

    parse_number(e, ID_OPACITY, &style->opacity, 1);
    parse_number(e, ID_FILL_OPACITY, &style->fillopacity, 1);
    parse_number(e, ID_STROKE_OPACITY, &style->strokeopacity, 1);
    parse_number(e, ID_STROKE_MITERLIMIT, &style->miterlimit, 0);

    parse_line_cap(e, ID_STROKE_LINECAP, &style->linecap);
    parse_line_join(e, ID_STROKE_LINEJOIN, &style->linejoin);
    parse_fill_rule(e, ID_FILL_RULE, &style->fillrule);

    parse_text_anchor(e, ID_TEXT_ANCHOR, &style->textanchor);
    parse_visibility(e, ID_VISIBILITY, &style->visibility);
}

typedef struct {
    int ref;
    plutovg_surface_t* surface;
    plutovg_t* pluto;
} canvas_t;

static canvas_t* canvas_create_for_surface(plutovg_surface_t* surface)
{
    canvas_t* canvas = malloc(sizeof(canvas_t));
    canvas->ref = 1;
    canvas->surface = plutovg_surface_reference(surface);
    canvas->pluto = plutovg_create(surface);
    return canvas;
}

static canvas_t* canvas_create(int width, int height)
{
    plutovg_surface_t* surface = plutovg_surface_create(width, height);
    canvas_t* canvas = canvas_create_for_surface(surface);
    plutovg_surface_destroy(surface);
    return canvas;
}

static canvas_t* canvas_create_similar(const canvas_t* canvas)
{
    if(canvas == NULL)
        return NULL;

    int width = plutovg_surface_get_width(canvas->surface);
    int height = plutovg_surface_get_height(canvas->surface);

    return canvas_create(width, height);
}

static void canvas_destroy(canvas_t* canvas)
{
    if(canvas == NULL)
        return;

    if(--canvas->ref == 0)
    {
        plutovg_surface_destroy(canvas->surface);
        plutovg_destroy(canvas->pluto);
        free(canvas);
    }
}

static canvas_t* canvas_reference(canvas_t* canvas)
{
    if(canvas == NULL)
        return NULL;

    ++canvas->ref;
    return canvas;
}

static void canvas_blend(canvas_t* canvas, const canvas_t* source, plutovg_operator_t op, double opacity)
{
    plutovg_set_source_surface(canvas->pluto, source->surface, 0, 0);
    plutovg_set_operator(canvas->pluto, op);
    plutovg_set_opacity(canvas->pluto, opacity);
    plutovg_identity_matrix(canvas->pluto);
    plutovg_paint(canvas->pluto);
}

typedef struct render_state {
    canvas_t* canvas;
    style_t style;
    plutovg_matrix_t matrix;
    plutovg_rect_t viewport;
    plutovg_rect_t bbox;
    plutovg_color_t color;
    struct render_state* next;
} render_state_t;

static render_state_t* render_state_create(void)
{
    render_state_t* state = malloc(sizeof(render_state_t));
    state->canvas = NULL;
    style_init(&state->style);
    plutovg_matrix_init_identity(&state->matrix);
    memset(&state->viewport, 0, sizeof(plutovg_rect_t));
    memset(&state->bbox, 0, sizeof(plutovg_rect_t));
    plutovg_color_init_rgb(&state->color, 0, 0, 0);
    state->next = NULL;
    return state;
}

static render_state_t* render_state_clone(const render_state_t* state)
{
    render_state_t* newstate = malloc(sizeof(render_state_t));
    newstate->canvas = canvas_reference(state->canvas);
    newstate->style = state->style;
    newstate->matrix = state->matrix;
    newstate->viewport = state->viewport;
    newstate->bbox = state->bbox;
    newstate->color = state->color;
    newstate->next = NULL;
    return newstate;
}

static void render_state_destroy(render_state_t* state)
{
    canvas_destroy(state->canvas);
    free(state);
}

typedef struct {
    document_t* document;
    render_state_t* state;
    plutovg_path_t* path;
    plutovg_font_t* font;
    double dpi;
} render_context_t;

static render_context_t* render_context_create(document_t* document, plutovg_font_t* font, double dpi)
{
    render_context_t* context = malloc(sizeof(render_context_t));
    context->document = document;
    context->state = render_state_create();
    context->path = plutovg_path_create();
    context->font = plutovg_font_reference(font);
    context->dpi = dpi;
    return context;
}

static void render_context_destroy(render_context_t* context)
{
    render_state_destroy(context->state);
    plutovg_path_destroy(context->path);
    plutovg_font_destroy(context->font);
    free(context);
}

static void render_context_push(render_context_t* context, const element_t* e)
{
    render_state_t* newstate = render_state_clone(context->state);
    style_t* style = &newstate->style;
    style->opacity = 1.0;
    parse_style(e, style);

    if(e->first && style->opacity != 1.0)
    {
        canvas_destroy(newstate->canvas);
        newstate->canvas = canvas_create_similar(context->state->canvas);
    }

    plutovg_matrix_t matrix;
    if(parse_transform_list(e, ID_TRANSFORM, &matrix))
        plutovg_matrix_multiply(&newstate->matrix, &matrix, &newstate->matrix);

    paint_t paint = {.type=paint_type_none};
    parse_paint(e, ID_COLOR, &paint);
    if(paint.type == paint_type_color)
        newstate->color = paint.color;

    newstate->bbox.x = 0;
    newstate->bbox.y = 0;
    newstate->bbox.w = 0;
    newstate->bbox.h = 0;

    newstate->next = context->state;
    context->state = newstate;
}

static void render_context_pop(render_context_t* context, const element_t* e)
{
    render_state_t* state = context->state;
    render_state_t* newstate = state->next;
    if(state->canvas != newstate->canvas)
    {
        style_t* style = &state->style;
        double opacity = e->first ? style->opacity : 1.0;
        canvas_blend(newstate->canvas, state->canvas, plutovg_operator_src_over, opacity);
    }

    plutovg_rect_t bbox;
    plutovg_matrix_t matrix = newstate->matrix;
    plutovg_matrix_invert(&matrix);
    plutovg_matrix_multiply(&matrix, &state->matrix, &matrix);
    plutovg_matrix_map_rect(&matrix, &state->bbox, &bbox);

    double l = MIN(newstate->bbox.x, bbox.x);
    double t = MIN(newstate->bbox.y, bbox.y);
    double r = MAX(newstate->bbox.x + newstate->bbox.w, bbox.x + bbox.w);
    double b = MAX(newstate->bbox.y + newstate->bbox.h, bbox.y + bbox.h);

    newstate->bbox.x = l;
    newstate->bbox.y = t;
    newstate->bbox.w = r - l;
    newstate->bbox.h = b - t;

    render_state_destroy(state);
    context->state = newstate;
}

#define SQRT2 1.41421356237309504880
static double resolve_length(const render_context_t* context, const length_t* length, char mode)
{
    if(length->units == length_unit_percent)
    {
        const render_state_t* state = context->state;
        double w = state->viewport.w;
        double h = state->viewport.h;
        double max = (mode == 'x') ? w : (mode == 'y') ? h : sqrt(w*w+h*h) / SQRT2;
        return length->value * max / 100.0;
    }

    return length_value(length, 1.0, context->dpi);
}

static const element_t* resolve_iri(const render_context_t* context, const string_t* value)
{
    const char* ptr = value->start;
    const char* end = value->end;

    int size = (int)(end - ptr);
    if(size > 1 && *ptr == '#')
    {
        const document_t* document = context->document;
        return hashmap_get(document->cache, ptr+1, end);
    }

    return NULL;
}

static const element_t* resolve_href(const render_context_t* context, const element_t* e)
{
    const string_t* value = findvalue(e, ID_XLINK_HREF);
    if(value == NULL)
        return NULL;

    return resolve_iri(context, value);
}

static plutovg_color_t find_current_color(const element_t* e)
{
    paint_t paint = {.type=paint_type_none};
    parse_paint(e, ID_COLOR, &paint);
    if(paint.type == paint_type_color)
        return paint.color;

    const element_t* parent = e->parent;
    if(parent == NULL)
    {
        plutovg_color_t color;
        plutovg_color_init_rgb(&color, 0, 0, 0);
        return color;
    }

    return find_current_color(parent);
}

static plutovg_color_t find_solid_color(const element_t* e, int id)
{
    paint_t paint = {.type=paint_type_none};
    parse_paint(e, id, &paint);

    if(paint.type == paint_type_color)
        return paint.color;
    if(paint.type == paint_type_current_color)
        return find_current_color(e);

    const element_t* parent = e->parent;
    if(parent == NULL)
    {
        plutovg_color_t color;
        plutovg_color_init_rgb(&color, 0, 0, 0);
        return color;
    }

    return find_solid_color(parent, id);
}

static double find_solid_opacity(const element_t* e, int id)
{
    double opacity;
    if(parse_number(e, id, &opacity, 1))
        return opacity;

    const element_t* parent = e->parent;
    if(parent == NULL)
        return 1.0;

    return find_solid_opacity(parent, id);
}

static plutovg_paint_t* resolve_solid_color(const element_t* e)
{
    plutovg_color_t color = find_solid_color(e, ID_SOLID_COLOR);
    color.a = find_solid_opacity(e, ID_SOLID_OPACITY);
    return plutovg_paint_create_color(&color);
}

static const element_t* findgradient(const render_context_t* context, const element_t* e)
{
    const element_t* ref = resolve_href(context, e);
    if(ref == NULL || !(ref->id == TAG_LINEAR_GRADIENT || ref->id == TAG_RADIAL_GRADIENT))
        return NULL;

    return ref;
}

static int parse_gradient_length(const render_context_t* context, const element_t* e, int id, length_t* length, int negative)
{
    if(parse_length(e, id, length, negative))
        return 1;

    const element_t* ref = findgradient(context, e);
    if(ref == NULL)
        return 0;

    return parse_gradient_length(context, ref, id, length, negative);
}

static const element_t* find_gradient_stops(const render_context_t* context, const element_t* e)
{
    const element_t* child = e->first;
    if(child && child->id == TAG_STOP)
        return child;

    const element_t* ref = findgradient(context, e);
    if(ref == NULL)
        return NULL;

    return find_gradient_stops(context, ref);
}

static plutovg_spread_method_t find_gradient_spread(const render_context_t* context, const element_t* e)
{
    plutovg_spread_method_t spread;
    if(parse_spread_method(e, ID_SPREAD_METHOD, &spread))
        return spread;

    const element_t* ref = findgradient(context, e);
    if(ref == NULL)
        return plutovg_spread_method_pad;

    return find_gradient_spread(context, ref);
}

static plutovg_matrix_t find_gradient_matrix(const render_context_t* context, const element_t* e)
{
    plutovg_matrix_t matrix;
    if(parse_transform_list(e, ID_GRADIENT_TRANSFORM, &matrix))
        return matrix;

    const element_t* ref = findgradient(context, e);
    if(ref == NULL)
    {
        plutovg_matrix_init_identity(&matrix);
        return matrix;
    }

    return find_gradient_matrix(context, ref);
}

static int find_gradient_units(const render_context_t* context, const element_t* e)
{
    int units;
    if(parse_units_type(e, ID_GRADIENT_UNITS, &units))
        return units;

    const element_t* ref = findgradient(context, e);
    if(ref == NULL)
        return units_type_object_bounding_box;

    return find_gradient_units(context, ref);
}

static double resolve_gradient_length(const render_context_t* context, const length_t* length, int units, char mode)
{
    if(units == units_type_object_bounding_box)
        return length_value(length, 1.0, context->dpi);

    return resolve_length(context, length, mode);
}

static void resolve_gradient_stops(plutovg_gradient_t* gradient, const element_t* e)
{
    while(e != NULL)
    {
        if(e->id == TAG_STOP)
        {
            double offset = 0.0;
            parse_number(e, ID_OFFSET, &offset, 1);
            plutovg_color_t color = find_solid_color(e, ID_STOP_COLOR);
            color.a = find_solid_opacity(e, ID_STOP_OPACITY);
            plutovg_gradient_add_stop_color(gradient, offset, &color);
        }

        e = e->next;
    }
}

static plutovg_paint_t* resolve_linear_gradient(const render_context_t* context, const element_t* e)
{
    const element_t* stops = find_gradient_stops(context, e);
    if(stops == NULL)
        return NULL;

    length_t x1 = {0, length_unit_px};
    length_t y1 = {0, length_unit_px};
    length_t x2 = {100, length_unit_percent};
    length_t y2 = {0, length_unit_px};

    parse_gradient_length(context, e, ID_X1, &x1, 1);
    parse_gradient_length(context, e, ID_Y1, &y1, 1);
    parse_gradient_length(context, e, ID_X2, &x2, 1);
    parse_gradient_length(context, e, ID_Y2, &y2, 1);

    plutovg_spread_method_t spread = find_gradient_spread(context, e);
    plutovg_matrix_t matrix = find_gradient_matrix(context, e);
    int units = find_gradient_units(context, e);

    if(units == units_type_object_bounding_box)
    {
        const render_state_t* state = context->state;
        plutovg_matrix_t m;
        plutovg_matrix_init_translate(&m, state->bbox.x, state->bbox.y);
        plutovg_matrix_scale(&m, state->bbox.w, state->bbox.h);
        plutovg_matrix_multiply(&matrix, &matrix, &m);
    }

    double _x1 = resolve_gradient_length(context, &x1, units, 'x');
    double _y1 = resolve_gradient_length(context, &y1, units, 'y');
    double _x2 = resolve_gradient_length(context, &x2, units, 'x');
    double _y2 = resolve_gradient_length(context, &y2, units, 'y');

    plutovg_paint_t* paint = plutovg_paint_create_linear(_x1, _y1, _x2, _y2);
    plutovg_gradient_t* gradient = plutovg_paint_get_gradient(paint);
    plutovg_gradient_set_matrix(gradient, &matrix);
    plutovg_gradient_set_spread(gradient, spread);
    resolve_gradient_stops(gradient, stops);
    return paint;
}

static plutovg_paint_t* resolve_radial_gradient(const render_context_t* context, const element_t* e)
{
    const element_t* stops = find_gradient_stops(context, e);
    if(stops == NULL)
        return NULL;

    length_t cx = {50, length_unit_percent};
    length_t cy = {50, length_unit_percent};
    length_t r = {50, length_unit_percent};
    length_t fx = {0, length_unit_unknown};
    length_t fy = {0, length_unit_unknown};

    parse_gradient_length(context, e, ID_CX, &cx, 1);
    parse_gradient_length(context, e, ID_CY, &cy, 1);
    parse_gradient_length(context, e, ID_R, &r, 0);
    parse_gradient_length(context, e, ID_FX, &fx, 1);
    parse_gradient_length(context, e, ID_FY, &fy, 1);

    plutovg_spread_method_t spread = find_gradient_spread(context, e);
    plutovg_matrix_t matrix = find_gradient_matrix(context, e);
    int units = find_gradient_units(context, e);

    if(units == units_type_object_bounding_box)
    {
        const render_state_t* state = context->state;
        plutovg_matrix_t m;
        plutovg_matrix_init_translate(&m, state->bbox.x, state->bbox.y);
        plutovg_matrix_scale(&m, state->bbox.w, state->bbox.h);
        plutovg_matrix_multiply(&matrix, &matrix, &m);
    }

    double _cx = resolve_gradient_length(context, &cx, units, 'x');
    double _cy = resolve_gradient_length(context, &cy, units, 'y');
    double _r = resolve_gradient_length(context, &r, units, 'o');
    double _fx = resolve_gradient_length(context, &fx, units, 'x');
    double _fy = resolve_gradient_length(context, &fy, units, 'y');

    if(!length_valid(fx)) _fx = _cx;
    if(!length_valid(fy)) _fy = _cy;

    plutovg_paint_t* paint = plutovg_paint_create_radial(_cx, _cy, _r, _fx, _fy, 0);
    plutovg_gradient_t* gradient = plutovg_paint_get_gradient(paint);
    plutovg_gradient_set_matrix(gradient, &matrix);
    plutovg_gradient_set_spread(gradient, spread);
    resolve_gradient_stops(gradient, stops);
    return paint;
}

static plutovg_paint_t* resolve_paint(const render_context_t* context, const paint_t* paint)
{
    if(paint->type == paint_type_none)
        return NULL;

    if(paint->type == paint_type_color)
        return plutovg_paint_create_color(&paint->color);
    if(paint->type == paint_type_current_color)
        return plutovg_paint_create_color(&context->state->color);

    const element_t* ref = resolve_iri(context, &paint->url);
    if(ref == NULL)
        return NULL;

    if(ref->id == TAG_SOLID_COLOR)
        return resolve_solid_color(ref);
    if(ref->id == TAG_LINEAR_GRADIENT)
        return resolve_linear_gradient(context, ref);
    if(ref->id == TAG_RADIAL_GRADIENT)
        return resolve_radial_gradient(context, ref);

    return NULL;
}

static void render_context_draw(render_context_t* context)
{
    render_state_t* state = context->state;
    style_t* style = &state->style;
    if(state->canvas == NULL || style->visibility == visibility_hidden)
        return;

    plutovg_t* pluto = state->canvas->pluto;
    plutovg_new_path(pluto);
    plutovg_add_path(pluto, context->path);
    plutovg_set_matrix(pluto, &state->matrix);

    plutovg_paint_t* fill = resolve_paint(context, &style->fill);
    if(fill)
    {
        plutovg_set_fill_rule(pluto, style->fillrule);
        plutovg_set_opacity(pluto, style->opacity * style->fillopacity);
        plutovg_set_source(pluto, fill);
        plutovg_fill_preserve(pluto);
    }

    plutovg_paint_t* stroke = resolve_paint(context, &style->stroke);
    if(stroke)
    {
        double linewidth = resolve_length(context, &style->linewidth, 'o');
        plutovg_set_line_width(pluto, linewidth);
        plutovg_set_line_cap(pluto, style->linecap);
        plutovg_set_line_join(pluto, style->linejoin);
        plutovg_set_miter_limit(pluto, style->miterlimit);
        plutovg_set_opacity(pluto, style->opacity * style->strokeopacity);
        plutovg_set_source(pluto, stroke);
        plutovg_stroke_preserve(pluto);
    }

    plutovg_paint_destroy(fill);
    plutovg_paint_destroy(stroke);
}

static int element_display_none(const element_t* e)
{
    int display = display_inline;
    parse_display(e, ID_DISPLAY, &display);
    return display == display_none;
}

static void render_element(render_context_t* context, const element_t* e);
static void render_children(render_context_t* context, const element_t* e);

static void render_symbol(render_context_t* context, const element_t* e, double x, double y, double w, double h)
{
    if(element_display_none(e))
        return;

    render_context_push(context, e);

    render_state_t* state = context->state;
    state->viewport.x = x;
    state->viewport.y = y;
    state->viewport.w = w;
    state->viewport.h = h;

    plutovg_rect_t viewbox;
    if(parse_view_box(e, ID_VIEW_BOX, &viewbox))
    {
        positioning_t position = {positioning_align_x_mid_y_mid, positioning_scale_meet};
        parse_positioning(e, ID_PRESERVE_ASPECT_RATIO, &position);

        plutovg_matrix_t matrix;
        positioning_get_matrix(&position, &matrix, &state->viewport, &viewbox);
        plutovg_matrix_multiply(&state->matrix, &matrix, &state->matrix);

        state->viewport = viewbox;
    }
    else
    {
        plutovg_matrix_translate(&state->matrix, x, y);
    }

    render_children(context, e);
    render_context_pop(context, e);
}

static void render_svg(render_context_t* context, const element_t* e)
{
    if(element_display_none(e))
        return;

    length_t w = {100, length_unit_percent};
    length_t h = {100, length_unit_percent};

    parse_length(e, ID_WIDTH, &w, 0);
    parse_length(e, ID_HEIGHT, &h, 0);

    if(length_zero(w) || length_zero(h))
        return;

    length_t x = {0, length_unit_px};
    length_t y = {0, length_unit_px};

    parse_length(e, ID_X, &x, 1);
    parse_length(e, ID_Y, &y, 1);

    double _x = resolve_length(context, &x, 'x');
    double _y = resolve_length(context, &y, 'y');
    double _w = resolve_length(context, &w, 'x');
    double _h = resolve_length(context, &h, 'y');

    render_symbol(context, e, _x, _y, _w, _h);
}

static void render_use(render_context_t* context, const element_t* e)
{
    if(element_display_none(e))
        return;

    const element_t* ref = resolve_href(context, e);
    if(ref == NULL)
        return;

    length_t w = {100, length_unit_percent};
    length_t h = {100, length_unit_percent};

    parse_length(e, ID_WIDTH, &w, 0);
    parse_length(e, ID_HEIGHT, &h, 0);

    if(length_zero(w) || length_zero(h))
        return;

    length_t x = {0, length_unit_px};
    length_t y = {0, length_unit_px};

    parse_length(e, ID_X, &x, 1);
    parse_length(e, ID_Y, &y, 1);

    render_context_push(context, e);

    double _x = resolve_length(context, &x, 'x');
    double _y = resolve_length(context, &y, 'y');
    double _w = resolve_length(context, &w, 'x');
    double _h = resolve_length(context, &h, 'y');

    plutovg_matrix_translate(&context->state->matrix, _x, _y);

    if(ref->id == TAG_SVG || ref->id == TAG_SYMBOL)
        render_symbol(context, ref, 0, 0, _w, _h);
    else
        render_element(context, ref);

    render_context_pop(context, e);
}

static void render_g(render_context_t* context, const element_t* e)
{
    if(element_display_none(e))
        return;

    render_context_push(context, e);
    render_children(context, e);
    render_context_pop(context, e);
}

static void render_line(render_context_t* context, const element_t* e)
{
    if(element_display_none(e))
        return;

    length_t x1 = {0, length_unit_px};
    length_t y1 = {0, length_unit_px};
    length_t x2 = {0, length_unit_px};
    length_t y2 = {0, length_unit_px};

    parse_length(e, ID_X1, &x1, 1);
    parse_length(e, ID_Y1, &y1, 1);
    parse_length(e, ID_X2, &x2, 1);
    parse_length(e, ID_Y2, &y2, 1);

    double _x1 = resolve_length(context, &x1, 'x');
    double _y1 = resolve_length(context, &y1, 'y');
    double _x2 = resolve_length(context, &x2, 'x');
    double _y2 = resolve_length(context, &y2, 'y');

    render_context_push(context, e);

    render_state_t* state = context->state;
    state->bbox.x = MIN(_x1, _x2);
    state->bbox.y = MIN(_y1, _y2);
    state->bbox.w = fabs(_x2 - _x1);
    state->bbox.h = fabs(_y2 - _y1);

    plutovg_path_t* path = context->path;
    plutovg_path_clear(path);
    plutovg_path_move_to(path, _x1, _y1);
    plutovg_path_line_to(path, _x2, _y2);
    render_context_draw(context);

    render_context_pop(context, e);
}

static void render_polyline(render_context_t* context, const element_t* e)
{
    if(element_display_none(e))
        return;

    plutovg_path_t* path = context->path;
    plutovg_path_clear(path);
    if(!parse_points(e, ID_POINTS, path, 0))
        return;

    render_context_push(context, e);

    render_state_t* state = context->state;
    path_bounding_box(path, &state->bbox);
    render_context_draw(context);

    render_context_pop(context, e);
}

static void render_polygon(render_context_t* context, const element_t* e)
{
    if(element_display_none(e))
        return;

    plutovg_path_t* path = context->path;
    plutovg_path_clear(path);
    if(!parse_points(e, ID_POINTS, path, 1))
        return;

    render_context_push(context, e);

    render_state_t* state = context->state;
    path_bounding_box(path, &state->bbox);
    render_context_draw(context);

    render_context_pop(context, e);
}

static void render_path(render_context_t* context, const element_t* e)
{
    if(element_display_none(e))
        return;

    plutovg_path_t* path = context->path;
    plutovg_path_clear(path);
    if(!parse_path(e, ID_D, path))
        return;

    render_context_push(context, e);

    render_state_t* state = context->state;
    path_bounding_box(path, &state->bbox);
    render_context_draw(context);

    render_context_pop(context, e);
}

static void render_ellipse(render_context_t* context, const element_t* e)
{
    if(element_display_none(e))
        return;

    length_t rx = {0, length_unit_px};
    length_t ry = {0, length_unit_px};

    parse_length(e, ID_RX, &rx, 0);
    parse_length(e, ID_RY, &ry, 0);

    if(length_zero(rx) || length_zero(ry))
        return;

    length_t cx = {0, length_unit_px};
    length_t cy = {0, length_unit_px};

    parse_length(e, ID_CX, &cx, 1);
    parse_length(e, ID_CY, &cy, 1);

    double _cx = resolve_length(context, &cx, 'x');
    double _cy = resolve_length(context, &cy, 'y');
    double _rx = resolve_length(context, &rx, 'x');
    double _ry = resolve_length(context, &ry, 'y');

    render_context_push(context, e);

    render_state_t* state = context->state;
    state->bbox.x = _cx - _rx;
    state->bbox.y = _cy - _ry;
    state->bbox.w = _rx + _rx;
    state->bbox.h = _ry + _ry;

    plutovg_path_t* path = context->path;
    plutovg_path_clear(path);
    plutovg_path_add_ellipse(path, _cx, _cy, _rx, _ry);
    render_context_draw(context);

    render_context_pop(context, e);
}

static void render_circle(render_context_t* context, const element_t* e)
{
    if(element_display_none(e))
        return;

    length_t r = {0, length_unit_px};
    parse_length(e, ID_R, &r, 0);
    if(length_zero(r))
        return;

    length_t cx = {0, length_unit_px};
    length_t cy = {0, length_unit_px};

    parse_length(e, ID_CX, &cx, 1);
    parse_length(e, ID_CY, &cy, 1);

    double _cx = resolve_length(context, &cx, 'x');
    double _cy = resolve_length(context, &cy, 'y');
    double _r = resolve_length(context, &r, 'o');

    render_context_push(context, e);

    render_state_t* state = context->state;
    state->bbox.x = _cx - _r;
    state->bbox.y = _cy - _r;
    state->bbox.w = _r + _r;
    state->bbox.h = _r + _r;

    plutovg_path_t* path = context->path;
    plutovg_path_clear(path);
    plutovg_path_add_circle(path, _cx, _cy, _r);
    render_context_draw(context);

    render_context_pop(context, e);
}

static void render_rect(render_context_t* context, const element_t* e)
{
    if(element_display_none(e))
        return;

    length_t w = {0, length_unit_px};
    length_t h = {0, length_unit_px};

    parse_length(e, ID_WIDTH, &w, 0);
    parse_length(e, ID_HEIGHT, &h, 0);

    if(length_zero(w) || length_zero(h))
        return;

    length_t x = {0, length_unit_px};
    length_t y = {0, length_unit_px};

    parse_length(e, ID_X, &x, 1);
    parse_length(e, ID_Y, &y, 1);

    double _x = resolve_length(context, &x, 'x');
    double _y = resolve_length(context, &y, 'y');
    double _w = resolve_length(context, &w, 'x');
    double _h = resolve_length(context, &h, 'y');

    length_t rx = {0, length_unit_unknown};
    length_t ry = {0, length_unit_unknown};

    parse_length(e, ID_RX, &rx, 0);
    parse_length(e, ID_RY, &ry, 0);

    double _rx = resolve_length(context, &rx, 'x');
    double _ry = resolve_length(context, &ry, 'y');

    if(!length_valid(rx)) _rx = _ry;
    if(!length_valid(ry)) _ry = _rx;

    render_context_push(context, e);

    render_state_t* state = context->state;
    state->bbox.x = _x;
    state->bbox.y = _y;
    state->bbox.w = _w;
    state->bbox.h = _h;

    plutovg_path_t* path = context->path;
    plutovg_path_clear(path);
    plutovg_path_add_round_rect(path, _x, _y, _w, _h, _rx, _ry);
    render_context_draw(context);

    render_context_pop(context, e);
}

static void render_element(render_context_t* context, const element_t* e)
{
    switch(e->id) {
    case TAG_SVG:
        render_svg(context, e);
        break;
    case TAG_USE:
        render_use(context, e);
        break;
    case TAG_G:
        render_g(context, e);
        break;
    case TAG_LINE:
        render_line(context, e);
        break;
    case TAG_POLYLINE:
        render_polyline(context, e);
        break;
    case TAG_POLYGON:
        render_polygon(context, e);
        break;
    case TAG_PATH:
        render_path(context, e);
        break;
    case TAG_ELLIPSE:
        render_ellipse(context, e);
        break;
    case TAG_CIRCLE:
        render_circle(context, e);
        break;
    case TAG_RECT:
        render_rect(context, e);
        break;
    }
}

static void render_children(render_context_t* context, const element_t* e)
{
    const element_t* child = e->first;
    while(child)
    {
        render_element(context, child);
        child = child->next;
    }
}

static void document_dimensions(document_t* document, plutovg_font_t* font, double* width, double* height, double dpi)
{
    length_t w = {0, length_unit_percent};
    length_t h = {0, length_unit_percent};

    parse_length(document->root, ID_WIDTH, &w, 0);
    parse_length(document->root, ID_HEIGHT, &h, 0);

    *width = length_value(&w, 1.0, dpi);
    *height = length_value(&h, 1.0, dpi);

    if(length_relative(&w) || length_relative(&h))
    {
        plutovg_rect_t viewbox = {0, 0, 0, 0};
        parse_view_box(document->root, ID_VIEW_BOX, &viewbox);

        if(length_relative(&w)) *width = viewbox.w;
        if(length_relative(&h)) *height = viewbox.h;
    }

    if(*width == 0.0 || *height == 0.0)
    {
        render_context_t* context = render_context_create(document, font, dpi);
        render_state_t* state = context->state;
        state->viewport.w = *width;
        state->viewport.h = *height;
        render_element(context, document->root);

        if(*width == 0.0) *width = state->bbox.w;
        if(*height == 0.0) *height = state->bbox.h;

        render_context_destroy(context);
    }
}

plutovg_surface_t* plutosvg_load_from_memory(const char* data, int size, plutovg_font_t* font, int width, int height, double dpi)
{
    document_t* document = parse_document(data, size);
    if(document == NULL)
        return NULL;

    double w, h;
    document_dimensions(document, font, &w, &h, dpi);
    if(w == 0.0 || h == 0.0)
    {
        document_destroy(document);
        return NULL;
    }

    if(width == 0 && height == 0)
    {
        width = (int)(ceil(w));
        height = (int)(ceil(h));
    }
    else if(width != 0 && height == 0)
    {
        height = (int)(ceil(width * h / w));
    }
    else if(height != 0 && width == 0)
    {
        width = (int)(ceil(height * w / h));
    }

    render_context_t* context = render_context_create(document, font, dpi);
    plutovg_surface_t* surface = plutovg_surface_create(width, height);
    render_state_t* state = context->state;
    plutovg_matrix_scale(&state->matrix, width / w, height / h);
    state->canvas = canvas_create_for_surface(surface);
    state->viewport.w = w;
    state->viewport.h = h;

    render_element(context, document->root);
    render_context_destroy(context);
    document_destroy(document);
    return surface;
}

plutovg_surface_t* plutosvg_load_from_file(const char* filename, plutovg_font_t* font, int width, int height, double dpi)
{
    FILE* fp = NULL;
    fopen_s(&fp, filename, "r");
    if(fp == NULL)
        return NULL;

    fseek(fp, 0, SEEK_END);
    size_t size = (size_t)ftell(fp);
    char* data = malloc(size);
    fseek(fp, 0, SEEK_SET);

    fread(data, size, 1, fp);
    fclose(fp);

    plutovg_surface_t* surface = plutosvg_load_from_memory(data, (int)size, font, width, height, dpi);
    free(data);
    return surface;
}

int plutosvg_dimensions_from_memory(const char* data, int size, plutovg_font_t* font, int* width, int* height, double dpi)
{
    document_t* document = parse_document(data, size);
    if(document == NULL)
        return 0;

    double w, h;
    document_dimensions(document, font, &w, &h, dpi);
    document_destroy(document);

    if(width) *width = (int)(ceil(w));
    if(height) *height = (int)(ceil(h));
    return 1;
}

int plutosvg_dimensions_from_file(const char* filename, plutovg_font_t* font, int* width, int* height, double dpi)
{
    FILE* fp = NULL;
    fopen_s(&fp, filename, "r");
    if(fp == NULL)
        return 0;

    fseek(fp, 0, SEEK_END);
    size_t size = (size_t)ftell(fp);
    char* data = malloc(size);
    fseek(fp, 0, SEEK_SET);

    fread(data, size, 1, fp);
    fclose(fp);

    int success = plutosvg_dimensions_from_memory(data, (int)size, font, width, height, dpi);
    free(data);
    return success;
}

#ifndef PLUTOVG_H
#define PLUTOVG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note plutovg_surface_t format is ARGB32_Premultiplied.
 */
typedef struct plutovg_surface {
    int ref;
    unsigned char *data;
    int owndata;
    int width;
    int height;
    int stride;
}plutovg_surface_t;
/**
 * @brief plutovg_surface_create
 * @param width
 * @param height
 * @return
 */
plutovg_surface_t* plutovg_surface_create(int width, int height);

/**
 * @brief plutovg_surface_create_for_data
 * @param data
 * @param width
 * @param height
 * @param stride
 * @return
 */
plutovg_surface_t* plutovg_surface_create_for_data(unsigned char* data, int width, int height, int stride);

/**
 * @brief plutovg_surface_reference
 * @param surface
 * @return
 */
plutovg_surface_t* plutovg_surface_reference(plutovg_surface_t* surface);

/**
 * @brief plutovg_surface_destroy
 * @param surface
 */
void plutovg_surface_destroy(plutovg_surface_t* surface);

/**
 * @brief plutovg_surface_get_reference_count
 * @param surface
 * @return
 */
int plutovg_surface_get_reference_count(const plutovg_surface_t* surface);

/**
 * @brief plutovg_surface_get_data
 * @param surface
 * @return
 */
unsigned char* plutovg_surface_get_data(const plutovg_surface_t* surface);

/**
 * @brief plutovg_surface_get_width
 * @param surface
 * @return
 */
int plutovg_surface_get_width(const plutovg_surface_t* surface);

/**
 * @brief plutovg_surface_get_height
 * @param surface
 * @return
 */
int plutovg_surface_get_height(const plutovg_surface_t* surface);

/**
 * @brief plutovg_surface_get_stride
 * @param surface
 * @return
 */
int plutovg_surface_get_stride(const plutovg_surface_t* surface);

/**
 * @brief plutovg_surface_write_to_png
 * @param surface
 * @param filename
 */
void plutovg_surface_write_to_png(const plutovg_surface_t* surface, const char* filename);

typedef struct plutovg_point plutovg_point_t;

struct plutovg_point {
    double x;
    double y;
};

typedef struct plutovg_rect plutovg_rect_t;

struct plutovg_rect {
    double x;
    double y;
    double w;
    double h;
};

/**
 * @brief plutovg_rect_init
 * @param rect
 * @param x
 * @param y
 * @param w
 * @param h
 */
void plutovg_rect_init(plutovg_rect_t* rect, double x, double y, double w, double h);

/**
 * @brief plutovg_rect_init_empty
 * @param rect
 */
void plutovg_rect_init_empty(plutovg_rect_t* rect);

/**
 * @brief plutovg_rect_init_invalid
 * @param rect
 */
void plutovg_rect_init_invalid(plutovg_rect_t* rect);

/**
 * @brief plutovg_rect_empty
 * @param rect
 * @return
 */
int plutovg_rect_empty(const plutovg_rect_t* rect);

/**
 * @brief plutovg_rect_invalid
 * @param rect
 * @return
 */
int plutovg_rect_invalid(const plutovg_rect_t* rect);

/**
 * @brief plutovg_rect_unite
 * @param rect
 * @param source
 */
void plutovg_rect_unite(plutovg_rect_t* rect, const plutovg_rect_t* source);

/**
 * @brief plutovg_rect_intersect
 * @param rect
 * @param source
 */
void plutovg_rect_intersect(plutovg_rect_t* rect, const plutovg_rect_t* source);

typedef struct plutovg_matrix plutovg_matrix_t;

struct plutovg_matrix {
    double m00; double m10;
    double m01; double m11;
    double m02; double m12;
};

/**
 * @brief plutovg_matrix_init
 * @param matrix
 * @param m00
 * @param m10
 * @param m01
 * @param m11
 * @param m02
 * @param m12
 */
void plutovg_matrix_init(plutovg_matrix_t* matrix, double m00, double m10, double m01, double m11, double m02, double m12);

/**
 * @brief plutovg_matrix_init_identity
 * @param matrix
 */
void plutovg_matrix_init_identity(plutovg_matrix_t* matrix);

/**
 * @brief plutovg_matrix_init_translate
 * @param matrix
 * @param x
 * @param y
 */
void plutovg_matrix_init_translate(plutovg_matrix_t* matrix, double x, double y);

/**
 * @brief plutovg_matrix_init_scale
 * @param matrix
 * @param x
 * @param y
 */
void plutovg_matrix_init_scale(plutovg_matrix_t* matrix, double x, double y);

/**
 * @brief plutovg_matrix_init_shear
 * @param matrix
 * @param x
 * @param y
 */
void plutovg_matrix_init_shear(plutovg_matrix_t* matrix, double x, double y);

/**
 * @brief plutovg_matrix_init_rotate
 * @param matrix
 * @param radians
 */
void plutovg_matrix_init_rotate(plutovg_matrix_t* matrix, double radians);

/**
 * @brief plutovg_matrix_init_rotate_translate
 * @param matrix
 * @param radians
 * @param x
 * @param y
 */
void plutovg_matrix_init_rotate_translate(plutovg_matrix_t* matrix, double radians, double x, double y);

/**
 * @brief plutovg_matrix_translate
 * @param matrix
 * @param x
 * @param y
 */
void plutovg_matrix_translate(plutovg_matrix_t* matrix, double x, double y);

/**
 * @brief plutovg_matrix_scale
 * @param matrix
 * @param x
 * @param y
 */
void plutovg_matrix_scale(plutovg_matrix_t* matrix, double x, double y);

/**
 * @brief plutovg_matrix_shear
 * @param matrix
 * @param x
 * @param y
 */
void plutovg_matrix_shear(plutovg_matrix_t* matrix, double x, double y);

/**
 * @brief plutovg_matrix_rotate
 * @param matrix
 * @param radians
 */
void plutovg_matrix_rotate(plutovg_matrix_t* matrix, double radians);

/**
 * @brief plutovg_matrix_rotate_translate
 * @param matrix
 * @param radians
 * @param x
 * @param y
 */
void plutovg_matrix_rotate_translate(plutovg_matrix_t* matrix, double radians, double x, double y);

/**
 * @brief plutovg_matrix_multiply
 * @param matrix
 * @param a
 * @param b
 */
void plutovg_matrix_multiply(plutovg_matrix_t* matrix, const plutovg_matrix_t* a, const plutovg_matrix_t* b);

/**
 * @brief plutovg_matrix_invert
 * @param matrix
 * @return
 */
int plutovg_matrix_invert(plutovg_matrix_t* matrix);

/**
 * @brief plutovg_matrix_map
 * @param matrix
 * @param x
 * @param y
 * @param _x
 * @param _y
 */
void plutovg_matrix_map(const plutovg_matrix_t* matrix, double x, double y, double* _x, double* _y);

/**
 * @brief plutovg_matrix_map_point
 * @param matrix
 * @param src
 * @param dst
 */
void plutovg_matrix_map_point(const plutovg_matrix_t* matrix, const plutovg_point_t* src, plutovg_point_t* dst);

/**
 * @brief plutovg_matrix_map_rect
 * @param matrix
 * @param src
 * @param dst
 */
void plutovg_matrix_map_rect(const plutovg_matrix_t* matrix, const plutovg_rect_t* src, plutovg_rect_t* dst);

typedef char plutovg_path_element_t;

enum {
    plutovg_path_element_move_to,
    plutovg_path_element_line_to,
    plutovg_path_element_cubic_to,
    plutovg_path_element_close
};

typedef struct plutovg_path plutovg_path_t;

/**
 * @brief plutovg_path_create
 * @return
 */
plutovg_path_t* plutovg_path_create(void);

/**
 * @brief plutovg_path_reference
 * @param path
 * @return
 */
plutovg_path_t* plutovg_path_reference(plutovg_path_t* path);

/**
 * @brief plutovg_path_destroy
 * @param path
 */
void plutovg_path_destroy(plutovg_path_t* path);

/**
 * @brief plutovg_path_get_reference_count
 * @param path
 * @return
 */
int plutovg_path_get_reference_count(const plutovg_path_t* path);

/**
 * @brief plutovg_path_move_to
 * @param path
 * @param x
 * @param y
 */
void plutovg_path_move_to(plutovg_path_t* path, double x, double y);

/**
 * @brief plutovg_path_line_to
 * @param path
 * @param x
 * @param y
 */
void plutovg_path_line_to(plutovg_path_t* path, double x, double y);

/**
 * @brief plutovg_path_quad_to
 * @param path
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void plutovg_path_quad_to(plutovg_path_t* path, double x1, double y1, double x2, double y2);

/**
 * @brief plutovg_path_cubic_to
 * @param path
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param x3
 * @param y3
 */
void plutovg_path_cubic_to(plutovg_path_t* path, double x1, double y1, double x2, double y2, double x3, double y3);

/**
 * @brief plutovg_path_arc_to
 * @param path
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param radius
 */
void plutovg_path_arc_to(plutovg_path_t* path, double x1, double y1, double x2, double y2, double radius);

/**
 * @brief plutovg_path_close
 * @param path
 */
void plutovg_path_close(plutovg_path_t* path);

/**
 * @brief plutovg_path_rel_move_to
 * @param path
 * @param dx
 * @param dy
 */
void plutovg_path_rel_move_to(plutovg_path_t* path, double dx, double dy);

/**
 * @brief plutovg_path_rel_line_to
 * @param path
 * @param dx
 * @param dy
 */
void plutovg_path_rel_line_to(plutovg_path_t* path, double dx, double dy);

/**
 * @brief plutovg_path_rel_quad_to
 * @param path
 * @param dx1
 * @param dy1
 * @param dx2
 * @param dy2
 */
void plutovg_path_rel_quad_to(plutovg_path_t* path, double dx1, double dy1, double dx2, double dy2);

/**
 * @brief plutovg_path_rel_cubic_to
 * @param path
 * @param dx1
 * @param dy1
 * @param dx2
 * @param dy2
 * @param dx3
 * @param dy3
 */
void plutovg_path_rel_cubic_to(plutovg_path_t* path, double dx1, double dy1, double dx2, double dy2, double dx3, double dy3);

/**
 * @brief plutovg_path_rel_arc_to
 * @param path
 * @param dx1
 * @param dy1
 * @param dx2
 * @param dy2
 * @param radius
 */
void plutovg_path_rel_arc_to(plutovg_path_t* path, double dx1, double dy1, double dx2, double dy2, double radius);

/**
 * @brief plutovg_path_add_rect
 * @param path
 * @param x
 * @param y
 * @param w
 * @param h
 */
void plutovg_path_add_rect(plutovg_path_t* path, double x, double y, double w, double h);

/**
 * @brief plutovg_path_add_round_rect
 * @param path
 * @param x
 * @param y
 * @param w
 * @param h
 * @param rx
 * @param ry
 */
void plutovg_path_add_round_rect(plutovg_path_t* path, double x, double y, double w, double h, double rx, double ry);

/**
 * @brief plutovg_path_add_ellipse
 * @param path
 * @param cx
 * @param cy
 * @param rx
 * @param ry
 */
void plutovg_path_add_ellipse(plutovg_path_t* path, double cx, double cy, double rx, double ry);

/**
 * @brief plutovg_path_add_circle
 * @param path
 * @param cx
 * @param cy
 * @param r
 */
void plutovg_path_add_circle(plutovg_path_t* path, double cx, double cy, double r);

/**
 * @brief plutovg_path_add_arc
 * @param path
 * @param cx
 * @param cy
 * @param r
 * @param a0
 * @param a1
 * @param ccw
 */
void plutovg_path_add_arc(plutovg_path_t* path, double cx, double cy, double r, double a0, double a1, int ccw);

/**
 * @brief plutovg_path_add_path
 * @param path
 * @param source
 * @param matrix
 */
void plutovg_path_add_path(plutovg_path_t* path, const plutovg_path_t* source, const plutovg_matrix_t* matrix);

/**
 * @brief plutovg_path_transform
 * @param path
 * @param matrix
 */
void plutovg_path_transform(plutovg_path_t* path, const plutovg_matrix_t* matrix);

/**
 * @brief plutovg_path_get_current_point
 * @param path
 * @param x
 * @param y
 */
void plutovg_path_get_current_point(const plutovg_path_t* path, double* x, double* y);

/**
 * @brief plutovg_path_get_element_count
 * @param path
 * @return
 */
int plutovg_path_get_element_count(const plutovg_path_t* path);

/**
 * @brief plutovg_path_get_elements
 * @param path
 * @return
 */
plutovg_path_element_t* plutovg_path_get_elements(const plutovg_path_t* path);

/**
 * @brief plutovg_path_get_point_count
 * @param path
 * @return
 */
int plutovg_path_get_point_count(const plutovg_path_t* path);

/**
 * @brief plutovg_path_get_points
 * @param path
 * @return
 */
plutovg_point_t* plutovg_path_get_points(const plutovg_path_t* path);

/**
 * @brief plutovg_path_clear
 * @param path
 */
void plutovg_path_clear(plutovg_path_t* path);

/**
 * @brief plutovg_path_empty
 * @param path
 * @return
 */
int plutovg_path_empty(const plutovg_path_t* path);

/**
 * @brief plutovg_path_clone
 * @param path
 * @return
 */
plutovg_path_t* plutovg_path_clone(const plutovg_path_t* path);

/**
 * @brief plutovg_path_clone_flat
 * @param path
 * @return
 */
plutovg_path_t* plutovg_path_clone_flat(const plutovg_path_t* path);

typedef struct plutovg_color plutovg_color_t;

struct plutovg_color {
    double r;
    double g;
    double b;
    double a;
};

/**
 * @brief plutovg_color_init_rgb
 * @param color
 * @param r
 * @param g
 * @param b
 */
void plutovg_color_init_rgb(plutovg_color_t* color, double r, double g, double b);

/**
 * @brief plutovg_color_init_rgba
 * @param color
 * @param r
 * @param g
 * @param b
 * @param a
 */
void plutovg_color_init_rgba(plutovg_color_t* color, double r, double g, double b, double a);

/**
 * @brief plutovg_color_init_rgb8
 * @param color
 * @param r
 * @param g
 * @param b
 */
void plutovg_color_init_rgb8(plutovg_color_t* color, unsigned char r, unsigned char g, unsigned char b);

/**
 * @brief plutovg_color_init_rgba8
 * @param color
 * @param r
 * @param g
 * @param b
 * @param a
 */
void plutovg_color_init_rgba8(plutovg_color_t* color, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

typedef int plutovg_spread_method_t;

enum {
    plutovg_spread_method_pad,
    plutovg_spread_method_reflect,
    plutovg_spread_method_repeat
};

typedef int plutovg_gradient_type_t;

enum {
    plutovg_gradient_type_linear,
    plutovg_gradient_type_radial
};

typedef struct plutovg_gradient_stop plutovg_gradient_stop_t;

struct plutovg_gradient_stop {
    double offset;
    plutovg_color_t color;
};

typedef struct plutovg_gradient plutovg_gradient_t;

/**
 * @brief plutovg_gradient_create_linear
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @return
 */
plutovg_gradient_t* plutovg_gradient_create_linear(double x1, double y1, double x2, double y2);

/**
 * @brief plutovg_gradient_create_radial
 * @param cx
 * @param cy
 * @param cr
 * @param fx
 * @param fy
 * @param fr
 * @return
 */
plutovg_gradient_t* plutovg_gradient_create_radial(double cx, double cy, double cr, double fx, double fy, double fr);

/**
 * @brief plutovg_gradient_reference
 * @param gradient
 * @return
 */
plutovg_gradient_t* plutovg_gradient_reference(plutovg_gradient_t* gradient);

/**
 * @brief plutovg_gradient_destroy
 * @param gradient
 */
void plutovg_gradient_destroy(plutovg_gradient_t* gradient);

/**
 * @brief plutovg_gradient_get_reference_count
 * @param gradient
 * @return
 */
int plutovg_gradient_get_reference_count(const plutovg_gradient_t* gradient);

/**
 * @brief plutovg_gradient_set_spread
 * @param gradient
 * @param spread
 */
void plutovg_gradient_set_spread(plutovg_gradient_t* gradient, plutovg_spread_method_t spread);

/**
 * @brief plutovg_gradient_get_spread
 * @param gradient
 * @return
 */
plutovg_spread_method_t plutovg_gradient_get_spread(const plutovg_gradient_t* gradient);

/**
 * @brief plutovg_gradient_set_matrix
 * @param gradient
 * @param matrix
 */
void plutovg_gradient_set_matrix(plutovg_gradient_t* gradient, const plutovg_matrix_t* matrix);

/**
 * @brief plutovg_gradient_get_matrix
 * @param gradient
 * @param matrix
 */
void plutovg_gradient_get_matrix(const plutovg_gradient_t* gradient, plutovg_matrix_t* matrix);

/**
 * @brief plutovg_gradient_add_stop_rgb
 * @param gradient
 * @param offset
 * @param r
 * @param g
 * @param b
 */
void plutovg_gradient_add_stop_rgb(plutovg_gradient_t* gradient, double offset, double r, double g, double b);

/**
 * @brief plutovg_gradient_add_stop_rgba
 * @param gradient
 * @param offset
 * @param r
 * @param g
 * @param b
 * @param a
 */
void plutovg_gradient_add_stop_rgba(plutovg_gradient_t* gradient, double offset, double r, double g, double b, double a);

/**
 * @brief plutovg_gradient_add_stop_color
 * @param gradient
 * @param offset
 * @param color
 */
void plutovg_gradient_add_stop_color(plutovg_gradient_t* gradient, double offset, const plutovg_color_t* color);

/**
 * @brief plutovg_gradient_add_stop
 * @param gradient
 * @param stop
 */
void plutovg_gradient_add_stop(plutovg_gradient_t* gradient, const plutovg_gradient_stop_t* stop);

/**
 * @brief plutovg_gradient_clear_stops
 * @param gradient
 */
void plutovg_gradient_clear_stops(plutovg_gradient_t* gradient);

/**
 * @brief plutovg_gradient_get_stop_count
 * @param gradient
 * @return
 */
int plutovg_gradient_get_stop_count(const plutovg_gradient_t* gradient);

/**
 * @brief plutovg_gradient_get_stops
 * @param gradient
 * @return
 */
plutovg_gradient_stop_t* plutovg_gradient_get_stops(const plutovg_gradient_t* gradient);

/**
 * @brief plutovg_gradient_get_type
 * @param gradient
 * @return
 */
plutovg_gradient_type_t plutovg_gradient_get_type(const plutovg_gradient_t* gradient);

/**
 * @brief plutovg_gradient_get_values_linear
 * @param gradient
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void plutovg_gradient_get_values_linear(const plutovg_gradient_t* gradient, double* x1, double* y1, double* x2, double* y2);

/**
 * @brief plutovg_gradient_get_values_radial
 * @param gradient
 * @param cx
 * @param cy
 * @param cr
 * @param fx
 * @param fy
 * @param fr
 */
void plutovg_gradient_get_values_radial(const plutovg_gradient_t* gradient, double* cx, double* cy, double* cr, double* fx, double* fy, double* fr);

/**
 * @brief plutovg_gradient_set_values_linear
 * @param gradient
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void plutovg_gradient_set_values_linear(plutovg_gradient_t* gradient, double x1, double y1, double x2, double y2);

/**
 * @brief plutovg_gradient_set_values_radial
 * @param gradient
 * @param cx
 * @param cy
 * @param cr
 * @param fx
 * @param fy
 * @param fr
 */
void plutovg_gradient_set_values_radial(plutovg_gradient_t* gradient, double cx, double cy, double cr, double fx, double fy, double fr);

/**
 * @brief plutovg_gradient_set_opacity
 * @param paint
 * @param opacity
 */
void plutovg_gradient_set_opacity(plutovg_gradient_t* paint, double opacity);

/**
 * @brief plutovg_gradient_get_opacity
 * @param paint
 * @return
 */
double plutovg_gradient_get_opacity(const plutovg_gradient_t* paint);

typedef int plutovg_texture_type_t;

enum {
    plutovg_texture_type_plain,
    plutovg_texture_type_tiled
};

typedef struct plutovg_texture plutovg_texture_t;

/**
 * @brief plutovg_texture_create
 * @param surface
 * @return
 */
plutovg_texture_t* plutovg_texture_create(plutovg_surface_t* surface);

/**
 * @brief plutovg_texture_reference
 * @param texture
 * @return
 */
plutovg_texture_t* plutovg_texture_reference(plutovg_texture_t* texture);

/**
 * @brief plutovg_texture_destroy
 * @param texture
 */
void plutovg_texture_destroy(plutovg_texture_t* texture);

/**
 * @brief plutovg_texture_get_reference_count
 * @param texture
 * @return
 */
int plutovg_texture_get_reference_count(const plutovg_texture_t* texture);

/**
 * @brief plutovg_texture_set_type
 * @param texture
 * @param type
 */
void plutovg_texture_set_type(plutovg_texture_t* texture, plutovg_texture_type_t type);

/**
 * @brief plutovg_texture_get_type
 * @param texture
 * @return
 */
plutovg_texture_type_t plutovg_texture_get_type(const plutovg_texture_t* texture);

/**
 * @brief plutovg_texture_set_matrix
 * @param texture
 * @param matrix
 */
void plutovg_texture_set_matrix(plutovg_texture_t* texture, const plutovg_matrix_t* matrix);

/**
 * @brief plutovg_texture_get_matrix
 * @param texture
 * @param matrix
 */
void plutovg_texture_get_matrix(const plutovg_texture_t* texture, plutovg_matrix_t* matrix);

/**
 * @brief plutovg_texture_set_surface
 * @param texture
 * @param surface
 */
void plutovg_texture_set_surface(plutovg_texture_t* texture, plutovg_surface_t* surface);

/**
 * @brief plutovg_texture_get_surface
 * @param texture
 * @return
 */
plutovg_surface_t* plutovg_texture_get_surface(const plutovg_texture_t* texture);

/**
 * @brief plutovg_texture_set_opacity
 * @param texture
 * @param opacity
 */
void plutovg_texture_set_opacity(plutovg_texture_t* texture, double opacity);

/**
 * @brief plutovg_texture_get_opacity
 * @param texture
 * @return
 */
double plutovg_texture_get_opacity(const plutovg_texture_t* texture);

typedef int plutovg_paint_type_t;

enum {
    plutovg_paint_type_color,
    plutovg_paint_type_gradient,
    plutovg_paint_type_texture
};

typedef struct plutovg_paint plutovg_paint_t;

/**
 * @brief plutovg_paint_create_rgb
 * @param r
 * @param g
 * @param b
 * @return
 */
plutovg_paint_t* plutovg_paint_create_rgb(double r, double g, double b);

/**
 * @brief plutovg_paint_create_rgba
 * @param r
 * @param g
 * @param b
 * @param a
 * @return
 */
plutovg_paint_t* plutovg_paint_create_rgba(double r, double g, double b, double a);

/**
 * @brief plutovg_paint_create_linear
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @return
 */
plutovg_paint_t* plutovg_paint_create_linear(double x1, double y1, double x2, double y2);

/**
 * @brief plutovg_paint_create_radial
 * @param cx
 * @param cy
 * @param cr
 * @param fx
 * @param fy
 * @param fr
 * @return
 */
plutovg_paint_t* plutovg_paint_create_radial(double cx, double cy, double cr, double fx, double fy, double fr);

/**
 * @brief plutovg_paint_create_for_surface
 * @param surface
 * @return
 */
plutovg_paint_t* plutovg_paint_create_for_surface(plutovg_surface_t* surface);

/**
 * @brief plutovg_paint_create_color
 * @param color
 * @return
 */
plutovg_paint_t* plutovg_paint_create_color(const plutovg_color_t* color);

/**
 * @brief plutovg_paint_create_gradient
 * @param gradient
 * @return
 */
plutovg_paint_t* plutovg_paint_create_gradient(plutovg_gradient_t* gradient);

/**
 * @brief plutovg_paint_create_texture
 * @param texture
 * @return
 */
plutovg_paint_t* plutovg_paint_create_texture(plutovg_texture_t* texture);

/**
 * @brief plutovg_paint_reference
 * @param paint
 * @return
 */
plutovg_paint_t* plutovg_paint_reference(plutovg_paint_t* paint);

/**
 * @brief plutovg_paint_destroy
 * @param paint
 */
void plutovg_paint_destroy(plutovg_paint_t* paint);

/**
 * @brief plutovg_paint_get_reference_count
 * @param paint
 * @return
 */
int plutovg_paint_get_reference_count(const plutovg_paint_t* paint);

/**
 * @brief plutovg_paint_get_type
 * @param paint
 * @return
 */
plutovg_paint_type_t plutovg_paint_get_type(const plutovg_paint_t* paint);

/**
 * @brief plutovg_paint_get_color
 * @param paint
 * @return
 */
plutovg_color_t* plutovg_paint_get_color(const plutovg_paint_t* paint);

/**
 * @brief plutovg_paint_get_gradient
 * @param paint
 * @return
 */
plutovg_gradient_t* plutovg_paint_get_gradient(const plutovg_paint_t* paint);

/**
 * @brief plutovg_paint_get_texture
 * @param paint
 * @return
 */
plutovg_texture_t* plutovg_paint_get_texture(const plutovg_paint_t* paint);

typedef int plutovg_font_style_t;

enum {
    plutovg_font_style_normal,
    plutovg_font_style_italic,
    plutovg_font_style_bold,
    plutovg_font_style_italic_bold
};

typedef struct plutovg_font_data_t plutovg_font_data_t;

/**
 * @brief plutovg_font_data_load_from_memory
 * @param data
 * @param datasize
 * @param owndata
 * @return
 */
plutovg_font_data_t* plutovg_font_data_load_from_memory(const unsigned char* data, int datasize, int owndata);

/**
 * @brief plutovg_font_data_load_from_file
 * @param filename
 * @return
 */
plutovg_font_data_t* plutovg_font_data_load_from_file(const char* filename);

/**
 * @brief plutovg_font_data_reference
 * @param data
 * @return
 */
plutovg_font_data_t* plutovg_font_data_reference(plutovg_font_data_t* data);

/**
 * @brief plutovg_font_data_destroy
 * @param data
 */
void plutovg_font_data_destroy(plutovg_font_data_t* data);

/**
 * @brief plutovg_font_data_get_reference_count
 * @param data
 * @return
 */
int plutovg_font_data_get_reference_count(const plutovg_font_data_t* data);

/**
 * @brief plutovg_font_data_face_count
 * @param data
 * @return
 */
int plutovg_font_data_face_count(const plutovg_font_data_t* data);

typedef struct plutovg_font_face plutovg_font_face_t;

/**
 * @brief plutovg_font_face_load_from_memory
 * @param data
 * @param datasize
 * @param owndata
 * @return
 */
plutovg_font_face_t* plutovg_font_face_load_from_memory(const unsigned char* data, int datasize, int owndata);

/**
 * @brief plutovg_font_face_load_from_file
 * @param filename
 * @return
 */
plutovg_font_face_t* plutovg_font_face_load_from_file(const char* filename);

/**
 * @brief plutovg_font_face_load_from_data
 * @param data
 * @param index
 * @return
 */
plutovg_font_face_t* plutovg_font_face_load_from_data(plutovg_font_data_t* data, int index);

/**
 * @brief plutovg_font_face_reference
 * @param face
 * @return
 */
plutovg_font_face_t* plutovg_font_face_reference(plutovg_font_face_t* face);

/**
 * @brief plutovg_font_face_destroy
 * @param face
 */
void plutovg_font_face_destroy(plutovg_font_face_t* face);

/**
 * @brief plutovg_font_face_get_reference_count
 * @param face
 * @return
 */
int plutovg_font_face_get_reference_count(const plutovg_font_face_t* face);

/**
 * @brief plutovg_font_face_get_style
 * @param face
 * @return
 */
plutovg_font_style_t plutovg_font_face_get_style(const plutovg_font_face_t* face);

/**
 * @brief plutovg_font_face_get_family
 * @param face
 * @return
 */
const char* plutovg_font_face_get_family(const plutovg_font_face_t* face);

/**
 * @brief plutovg_font_face_get_char_path
 * @param face
 * @param ch
 * @return
 */
plutovg_path_t* plutovg_font_face_get_char_path(const plutovg_font_face_t* face, int ch);

/**
 * @brief plutovg_font_face_get_char_extents
 * @param face
 * @param ch
 * @param rect
 */
void plutovg_font_face_get_char_extents(const plutovg_font_face_t* face, int ch, plutovg_rect_t* rect);

/**
 * @brief plutovg_font_face_get_char_advance
 * @param face
 * @param ch
 * @return
 */
double plutovg_font_face_get_char_advance(const plutovg_font_face_t* face, int ch);

/**
 * @brief plutovg_font_face_get_matrix
 * @param face
 * @param size
 * @param matrix
 */
void plutovg_font_face_get_matrix(const plutovg_font_face_t* face, double size, plutovg_matrix_t* matrix);

/**
 * @brief plutovg_font_face_get_scale
 * @param face
 * @param size
 * @return
 */
double plutovg_font_face_get_scale(const plutovg_font_face_t* face, double size);

/**
 * @brief plutovg_font_face_get_ascent
 * @param face
 * @return
 */
double plutovg_font_face_get_ascent(const plutovg_font_face_t* face);

/**
 * @brief plutovg_font_face_get_descent
 * @param face
 * @return
 */
double plutovg_font_face_get_descent(const plutovg_font_face_t* face);

/**
 * @brief plutovg_font_face_get_line_gap
 * @param face
 * @return
 */
double plutovg_font_face_get_line_gap(const plutovg_font_face_t* face);

/**
 * @brief plutovg_font_face_get_leading
 * @param face
 * @return
 */
double plutovg_font_face_get_leading(const plutovg_font_face_t* face);

/**
 * @brief plutovg_font_face_get_kerning
 * @param face
 * @param ch1
 * @param ch2
 * @return
 */
double plutovg_font_face_get_kerning(const plutovg_font_face_t* face, int ch1, int ch2);

/**
 * @brief plutovg_font_face_get_extents
 * @param face
 * @param rect
 */
void plutovg_font_face_get_extents(const plutovg_font_face_t* face, plutovg_rect_t* rect);

typedef struct plutovg_font plutovg_font_t;

/**
 * @brief plutovg_font_load_from_memory
 * @param data
 * @param datasize
 * @param owndata
 * @param size
 * @return
 */
plutovg_font_t* plutovg_font_load_from_memory(const unsigned char* data, int datasize, int owndata, double size);

/**
 * @brief plutovg_font_load_from_file
 * @param filename
 * @param size
 * @return
 */
plutovg_font_t* plutovg_font_load_from_file(const char* filename, double size);

/**
 * @brief plutovg_font_load_from_data
 * @param data
 * @param index
 * @param size
 * @return
 */
plutovg_font_t* plutovg_font_load_from_data(plutovg_font_data_t* data, int index, double size);

/**
 * @brief plutovg_font_load_from_face
 * @param face
 * @param size
 * @return
 */
plutovg_font_t* plutovg_font_load_from_face(plutovg_font_face_t* face, double size);

/**
 * @brief plutovg_font_reference
 * @param font
 * @return
 */
plutovg_font_t* plutovg_font_reference(plutovg_font_t* font);

/**
 * @brief plutovg_font_destroy
 * @param font
 */
void plutovg_font_destroy(plutovg_font_t* font);

/**
 * @brief plutovg_font_get_reference_count
 * @param font
 * @return
 */
int plutovg_font_get_reference_count(const plutovg_font_t* font);

/**
 * @brief plutovg_font_set_face
 * @param font
 * @param face
 */
void plutovg_font_set_face(plutovg_font_t* font, plutovg_font_face_t* face);

/**
 * @brief plutovg_font_get_face
 * @param font
 * @return
 */
plutovg_font_face_t* plutovg_font_get_face(const plutovg_font_t* font);

/**
 * @brief plutovg_font_set_size
 * @param font
 * @param size
 */
void plutovg_font_set_size(plutovg_font_t* font, double size);

/**
 * @brief plutovg_font_get_size
 * @param font
 * @return
 */
double plutovg_font_get_size(const plutovg_font_t* font);

/**
 * @brief plutovg_font_get_style
 * @param font
 * @return
 */
plutovg_font_style_t plutovg_font_get_style(const plutovg_font_t* font);

/**
 * @brief plutovg_font_get_family
 * @param font
 * @return
 */
const char* plutovg_font_get_family(const plutovg_font_t* font);

/**
 * @brief plutovg_font_get_scale
 * @param font
 * @return
 */
double plutovg_font_get_scale(const plutovg_font_t* font);

/**
 * @brief plutovg_font_get_ascent
 * @param font
 * @return
 */
double plutovg_font_get_ascent(const plutovg_font_t* font);

/**
 * @brief plutovg_font_get_descent
 * @param font
 * @return
 */
double plutovg_font_get_descent(const plutovg_font_t* font);

/**
 * @brief plutovg_font_get_line_gap
 * @param font
 * @return
 */
double plutovg_font_get_line_gap(const plutovg_font_t* font);

/**
 * @brief plutovg_font_get_leading
 * @param font
 * @return
 */
double plutovg_font_get_leading(const plutovg_font_t* font);

/**
 * @brief plutovg_font_get_kerning
 * @param font
 * @param ch1
 * @param ch2
 * @return
 */
double plutovg_font_get_kerning(const plutovg_font_t* font, int ch1, int ch2);

/**
 * @brief plutovg_font_get_char_advance
 * @param font
 * @param ch
 * @return
 */
double plutovg_font_get_char_advance(const plutovg_font_t* font, int ch);

/**
 * @brief plutovg_font_get_text_advance
 * @param font
 * @param utf8
 * @return
 */
double plutovg_font_get_text_advance(const plutovg_font_t* font, const char* utf8);

/**
 * @brief plutovg_font_get_textn_advance
 * @param font
 * @param utf8
 * @param size
 * @return
 */
double plutovg_font_get_textn_advance(const plutovg_font_t* font, const char* utf8, int size);

/**
 * @brief plutovg_font_get_char_extents
 * @param font
 * @param ch
 * @param rect
 */
void plutovg_font_get_char_extents(const plutovg_font_t* font, int ch, plutovg_rect_t* rect);

/**
 * @brief plutovg_font_get_text_extents
 * @param font
 * @param utf8
 * @param rect
 */
void plutovg_font_get_text_extents(const plutovg_font_t* font, const char* utf8, plutovg_rect_t* rect);

/**
 * @brief plutovg_font_get_textn_extents
 * @param font
 * @param utf8
 * @param size
 * @param rect
 */
void plutovg_font_get_textn_extents(const plutovg_font_t* font, const char* utf8, int size, plutovg_rect_t* rect);

/**
 * @brief plutovg_font_get_extents
 * @param font
 * @param rect
 */
void plutovg_font_get_extents(const plutovg_font_t* font, plutovg_rect_t* rect);

/**
 * @brief plutovg_font_get_char_path
 * @param font
 * @param ch
 * @return
 */
plutovg_path_t* plutovg_font_get_char_path(const plutovg_font_t* font, int ch);

/**
 * @brief plutovg_font_get_text_path
 * @param font
 * @param utf8
 * @return
 */
plutovg_path_t* plutovg_font_get_text_path(const plutovg_font_t* font, const char* utf8);

/**
 * @brief plutovg_font_get_textn_path
 * @param font
 * @param utf8
 * @param size
 * @return
 */
plutovg_path_t* plutovg_font_get_textn_path(const plutovg_font_t* font, const char* utf8, int size);

typedef int plutovg_line_cap_t;

enum {
    plutovg_line_cap_butt,
    plutovg_line_cap_round,
    plutovg_line_cap_square
};

typedef int plutovg_line_join_t;

enum {
    plutovg_line_join_miter,
    plutovg_line_join_round,
    plutovg_line_join_bevel
};

typedef int plutovg_fill_rule_t;

enum {
    plutovg_fill_rule_non_zero,
    plutovg_fill_rule_even_odd
};

typedef int plutovg_operator_t;

enum {
    plutovg_operator_src,
    plutovg_operator_src_over,
    plutovg_operator_dst_in,
    plutovg_operator_dst_out
};

typedef struct plutovg plutovg_t;

/**
 * @brief Creates a new rendering context for rendering to surface target.
 * @param surface - a target surface for the context
 * @return Returns a newly allocated context with a reference count of 1.
 */
plutovg_t* plutovg_create(plutovg_surface_t* surface);

/**
 * @brief Increases the reference count of the context by 1.
 * @param pluto - a pluto context
 * @return Returns the referenced context.
 */
plutovg_t* plutovg_reference(plutovg_t* pluto);

/**
 * @brief Decrements the reference count for the context by 1.
 * If the reference count on the context falls to 0, the context is freed.
 * @param pluto - a pluto context
 */
void plutovg_destroy(plutovg_t* pluto);

/**
 * @brief Gets the reference count of the context.
 * @param pluto - a pluto context
 * @return Returns the reference count of the context.
 */
int plutovg_get_reference_count(const plutovg_t* pluto);

/**
 * @brief Saves the entire state of the context by pushing the current state onto a stack.
 * @param pluto - a pluto context
 */
void plutovg_save(plutovg_t* pluto);

/**
 * @brief Restores the most recently saved canvas state by popping the top entry in the drawing state stack.
 * @param pluto - a pluto context
 */
void plutovg_restore(plutovg_t* pluto);

/**
 * @brief Sets the source paint of the context to an opaque color.
 * The color components are floating point numbers in the range 0 to 1.
 * @param pluto - a pluto context
 * @param r - red component of color
 * @param g - green component of color
 * @param b - blue component of color
 */
void plutovg_set_source_rgb(plutovg_t* pluto, double r, double g, double b);

/**
 * @brief Sets the source paint of the context to a translucent color.
 * The color and alpha components are floating point numbers in the range 0 to 1.
 * @param pluto - a pluto context
 * @param r - red component of color
 * @param g - green component of color
 * @param b - blue component of color
 * @param a - alpha component of color
 */
void plutovg_set_source_rgba(plutovg_t* pluto, double r, double g, double b, double a);

/**
 * @brief Sets the source paint of the context to a texture.
 * @param pluto - a pluto context
 * @param surface - a surface to be used to set the source paint
 * @param x - user-space x coordinate for surface origin
 * @param y - user-space y coordinate for surface origin
 */
void plutovg_set_source_surface(plutovg_t* pluto, plutovg_surface_t* surface, double x, double y);

/**
 * @brief Sets the source paint of the context to a color.
 * @param pluto - a pluto context
 * @param color - a color to be used to set the source paint
 */
void plutovg_set_source_color(plutovg_t* pluto, const plutovg_color_t* color);

/**
 * @brief Sets the source paint of the context to a gradient.
 * @param pluto - a pluto context
 * @param gradient - a gradient to be used to set the source paint
 */
void plutovg_set_source_gradient(plutovg_t* pluto, plutovg_gradient_t* gradient);

/**
 * @brief Sets the source paint of the context to a texture.
 * @param pluto - a pluto context
 * @param texture - a texture to be used to set the source paint
 */
void plutovg_set_source_texture(plutovg_t* pluto, plutovg_texture_t* texture);

/**
 * @brief Sets the source paint of the context to a paint.
 * @param pluto - a pluto context
 * @param source - a paint to be used a the source for any subsequent drawing operation.
 */
void plutovg_set_source(plutovg_t* pluto, plutovg_paint_t* source);

/**
 * @brief Gets the current source paint.
 * @param pluto - a pluto context
 * @return The current source paint
 */
plutovg_paint_t* plutovg_get_source(const plutovg_t* pluto);

/**
 * @brief Sets the compositing operator to be used for drawing operations.
 * @param pluto - a pluto context
 * @param op - a compositing operator
 */
void plutovg_set_operator(plutovg_t* pluto, plutovg_operator_t op);

/**
 * @brief Sets the opacity value that is applied to paints before they are drawn onto the context.
 * @param pluto - a pluto context
 * @param opacity - an opacity value is a floating point number in the range 0 to 1
 */
void plutovg_set_opacity(plutovg_t* pluto, double opacity);

/**
 * @brief Sets the fill rule to be used for filling and clipping operations.
 * @param pluto - a pluto context
 * @param winding - a fill rule
 */
void plutovg_set_fill_rule(plutovg_t* pluto, plutovg_fill_rule_t winding);

/**
 * @brief Gets the current compositing operator.
 * @param pluto - a pluto context
 * @return Returns the current compositing operator.
 */
plutovg_operator_t plutovg_get_operator(const plutovg_t* pluto);

/**
 * @brief Gets the current opacity.
 * @param pluto - a pluto context
 * @return Returns the current opacity.
 */
double plutovg_get_opacity(const plutovg_t* pluto);

/**
 * @brief Gets the current fill rule.
 * @param pluto - a pluto context
 * @return Returns the current fill rule.
 */
plutovg_fill_rule_t plutovg_get_fill_rule(const plutovg_t* pluto);

/**
 * @brief Sets the current line width.
 * @param pluto - a pluto context
 * @param width - a line width
 */
void plutovg_set_line_width(plutovg_t* pluto, double width);

/**
 * @brief Sets the current line cap.
 * @param pluto - a pluto context
 * @param cap - a line cap style
 */
void plutovg_set_line_cap(plutovg_t* pluto, plutovg_line_cap_t cap);

/**
 * @brief Sets the current line join.
 * @param pluto - a pluto context
 * @param cap - a line join style
 */
void plutovg_set_line_join(plutovg_t* pluto, plutovg_line_join_t join);

/**
 * @brief Sets the current miter limit.
 * @param pluto - a pluto context
 * @param limit - a miter limit
 */
void plutovg_set_miter_limit(plutovg_t* pluto, double limit);

/**
 * @brief Sets the current dash pattern.
 * @param pluto - a pluto context
 * @param offset - an offset into the dash pattern at which the stroke start
 * @param data - an array specifying alternate lengths of lines and gaps which describe the pattern
 * @param size - the length of the data array
 */
void plutovg_set_dash(plutovg_t* pluto, double offset, const double* data, int size);

/**
* @brief Gets the current line width.
* @param pluto - a pluto context
* @return Returns the current line width.
 */
double plutovg_get_line_width(const plutovg_t* pluto);

/**
* @brief Gets the current line cap.
* @param pluto - a pluto context
* @return Returns the current line cap.
 */
plutovg_line_cap_t plutovg_get_line_cap(const plutovg_t* pluto);

/**
* @brief Gets the current line join.
* @param pluto - a pluto context
* @return Returns the current line join.
 */
plutovg_line_join_t plutovg_get_line_join(const plutovg_t* pluto);

/**
 * @brief Gets the current miter limit.
 * @param pluto - a pluto context
 * @return Returns the current miter limit.
*/
double plutovg_get_miter_limit(const plutovg_t* pluto);

/**
 * @brief plutovg_get_dash_offset
 * @param pluto
 * @return
 */
double plutovg_get_dash_offset(const plutovg_t* pluto);

/**
 * @brief plutovg_get_dash_data
 * @param pluto
 * @return
 */
const double* plutovg_get_dash_data(const plutovg_t* pluto);

/**
 * @brief plutovg_get_dash_count
 * @param pluto
 * @return
 */
int plutovg_get_dash_count(const plutovg_t* pluto);

/**
 * @brief Adds a translation transformation to the current matrix.
 * @param pluto - a pluto context
 * @param x - amount to translate in the x direction
 * @param y - amount to translate in the y direction
 */
void plutovg_translate(plutovg_t* pluto, double x, double y);

/**
 * @brief Adds a scale transformation to the current matrix.
 * @param pluto - a pluto context
 * @param x - scale factor for x dimension
 * @param y - scale factor for y dimension
 */
void plutovg_scale(plutovg_t* pluto, double x, double y);

/**
 * @brief Adds a rotation transformation to the current matrix.
 * @param pluto - a pluto context
 * @param radians - angle in radians by which the matrix will be rotated.
 */
void plutovg_rotate(plutovg_t* pluto, double radians);

/**
 * @brief Adds a transformation matrix to the current matrix.
 * @param pluto - a pluto context
 * @param matrix - a transformation matrix
 */
void plutovg_transform(plutovg_t* pluto, const plutovg_matrix_t* matrix);

/**
 * @brief Sets the current matrix.
 * @param pluto - a pluto context
 * @param matrix - a transformation matrix
 */
void plutovg_set_matrix(plutovg_t* pluto, const plutovg_matrix_t* matrix);

/**
 * @brief Resets the current matrix to identity
 * @param pluto - a pluto context
 */
void plutovg_identity_matrix(plutovg_t* pluto);

/**
 * @brief Gets the current matrix.
 * @param pluto - a pluto context
 * @return Returns the current matrix.
 */
void plutovg_get_matrix(const plutovg_t* pluto, plutovg_matrix_t* matrix);

/**
 * @brief Begins a new sub-path at the point specified by the given coordinates.
 * @param pluto - a pluto context
 * @param x - the x coordinate of the new position
 * @param y - the y coordinate of the new position
 */
void plutovg_move_to(plutovg_t* pluto, double x, double y);

/**
 * @brief Adds a straight line to the current sub-path by connecting the sub-path's last point to the given coordinates.
 * @param pluto - a pluto context
 * @param x - the x coordinate of the line's end point
 * @param y - the y coordinate of the line's end point
 */
void plutovg_line_to(plutovg_t* pluto, double x, double y);

/**
 * @brief Adds a quadratic Bézier curve to the current sub-path.
 * @param pluto - a pluto context
 * @param x1 - the x coordinate of the control point
 * @param y1 - the y coordinate of the control point
 * @param x2 - the x coordinate of the end point
 * @param y2 - the y coordinate of the end point
 */
void plutovg_quad_to(plutovg_t* pluto, double x1, double y1, double x2, double y2);

/**
 * @brief Adds a cubic Bézier curve to the current sub-path.
 * @param pluto - a pluto context
 * @param x1 - the x coordinate of the first control point
 * @param y1 - the y coordinate of the first control point
 * @param x2 - the x coordinate of the second control point
 * @param y2 - the y coordinate of the second control point
 * @param x3 - the x coordinate of the end point
 * @param y3 - the y coordinate of the end point
 */
void plutovg_cubic_to(plutovg_t* pluto, double x1, double y1, double x2, double y2, double x3, double y3);

/**
 * @brief plutovg_rel_move_to
 * @param pluto
 * @param dx
 * @param dy
 */
void plutovg_rel_move_to(plutovg_t* pluto, double dx, double dy);

/**
 * @brief plutovg_rel_line_to
 * @param pluto
 * @param dx
 * @param dy
 */
void plutovg_rel_line_to(plutovg_t* pluto, double dx, double dy);

/**
 * @brief plutovg_rel_quad_to
 * @param pluto
 * @param dx1
 * @param dy1
 * @param dx2
 * @param dy2
 */
void plutovg_rel_quad_to(plutovg_t* pluto, double dx1, double dy1, double dx2, double dy2);

/**
 * @brief plutovg_rel_cubic_to
 * @param pluto
 * @param dx1
 * @param dy1
 * @param dx2
 * @param dy2
 * @param dx3
 * @param dy3
 */
void plutovg_rel_cubic_to(plutovg_t* pluto, double dx1, double dy1, double dx2, double dy2, double dx3, double dy3);

/**
 * @brief plutovg_rect
 * @param pluto
 * @param x
 * @param y
 * @param w
 * @param h
 */
void plutovg_rect(plutovg_t* pluto, double x, double y, double w, double h);

/**
 * @brief plutovg_round_rect
 * @param pluto
 * @param x
 * @param y
 * @param w
 * @param h
 * @param rx
 * @param ry
 */
void plutovg_round_rect(plutovg_t* pluto, double x, double y, double w, double h, double rx, double ry);

/**
 * @brief plutovg_ellipse
 * @param pluto
 * @param cx
 * @param cy
 * @param rx
 * @param ry
 */
void plutovg_ellipse(plutovg_t* pluto, double cx, double cy, double rx, double ry);

/**
 * @brief plutovg_circle
 * @param pluto
 * @param cx
 * @param cy
 * @param r
 */
void plutovg_circle(plutovg_t* pluto, double cx, double cy, double r);

/**
 * @brief plutovg_arc
 * @param path
 * @param cx
 * @param cy
 * @param r
 * @param a0
 * @param a1
 * @param ccw
 */
void plutovg_arc(plutovg_t* path, double cx, double cy, double r, double a0, double a1, int ccw);

/**
 * @brief plutovg_add_path
 * @param pluto
 * @param path
 */
void plutovg_add_path(plutovg_t* pluto, const plutovg_path_t* path);

/**
 * @brief plutovg_new_path
 * @param pluto
 */
void plutovg_new_path(plutovg_t* pluto);

/**
 * @brief plutovg_close_path
 * @param pluto
 */
void plutovg_close_path(plutovg_t* pluto);

/**
 * @brief plutovg_get_current_point
 * @param pluto
 * @param x
 * @param y
 */
void plutovg_get_current_point(const plutovg_t* pluto, double* x, double* y);

/**
 * @brief plutovg_get_path
 * @param pluto
 * @return
 */
plutovg_path_t* plutovg_get_path(const plutovg_t* pluto);

/**
 * @brief plutovg_set_font
 * @param pluto
 * @param font
 */
void plutovg_set_font(plutovg_t* pluto, plutovg_font_t* font);

/**
 * @brief plutovg_set_font_size
 * @param pluto
 * @param size
 */
void plutovg_set_font_size(plutovg_t* pluto, double size);

/**
 * @brief plutovg_get_font
 * @param pluto
 * @return
 */
plutovg_font_t* plutovg_get_font(const plutovg_t* pluto);

/**
 * @brief plutovg_get_font_size
 * @param pluto
 * @return
 */
double plutovg_get_font_size(const plutovg_t* pluto);

/**
 * @brief plutovg_char
 * @param pluto
 * @param ch
 * @param x
 * @param y
 */
void plutovg_char(plutovg_t* pluto, int ch, double x, double y);

/**
 * @brief plutovg_text
 * @param pluto
 * @param utf8
 * @param x
 * @param y
 */
void plutovg_text(plutovg_t* pluto, const char* utf8, double x, double y);

/**
 * @brief plutovg_textn
 * @param pluto
 * @param utf8
 * @param size
 * @param x
 * @param y
 */
void plutovg_textn(plutovg_t* pluto, const char* utf8, int size, double x, double y);

/**
 * @brief plutovg_fill
 * @param pluto
 */
void plutovg_fill(plutovg_t* pluto);

/**
 * @brief plutovg_stroke
 * @param pluto
 */
void plutovg_stroke(plutovg_t* pluto);

/**
 * @brief plutovg_clip
 * @param pluto
 */
void plutovg_clip(plutovg_t* pluto);

/**
 * @brief plutovg_paint
 * @param pluto
 */
void plutovg_paint(plutovg_t* pluto);

/**
 * @brief plutovg_fill_preserve
 * @param pluto
 */
void plutovg_fill_preserve(plutovg_t* pluto);

/**
 * @brief plutovg_stroke_preserve
 * @param pluto
 */
void plutovg_stroke_preserve(plutovg_t* pluto);

/**
 * @brief plutovg_clip_preserve
 * @param pluto
 */
void plutovg_clip_preserve(plutovg_t* pluto);

/**
 * @brief plutovg_fill_extents
 * @param pluto
 * @param rect
 */
void plutovg_fill_extents(plutovg_t* pluto, plutovg_rect_t* rect);

/**
 * @brief plutovg_stroke_extents
 * @param pluto
 * @param rect
 */
void plutovg_stroke_extents(plutovg_t* pluto, plutovg_rect_t* rect);

/**
 * @brief plutovg_clip_extents
 * @param pluto
 * @param rect
 */
void plutovg_clip_extents(plutovg_t* pluto, plutovg_rect_t* rect);

/**
 * @brief Reset the current clip region to its original, unrestricted state.
 * @param pluto - a pluto context
 */
void plutovg_reset_clip(plutovg_t* pluto);

#ifdef __cplusplus
}
#endif

#endif // PLUTOVG_H

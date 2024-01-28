#pragma once


#include "plutosvg.h"

namespace Nilesoft
{
	namespace Shell
	{
		struct PlutoVG
		{
			plutovg_surface_t *surface{};
			plutovg_t *ctx{};

			PlutoVG()
			{
			}

			PlutoVG(int width, int height)
			{
				surface = plutovg_surface_create(width, height);
				ctx = plutovg_create(surface);
			}

			PlutoVG(const char *data, int size, int width, int height, double dpi = 96.0)
			{
				surface = plutosvg_load_from_memory(data, size, nullptr, width, height, dpi);
			}

			PlutoVG(const char *filename, int width, int height, double dpi = 96.0)
			{
				surface = plutosvg_load_from_file(filename, nullptr, width, height, dpi);
			}

			~PlutoVG()
			{
				destroy();
			}

			operator plutovg_surface_t *() { return surface; }
			operator plutovg_t *() { return ctx; }
			explicit operator bool() { return surface != nullptr; }

			uint8_t *data() { return surface->data; }
			int stride() const { return surface->stride; }

			int width() const { return surface->width; }
			int height() const { return surface->height; }

			PlutoVG &destroy()
			{
				if(surface) plutovg_surface_destroy(surface);
				if(ctx) plutovg_destroy(ctx);
				surface = nullptr;
				ctx = nullptr;
				return *this;
			}
			PlutoVG &create(int width, int height)
			{
				destroy();
				if(!surface)
					surface = plutovg_surface_create(width, height);
				if(surface)
					ctx = plutovg_create(surface);
				return *this;
			}
			PlutoVG &load_from_memory(const char *data, int size, int width, int height, double dpi = 96.0) { surface = plutosvg_load_from_memory(data, size, nullptr, width, height, dpi); return *this; }
			PlutoVG &load_from_file(const char *filename, int width = 0, int height = 0, double dpi = 96.0) { surface = plutosvg_load_from_file(filename, nullptr, width, height, dpi); return *this; }
			PlutoVG &ellipse(double cx, double cy, double rx, double ry) { plutovg_ellipse(ctx, cx, cy, rx, ry); return *this; }
			PlutoVG &circle(double cx, double cy, double r) { plutovg_circle(ctx, cx, cy, r); return *this; }
			PlutoVG &arc(double cx, double cy, double r, double a0, double a1, int ccw) { plutovg_arc(ctx, cx, cy, r, a0, a1, ccw); return *this; }
			PlutoVG &rect(double x, double y, double w, double h, double r = 0.0)
			{
				if(r == 0.0)
					plutovg_rect(ctx, x, y, w, h);
				else
					plutovg_round_rect(ctx, x, y, w, h, r, r);
				return *this;
			}
			PlutoVG &rect(double x, double y, double w, double h, double rx, double ry)
			{
				plutovg_round_rect(ctx, x, y, w, h, rx, ry);
				return *this;
			}
			PlutoVG &fill(bool preserve = false)
			{
				if(preserve) plutovg_fill_preserve(ctx);
				else plutovg_fill(ctx);
				return *this;
			}
			PlutoVG &fill(uint32_t rgb, uint8_t a = 255, bool preserve = false)
			{
				return source_color(rgb, a).fill(preserve);
			}
			PlutoVG &fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255, bool preserve = false)
			{
				return source_color(r, g, b, a).fill(preserve);
			}
			PlutoVG &fill(plutovg_gradient_t *gradient, bool preserve = false)
			{
				return source_color(gradient).fill(preserve);
			}
			PlutoVG &stroke(bool preserve = false) {
				if(preserve) plutovg_stroke_preserve(ctx);
				else plutovg_stroke(ctx);
				return *this;
			}
			PlutoVG &stroke_width(double width) { return line_width(width); }
			PlutoVG &stroke_fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255, bool preserve = false) {
				return source_color(r, g, b, a).stroke(preserve);
			}
			PlutoVG &stroke_fill(uint32_t rgb, uint8_t a = 255, bool preserve = false) {
				return source_color(rgb, a).stroke(preserve);
			}
			PlutoVG &clear(bool preserve = false, int op = plutovg_operator_dst_out) {
				auto old_op = get_operator();
				return set_operator(op).fill(0xffffffff, 0xff, preserve).set_operator(old_op);
			}
			PlutoVG &source_color(uint32_t rgba)
			{
				return source_color(get_color(rgba, 16), get_color(rgba, 8), get_color(rgba, 0), get_color(rgba, 24));
			}
			PlutoVG &source_color(uint32_t rgb, uint8_t a)
			{
				return source_color(get_color(rgb, 16), get_color(rgb, 8), get_color(rgb, 0), a);
			}
			PlutoVG &source_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
			{
				plutovg_set_source_rgba(ctx, double(r) / 255.0, double(g) / 255.0, double(b) / 255.0, double(a) / 255.0);
				return *this;
			}
			PlutoVG &source_color(plutovg_gradient_t *gradient)
			{
				plutovg_set_source_gradient(ctx, gradient);
				return *this;
			}
			PlutoVG &set_operator(plutovg_operator_t op)
			{
				plutovg_set_operator(ctx, op);
				return *this;
			}
			plutovg_operator_t get_operator() const
			{
				return plutovg_get_operator(ctx);
			}
			PlutoVG &set_fill_rule(plutovg_fill_rule_t winding)
			{
				plutovg_set_fill_rule(ctx, winding);
				return *this;
			}
			plutovg_fill_rule_t get_fill_rule() const
			{
				return plutovg_get_fill_rule(ctx);
			}
			plutovg_gradient_t *gradient_linear(double x1, double y1, double x2, double y2)
			{
				return plutovg_gradient_create_linear(x1, y1, x2, y2);
			}
			PlutoVG &gradient_add_stop(plutovg_gradient_t *gradient, double offset, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
			{
				plutovg_gradient_add_stop_rgba(gradient, offset, double(r / 255.0), double(g / 255.0), double(b / 255.0), double(a / 255.0));
				return *this;
			}
			PlutoVG &gradient_add_stop(plutovg_gradient_t *gradient, double offset, uint32_t rgb, uint8_t a = 255)
			{
				return gradient_add_stop(gradient, offset, get_color(rgb, 16), get_color(rgb, 8), get_color(rgb, 0), a);
			}
			PlutoVG &clip(bool preserve = false) { if(preserve) plutovg_clip_preserve(ctx); else plutovg_clip(ctx); return *this; }
			PlutoVG &paint() { plutovg_paint(ctx); return *this; }
			PlutoVG &save() { plutovg_save(ctx); return *this; }
			PlutoVG &restore() { plutovg_restore(ctx); return *this; }
			PlutoVG &new_path() { plutovg_new_path(ctx); return *this; }
			PlutoVG &line_width(double width) { plutovg_set_line_width(ctx, width); return *this; }
			PlutoVG &line_cap(plutovg_line_cap_t cap) { plutovg_set_line_cap(ctx, cap); return *this; }
			PlutoVG &line_join(plutovg_line_join_t join) { plutovg_set_line_join(ctx, join); return *this; }
			PlutoVG &fill_rule(plutovg_fill_rule_t winding) { plutovg_set_fill_rule(ctx, winding); return *this; }
			PlutoVG &path_add(plutovg_path_t *path) { plutovg_add_path(ctx, path); return *this; }
			PlutoVG &set_pixel(int x, int y, uint32_t clr)
			{
				auto w = width();
				auto h = height();
				if(x < 0 || y < 0 || x >= w || y >= h)
					return *this;
				auto pixel = (uint32_t *)(surface->data + (y * surface->stride)) + x;
				*pixel = clr;
				return *this;
			}
			PlutoVG &set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			{
				auto w = surface->width;
				auto h = surface->height;
				if(x < 0 || y < 0 || x >= w || y >= h)
					return *this;
				auto pixel = (surface->data + (y * surface->stride) + (x * 4));
				pixel[0] = b; pixel[1] = g;
				pixel[2] = r; pixel[3] = a;
				return *this;
			}
			uint32_t get_pixel(int x, int y)
			{
				auto w = width();
				auto h = height();

				if(x < 0 || y < 0 || x >= w || y >= h)
					return CLR_INVALID;

				//return ((uint32_t *)surface->data)[x + (y * w)];
				
				// by bytes
				return *(uint32_t *)((surface->data + (y * surface->stride)) + x);
			}
			PlutoVG &shadow(double x, double y, double w, double h, double r = 0,
							double border = 0, double offset = 8.0, uint32_t color = 0xff000000, uint8_t opacity = 10)
			{
				x -= border;
				y += offset - border;
				w += border + border;
				h += border + border;
				r = r > 0 ? r : 8.0;

				for(uint8_t i = 0; i < 8; i++)
					rect(x + i, y + i, w - i - i, h - i - i, r - i).fill(color, opacity);

				for(double i = 4; i > 0; i--)
					rect(x - i, y - i, w + i + i, h + i + i, r + i).fill(color, opacity);

				for(double i = 6; i <= 10; i += 4)
					rect(x - i, y - i, w + i + i, h + i + i, r + i).fill(color, opacity);

				//rect(x - 10, y - 10, w + 20, h + 20, r + 10).fill(color, opacity);
				/*
				uint8_t size = 6;
				for(uint8_t i = size, step = 1; i > 0; i--)
				{
					if(i == size)
						step = 4;
					else if(i == size - 1)
						step = 2;
					else
						step = 1;

					auto f = step + i;
					rect(x - f, y - f,
						 w + f + f, h + f + f,
						 r + f)
						.fill(color, opacity);
				}*/

				return *this;
			};
			PlutoVG &shadow_small(double x, double y, double w, double h, double r = 0,
								  double border = 0, double offset = 8.0, uint32_t color = 0xff000000, uint8_t opacity = 30)
			{
				x -= border - 4;
				y += offset - border;
				w += border + border - 8;
				h += border + border - offset;
				r = r > 0 ? r : 8.0;

				for(uint8_t i = 0; i < 8; i++)
					rect(x + i, y + i, w - i - i, h - i - i, r - i).fill(color, opacity);

				uint8_t size = 6;
				for(uint8_t i = size, step = 4; i > 0; i--)
				{
					if(i == size)
						step = 4;
					else if(i == size - 1)
						step = 2;
					else
						step = 1;

					auto f = step + i;
					rect(x - f, y - f,
						 w + f + f, h + f + f,
						 r + f)
						.fill(color, opacity);
				}

				return *this;
			};

			HBITMAP tobitmap(uint8_t **lpbits = nullptr) const
			{
				/*std::uint8_t *ptr = surface->data;
				std::uint8_t *end = surface->data + surface->height * surface->stride;
				while(ptr < end)
				{
					std::uint8_t r = ptr[0];
					std::uint8_t g = ptr[1];
					std::uint8_t b = ptr[2];
					std::uint8_t a = ptr[3];

					ptr[0] = b;
					ptr[1] = g;
					ptr[2] = r;
					ptr[3] = a;

					ptr += 4;
				}

				HBITMAP hbitmap = ::CreateBitmap(surface->width, surface->height, 1, 32, surface->data);
				*/

				HBITMAP bitmap{};
				uint32_t *dst_bits = nullptr;
				if(surface && surface->data)
				{
					BITMAPINFOHEADER bi = { sizeof(bi), surface->width, -surface->height, 1, 32 };
					bitmap = ::CreateDIBSection(nullptr, reinterpret_cast<BITMAPINFO *>(&bi), DIB_RGB_COLORS,
												reinterpret_cast<void **>(&dst_bits), nullptr, 0);
					if(bitmap)
					{
						auto src_bits = reinterpret_cast<uint32_t *>(surface->data);
						for(long i = 0; i < surface->width * surface->height; i++)
							dst_bits[i] = src_bits[i];
						
						if(lpbits)
							*lpbits = reinterpret_cast<uint8_t *>(dst_bits);
					}
				}

				return bitmap;
			}
			
			HBITMAP tobitmap(Nilesoft::Drawing::Color const &color) const
			{
				HBITMAP bitmap{};
				uint32_t *dst_bits{};
				if(surface && surface->data)
				{
					BITMAPINFOHEADER bi = { sizeof(bi), surface->width, -surface->height, 1, 32 };
					bitmap = ::CreateDIBSection(nullptr, reinterpret_cast<BITMAPINFO *>(&bi), DIB_RGB_COLORS,
												reinterpret_cast<void **>(&dst_bits), nullptr, 0);
					if(bitmap)
					{
						auto p = reinterpret_cast<uint32_t *>(surface->data);
						for(long i = 0; i < surface->width * surface->height; i++)
						{
							auto a = p[i] >> 24;
							if(a == 0) continue;
							auto r = (color.r() * a) / 255;
							auto g = (color.g() * a) / 255;
							auto b = (color.b() * a) / 255;
							dst_bits[i] = (a << 24) | (b << 16) | (g << 8) | r;
						}
					}
				}

				return bitmap;
			}

			PlutoVG &rgba()
			{
				auto width = surface->width;
				auto height = surface->height;
				auto stride = surface->stride;
				auto data = surface->data;
				for(int y = 0; y < height; y++)
				{
					auto pixels = reinterpret_cast<uint32_t *>(data + stride * y);
					for(int x = 0; x < width; x++)
					{
						auto pixel = pixels[x];
						auto a = (pixel >> 24) & 0xFF;
						if(a == 0)
							continue;

						auto r = (pixel >> 16) & 0xFF;
						auto g = (pixel >> 8) & 0xFF;
						auto b = (pixel >> 0) & 0xFF;
						if(a != 255)
						{
							r = (r * 255) / a;
							g = (g * 255) / a;
							b = (b * 255) / a;
						}

						pixels[x] = (a << 24) | (b << 16) | (g << 8) | r;
					}
				}
				return *this;
			}

			PlutoVG &rgba0()
			{
				//[0][1][2][3]
				auto pixels = reinterpret_cast<uint8_t *>(surface->data);
				auto end = pixels + (surface->height * surface->width * 4);
				while(pixels < end)
				{
					auto a = pixels[3];
					if(a > 0 && a < 255)
					{
						a *= 2;
						//pixels[0] = (pixels[0] * a) / 255;
						//pixels[1] = (pixels[1] * a) / 255;
						//pixels[2] = (pixels[2] * a) / 255;
						pixels[3] = a;
					}
					pixels += 4;
				}
				return *this;
			}

			PlutoVG &luminance(int value = 6)
			{
				auto width = surface->width;
				auto height = surface->height;
				auto stride = surface->stride;
				auto data = surface->data;
				for(int y = 0; y < height; y++)
				{
					auto pixels = reinterpret_cast<uint32_t *>(data + stride * y);
					for(int x = 0; x < width; x++)
					{
						auto pixel = pixels[x];
						auto r = (pixel >> 16) & 0xFF;
						auto g = (pixel >> 8) & 0xFF;
						auto b = (pixel >> 0) & 0xFF;
						auto l = (2 * r + 3 * g + b) / value;

						pixels[x] = l << 24;
					}
				}
				return *this;
			}
			PlutoVG &clear(unsigned int value)
			{
				auto r = (value >> 24) & 0xFF;
				auto g = (value >> 16) & 0xFF;
				auto b = (value >> 8) & 0xFF;
				auto a = (value >> 0) & 0xFF;

				plutovg_set_source_rgba(ctx, r / 255.0, g / 255.0, b / 255.0, a / 255.0);
				plutovg_set_opacity(ctx, 1.0);
				plutovg_set_operator(ctx, plutovg_operator_src);
				plutovg_paint(ctx);
			}

			static plutovg_path_t *path_create() { return plutovg_path_create(); }
			static void path_destroy(plutovg_path_t *path) { plutovg_path_destroy(path);  }
			static void gradient_destroy(plutovg_gradient_t *gradient) {
				plutovg_gradient_destroy(gradient);
			}
			static auto clr(double c)->double {
				return double(c / 255.0);
			};
			//0=b, g=8, r=16, a=24
			static uint8_t get_color(uint32_t rgba, int8_t index)
			{
				//if(index == 0)return uint8_t(rgba & 0xff);
				return uint8_t((rgba >> index) & 0xff);
			}
			static uint8_t get_color_r(uint32_t rgba) { return uint8_t((rgba >> 16) & 0xff); }
			static uint8_t get_color_g(uint32_t rgba) { return uint8_t((rgba >> 8) & 0xff); }
			static uint8_t get_color_b(uint32_t rgba) { return uint8_t(rgba & 0xff); }
			static uint8_t get_color_a(uint32_t rgba) { return uint8_t((rgba >> 24) & 0xff); }
		};

		struct Gradient
		{
			Gradient() {}
			Gradient(double x1, double y1, double x2, double y2)
			{
				create_linear(x1, y1, x2, y2);
			}
			Gradient(double cx, double cy, double cr, double fx, double fy, double fr)
			{
				create_radial(cx, cy, cr, fx, fy, fr);
			}

			Gradient &create_linear(double x1, double y1, double x2, double y2)
			{
				this->~Gradient();
				_gradient = plutovg_gradient_create_linear(x1, y1, x2, y2);
				return *this;
			}

			Gradient &create_radial(double cx, double cy, double cr, double fx, double fy, double fr)
			{
				this->~Gradient();
				_gradient = plutovg_gradient_create_radial(cx, cy, cr, fx, fy, fr);
				return *this;
			}

			~Gradient()
			{
				if(_gradient)
				{
					plutovg_gradient_destroy(_gradient);
					_gradient = {};
				}
			}

			operator plutovg_gradient_t *() { return _gradient; }
			explicit operator bool()const { return _gradient != nullptr; }

			Gradient &add_stop(double offset, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
			{
				plutovg_gradient_add_stop_rgba(_gradient, offset, double(r / 255.0), double(g / 255.0), double(b / 255.0), double(a / 255.0));
				return *this;
			}

			Gradient &add_stop(double offset, uint32_t rgb, uint8_t a = 255)
			{
				return add_stop(offset, PlutoVG::get_color(rgb, 16), PlutoVG::get_color(rgb, 8), PlutoVG::get_color(rgb, 0), a);
			}

		private:
			plutovg_gradient_t *_gradient{};
		};

	}
}

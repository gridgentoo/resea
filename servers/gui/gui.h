#ifndef __GUI_H__
#define __GUI_H__

#include <list.h>
#include <types.h>

//
#include "canvas.h"

struct os_ops {
    canvas_t (*get_back_buffer)(void);
    void (*swap_buffer)(void);
    void *(*get_icon_png)(enum icon_type icon, unsigned *file_size);
    void *(*open_asset_file)(const char *name, unsigned *file_size);
};

__nonnull struct surface *window_create(const char *title, int width,
                                        int height, void *user_ctx);
__nullable struct surface *button_create(struct surface *window, int x, int y,
                                         const char *label, void *user_ctx);

void gui_render(void);
void gui_move_mouse(int x_delta, int y_delta, bool clicked_left,
                    bool clicked_right);
void gui_init(int screen_width_, int screen_height_, struct os_ops *os_);

#endif

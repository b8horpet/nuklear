/*
 * Nuklear - v1.40.8 - public domain
 * no warrenty implied; use at your own risk.
 * authored from 2015-2017 by Micha Mettke
 */
/*
 * ==============================================================
 *
 *                              API
 *
 * ===============================================================
 */
#ifndef NK_SDL_NATIVE_H_
#define NK_SDL_NATIVE_H_
struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;
NK_API struct nk_context*   nk_sdl_init(struct SDL_Window *, struct SDL_Renderer *);
NK_API int                  nk_sdl_handle_event(union SDL_Event*);
NK_API void                 nk_sdl_render(struct SDL_Renderer*);
NK_API void                 nk_sdl_shutdown(void);
NK_API void                 nk_sdl_font_stash_begin(struct nk_font_atlas **atlas);
NK_API void                 nk_sdl_font_stash_end(void);
// NK_API void                 nk_sdl_paste(nk_handle, struct nk_text_edit*);
// NK_API void                 nk_sdl_copy(nk_handle, const char*, int len);

// /* Image */
#ifdef NK_SDL_NATIVE_INCLUDE_STB_IMAGE
NK_API struct nk_image nk_sdl_load_image_from_file(char const *filename);
#endif

#endif
// /*
//  * ==============================================================
//  *
//  *                          IMPLEMENTATION
//  *
//  * ===============================================================
//  */
#ifdef NK_SDL_IMPLEMENTATION
#include <SDL2/SDL.h>
// #include <sys/time.h>
// #include <unistd.h>
// #include <time.h>


#ifdef NK_SDL_NATIVE_INCLUDE_STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include "../../example/stb_image.h"
#endif

static struct  {
    struct nk_context ctx;
    struct SDL_Window* window;
    struct SDL_Renderer* renderer;
    struct nk_font_atlas atlas;
    struct nk_draw_null_texture null_tex;
    struct SDL_Texture* theFont;
} sdl;


NK_API struct nk_context*
nk_sdl_init(struct SDL_Window* window, struct SDL_Renderer* renderer)
{
    sdl.window = window;
    sdl.renderer = renderer;
    nk_init_default(&sdl.ctx, 0);
    return &sdl.ctx;
}

// NK_API void
// nk_xlib_paste(nk_handle handle, struct nk_text_edit* edit)
// {
//     NK_UNUSED(handle);
//     /* Paste in X is asynchronous, so can not use a temporary text edit */
//     NK_ASSERT(edit != &xlib.ctx.text_edit && "Paste not supported for temporary editors");
//     xlib.clipboard_target = edit;
//     /* Request the contents of the primary buffer */
//     XConvertSelection(xlib.dpy, XA_PRIMARY, XA_STRING, XA_PRIMARY, xlib.root, CurrentTime);
// }

// NK_API void
// nk_xlib_copy(nk_handle handle, const char* str, int len)
// {
//     NK_UNUSED(handle);
//     free(xlib.clipboard_data);
//     xlib.clipboard_len = 0;
//     xlib.clipboard_data = malloc((size_t)len);
//     if (xlib.clipboard_data) {
//         memcpy(xlib.clipboard_data, str, (size_t)len);
//         xlib.clipboard_len = len;
//         XSetSelectionOwner(xlib.dpy, XA_PRIMARY, xlib.root, CurrentTime);
//         XSetSelectionOwner(xlib.dpy, xlib.xa_clipboard, xlib.root, CurrentTime);
//     }
// }

NK_API int
nk_sdl_handle_event(union SDL_Event* evt)
{
    struct nk_context *ctx = &sdl.ctx;

    /* optional grabbing behavior */
    if (ctx->input.mouse.grab) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
        ctx->input.mouse.grab = 0;
    } else if (ctx->input.mouse.ungrab) {
        int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
        SDL_SetRelativeMouseMode(SDL_FALSE);
        SDL_WarpMouseInWindow(sdl.window, x, y);
        ctx->input.mouse.ungrab = 0;
    }
    if (evt->type == SDL_KEYUP || evt->type == SDL_KEYDOWN) {
        /* key events */
        int down = evt->type == SDL_KEYDOWN;
        const Uint8* state = SDL_GetKeyboardState(0);
        SDL_Keycode sym = evt->key.keysym.sym;
        if (sym == SDLK_RSHIFT || sym == SDLK_LSHIFT)
            nk_input_key(ctx, NK_KEY_SHIFT, down);
        else if (sym == SDLK_DELETE)
            nk_input_key(ctx, NK_KEY_DEL, down);
        else if (sym == SDLK_RETURN)
            nk_input_key(ctx, NK_KEY_ENTER, down);
        else if (sym == SDLK_TAB)
            nk_input_key(ctx, NK_KEY_TAB, down);
        else if (sym == SDLK_BACKSPACE)
            nk_input_key(ctx, NK_KEY_BACKSPACE, down);
        else if (sym == SDLK_HOME) {
            nk_input_key(ctx, NK_KEY_TEXT_START, down);
            nk_input_key(ctx, NK_KEY_SCROLL_START, down);
        } else if (sym == SDLK_END) {
            nk_input_key(ctx, NK_KEY_TEXT_END, down);
            nk_input_key(ctx, NK_KEY_SCROLL_END, down);
        } else if (sym == SDLK_PAGEDOWN) {
            nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
        } else if (sym == SDLK_PAGEUP) {
            nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
        } else if (sym == SDLK_z)
            nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_r)
            nk_input_key(ctx, NK_KEY_TEXT_REDO, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_c)
            nk_input_key(ctx, NK_KEY_COPY, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_v)
            nk_input_key(ctx, NK_KEY_PASTE, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_x)
            nk_input_key(ctx, NK_KEY_CUT, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_b)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_e)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_UP)
            nk_input_key(ctx, NK_KEY_UP, down);
        else if (sym == SDLK_DOWN)
            nk_input_key(ctx, NK_KEY_DOWN, down);
        else if (sym == SDLK_LEFT) {
            if (state[SDL_SCANCODE_LCTRL])
                nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
            else nk_input_key(ctx, NK_KEY_LEFT, down);
        } else if (sym == SDLK_RIGHT) {
            if (state[SDL_SCANCODE_LCTRL])
                nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
            else nk_input_key(ctx, NK_KEY_RIGHT, down);
        } else return 0;
        return 1;
    } else if (evt->type == SDL_MOUSEBUTTONDOWN || evt->type == SDL_MOUSEBUTTONUP) {
        /* mouse button */
        int down = evt->type == SDL_MOUSEBUTTONDOWN;
        const int x = evt->button.x, y = evt->button.y;
        if (evt->button.button == SDL_BUTTON_LEFT) {
            if (evt->button.clicks > 1)
                nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
            nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
        } else if (evt->button.button == SDL_BUTTON_MIDDLE)
            nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
        else if (evt->button.button == SDL_BUTTON_RIGHT)
            nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
        return 1;
    } else if (evt->type == SDL_MOUSEMOTION) {
        /* mouse motion */
        if (ctx->input.mouse.grabbed) {
            int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
            nk_input_motion(ctx, x + evt->motion.xrel, y + evt->motion.yrel);
        } else nk_input_motion(ctx, evt->motion.x, evt->motion.y);
        return 1;
    } else if (evt->type == SDL_TEXTINPUT) {
        /* text input */
        nk_glyph glyph;
        memcpy(glyph, evt->text.text, NK_UTF_SIZE);
        nk_input_glyph(ctx, glyph);
        return 1;
    } else if (evt->type == SDL_MOUSEWHEEL) {
        /* mouse wheel */
        nk_input_scroll(ctx,nk_vec2((float)evt->wheel.x,(float)evt->wheel.y));
        return 1;
    }
    return 0;
}

NK_API void
nk_sdl_shutdown(void)
{
    nk_font_atlas_clear(&sdl.atlas);
    nk_free(&sdl.ctx);
    memset(&sdl, 0, sizeof(sdl));
}

NK_API void
nk_sdl_render(struct SDL_Renderer* renderer)
{
    const struct nk_command *cmd;
    nk_foreach(cmd, &sdl.ctx)
    {
        switch (cmd->type) {
        case NK_COMMAND_NOP: break;
        case NK_COMMAND_SCISSOR: {
            const struct nk_command_scissor *s = (const struct nk_command_scissor*)cmd;
            const struct SDL_Rect clip = {(int)s->x, (int)s->y, (int)s->w, (int)s->h};
            SDL_RenderSetClipRect(sdl.renderer, &clip);
        } break;
        case NK_COMMAND_LINE: {
            const struct nk_command_line *l = (const struct nk_command_line *)cmd;
            SDL_SetRenderDrawColor(sdl.renderer, l->color.r, l->color.g, l->color.b, l->color.a);
            SDL_RenderDrawLine(sdl.renderer, l->begin.x, l->begin.y, l->end.x, l->end.y);
        } break;
        case NK_COMMAND_RECT: {
            const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
            const struct SDL_Rect rect = {r->x, r->y, r->w, r->h};
            SDL_SetRenderDrawColor(sdl.renderer, r->color.r, r->color.g, r->color.b, r->color.a);
            SDL_RenderDrawRect(sdl.renderer, &rect);
        } break;
        case NK_COMMAND_RECT_FILLED: {
            const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
            const struct SDL_Rect rect = {r->x, r->y, r->w, r->h};
            SDL_SetRenderDrawColor(sdl.renderer, r->color.r, r->color.g, r->color.b, r->color.a);
            SDL_RenderFillRect(sdl.renderer, &rect);
        } break;
        case NK_COMMAND_CIRCLE: {
            const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
            const struct SDL_Rect rect = {c->x, c->y, c->w, c->h};
            SDL_SetRenderDrawColor(sdl.renderer, c->color.r, c->color.g, c->color.b, c->color.a);
            SDL_RenderDrawRect(sdl.renderer, &rect);
//             nk_xsurf_stroke_circle(surf, c->x, c->y, c->w, c->h, c->line_thickness, c->color);
        } break;
        case NK_COMMAND_CIRCLE_FILLED: {
            const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
//             nk_xsurf_fill_circle(surf, c->x, c->y, c->w, c->h, c->color);
            const struct SDL_Rect rect = {c->x, c->y, c->w, c->h};
            SDL_SetRenderDrawColor(sdl.renderer, c->color.r, c->color.g, c->color.b, c->color.a);
            SDL_RenderFillRect(sdl.renderer, &rect);
        } break;
        case NK_COMMAND_TRIANGLE: {
            const struct nk_command_triangle*t = (const struct nk_command_triangle*)cmd;
//             nk_xsurf_stroke_triangle(surf, t->a.x, t->a.y, t->b.x, t->b.y,
//                 t->c.x, t->c.y, t->line_thickness, t->color);
            SDL_SetRenderDrawColor(sdl.renderer, t->color.r, t->color.g, t->color.b, t->color.a);
            SDL_RenderDrawLine(sdl.renderer, t->a.x, t->a.y, t->b.x, t->b.y);
            SDL_RenderDrawLine(sdl.renderer, t->b.x, t->b.y, t->c.x, t->c.y);
            SDL_RenderDrawLine(sdl.renderer, t->c.x, t->c.y, t->a.x, t->a.y);
        } break;
        case NK_COMMAND_TRIANGLE_FILLED: {
            const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled *)cmd;
//             nk_xsurf_fill_triangle(surf, t->a.x, t->a.y, t->b.x, t->b.y,
//                 t->c.x, t->c.y, t->color);
        } break;
        case NK_COMMAND_POLYGON: {
            const struct nk_command_polygon *p =(const struct nk_command_polygon*)cmd;
//             nk_xsurf_stroke_polygon(surf, p->points, p->point_count, p->line_thickness,p->color);
        } break;
        case NK_COMMAND_POLYGON_FILLED: {
            const struct nk_command_polygon_filled *p = (const struct nk_command_polygon_filled *)cmd;
//             nk_xsurf_fill_polygon(surf, p->points, p->point_count, p->color);
        } break;
        case NK_COMMAND_POLYLINE: {
            const struct nk_command_polyline *p = (const struct nk_command_polyline *)cmd;
//             nk_xsurf_stroke_polyline(surf, p->points, p->point_count, p->line_thickness, p->color);
        } break;
        case NK_COMMAND_TEXT: {
            const struct nk_command_text *t = (const struct nk_command_text*)cmd;

            int x = t->x;
            int y = t->y;
            // int w = t->w;
            // int h = t->h;
            const int ww = 512;
            const int hh = 128;
            const char* str = t->string;
            const int len = t->length;
            for(int i=0; i<len; ++i)
            {
                int idx = (str[i] - 32);
                const struct nk_font_glyph* g = &sdl.atlas.glyphs[idx];
                SDL_Rect dest = {x+(int)g->x0,y+(int)g->y0,(int)g->w,(int)g->h};
                int x0 = (int)((float)(ww)*g->u0);
                int y0 = (int)((float)(hh)*g->v0);
                int x1 = (int)((float)(ww)*g->u1);
                int y1 = (int)((float)(hh)*g->v1);
                SDL_Rect src = {x0, y0, x1-x0, y1-y0 };
                SDL_RenderCopy(sdl.renderer,sdl.theFont,&src,&dest);
                x += (int)g->xadvance+1;
            }
        } break;
        case NK_COMMAND_CURVE: {
            const struct nk_command_curve *q = (const struct nk_command_curve *)cmd;
//             nk_xsurf_stroke_curve(surf, q->begin, q->ctrl[0], q->ctrl[1],
//                 q->end, 22, q->line_thickness, q->color);
        } break;
        case NK_COMMAND_IMAGE: {
            const struct nk_command_image *i = (const struct nk_command_image *)cmd;
            if(i->img.handle.id == 0) break;
            SDL_Rect dest = {i->x,i->y,i->w,i->h};
            SDL_Rect src, *src_ptr = NULL;
            if(nk_image_is_subimage(&i->img))
            {
                src_ptr = &src;
                src.x = i->img.region[0];
                src.y = i->img.region[1];
                src.w = i->img.region[2];
                src.h = i->img.region[3];
            }
            SDL_RenderCopy(sdl.renderer,(struct SDL_Texture*) i->img.handle.ptr, src_ptr, &dest);
        } break;
        case NK_COMMAND_RECT_MULTI_COLOR:
        case NK_COMMAND_ARC:
        case NK_COMMAND_ARC_FILLED:
        case NK_COMMAND_CUSTOM:
        default:
            break;
        }
    }
    nk_clear(&sdl.ctx);
}

NK_API void
nk_sdl_font_stash_begin(struct nk_font_atlas **atlas)
{
    nk_font_atlas_init_default(&sdl.atlas);
    nk_font_atlas_begin(&sdl.atlas);
    *atlas = &sdl.atlas;
}

NK_API void
nk_sdl_font_stash_end(void)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    const uint32_t rmask = 0xff000000;
    const uint32_t gmask = 0x00ff0000;
    const uint32_t bmask = 0x0000ff00;
    const uint32_t amask = 0x000000ff;
#else
    const uint32_t rmask = 0x000000ff;
    const uint32_t gmask = 0x0000ff00;
    const uint32_t bmask = 0x00ff0000;
    const uint32_t amask = 0xff000000;
#endif
    const void *image; int w, h;
    image = nk_font_atlas_bake(&sdl.atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    printf("font baked into %dx%d bitmap\n", w, h);
    SDL_Surface* theFont = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
    SDL_LockSurface(theFont);
    memcpy(theFont->pixels,image,w*h*4);
    SDL_UnlockSurface(theFont);
    sdl.theFont = SDL_CreateTextureFromSurface(sdl.renderer, theFont);
    SDL_FreeSurface(theFont);
    nk_font_atlas_end(&sdl.atlas, nk_handle_id(0), &sdl.null_tex);
    if (sdl.atlas.default_font)
        nk_style_set_font(&sdl.ctx, &sdl.atlas.default_font->handle);
}

#ifdef NK_SDL_NATIVE_INCLUDE_STB_IMAGE
NK_INTERN struct nk_image
nk_stbi_image_to_sdl_native(unsigned char *data, int width, int height, int channels) {
    if (data == NULL) return nk_image_id(0);
    int errorcode;
    long image_size_in_bytes = width*height*channels;
    long pitch = width*channels;
    struct SDL_Texture* texture = SDL_CreateTexture(sdl.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    if(!texture) return nk_image_id(0);
    errorcode = SDL_UpdateTexture(texture, NULL, data, pitch);
    if(errorcode)
    {
        SDL_DestroyTexture(texture);
        return nk_image_id(0);
    }
    struct nk_image img = nk_image_ptr((void*)texture);
    return img;
}

NK_API struct nk_image
nk_sdl_load_image_from_file(char const *filename)
{
    int x,y,n;
    unsigned char *data;
    data = stbi_load(filename, &x, &y, &n, 4);
    struct nk_image retval = nk_stbi_image_to_sdl_native(data, x, y, n);
    stbi_image_free(data);
    return retval;
}
#endif

#endif

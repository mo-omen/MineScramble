/*
    A simple graphics library for CSE 20211 by Douglas Thain
    For course assignments, you should not modify this file.
    For complete documentation, see:
    http://www.nd.edu/~dthain/courses/cse20211/fall2013/gfx
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gfx.h"

static Display *gfx_display = 0;
static Window gfx_window;
static GC gfx_gc;
static Colormap gfx_colormap;
static int gfx_fast_color;

static int saved_xpos = 0;
static int saved_ypos = 0;

static unsigned int gfx_bgr = 0;
static unsigned int gfx_bgg = 0;
static unsigned int gfx_bgb = 0;

void gfx_open(int width, int height, const char *title)
{
    gfx_display = XOpenDisplay(0);
    if (!gfx_display) {
        fprintf(stderr, "gfx_open: unable to open the graphics window.\n");
        exit(1);
    }

    Visual *visual = DefaultVisual(gfx_display, 0);
    if (visual && visual->class == TrueColor) {
        gfx_fast_color = 1;
    } else {
        gfx_fast_color = 0;
    }

    int blackColor = BlackPixel(gfx_display, DefaultScreen(gfx_display));
    int whiteColor = WhitePixel(gfx_display, DefaultScreen(gfx_display));

    gfx_window = XCreateSimpleWindow(gfx_display, DefaultRootWindow(gfx_display),
                                     0, 0, width, height, 0, blackColor, blackColor);

    XSetWindowAttributes attr;
    attr.backing_store = Always;
    XChangeWindowAttributes(gfx_display, gfx_window, CWBackingStore, &attr);

    XStoreName(gfx_display, gfx_window, title);

    XSelectInput(gfx_display, gfx_window,
                 StructureNotifyMask | KeyPressMask | ButtonPressMask | ExposureMask);

    XMapWindow(gfx_display, gfx_window);

    gfx_gc = XCreateGC(gfx_display, gfx_window, 0, 0);

    gfx_colormap = DefaultColormap(gfx_display, 0);

    XSetForeground(gfx_display, gfx_gc, whiteColor);

    /* Wait for the MapNotify event */
    for (;;) {
        XEvent e;
        XNextEvent(gfx_display, &e);
        if (e.type == MapNotify)
            break;
    }
}

void gfx_point(int x, int y)
{
    XDrawPoint(gfx_display, gfx_window, gfx_gc, x, y);
}

void gfx_line(int x1, int y1, int x2, int y2)
{
    XDrawLine(gfx_display, gfx_window, gfx_gc, x1, y1, x2, y2);
}

void gfx_color(int r, int g, int b)
{
    XColor color;

    if (gfx_fast_color) {
        color.pixel = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
    } else {
        color.pixel = 0;
        color.red = r << 8;
        color.green = g << 8;
        color.blue = b << 8;
        XAllocColor(gfx_display, gfx_colormap, &color);
    }

    XSetForeground(gfx_display, gfx_gc, color.pixel);
}

void gfx_clear()
{
    XColor color;
    if (gfx_fast_color) {
        color.pixel = ((gfx_bgr & 0xff) << 16) | ((gfx_bgg & 0xff) << 8) | (gfx_bgb & 0xff);
    } else {
        color.pixel = 0;
        color.red = gfx_bgr << 8;
        color.green = gfx_bgg << 8;
        color.blue = gfx_bgb << 8;
        XAllocColor(gfx_display, gfx_colormap, &color);
    }
    XSetBackground(gfx_display, gfx_gc, color.pixel);
    XSetForeground(gfx_display, gfx_gc, color.pixel);

    XFillRectangle(gfx_display, gfx_window, gfx_gc, 0, 0, gfx_xsize(), gfx_ysize());
}

void gfx_clear_color(int r, int g, int b)
{
    gfx_bgr = r;
    gfx_bgg = g;
    gfx_bgb = b;
}

char gfx_wait()
{
    XEvent event;

    gfx_flush();

    while (1) {
        XNextEvent(gfx_display, &event);

        if (event.type == KeyPress) {
            saved_xpos = event.xkey.x;
            saved_ypos = event.xkey.y;
            KeySym key;
            char buf[2];
            int n = XLookupString(&event.xkey, buf, 1, &key, 0);
            if (n == 1) return buf[0];
        } else if (event.type == ButtonPress) {
            saved_xpos = event.xbutton.x;
            saved_ypos = event.xbutton.y;
            return event.xbutton.button;
        }
    }
}

int gfx_xpos()
{
    return saved_xpos;
}

int gfx_ypos()
{
    return saved_ypos;
}

int gfx_xsize()
{
    XWindowAttributes w;
    XGetWindowAttributes(gfx_display, gfx_window, &w);
    return w.width;
}

int gfx_ysize()
{
    XWindowAttributes w;
    XGetWindowAttributes(gfx_display, gfx_window, &w);
    return w.height;
}

int gfx_event_waiting()
{
    XEvent event;
    gfx_flush();

    while (XCheckMaskEvent(gfx_display, -1, &event)) {
        if (event.type == KeyPress) {
            saved_xpos = event.xkey.x;
            saved_ypos = event.xkey.y;
            XPutBackEvent(gfx_display, &event);
            return 1;
        } else if (event.type == ButtonPress) {
            saved_xpos = event.xbutton.x;
            saved_ypos = event.xbutton.y;
            XPutBackEvent(gfx_display, &event);
            return 1;
        }
    }

    return 0;
}

void gfx_flush()
{
    XFlush(gfx_display);
}

void gfx_text(int x, int y, const char *text)
{
    XDrawString(gfx_display, gfx_window, gfx_gc, x, y, text, strlen(text));
}

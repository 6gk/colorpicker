/* Copyright (c) 2015 Jonas Kulla <Nyocurio@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <X11/cursorfont.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <getopt.h>

int main(int argc, char *argv[]) {

	int opt;
	int one_shot = 0;
	int quit_on_keypress = 0;
	int output_format = 0x11;

	static struct option long_options[] = {
		{"one-shot", no_argument, NULL, 'o'},
		{"quit-on-keypress", no_argument, NULL, 'q'},
		{"help", no_argument, NULL, 'h'},
		{"rgb", no_argument, NULL, 'r'},
		{"hex", no_argument, NULL, 'd'}
	};

	while ((opt = getopt_long(argc, argv, "oqhrd", long_options, NULL)) != -1) {
		switch(opt) {
			case 'o':
				one_shot = 1;
				break;
			case 'c':
			case 'q':
				quit_on_keypress = 1;
				break;
			case 'r':
				output_format &= 0x1;
				break;
			case 'd':
				output_format &= 0x10;
				break;
			case 'h':
				printf( "colorpicker [options]\n"
						"  -h, --help:             show this help\n"
						"  -o, --one-shot:         one shot\n"
						"  -q, --quit-on-keypress: quit on keypress\n"
						"  -d, --hex:              hex only\n"
						"  -r, --rgb:              rgb only\n");
				return 0;
			default:
				return 1;
		}
	}

	Display *display = XOpenDisplay(NULL);
	Window root = DefaultRootWindow(display);

	Cursor cursor = XCreateFontCursor(display, 130);
	XGrabPointer(display, root, 0,  ButtonPressMask, GrabModeAsync, GrabModeAsync, root, cursor, CurrentTime);

	XWindowAttributes gwa;
	XGetWindowAttributes(display, root, &gwa);

	XGrabKeyboard(display, root, 0, GrabModeAsync, GrabModeAsync, CurrentTime);

	for(;;) {
		XEvent e;
		XNextEvent(display, &e);
		XImage *image = XGetImage(display, root, 0, 0, gwa.width, gwa.height, AllPlanes, ZPixmap);
		//printf("%d",e.xbutton.button);
		if ((e.type == ButtonPress && e.xbutton.button == Button1) ||
				(e.type == 2 && e.xbutton.button == 196) ||
				(e.type == 2 && e.xbutton.button == 225) ||
				(e.type == 2 && e.xbutton.button == 193)) {
				unsigned long pixel = XGetPixel(image, e.xbutton.x_root, e.xbutton.y_root);
				if (output_format & 0x1) {
					printf("%d,%d,%d ", (pixel >> 0x10) & 0xFF, (pixel >> 0x08) & 0xFF, pixel & 0xFF);
				}
				if (output_format & 0x10) {
					printf("#%06X", pixel);
				}
				puts("");
				if (one_shot) {
					break;
				}
		} else if (e.type == ButtonPress ||
				(e.type == KeyPress && (e.xkey.keycode == 53 || quit_on_keypress))) {
			break;
		}
		XDestroyImage(image);
		fflush(stdout);
	}

	/* will be done on connection close anw */
	XUngrabPointer(display, CurrentTime);
	XUngrabKeyboard(display, CurrentTime);

	XFreeCursor(display, cursor);
	XDestroyWindow(display, root);
	XCloseDisplay(display);
}

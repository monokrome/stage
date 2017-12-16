#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "config.h"

Display *GetDisplay();
void detail();
void apply();
char *getMonitorFingerprint(XRRMonitorInfo *monitor);
void initialize();
void cleanup();

const char *FINGERPRINT_FORMAT = "%s:%dx%d\n";

typedef enum {
	EXIT_NO_DISPLAY = 1,
	EXIT_OPEN_DISPLAY_FAILED = 2,
	EXIT_FINGERPRINTING_FAILED = 3,
	EXIT_NO_MEMORY = 10,
} SExitCode;

Display        *display;
XRRMonitorInfo *monitors;

int numMonitors = 0;

void initialize() {
	display = GetDisplay();
	monitors = XRRGetMonitors(display, DefaultRootWindow(display), true, &numMonitors);
}

void cleanup() {
	if (display != NULL) free(display);
	if (monitors != NULL) free(monitors);
}

void fatal(int code, char *message) {
	fprintf(stderr, "%s\n", message);
	exit(code);
}

void checkAlloc(char *ptr) {
	if (ptr == NULL) fatal(EXIT_NO_MEMORY, "Could not allocated necessary memory.");
}

char *getMonitorFingerprint(XRRMonitorInfo *monitor) {
	char *fingerprint;
	int length;

	char *name = XGetAtomName(display, monitor->name);

	length = sprintf(NULL, FINGERPRINT_FORMAT,
		name,
		monitor->width,
		monitor->height
	);

	// Add an additional character for NULL byte \0
	fingerprint = calloc(length+1, sizeof(char));
	checkAlloc(fingerprint);

	if (length != snprintf(fingerprint, length, FINGERPRINT_FORMAT, name, monitors[0].width, monitors[0].height))
		fatal(EXIT_FINGERPRINTING_FAILED, "Could not fingerprint monitor.");

	return fingerprint;
}

Display *GetDisplay() {
	if (displayIdentifier == NULL) displayIdentifier = getenv("DISPLAY");
	if (displayIdentifier == NULL) fatal(EXIT_NO_DISPLAY, "DISPLAY environment variable must be set.");
	display = XOpenDisplay(displayIdentifier);
	if (display == NULL) fatal(EXIT_OPEN_DISPLAY_FAILED, "Could not open DISPLAY.");
	return display;
}

void detail() {
	printf("Total monitors: %d\n", numMonitors);

	for (int i = 0; i < numMonitors; ++i)
		printf("%s\n", getMonitorFingerprint(&monitors[i]));
}

void apply() {}

int main() {
	initialize();
	detail();

	apply();
	cleanup();

	return EXIT_SUCCESS;
}

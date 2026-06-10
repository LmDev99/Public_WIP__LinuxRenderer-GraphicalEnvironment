/*
 * Copyright (c) 2026 Logan Mottern
 * All rights reserved.
 *
 * This source code is provided for portfolio and review purposes only.
 * No permission is granted to copy, modify, distribute, sublicense, or use
 * this software except with the express written permission of the copyright owner.
 */

#pragma once
#include <xcb/xcb.h>
#include <stdlib.h>
#include <string.h>
#include "cameraData.hpp"

typedef struct {
    xcb_connection_t* connection;
    xcb_screen_t* screen;
    xcb_window_t window;
} XcbWindow;




// Function to retrieve the atom for a given property name
xcb_atom_t GET_ATOM(xcb_connection_t* connection, const char* name) {
    // Send a request to the X server to intern an atom with the specified name
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 0, strlen(name), name);

    // Retrieve the reply from the X server
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(connection, cookie, NULL);

    if (reply) {
        xcb_atom_t atom = reply->atom;
        free(reply);  // Free the reply to avoid memory leak
        return atom;  // Return the atom id
    }

    return XCB_NONE;  // Return an invalid atom if the request failed
}


int CREATE_XCB_WINDOW(int width, int height, int fullscreen,XcbWindow* xcbWindow) {
    int screenNumber;
    
    xcbWindow->connection = xcb_connect(NULL, &screenNumber);
    if (xcb_connection_has_error(xcbWindow->connection)) {
        // Handle connection error
        return -1;
    }

    const xcb_setup_t* setup = xcb_get_setup(xcbWindow->connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    for (int i = 0; i < screenNumber; ++i) {
        xcb_screen_next(&iter);
    }
    xcbWindow->screen = iter.data;

    xcbWindow->window = xcb_generate_id(xcbWindow->connection);

    uint16_t window_width = width;
    uint16_t window_height = height;

    if (fullscreen) {
        window_width = xcbWindow->screen->width_in_pixels;
        window_height = xcbWindow->screen->height_in_pixels;
    }
    uint32_t event_mask = XCB_EVENT_MASK_POINTER_MOTION;  // Monitor mouse movement
    // Add other event masks as needed, for example:
    // event_mask |= XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;

    uint32_t values[1] = { event_mask};  // Additional parameters can be set in values array if needed

    xcb_create_window(xcbWindow->connection,
                      XCB_COPY_FROM_PARENT,
                      xcbWindow->window,
                      xcbWindow->screen->root,
                      0, 0,
                      window_width, window_height,
                      0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      xcbWindow->screen->root_visual,
                      XCB_CW_EVENT_MASK, values);

    if (fullscreen) {
        xcb_atom_t wmState = GET_ATOM(xcbWindow->connection, "_NET_WM_STATE");
        xcb_atom_t wmStateFullscreen = GET_ATOM(xcbWindow->connection, "_NET_WM_STATE_FULLSCREEN");

        xcb_change_property(xcbWindow->connection, XCB_PROP_MODE_REPLACE, xcbWindow->window,
                            wmState, XCB_ATOM_ATOM, 32, 1, &wmStateFullscreen);
    }

    //xcb_map_window(xcbWindow->connection, xcbWindow->window); this will display the window, we want to delay this until app is ready
    xcb_flush(xcbWindow->connection);

    return 0;
}


void DISPLAY_XCB_WINDOW(XcbWindow* xcbWindow){
    xcb_map_window(xcbWindow->connection, xcbWindow->window);

}

void MINIMIZE_XCB_WINDOW(xcb_connection_t* connection, xcb_window_t window, xcb_atom_t wmState) {
    // Pause or stop Vulkan operations
    //pause_vulkan_operations();

    // Minimize the window
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, wmState, XCB_ATOM_ATOM, 32, 0, NULL);
    xcb_flush(connection);

    // The application is now minimized to the taskbar
}







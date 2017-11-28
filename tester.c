#include "wiiuse.h" /* for wiimote_t, classic_ctrl_t, etc */
#include <stdio.h>  /* for printf */

#define MAX_WIIMOTES 4

float kgTolbs = 2.20462; // conversion factor

// checks if wiimote is still connected
short any_wiimote_connected(wiimote **wm, int wiimotes) {
  if (!wm)
    return 0;

  for (int i = 0; i < wiimotes; i++)
    if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i]))
      return 1;

  return 0;
}

int main(int argc, char **argv) {

  wiimote **wiimotes;
  int found, connected;
  wiimotes = wiiuse_init(MAX_WIIMOTES);

  // find wiimotes
  found = wiiuse_find(wiimotes, MAX_WIIMOTES, 25); // timeout is 25 sec
  if (!found) {
    printf("No Wii Balance Board found.\n");
    return 0;
  }

  // connect to found wiimotes 
  connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
  if (connected) {
    printf("Connected to Wii Balance Board.\n");
  } else {
    printf("Failed to connect to any Wii Balance Board.\n");
    return 0;
  }

  wiimote *wm = wiimotes[0]; // setup pointer to balance board

  // the main loop
  while (any_wiimote_connected(wiimotes, MAX_WIIMOTES)) {

    // check if anything happened with the wiiboard
    if (wiiuse_poll(wiimotes, MAX_WIIMOTES) && wm->event == WIIUSE_EVENT)
    {
        struct wii_board_t *wb = (wii_board_t *)&wm->exp.wb;

        // get weights for each leg
        float tl = wb->tl * kgTolbs;
        float tr = wb->tr * kgTolbs;
        float bl = wb->bl * kgTolbs;
        float br = (wb->br * kgTolbs) - 5.1;

        // calculate derived variables
        float total = tl + tr + bl + br;
        float left = tl + bl;
        float right = tr + br;
        printf("data,%f,%f,%f\n", total, left, right);
    }
  }

  // When finished, disconnect the wiimotes
  wiiuse_cleanup(wiimotes, MAX_WIIMOTES);

  return 0;
}

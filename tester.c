/**
 *	@file
 *
 *	@brief Limb Load Monitor Tester, using the wiiuse API.
 *
 */

#include "wiiuse.h" /* for wiimote_t, classic_ctrl_t, etc */
#include <stdio.h>  /* for printf */

#define MAX_WIIMOTES 1

float kgTolbs = 2.20462;

/**
 *	@brief Callback that handles an event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *
 *	This function is called automatically by the wiiuse library when an
 *	event occurs on the specified wiimote.
 */
void handle_event(struct wiimote_t *wm) {
  // printf("\n\n--- EVENT [id %i] ---\n", wm->unid);
  struct wii_board_t *wb = (wii_board_t *)&wm->exp.wb;
  float total = wb->tl + wb->tr + wb->bl + wb->br;
  float left = wb->tl + wb->bl;
  float right = wb->tr + wb->br;
  float x = ((wb->tr + wb->br) / total) * 2 - 1;
  float y = ((wb->tl + wb->tr) / total) * 2 - 1;
  printf("Weight: %f kg @ (%f, %f)\n", total * kgTolbs, x, y);
  printf("Left: %f kg, Right: %f kg\n", left * kgTolbs, right * kgTolbs);
  printf("Interpolated weight: TL:%f  TR:%f  BL:%f  BR:%f\n", wb->tl, wb->tr,
         wb->bl, wb->br);
  printf("Raw: TL:%d  TR:%d  BL:%d  BR:%d\n", wb->rtl, wb->rtr, wb->rbl,
         wb->rbr);
}

/**
 *	@brief Callback that handles a controller status event.
 *
 *	@param wm				Pointer to a wiimote_t
 *structure.
 *	@param attachment		Is there an attachment? (1 for yes, 0
 *for no)
 *	@param speaker			Is the speaker enabled? (1 for yes, 0
 *for no)
 *	@param ir				Is the IR support enabled? (1
 *for yes, 0 for no)
 *	@param led				What LEDs are lit.
 *	@param battery_level	Battery level, between 0.0 (0%) and 1.0 (100%).
 *
 *	This occurs when either the controller status changed
 *	or the controller status was requested explicitly by
 *	wiiuse_status().
 *
 *	One reason the status can change is if the nunchuk was
 *	inserted or removed from the expansion port.
 */
void handle_ctrl_status(struct wiimote_t *wm) {
  printf("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

  printf("attachment:      %i\n", wm->exp.type);
  printf("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
  printf("ir:              %i\n", WIIUSE_USING_IR(wm));
  printf("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1),
         WIIUSE_IS_LED_SET(wm, 2), WIIUSE_IS_LED_SET(wm, 3),
         WIIUSE_IS_LED_SET(wm, 4));
  printf("battery:         %f %%\n", wm->battery_level);
}

/**
 *	@brief Callback that handles a disconnection event.
 *
 *	@param wm				Pointer to a wiimote_t
 *structure.
 *
 *	This can happen if the POWER button is pressed, or
 *	if the connection is interrupted.
 */
void handle_disconnect(wiimote *wm) {
  printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
}

void test(struct wiimote_t *wm, byte *data, unsigned short len) {
  printf("test: %i [%x %x %x %x]\n", len, data[0], data[1], data[2], data[3]);
}

short any_wiimote_connected(wiimote **wm, int wiimotes) {
  int i;
  if (!wm) {
    return 0;
  }

  for (i = 0; i < wiimotes; i++) {
    if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i])) {
      return 1;
    }
  }

  return 0;
}

/**
 *	@brief main()
 *
 *	Connect to up to two wiimotes and print any events
 *	that occur on either device.
 */
// ==========================================================================
int main(int argc, char **argv) {

  // print initialization text
  printf("=================================\n");
  printf("| Limb Load Monitor Tester v0.1 |\n");
  printf("=================================\n");

  wiimote **wiimotes;
  int found, connected;

  /*
   *	Initialize an array of wiimote objects.
   *
   *	The parameter is the number of wiimotes I want to create.
   */
  wiimotes = wiiuse_init(MAX_WIIMOTES);

  /*
   *	Find wiimote devices
   *
   *	Now we need to find some wiimotes.
   *	Give the function the wiimote array we created, and tell it there
   *	are MAX_WIIMOTES wiimotes we are interested in.
   *
   *	Set the timeout to be 5 seconds.
   *
   *	This will return the number of actual wiimotes that are in discovery
   *mode.
   */
  found = wiiuse_find(wiimotes, MAX_WIIMOTES, 5);
  if (!found) {
    printf("No Wii Balance Board found.\n");
    return 0;
  }

  /*
   *	Connect to the wiimotes
   *
   *	Now that we found some wiimotes, connect to them.
   *	Give the function the wiimote array and the number
   *	of wiimote devices we found.
   *
   *	This will return the number of established connections to the found
   *wiimotes.
   */
  connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
  if (connected) {
    printf("Connected to Wii Balance Board.\n");
  } else {
    printf("Failed to connect to any Wii Balance Board.\n");
    return 0;
  }

  /*
   *	Maybe I'm interested in the battery power of the 0th
   *	wiimote.  This should be WIIMOTE_ID_1 but to be sure
   *	you can get the wiimote associated with WIIMOTE_ID_1
   *	using the wiiuse_get_by_id() function.
   *
   *	A status request will return other things too, like
   *	if any expansions are plugged into the wiimote or
   *	what LEDs are lit.
   */
  /* wiiuse_status(wiimotes[0]); */

  /*
   *	This is the main loop
   *
   *	wiiuse_poll() needs to be called with the wiimote array
   *	and the number of wiimote structures in that array
   *	(it doesn't matter if some of those wiimotes are not used
   *	or are not connected).
   *
   *	This function will set the event flag for each wiimote
   *	when the wiimote has things to report.
   */
  while (any_wiimote_connected(wiimotes, MAX_WIIMOTES)) {
    if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
      /*
       *	This happens if something happened on any wiimote.
       *	So go through each one and check if anything happened.
       */
      int i = 0;
      for (; i < MAX_WIIMOTES; ++i) {
        switch (wiimotes[i]->event) {
        case WIIUSE_EVENT:
          /* a generic event occurred */
          handle_event(wiimotes[i]);
          break;

        case WIIUSE_STATUS:
          /* a status event occurred */
          handle_ctrl_status(wiimotes[i]);
          break;

        case WIIUSE_DISCONNECT:
        case WIIUSE_UNEXPECTED_DISCONNECT:
          /* the wiimote disconnected */
          handle_disconnect(wiimotes[i]);
          break;

        default:
          break;
        }
      }
    }
  }

  /*
   *	Disconnect the wiimotes
   */
  wiiuse_cleanup(wiimotes, MAX_WIIMOTES);

  return 0;
}
#include "bug/debug.h"
#include "tshield/tshield.h"

void debug_led(int led, int value) {
  tshield_led(led, value);
}
#include QMK_KEYBOARD_H
#include "rgblight.h"

extern rgblight_config_t rgblight_config;

#define DEFAULT_LAYER 0
#define LAYER_1 1

static rgblight_config_t layers[2];

// Keyboard Layout
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT_60_ansi(
        KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC,
        KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS,
        MO(1), KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT,
        KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_RALT, KC_RGUI, KC_APP, KC_RCTL),

    LAYOUT_60_ansi(
        KC_GRV, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_DEL,
        KC_TRNS, RGB_HUD, RGB_HUI, RGB_RMOD, RGB_MOD, KC_NO, KC_NO, KC_PGUP, KC_UP, KC_PGDN, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_TRNS, KC_MUTE, KC_VOLD, KC_VOLU, KC_MPLY, KC_NO, KC_HOME, KC_LEFT, KC_DOWN, KC_RGHT, KC_INS, KC_NO, KC_TRNS,
        KC_TRNS, RGB_TOG, RGB_VAD, RGB_VAI, KC_NO, KC_NO, KC_END, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS)

};

void update_layer_1(void)
{
  layers[LAYER_1].raw = layers[DEFAULT_LAYER].raw;
  layers[LAYER_1].mode = RGBLIGHT_MODE_BREATHING + 2;
  layers[LAYER_1].hue = (layers[LAYER_1].hue + 180) % 360;
}

// Startup method
void matrix_init_user(void)
{
  layers[DEFAULT_LAYER].raw = eeconfig_read_rgblight();

  update_layer_1();
  rgblight_set();
}

// Layer switching
uint32_t layer_state_set_user(uint32_t state)
{
  switch (biton32(state))
  {
  case DEFAULT_LAYER:
  case LAYER_1:
    rgblight_update_dword(layers[biton32(state)].raw);
    break;
  }
  return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record)
{
  if (!record->event.pressed)
  {
    return true;
  }

  bool changed = false;

  switch(keycode)
  {
  case RGB_HUD:
    if (layers[DEFAULT_LAYER].hue - RGBLIGHT_HUE_STEP < 0)
      layers[DEFAULT_LAYER].hue = (layers[DEFAULT_LAYER].hue + 360 - RGBLIGHT_HUE_STEP) % 360;
    else
      layers[DEFAULT_LAYER].hue = (layers[DEFAULT_LAYER].hue - RGBLIGHT_HUE_STEP) % 360;
    changed = true;
    break;
  case RGB_HUI:
    layers[DEFAULT_LAYER].hue = (layers[DEFAULT_LAYER].hue + RGBLIGHT_HUE_STEP) % 360;
    changed = true;
    break;
  case RGB_VAD:
    layers[DEFAULT_LAYER].val -= RGBLIGHT_VAL_STEP;
    if (layers[DEFAULT_LAYER].val < 0)
      layers[DEFAULT_LAYER].val = 0;
    changed = true;
    break;
  case RGB_VAI:
    if (layers[DEFAULT_LAYER].val + RGBLIGHT_VAL_STEP <= RGBLIGHT_LIMIT_VAL)
      layers[DEFAULT_LAYER].val += RGBLIGHT_VAL_STEP;
    changed = true;
    break;
  case RGB_MOD:
    layers[DEFAULT_LAYER].mode = layers[DEFAULT_LAYER].mode + 1;
    if (layers[DEFAULT_LAYER].mode > RGBLIGHT_MODES)
      layers[DEFAULT_LAYER].mode = 1;
    changed = true;
    break;
  case RGB_RMOD:
    layers[DEFAULT_LAYER].mode = layers[DEFAULT_LAYER].mode - 1;
    if (layers[DEFAULT_LAYER].mode < 1)
      layers[DEFAULT_LAYER].mode = RGBLIGHT_MODES;
    changed = true;
    break;
  }

  if (changed) {
    //rgblight_config.raw = layers[DEFAULT_LAYER].raw;
    update_layer_1();
    eeconfig_update_rgblight(layers[DEFAULT_LAYER].raw);
    rgblight_update_dword(layers[DEFAULT_LAYER].raw);
    return false;
  }
  return true;
}

int slide_towards(int last, int desired, int step)
{
  if (last > desired)
  {
    last -= step;
    if (last < desired)
      last = desired;
  }
  else
  {
    last += step;
    if (last > desired)
      last = desired;
  }


  return last;
}

// void matrix_scan_user(void)
// {
//   if (rgb_last_h != rgb_desired_h)
//   {
//     rgb_last_h = slide_towards(rgb_last_h, rgb_desired_h, RGB_H_TRANSITION_STEP);
//     rgb_update = true;
//   }

//   if (rgb_last_s != rgb_desired_s)
//   {
//     rgb_last_s = slide_towards(rgb_last_s, rgb_desired_s, RGB_S_TRANSITION_STEP);
//     rgb_update = true;
//   }

//   if (rgb_update)
//   {
//     rgblight_sethsv_noeeprom(rgb_last_h, rgb_last_s, rgblight_config.val);
//     rgblight_set();
//     rgb_update = false;
//   }
// }

void suspend_power_down_user(void)
{
  rgblight_disable_noeeprom();
  //rgblight_config.enable = false;
  //rgblight_set();
}

void suspend_wakeup_init_user(void)
{
  rgblight_config.raw = eeconfig_read_rgblight();
  rgblight_set();
}

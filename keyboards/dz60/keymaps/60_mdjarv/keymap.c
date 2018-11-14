#include QMK_KEYBOARD_H

#define RGB_H_TRANSITION_STEP 8
#define RGB_S_TRANSITION_STEP 10

#define RGB_DEFAULT_HUE 24
#define RGB_DEFAULT_SATURATION 0xFF

#define RGB_LAYER1_HUE 0
#define RGB_LAYER1_SATURATION 0

static uint8_t rgb_last_h = RGB_DEFAULT_HUE;
static uint8_t rgb_last_s = RGB_DEFAULT_SATURATION;

static uint8_t rgb_desired_h = RGB_DEFAULT_HUE;
static uint8_t rgb_desired_s = RGB_DEFAULT_SATURATION;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    LAYOUT_60_ansi(
        KC_ESC , KC_1   , KC_2   , KC_3   , KC_4   , KC_5   , KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_MINS, KC_EQL , KC_BSPC,
        KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   , KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_LBRC, KC_RBRC, KC_BSLS,
        MO(1)  , KC_A   , KC_S   , KC_D   , KC_F   , KC_G   , KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_QUOT, KC_ENT ,
        KC_LSFT, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   , KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_RSFT,
        KC_LCTL, KC_LGUI, KC_LALT, KC_SPC , KC_RALT, KC_RGUI, KC_APP , KC_RCTL
    ),

    LAYOUT_60_ansi(
        KC_GRV , KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 , KC_DEL ,
        KC_TRNS, KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_PGUP, KC_UP  , KC_PGDN, KC_NO  , KC_NO  , KC_NO  , KC_NO  ,
        KC_TRNS, KC_MUTE, KC_VOLD, KC_VOLU, KC_MPLY, KC_NO  , KC_HOME, KC_LEFT, KC_DOWN, KC_RGHT, KC_INS , KC_NO  , KC_TRNS,
        KC_TRNS, RGB_TOG, RGB_VAD, RGB_VAI, KC_NO  , KC_NO  , KC_END , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    )

};

uint32_t layer_state_set_user(uint32_t state)
{
  switch (biton32(state))
  {
  case 0:
    rgb_desired_h = RGB_DEFAULT_HUE;
    rgb_desired_s = RGB_DEFAULT_SATURATION;
    break;
  case 1:
    rgb_desired_h = RGB_LAYER1_HUE;
    rgb_desired_s = RGB_LAYER1_SATURATION;
    break;
  }
  return state;
}

void matrix_init_user(void)
{
  rgblight_enable();
  rgblight_sethsv(RGB_DEFAULT_HUE, RGB_DEFAULT_SATURATION, 0xFF);
  rgblight_mode(RGBLIGHT_MODE_STATIC_LIGHT);
}

static bool rgb_update = false;

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

void matrix_scan_user(void)
{
  if (rgb_last_h != rgb_desired_h)
  {
    rgb_last_h = slide_towards(rgb_last_h, rgb_desired_h, RGB_H_TRANSITION_STEP);
    rgb_update = true;
  }

  if (rgb_last_s != rgb_desired_s)
  {
    rgb_last_s = slide_towards(rgb_last_s, rgb_desired_s, RGB_S_TRANSITION_STEP);
    rgb_update = true;
  }

  if (rgb_update)
  {
    rgblight_sethsv(rgb_last_h, rgb_last_s, rgblight_get_val());
    rgb_update = false;
  }
}

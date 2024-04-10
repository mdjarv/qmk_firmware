/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "quantum.h"

#ifdef SWAP_HANDS_ENABLE
__attribute__((weak)) const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
    // Left
    {{0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}},
    {{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}},
    {{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}},
    {{0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}},
    // Right
    {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}},
    {{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}},
    {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}},
    {{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}}
};
#endif

#ifdef OLED_ENABLE

oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        // return OLED_ROTATION_180; // flips the display 180 degrees if offhand
        return OLED_ROTATION_270; // flips the display 180 degrees if offhand
    }
    return rotation;
}

static void oled_render_layer_state(void) {
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case 0:
            oled_write_ln_P(PSTR("Default"), false);
            break;
        case 1:
            oled_write_ln_P(PSTR("Lower"), false);
            break;
        case 2:
            oled_write_ln_P(PSTR("Raise"), false);
            break;
        case 3:
            oled_write_ln_P(PSTR("Adjust"), false);
            break;
        default:
            oled_write_ln_P(PSTR("Undef"), false);
            break;
    }
}

char     key_name = ' ';
uint16_t last_keycode;
uint8_t  last_row;
uint8_t  last_col;

static const char PROGMEM code_to_name[60] = {' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\', '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

static void set_keylog(uint16_t keycode, keyrecord_t *record) {
    // save the row and column (useful even if we can't find a keycode to show)
    last_row = record->event.key.row;
    last_col = record->event.key.col;

    key_name     = ' ';
    last_keycode = keycode;
    if (IS_QK_MOD_TAP(keycode)) {
        if (record->tap.count) {
            keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
        } else {
            keycode = 0xE0 + biton(QK_MOD_TAP_GET_MODS(keycode) & 0xF) + biton(QK_MOD_TAP_GET_MODS(keycode) & 0x10);
        }
    } else if (IS_QK_LAYER_TAP(keycode) && record->tap.count) {
        keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    } else if (IS_QK_MODS(keycode)) {
        keycode = QK_MODS_GET_BASIC_KEYCODE(keycode);
    } else if (IS_QK_ONE_SHOT_MOD(keycode)) {
        keycode = 0xE0 + biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0xF) + biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0x10);
    }
    if (keycode > ARRAY_SIZE(code_to_name)) {
        return;
    }

    // update keylog
    key_name = pgm_read_byte(&code_to_name[keycode]);
}

static const char *depad_str(const char *depad_str, char depad_char) {
    while (*depad_str == depad_char)
        ++depad_str;
    return depad_str;
}

static void oled_render_keylog(void) {
    oled_write_char('0' + last_row, false);
    oled_write_P(PSTR("x"), false);
    oled_write_char('0' + last_col, false);
    oled_write_P(PSTR(", k"), false);
    const char *last_keycode_str = get_u16_str(last_keycode, ' ');
    oled_write(depad_str(last_keycode_str, ' '), false);
    oled_write_P(PSTR(":"), false);
    oled_write_char(key_name, false);
}

// static void render_bootmagic_status(bool status) {
//     /* Show Ctrl-Gui Swap options */
//     static const char PROGMEM logo[][2][3] = {
//         {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
//         {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
//     };
//     if (status) {
//         oled_write_ln_P(logo[0][0], false);
//         oled_write_ln_P(logo[0][1], false);
//     } else {
//         oled_write_ln_P(logo[1][0], false);
//         oled_write_ln_P(logo[1][1], false);
//     }
// }

__attribute__((weak)) void oled_render_ctrl_shift(void) {
    // clang-format off
    static const char PROGMEM ctrl_off_shift_off[] = {
      0x89, 0x8A, 0x8B, 0x8C, 10,
      0xA9, 0xAA, 0xAB, 0xAC, 0,
    };

    static const char PROGMEM ctrl_on_shift_off[] = {
      0x91, 0x92, 0x8B, 0x8C, 10,
      0xB1, 0xB2, 0xAB, 0xAC, 0,
    };

    static const char PROGMEM ctrl_off_shift_on[] = {
      0x89, 0x8A, 0xCD, 0xCE, 10,
      0xA9, 0xAA, 0xCF, 0xD0, 0,
    };

    static const char PROGMEM ctrl_on_shift_on[] = {
      0x91, 0x92, 0xCD, 0xCE, 10,
      0xB1, 0xB2, 0xCF, 0xD0, 0,
    };

    // clang-format on

    const uint8_t mods = get_mods() | get_oneshot_mods() | get_weak_mods();

    if (mods & (MOD_BIT(KC_LCTL) | MOD_BIT(KC_RCTL))) {
        if (mods & (MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT))) {
            oled_write_P(ctrl_on_shift_on, false);
        } else {
            oled_write_P(ctrl_on_shift_off, false);
        }
    } else {
        if (mods & (MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT))) {
            oled_write_P(ctrl_off_shift_on, false);
        } else {
            oled_write_P(ctrl_off_shift_off, false);
        }
    }
}

__attribute__((weak)) void oled_render_os_alt(void) {
    // clang-format off
    static const char PROGMEM os_off_alt_off[] = {
      0x85, 0x86, 0x87, 0x88, 10,
      0xA5, 0xA6, 0xA7, 0xA8, 0,
    };

    static const char PROGMEM os_on_alt_off[] = {
      0x8D, 0x8E, 0x87, 0x88, 10,
      0xAD, 0xAE, 0xA7, 0xA8, 0,
    };

    static const char PROGMEM os_off_alt_on[] = {
      0x85, 0x86, 0x8F, 0x90, 10,
      0xA5, 0xA6, 0xAF, 0xB0, 0,
    };

    static const char PROGMEM os_on_alt_on[] = {
      0x8D, 0x8E, 0x8F, 0x90, 10,
      0xAD, 0xAE, 0xAF, 0xB0, 0,
    };

    // clang-format on

    const uint8_t mods = get_mods() | get_oneshot_mods() | get_weak_mods();

    if (mods & (MOD_BIT(KC_LALT) | MOD_BIT(KC_RALT))) {
        if (mods & (MOD_BIT(KC_LGUI) | MOD_BIT(KC_RGUI))) {
            oled_write_P(os_on_alt_on, false);
        } else {
            oled_write_P(os_off_alt_on, false);
        }
    } else {
        if (mods & (MOD_BIT(KC_LGUI) | MOD_BIT(KC_RGUI))) {
            oled_write_P(os_on_alt_off, false);
        } else {
            oled_write_P(os_off_alt_off, false);
        }
    }
}

__attribute__((weak)) void oled_render_layer_sprite(void) {
    // clang-format off
    static const char PROGMEM qmk_layer_lower[] = {
      0x20, 0x9A, 0x9B, 0x9C, 10,
      0x20, 0xBA, 0xBB, 0xBC, 10,
      0x20, 0xDA, 0xDB, 0xDC, 0,
    };

    static const char PROGMEM qmk_layer_default[] = {
      0x20, 0x94, 0x95, 0x96, 10,
      0x20, 0xB4, 0xB5, 0xB6, 10,
      0x20, 0xD4, 0xD5, 0xD6, 0,
    };
    static const char PROGMEM qmk_layer_raise[] = {
      0x20, 0x97, 0x98, 0x99, 10,
      0x20, 0xB7, 0xB8, 0xB9, 10,
      0x20, 0xD7, 0xD8, 0xD9, 0,
    };

    static const char PROGMEM qmk_layer_adjust[] = {
      0x20, 0x9D, 0x9E, 0x9F, 10,
      0x20, 0xBD, 0xBE, 0xBF, 10,
      0x20, 0xDD, 0xDE, 0xDF, 0,
    };

    // clang-format on
    // oled_write_P(crkbd_logo, false);

    switch (get_highest_layer(layer_state)) {
        case 0:
            oled_write_P(qmk_layer_default, false);
            break;
        case 1:
            oled_write_P(qmk_layer_lower, false);
            break;
        case 2:
            oled_write_P(qmk_layer_raise, false);
            break;
        case 3:
            oled_write_P(qmk_layer_adjust, false);
            break;
        default:
            oled_write_P(qmk_layer_default, false);
            break;
    }
}

bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }
    if (is_keyboard_master()) {
        oled_render_layer_state();
        oled_render_keylog();
    } else {
        oled_set_cursor(0, 0);
        oled_render_layer_sprite();
        oled_set_cursor(0, 4);
        oled_render_ctrl_shift();
        oled_set_cursor(0, 6);
        oled_render_os_alt();
    }
    return false;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        set_keylog(keycode, record);
    }
    return process_record_user(keycode, record);
}
#endif // OLED_ENABLE

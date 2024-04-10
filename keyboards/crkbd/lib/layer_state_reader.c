#include "quantum.h"

// in the future, should use (1U<<_LAYER_NAME) instead, but needs to be moved to keymap,c
#define L_BASE 0
#define L_LOWER 2
#define L_RAISE 4
#define L_ADJUST 8
#define L_ADJUST_TRI 14

// char layer_state_str[24];

// const char *read_layer_state(void) {
//   switch (layer_state)
//   {
//   case L_BASE:
//     snprintf(layer_state_str, sizeof(layer_state_str), "Layer: Default");
//     break;
//   case L_RAISE:
//     snprintf(layer_state_str, sizeof(layer_state_str), "Layer: Raise");
//     break;
//   case L_LOWER:
//     snprintf(layer_state_str, sizeof(layer_state_str), "Layer: Lower");
//     break;
//   case L_ADJUST:
//   case L_ADJUST_TRI:
//     snprintf(layer_state_str, sizeof(layer_state_str), "Layer: Adjust");
//     break;
//   default:
// #if defined (LAYER_STATE_32BIT)
//     snprintf(layer_state_str, sizeof(layer_state_str), "Layer: Undef-%ld", layer_state);
// #else
//     snprintf(layer_state_str, sizeof(layer_state_str), "Layer: Undef-%d", layer_state);
// #endif
//   }
//
//   return layer_state_str;
// }

const char *read_layer_state(void) {
    // clang-format off
    static const char qmk_layer_lower[] = {
        0x97, 0x98, 10,
        0xB7, 0xB8, 10,
        0xD7, 0xD8,
        0,
    };

    static const char qmk_layer_default[] = {
        0x95, 0x96, 10,
        0xB5, 0xB6, 10,
        0xD5, 0xD6,
        0,
    };
    static const char qmk_layer_raise[] = {
        0x99, 0x9A, 10,
        0xB9, 0xBA, 10,
        0xD9, 0xDA,
        0,
    };

    static const char qmk_layer_adjust[] = {
        0x9B, 0x9C, 10,
        0xBB, 0xBC, 10,
        0xDB, 0xDC,
        0,
    };

    // clang-format on

  switch (get_highest_layer(layer_state))
  {
      case 0:
        return qmk_layer_default;
      case 1:
        return qmk_layer_raise;
      case 2:
        return qmk_layer_lower;
      case 3:
        return qmk_layer_adjust;
      default:
        return qmk_layer_default;
  }
}

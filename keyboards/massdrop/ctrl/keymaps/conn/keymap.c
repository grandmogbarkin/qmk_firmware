#include QMK_KEYBOARD_H
#include <stdio.h>
#include "secrets.h"

enum ctrl_keycodes {
    L_BRI = SAFE_RANGE, //LED Brightness Increase
    L_BRD,              //LED Brightness Decrease
    L_EDG_I,            //LED Edge Brightness Increase
    L_EDG_D,            //LED Edge Brightness Decrease
    L_EDG_M,            //LED Edge lighting mode
    L_PTN,              //LED Pattern Select Next
    L_PTP,              //LED Pattern Select Previous
    L_PSI,              //LED Pattern Speed Increase
    L_PSD,              //LED Pattern Speed Decrease
    L_T_MD,             //LED Toggle Mode
    L_T_ONF,            //LED Toggle On / Off
    L_ON,               //LED On
    L_OFF,              //LED Off
    L_T_BR,             //LED Toggle Breath Effect
    L_T_PTD,            //LED Toggle Scrolling Pattern Direction and effect
    U_T_AGCR,           //USB Toggle Automatic GCR control
    DBG_TOG,            //DEBUG Toggle On / Off
    DBG_MTRX,           //DEBUG Toggle Matrix Prints
    DBG_KBD,            //DEBUG Toggle Keyboard Prints
    DBG_MOU,            //DEBUG Toggle Mouse Prints
    DBG_FAC,            //DEBUG Factory light testing (All on white)
    MD_BOOT             //Restart into bootloader after hold timeout
};

#define TG_NKRO MAGIC_TOGGLE_NKRO //Toggle 6KRO / NKRO mode

keymap_config_t keymap_config;

#define P_HSCR_C MT(MOD_LGUI | MOD_LCTL, LGUI(LALT(KC_C)))

// ****** TAPDANCE *****
// tapdance keycodes
enum td_keycodes {
  SCR_CENTER // `LGUI | LCTL` when held, `LGUI(LALT(KC_C))` when tapped.
};

// define a type containing as many tapdance states as you need
typedef enum {
  SINGLE_TAP,
  SINGLE_HOLD
} td_state_t;

// create a global instance of the tapdance state type
static td_state_t td_state;

// declare your tapdance functions:

// function to determine the current tapdance state
int cur_dance (qk_tap_dance_state_t *state);

// `finished` and `reset` functions for each tapdance keycode
void scr_center_finished (qk_tap_dance_state_t *state, void *user_data);
void scr_center_reset (qk_tap_dance_state_t *state, void *user_data);
// ****** TAPDANCE *****

#define P_3RDS LCTL(LALT(KC_RIGHT))

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,             LGUI(LSFT(KC_3)), LGUI(LSFT(KC_4)), P_3RDS, \
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,   KC_INS,  KC_HOME, KC_PGUP, \
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,   KC_DEL,  KC_END,  KC_PGDN, \
        KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT, \
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,                              KC_UP, \
        KC_LCTL, KC_LALT, KC_LGUI,                   KC_SPC,                             KC_RGUI, MO(1),   KC_RALT, KC_RCTL,            KC_LEFT, KC_DOWN, KC_RGHT \
    ),
    [1] = LAYOUT(
        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_EJCT,            LALT(LCTL(LSFT(KC_LEFT))), LALT(LCTL(LSFT(KC_RIGHT))), KC_MUTE, \
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______, _______,   KC_MPLY, KC_MSTP, KC_VOLU, \
        L_T_BR,  L_PSD,   L_BRI,   L_PSI,   L_EDG_I, DBG_TOG, DBG_MTRX,DBG_KBD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,   KC_MPRV, KC_MNXT, KC_VOLD, \
        L_T_PTD, L_PTP,   L_BRD,   L_PTN,   L_EDG_D, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, \
        _______, L_T_MD,  L_T_ONF, XXXXXXX, L_EDG_M, MD_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, LCTL(LSFT(KC_S)), LALT(LCTL(LGUI(LSFT(KC_L)))), LGUI(LSFT(KC_H)), _______, \
        _______, _______, _______,                   DBG_FAC,                            KC_F11,  _______, TD(SCR_CENTER), TO(2),            _______, _______, _______ \
    ),
    [2] = LAYOUT(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX, XXXXXXX, \
        XXXXXXX, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    XXXXXXX, XXXXXXX, KC_BSPC,   XXXXXXX, XXXXXXX, XXXXXXX, \
        XXXXXXX, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,   XXXXXXX, XXXXXXX, XXXXXXX, \
        XXXXXXX, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT, \
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,                              XXXXXXX, \
        XXXXXXX, XXXXXXX, XXXXXXX,                   KC_SPC,                             XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX, XXXXXXX \
    ),
    /*
    [X] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,            _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
        _______, _______, _______, _______, _______, _______, TG_NKRO, _______, _______, _______, _______, _______,                              _______, \
        _______, _______, _______,                   _______,                            _______, _______, _______, _______,            _______, _______, _______ \
    ),
    */
};

// ****** TAPDANCE *****
// determine the tapdance state to return
int cur_dance (qk_tap_dance_state_t *state) {
  if (state->count == 1) {
    if (state->interrupted || !state->pressed) { return SINGLE_TAP; }
    else { return SINGLE_HOLD; }
  }
  // if (state->count == 2) { return DOUBLE_SINGLE_TAP; }
  else { return 2; } // any number higher than the maximum state value you return above
}

// handle the possible states for each tapdance keycode you define:

void scr_center_finished (qk_tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  switch (td_state) {
    case SINGLE_TAP:
      register_code16(LGUI(LALT(KC_C)));
      break;
    case SINGLE_HOLD:
      register_mods(MOD_BIT(KC_LGUI) | MOD_BIT(KC_LCTL)); // for a layer-tap key, use `layer_on(_MY_LAYER)` here
      break;
    // case DOUBLE_SINGLE_TAP: // allow nesting of 2 parens `((` within tapping term
    //   tap_code16(KC_LPRN);
    //   register_code16(KC_LPRN);
  }
}

void scr_center_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (td_state) {
    case SINGLE_TAP:
      unregister_code16(LGUI(LALT(KC_C)));
      break;
    case SINGLE_HOLD:
      unregister_mods(MOD_BIT(KC_LGUI) | MOD_BIT(KC_LCTL)); // for a layer-tap key, use `layer_off(_MY_LAYER)` here
      break;
    // case DOUBLE_SINGLE_TAP:
    //   unregister_code16(KC_LPRN);
  }
}

// define `ACTION_TAP_DANCE_FN_ADVANCED()` for each tapdance keycode, passing in `finished` and `reset` functions
qk_tap_dance_action_t tap_dance_actions[] = {
  [SCR_CENTER] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, scr_center_finished, scr_center_reset)
};
// ****** TAPDANCE *****

static uint8_t scroll_effect = 0;

// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {
  led_animation_id = 7; // 8
  led_animation_speed = 6.0f;
  // scroll_effect = 4;
  // led_animation_circular = 1;
  scroll_effect = 6;
  led_animation_bounce = 1;
};

// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {
};

#define MODS_SHIFT (get_mods() & MOD_BIT(KC_LSHIFT) || get_mods() & MOD_BIT(KC_RSHIFT))
#define MODS_CTRL (get_mods() & MOD_BIT(KC_LCTL) || get_mods() & MOD_BIT(KC_RCTRL))
#define MODS_ALT (get_mods() & MOD_BIT(KC_LALT) || get_mods() & MOD_BIT(KC_RALT))

int modulo(int n,int M){
  return ((n % M) + M) % M;
}

void decrypt(char *msg, int msgLen, char* key, int keyLen, char* decryptedMsg) {
  const int u_len = 95;
  const int u_base = 32;

  int i;
  for(i = 0; i < msgLen; ++i) {
      decryptedMsg[i] = modulo(((msg[i]-u_base) - (key[i%keyLen]-u_base)), u_len) + u_base;
  }

  decryptedMsg[i] = '\0';
}

bool process_custom_record_keymap(uint16_t keycode, keyrecord_t *record) {
  static char key[100];
  static int key_len = 0;
  static char dec_map[100];
  static int dec_len = -1;
  static int shifted = 0;

  static int keycode_map[] = {
    0, 0, 0, 0,
    97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, // letters
    49, 50, 51, 52, 53, 54, 55, 56, 57, 48, // numbers
    0, 0, 0, 0,
    32, // space
    0, 0,
    91, 93, 92, // [ ] slash
    0,
    59, 39, // ; '
    0,
    44, 46, 47 // , . /
  };
  static int keycode_shift_map[] = {
    0, 0, 0, 0,
    65, 66, 67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90, // caps
    33, 64, 35, 36, 37, 94, 38, 42, 40, 41, // number row
    0, 0, 0, 0,
    32, // space
    0, 0,
    123, 125, 124, // { } |
    0,
    58, 34, // : "
    0,
    60, 62, 63 // < > ?
  };

  if (!IS_LAYER_ON(2)) return true;  //Process all non-layer 2 keycodes normally

  switch (keycode) {
    case KC_ENT:
      if (record->event.pressed) {
        key[key_len] = '\0';
        if (dec_len >= 0) {
          int msg_len = dec_len * 4;
          char msg[msg_len + 3];
          int j, final_len;
          for (final_len = 0, j = 0; j < dec_len; ++j) {
            for (int k = 0; k < 4; ++k) {
              msg[(j*4)+k] = keycode_vignere_enc[dec_map[j] - 33].str[k];
              ++final_len;
            }
          }
          if (dec_len % 2) {
            msg[final_len++] = 'f';
            msg[final_len++] = 'R';
          } else {
            msg[final_len++] = 'd';
          }
          char decrypted[j];
          decrypt(msg, final_len, key, key_len, decrypted);
          send_string(decrypted);
          key_len = 0;
          dec_len = -1;
          layer_off(2);
        } else {
          dec_len = 0;
        }
        shifted = 0;
      }
      break;
    case KC_RSFT:
    case KC_LSFT:
      if (record->event.pressed) {
        shifted = 1;
      } else {
        shifted = 0;
      }
      break;
    case KC_BSPC:
      if (record->event.pressed) {
        if (dec_len < 0 && key_len > 0) {
          key_len--;
        } else if (dec_len > 0) {
          dec_len--;
        }
      }
      break;
    default:
      if (record->event.pressed) {
        char next_char = shifted ? keycode_shift_map[keycode] : keycode_map[keycode];
        if (dec_len < 0) {
          key[key_len++] = next_char;
        } else {
          dec_map[dec_len++] = next_char;
        }
      }
      break;
  }
  unregister_code(keycode);
  return false; // Don't process anything else
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint32_t key_timer;

    switch (keycode) {
        case L_BRI:
            if (record->event.pressed) {
                if (LED_GCR_STEP > LED_GCR_MAX - gcr_desired) gcr_desired = LED_GCR_MAX;
                else gcr_desired += LED_GCR_STEP;
                if (led_animation_breathing) gcr_breathe = gcr_desired;
            }
            return false;
        case L_BRD:
            if (record->event.pressed) {
                if (LED_GCR_STEP > gcr_desired) gcr_desired = 0;
                else gcr_desired -= LED_GCR_STEP;
                if (led_animation_breathing) gcr_breathe = gcr_desired;
            }
            return false;
        case L_EDG_M:
            if (record->event.pressed) {
                led_edge_mode++;
                if (led_edge_mode > LED_EDGE_MODE_MAX) {
                    led_edge_mode = LED_EDGE_MODE_ALL;
                }
            }
            return false;
        case L_EDG_I:
            if (record->event.pressed) {
                led_edge_brightness += 0.1;
                if (led_edge_brightness > 1) { led_edge_brightness = 1; }
            }
            return false;
        case L_EDG_D:
            if (record->event.pressed) {
                led_edge_brightness -= 0.1;
                if (led_edge_brightness < 0) { led_edge_brightness = 0; }
            }
            return false;
        case L_PTN:
            if (record->event.pressed) {
                if (led_animation_id == led_setups_count - 1) led_animation_id = 0;
                else led_animation_id++;
            }
            return false;
        case L_PTP:
            if (record->event.pressed) {
                if (led_animation_id == 0) led_animation_id = led_setups_count - 1;
                else led_animation_id--;
            }
            return false;
        case L_PSI:
            if (record->event.pressed) {
                led_animation_speed += ANIMATION_SPEED_STEP;
            }
            return false;
        case L_PSD:
            if (record->event.pressed) {
                led_animation_speed -= ANIMATION_SPEED_STEP;
                if (led_animation_speed < 0) led_animation_speed = 0;
            }
            return false;
        case L_T_MD:
            if (record->event.pressed) {
                led_lighting_mode++;
                if (led_lighting_mode > LED_MODE_MAX_INDEX) led_lighting_mode = LED_MODE_NORMAL;
            }
            return false;
        case L_T_ONF:
            if (record->event.pressed) {
                I2C3733_Control_Set(!I2C3733_Control_Get());
            }
            return false;
        case L_ON:
            if (record->event.pressed) {
                I2C3733_Control_Set(1);
            }
            return false;
        case L_OFF:
            if (record->event.pressed) {
                I2C3733_Control_Set(0);
            }
            return false;
        case L_T_BR:
            if (record->event.pressed) {
                led_animation_breathing = !led_animation_breathing;
                if (led_animation_breathing) {
                    gcr_breathe = gcr_desired;
                    led_animation_breathe_cur = BREATHE_MIN_STEP;
                    breathe_dir = 1;
                }
            }
            return false;
        case L_T_PTD:
            if (record->event.pressed) {
                scroll_effect++;
                if (scroll_effect == 1) {               //Patterns with scroll move horizontal (Right to left)
                    led_animation_direction = 1;
                    led_animation_orientation = 0;
                    led_animation_circular = 0;
                    led_animation_bounce = 0;
                } else if (scroll_effect == 2) {        //Patterns with scroll move vertical (Top to bottom)
                    led_animation_direction = 1;
                    led_animation_orientation = 1;
                    led_animation_circular = 0;
                    led_animation_bounce = 0;
                } else if (scroll_effect == 3) {        //Patterns with scroll move vertical (Bottom to top)
                    led_animation_direction = 0;
                    led_animation_orientation = 1;
                    led_animation_circular = 0;
                    led_animation_bounce = 0;
                } else if (scroll_effect == 4) {        //Patterns with scroll explode from center
                    led_animation_direction = 0;
                    led_animation_orientation = 0;
                    led_animation_circular = 1;
                    led_animation_bounce = 0;
                } else if (scroll_effect == 5) {        //Patterns with scroll implode on center
                    led_animation_direction = 1;
                    led_animation_orientation = 0;
                    led_animation_circular = 1;
                    led_animation_bounce = 0;
                } else if (scroll_effect == 6) {        // Patterns with scroll bounce left/right
                  led_animation_direction = 0;
                  led_animation_orientation = 0;
                  led_animation_circular = 0;
                  led_animation_bounce = 1;
                } else {                                //Patterns with scroll move horizontal (Left to right)
                    scroll_effect = 0;
                    led_animation_direction = 0;
                    led_animation_orientation = 0;
                    led_animation_circular = 0;
                    led_animation_bounce = 0;
                }
            }
            return false;
        case U_T_AGCR:
            if (record->event.pressed && MODS_SHIFT && MODS_CTRL) {
                TOGGLE_FLAG_AND_PRINT(usb_gcr_auto, "USB GCR auto mode");
            }
            return false;
        case DBG_FAC:
            if (record->event.pressed && MODS_SHIFT && MODS_CTRL) {
                led_lighting_mode = LED_MODE_NORMAL;
                led_edge_brightness = 1;
                led_edge_mode = LED_EDGE_MODE_ALL;
                led_animation_breathing = 0;
                led_animation_id = 7; //led_programs.c led_setups leds_white index
                gcr_desired = LED_GCR_MAX;
                I2C3733_Control_Set(1);
            }
            return false;
        case DBG_TOG:
            if (record->event.pressed) {
                TOGGLE_FLAG_AND_PRINT(debug_enable, "Debug mode");
            }
            return false;
        case DBG_MTRX:
            if (record->event.pressed) {
                TOGGLE_FLAG_AND_PRINT(debug_matrix, "Debug matrix");
            }
            return false;
        case DBG_KBD:
            if (record->event.pressed) {
                TOGGLE_FLAG_AND_PRINT(debug_keyboard, "Debug keyboard");
            }
            return false;
        case DBG_MOU:
            if (record->event.pressed) {
                TOGGLE_FLAG_AND_PRINT(debug_mouse, "Debug mouse");
            }
            return false;
        case MD_BOOT:
            if (record->event.pressed) {
                key_timer = timer_read32();
            } else {
                if (timer_elapsed32(key_timer) >= BOOTKEY_HOLD_MS) {
                    reset_keyboard();
                }
            }
            return false;
        default:
            return process_custom_record_keymap(keycode, record);
    }
}

led_instruction_t led_instructions[] = {
    //LEDs are normally inactive, no processing is performed on them
    //Flags are used in matching criteria for an LED to be active and indicate how to color it
    //Flags can be found in tmk_core/protocol/arm_atsam/led_matrix.h (prefixed with LED_FLAG_)
    //LED IDs can be found in config_led.h in the keyboard's directory
    //Examples are below

    //All LEDs use the user's selected pattern (this is the factory default)
    //{ .flags = LED_FLAG_USE_ROTATE_PATTERN }

    // layer 0 rotate pattern, layer 1 off
    { .flags = LED_FLAG_USE_ROTATE_PATTERN |  LED_FLAG_MATCH_LAYER, .layer = 0 },
    // layer 1 edge pattern
    { .flags = LED_FLAG_MATCH_ID | LED_FLAG_MATCH_LAYER | LED_FLAG_USE_ROTATE_PATTERN, .layer = 1, .id2 = 4286578688, .id3 = 8388607 },
    // layer 1 LED keys - blue
    { .flags = LED_FLAG_MATCH_ID | LED_FLAG_MATCH_LAYER | LED_FLAG_USE_RGB, .layer = 1, .id0 = 0, .id1 = 8126526, .id2 = 11, .id3 = 0, .r = 0, .g = 0, .b = 255 },
    // layer 1 mod keys - red
    { .flags = LED_FLAG_MATCH_ID | LED_FLAG_MATCH_LAYER | LED_FLAG_USE_RGB, .layer = 1, .id0 = 0, .id1 = 448, .id2 = 16, .id3 = 0, .r = 255, .g = 0, .b = 0 },
    // layer 1 lock keys - purple
    { .flags = LED_FLAG_MATCH_ID | LED_FLAG_MATCH_LAYER | LED_FLAG_USE_RGB, .layer = 1, .id0 = 0, .id1 = 0, .id2 = 67328, .id3 = 0, .r = 148, .g = 0, .b = 211 },
    // layer 1 os keys - green
    { .flags = LED_FLAG_MATCH_ID | LED_FLAG_MATCH_LAYER | LED_FLAG_USE_RGB, .layer = 1, .id0 = 3221286912, .id1 = 229377, .id2 = 262144, .id3 = 0, .r = 0, .g = 255, .b = 0 },
    // layer 1 passthrough keys - yellow
    { .flags = LED_FLAG_MATCH_ID | LED_FLAG_MATCH_LAYER | LED_FLAG_USE_RGB, .layer = 1, .id0 = 939524096, .id1 = 0, .id2 = 7342080, .id3 = 0, .r = 255, .g = 255, .b = 0 },

    //Specific LEDs use the user's selected pattern while all others are off
    // { .flags = LED_FLAG_MATCH_ID | LED_FLAG_USE_ROTATE_PATTERN, .id0 = 0xFFFFFFFF, .id1 = 0xAAAAAAAA, .id2 = 0x55555555, .id3 = 0x11111111 },

    //Specific LEDs use specified RGB values while all others are off
    // { .flags = LED_FLAG_MATCH_ID | LED_FLAG_USE_RGB, .id0 = 0xFF, .id1 = 0x00FF, .id2 = 0x0000FF00, .id3 = 0xFF000000, .r = 75, .g = 150, .b = 225 },

    //All LEDs use the user's selected pattern
    //On layer 1, all key LEDs (except the top row which keeps active pattern) are red while all edge LEDs are green
    //When layer 1 is active, key LEDs use red    (id0  32 -  17: 1111 1111 1111 1111 0000 0000 0000 0000 = 0xFFFF0000) (except top row 16 - 1)
    //When layer 1 is active, key LEDs use red    (id1  64 -  33: 1111 1111 1111 1111 1111 1111 1111 1111 = 0xFFFFFFFF)
    //When layer 1 is active, key LEDs use red    (id2  87 -  65: 0000 0000 0111 1111 1111 1111 1111 1111 = 0x007FFFFF)
    //When layer 1 is active, edge LEDs use green (id2  95 -  88: 1111 1111 1000 0000 0000 0000 0000 0000 = 0xFF800000)
    //When layer 1 is active, edge LEDs use green (id3 119 -  96: 0000 0000 1111 1111 1111 1111 1111 1111 = 0x00FFFFFF)
    // { .flags = LED_FLAG_USE_ROTATE_PATTERN },
    // { .flags = LED_FLAG_MATCH_ID | LED_FLAG_MATCH_LAYER | LED_FLAG_USE_RGB, .id0 = 0xFFFF0000, .id1 = 0xFFFFFFFF, .id2 = 0x007FFFFF, .r = 255, .layer = 1 },
    // { .flags = LED_FLAG_MATCH_ID | LED_FLAG_MATCH_LAYER | LED_FLAG_USE_RGB, .id2 = 0xFF800000, .id3 = 0x00FFFFFF, .g = 127, .layer = 1 },

    //All key LEDs use red while edge LEDs use the active pattern
    //All key LEDs use red     (id0  32 -   1: 1111 1111 1111 1111 1111 1111 1111 1111 = 0xFFFFFFFF)
    //All key LEDs use red     (id1  64 -  33: 1111 1111 1111 1111 1111 1111 1111 1111 = 0xFFFFFFFF)
    //All key LEDs use red     (id2  87 -  65: 0000 0000 0111 1111 1111 1111 1111 1111 = 0x007FFFFF)
    //Edge uses active pattern (id2  95 -  88: 1111 1111 1000 0000 0000 0000 0000 0000 = 0xFF800000)
    //Edge uses active pattern (id3 119 -  96: 0000 0000 1111 1111 1111 1111 1111 1111 = 0x00FFFFFF)
    // { .flags = LED_FLAG_MATCH_ID | LED_FLAG_USE_RGB, .id0 = 0xFFFFFFFF, .id1 = 0xFFFFFFFF, .id2 = 0x007FFFFF, .r = 255 },
    // { .flags = LED_FLAG_MATCH_ID | LED_FLAG_USE_ROTATE_PATTERN , .id2 = 0xFF800000, .id3 = 0x00FFFFFF },

    //end must be set to 1 to indicate end of instruction set
     { .end = 1 }
};

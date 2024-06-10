// Copyright 2023 Massdrop, Inc.
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "secrets.h"

enum custom_keycodes {
    TEST_1 = SAFE_RANGE,
    TEST_2,
    TEST_3,
    TEST_4,
};

#define P_LOCK LCTL(LALT(LSFT(LGUI(KC_L))))
#define P_WINRST LALT(LCTL(KC_BSPC))
#define P_3RDS LCTL(LALT(KC_RIGHT))
#define P_SCRLEFT LALT(LCTL(LSFT(KC_LEFT)))
#define P_SCRRIGHT LALT(LCTL(LSFT(KC_RIGHT)))
#define CODE_LAYER 3

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_tkl_ansi(
        KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,    LGUI(LSFT(KC_3)), LGUI(LSFT(KC_4)), P_3RDS,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,   KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,   KC_DEL,  KC_END,  KC_PGDN,
        KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,            KC_UP,
        KC_LCTL, KC_LALT, KC_LGUI,                   KC_SPC,                                      KC_RGUI, MO(1),   KC_RALT, KC_RCTL,   KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [1] = LAYOUT_tkl_ansi(
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    P_SCRLEFT, P_SCRRIGHT, KC_MUTE,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    KC_MPLY, KC_MSTP, KC_VOLU,
        _______, RGB_TOG, RGB_VAI, RGB_SPI, RGB_HUI, RGB_SAI, _______, _______, _______, _______, _______, _______, _______, _______,    KC_MPRV, KC_MNXT, KC_VOLD,
        _______, RGB_MOD, RGB_VAD, RGB_SPD, RGB_HUD, RGB_SAD, _______, _______, _______, _______, _______, _______,          _______,
        _______,          RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW,_______, NK_TOGG, _______, _______, LCTL(LSFT(KC_S)), P_LOCK, LGUI(LSFT(KC_H)),     _______,
        MO(2),   _______, _______,                            EE_CLR,                             P_WINRST,_______, _______, TO(3),      _______, _______, _______
    ),
    [2] = LAYOUT_tkl_ansi(
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    _______, _______, _______,
        _______, TEST_1,  TEST_2,  TEST_3,  TEST_4,  _______, _______, _______, _______, _______, _______, _______, _______, QK_BOOT,    _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, AG_TOGG, _______, _______, _______, CL_TOGG,    _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,             _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______,    _______, _______, _______
    ),
    [CODE_LAYER] = LAYOUT_tkl_ansi(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    XXXXXXX, XXXXXXX, KC_BSPC,   XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,   XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,                              XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX,                   KC_SPC,                             XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX, XXXXXXX
    )
};

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
  if (!IS_LAYER_ON(CODE_LAYER)) return true;  //Process all non-code-layer keycodes normally

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

  switch (keycode) {
    case KC_ENT:
      if (record->event.pressed) {
        key[key_len] = '\0';
        if (dec_len >= 0) {
          int msg_len = dec_len * 4;
          char msg[msg_len+1];
          int j, final_len;
          for (final_len = 0, j = 0; j < dec_len; ++j) {
              int kMax = 4;
              // Allow for some number keys to only use a subset of the characters
              if (dec_map[j]-33 >= 15 && dec_map[j]-33 <= 18) { // 0-3 use 2 characters
                  kMax = 2;
              } else if (dec_map[j]-33 >= 19 && dec_map[j]-33 <=24) { // 4-9 use 1 character
                  kMax = 1;
              }
              for (int k = 0; k < kMax; ++k) {
                  msg[(j*4)+k] = keycode_vignere_enc[dec_map[j] - 33].str[k];
                  ++final_len;
              }
          }
          msg[final_len] = '\0'; // technically unnecessary, but why not.
          char decrypted[j];
          decrypt(msg, final_len, key, key_len, decrypted);
          send_string(decrypted);
          key_len = 0;
          dec_len = -1;
          layer_off(CODE_LAYER);
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
    if (!record->event.pressed) {
        switch (keycode) {
            case TEST_1:
                rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
                rgb_matrix_sethsv_noeeprom(HSV_RED);
                break;
            case TEST_2:
                rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
                rgb_matrix_sethsv_noeeprom(HSV_GREEN);
                break;
            case TEST_3:
                rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
                rgb_matrix_sethsv_noeeprom(HSV_BLUE);
                break;
            case TEST_4:
                rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
                rgb_matrix_sethsv_noeeprom(HSV_WHITE);
                break;
        }
    }
    return process_custom_record_keymap(keycode, record);
};

/*
Copyright 2016 Google Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "ergodox.h"
#include "debug.h"
#include "timer.h"
#include "action_layer.h"
#include "version.h"

#define BASE        0 // PC default layer
#define MEDIA       1 // PC media layer
#define OSX         2 // Mac OSX layer
#define OSXMEDIA    3 // Mac OSX media layer

uint16_t kf_timers[12];

enum {
  KF_VERSION,

  // Function / number keys
  KF_1, // 1, F1
  KF_2, // 2, F2
  KF_3, // ...
  KF_4,
  KF_5,
  KF_6,
  KF_7,
  KF_8,
  KF_9,
  KF_10,
  KF_11  // -, F11
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Keymap 0: Basic layer
 *
 * ,---------------------------------------------------.           ,---------------------------------------------------.
 * |   =    |   1  |   2   |   3  |   4  |   5  |      |           |      |   6  |   7  |   8  |   9   |   0  |   -    |
 * |--------+------+-------+------+------+-------------|           |------+------+------+------+-------+------+--------|
 * | Tab    |   "  |   ,   |   .  |   P  |   Y  |  [   |           |  ]   |   Y  |   G  |   C  |   R   |   L  |   \    |
 * |--------+------+-------+------+------+------|      |           |      |------+------+------+-------+------+--------|
 * |Ctrl/Esc|   A  |   O   |   E  |   U  |   I  |------|           |------|   D  |   H  |   T  |   N   |   S  |'/MEDIA |
 * |--------+------+-------+------+------+------|  (   |           |  )   |------+------+------+-------+------+--------|
 * | LShift |   ;  |   Q   |   J  |   K  |   X  |      |           |      |   B  |   M  |   W  |   V   |   Z  |   /    |
 * `--------+------+-------+------+------+-------------'           `-------------+------+------+-------+------+--------'
 *   | Cut  | Copy | Paste | Left | Right|                                       |  Up  | Down |AltLGui|   ~  | ~OSX |
 *   `-----------------------------------'                                       `-----------------------------------'
 *                                        ,-------------.       ,---------------.
 *                                        | Alt  | LGui |       |      |        |
 *                                 ,------|------|------|       |------+--------+------.
 *                                 |      |      | Home |       | PgUp |        |      |
 *                                 | Back | Del  |------|       |------|  Enter |Space |
 *                                 | Space|      | End  |       | PgDn |        |      |
 *                                 `--------------------'       `----------------------'
 */

/*
TD = TAP DANCE
M = Macro

ALT+SUPER is used for switching input language mode.
*/

// If it accepts an argument (i.e, is a function), it doesn't need KC_.
// Otherwise, it needs KC_*
[BASE] = KEYMAP(  // layer 0 : default
        // left hand
        KC_EQL,         M(KF_1),    M(KF_2),        M(KF_3),    M(KF_4),    M(KF_5),    KC_NO,
        KC_TAB,         KC_QUOT,    KC_COMM,        KC_DOT,     KC_P,       KC_Y,       KC_LBRC,
        CTL_T(KC_ESC),  KC_A,       KC_O,           KC_E,       KC_U,       KC_I,
        KC_LSFT,        KC_SCLN,    KC_Q,           KC_J,       KC_K,       KC_X,       KC_LPRN,
        LCTL(KC_X),     LCTL(KC_C), LCTL(KC_V),     KC_LEFT,KC_RGHT,
                                               KC_LALT,  KC_LGUI,
                                                              KC_HOME,
                                               KC_BSPC,KC_DELT,KC_END,
        // right hand
        KC_NO,          M(KF_6),    M(KF_7),    M(KF_8),    M(KF_9),        M(KF_10),       M(KF_11),
        KC_RBRC,        KC_F,       KC_G,       KC_C,       KC_R,           KC_L,           KC_BSLS,
                        KC_D,       KC_H,       KC_T,       KC_N,           KC_S,           LT(MEDIA, KC_QUOT),
        KC_RPRN,        KC_B,       KC_M,       KC_W,       KC_V,           KC_Z,           KC_SLSH,
                                    KC_UP,      KC_DOWN,    LALT(KC_LGUI),  KC_GRV,         TO(OSX, 1),
        KC_NO,        KC_NO,
        KC_PGUP,
        KC_PGDN, KC_ENT, KC_SPC
    ),

/* Keymap 1: Media and mouse keys
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * |        |      |      |      |      |      |      |           |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
 * |        |      |      | MsUp |      |      |      |           |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * |        |      |MsLeft|MsDown|MsRght|      |------|           |------|      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |           |      |      |      | Prev | Next |      |        |
 * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
 *   |      |      |      | Lclk | Rclk |                                       |VolUp |VolDn | Mute | Play |      |
 *   `----------------------------------'                                       `----------------------------------'
 *                                        ,-------------.       ,-------------.
 *                                        |      |      |       |      |      |
 *                                 ,------|------|------|       |------+------+------.
 *                                 |      |      |      |       |      |      |Brwser|
 *                                 |      |      |------|       |------|      |Back  |
 *                                 |      |      |      |       |      |      |      |
 *                                 `--------------------'       `--------------------'
 */
// MEDIA AND MOUSE
[MEDIA] = KEYMAP(
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS, KC_TRNS, KC_TRNS, KC_MS_U, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS, KC_TRNS, KC_MS_L, KC_MS_D, KC_MS_R, KC_TRNS,
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS, KC_TRNS, KC_TRNS, KC_BTN1, KC_BTN2,
                                           KC_TRNS, KC_TRNS,
                                                    KC_TRNS,
                                  KC_TRNS, KC_TRNS, KC_TRNS,
       // right hand
       KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS,  KC_TRNS, KC_TRNS, KC_MPRV, KC_MNXT, KC_TRNS, KC_TRNS,
                          KC_VOLU, KC_VOLD, KC_MUTE, KC_MPLY, KC_TRNS,
       KC_TRNS, KC_TRNS,
       KC_TRNS,
       KC_TRNS, KC_TRNS, KC_WBAK
),

/* Keymap 2: MAC OSX layer
 *
 * ,---------------------------------------------------.           ,---------------------------------------------------.
 * |   =    |   1  |   2   |   3  |   4  |   5  |      |           |      |   6  |   7  |   8  |   9   |   0  |   -    |
 * |--------+------+-------+------+------+-------------|           |------+------+------+------+-------+------+--------|
 * | Tab    |   "  |   ,   |   .  |   P  |   Y  |  [   |           |  ]   |   Y  |   G  |   C  |   R   |   L  |   \    |
 * |--------+------+-------+------+------+------|      |           |      |------+------+------+-------+------+--------|
 * |Ctrl/Esc|   A  |   O   |   E  |   U  |   I  |------|           |------|   D  |   H  |   T  |   N   |   S  |'/MEDIA |
 * |--------+------+-------+------+------+------|  (   |           |  )   |------+------+------+-------+------+--------|
 * | LShift |   ;  |   Q   |   J  |   K  |   X  |      |           |      |   B  |   M  |   W  |   V   |   Z  |   /    |
 * `--------+------+-------+------+------+-------------'           `-------------+------+------+-------+------+--------'
 *   | Cut  | Copy | Paste | Left | Right|                                       |  Up  | Down |LGuiSPC|   ~  | ~PC  |
 *   `-----------------------------------'                                       `-----------------------------------'
 *                                        ,-------------.       ,---------------.
 *                                        | Alt  | LGui |       |      |        |
 *                                 ,------|------|------|       |------+--------+------.
 *                                 |      |      | Home |       | PgUp |        |      |
 *                                 | Back | Del  |------|       |------|  Enter |Space |
 *                                 | Space|      | End  |       | PgDn |        |      |
 *                                 `--------------------'       `----------------------'
 */

[OSX] = KEYMAP(  // layer 0 : default
        // left hand
        KC_EQL,         M(KF_1),    M(KF_2),        M(KF_3),    M(KF_4),    M(KF_5),    KC_NO,
        KC_TAB,         KC_QUOT,    KC_COMM,        KC_DOT,     KC_P,       KC_Y,       KC_LBRC,
        CTL_T(KC_ESC),  KC_A,       KC_O,           KC_E,       KC_U,       KC_I,
        KC_LSFT,        KC_SCLN,    KC_Q,           KC_J,       KC_K,       KC_X,       KC_LPRN,
        LGUI(KC_X),     LGUI(KC_C), LGUI(KC_V),     KC_LEFT,    KC_RGHT,
                                               KC_LALT,  KC_LGUI,
                                                              KC_HOME,
                                               KC_BSPC,KC_DELT,KC_END,
        // right hand
        KC_NO,          M(KF_6),    M(KF_7),    M(KF_8),    M(KF_9),        M(KF_10),       M(KF_11),
        KC_RBRC,        KC_F,       KC_G,       KC_C,       KC_R,           KC_L,           KC_BSLS,
                        KC_D,       KC_H,       KC_T,       KC_N,           KC_S,           LT(OSXMEDIA, KC_QUOT),
        KC_RPRN,        KC_B,       KC_M,       KC_W,       KC_V,           KC_Z,           KC_SLSH,
                                    KC_UP,      KC_DOWN,    LGUI(KC_SPC),   KC_GRV,         TO(BASE, 1),
        KC_NO,        KC_NO,
        KC_PGUP,
        KC_PGDN, KC_ENT, KC_SPC
    ),

/* Keymap 3: Media and mouse keys (MAC OSX)
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * |        |      |      |      |      |      |      |           |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
 * |        |      |      | MsUp |      |      |      |           |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * |        |      |MsLeft|MsDown|MsRght|      |------|           |------|      |      |      |      |      |        |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |           |      |      |      | Prev | Next |      |        |
 * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
 *   |      |      |      | Lclk | Rclk |                                       |VolUp |VolDn | Mute | Play |      |
 *   `----------------------------------'                                       `----------------------------------'
 *                                        ,-------------.       ,-------------.
 *                                        |      |      |       |      |      |
 *                                 ,------|------|------|       |------+------+------.
 *                                 |      |      |      |       |      |      |Brwser|
 *                                 |      |      |------|       |------|      |Back  |
 *                                 |      |      |      |       |      |      |      |
 *                                 `--------------------'       `--------------------'
 */
// MEDIA AND MOUSE for OSX
[OSXMEDIA] = KEYMAP(
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS, KC_TRNS, KC_TRNS, KC_MS_U, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS, KC_TRNS, KC_MS_L, KC_MS_D, KC_MS_R, KC_TRNS,
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS, KC_TRNS, KC_TRNS, KC_BTN1, KC_BTN2,
                                           KC_TRNS, KC_TRNS,
                                                    KC_TRNS,
                                  KC_TRNS, KC_TRNS, KC_TRNS,
       // right hand
       KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS,  KC_TRNS, KC_TRNS, KC_MPRV, KC_MNXT, KC_TRNS, KC_TRNS,
                          KC_VOLU, KC_VOLD, KC_MUTE, KC_MPLY, KC_TRNS,
       KC_TRNS, KC_TRNS,
       KC_TRNS,
       KC_TRNS, KC_TRNS, KC_WBAK
),

};

void ang_handle_kf(keyrecord_t *record, uint8_t id) {
    uint8_t code = id - KF_1;

    if (record->event.pressed) {
        kf_timers[code] = timer_read();
    } else {
        uint8_t kc;
    
        if (timer_elapsed(kf_timers[code]) > TAPPING_TERM) {
            // Long press
            kc = KC_F1 + code;
        } else {
          if (id == KF_11) {
              kc = KC_MINS;
          } else {
              kc = KC_1 + code;
          }
        }
    
        register_code (kc);
        unregister_code (kc);
    }
}

const uint16_t PROGMEM fn_actions[] = {
};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt) {
    // MACRODOWN only works in this function
    switch(id) {
        case KF_VERSION:
            if (record->event.pressed) {
                SEND_STRING (QMK_KEYBOARD "/" QMK_KEYMAP " @ " QMK_VERSION);
            }
            break;
        case KF_1 ... KF_11:
            ang_handle_kf(record, id);
            break;
      }
    return MACRO_NONE;
}

// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {

}

// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {

    uint8_t layer = biton32(layer_state);

    ergodox_board_led_off();
    ergodox_right_led_1_off();
    ergodox_right_led_2_off();
    ergodox_right_led_3_off();
    switch (layer) {
        case MEDIA:
            ergodox_right_led_1_on();
            break;
        case OSX:
            ergodox_right_led_2_on();
            break;
        case OSXMEDIA:
            ergodox_right_led_1_on();
            ergodox_right_led_2_on();
            break;
        default:
            // none
            break;
    }

}

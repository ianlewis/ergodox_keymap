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
#include "mousekey.h"
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

  // Diagonal mouse movement
  A_MUL,
  A_MUR,
  A_MDL,
  A_MDR
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Keymap 0: Basic layer
 *
 * ,---------------------------------------------------.           ,---------------------------------------------------.
 * |   =    |   1  |   2   |   3  |   4  |   5  |      |           |      |   6  |   7   |   8  |   9  |   0  |   `    |
 * |--------+------+-------+------+------+-------------|           |------+------+-------+------+------+------+--------|
 * | Tab    |   "  |   ,   |   .  |   P  |   Y  |  [   |           |  ]   |   Y  |   G   |   C  |   R  |   L  |   \    |
 * |--------+------+-------+------+------+------|      |           |      |------+-------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   O   |   E  |   U  |   I  |------|           |------|   D  |   H   |   T  |   N  |   S  |-/MEDIA |
 * |--------+------+-------+------+------+------|  (   |           |  )   |------+-------+------+------+------+--------|
 * | LShift |   ;  |   Q   |   J  |   K  |   X  |      |           |      |   B  |   M   |   W  |   V  |   Z  |   /    |
 * `--------+------+-------+------+------+-------------'           `-------------+-------+------+------+------+--------'
 *   | Cut  | Copy | Paste | Alt  | LGui |                                       |AltLGui|      |      |      | ~OSX |
 *   `-----------------------------------'                                       `-----------------------------------'
 *                                        ,-------------.       ,---------------.
 *                                        |      |      |       |      |        |
 *                                 ,------|------|------|       |------+--------+------.
 *                                 |      |      |      |       |      |        |      |
 *                                 | Back | Del  |------|       |------|  Enter |Space |
 *                                 | Space|      |      |       |      |        |      |
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
        KC_EQL,         KC_1,       KC_2,           KC_3,       KC_4,       KC_5,       KC_NO,
        KC_TAB,         KC_QUOT,    KC_COMM,        KC_DOT,     KC_P,       KC_Y,       KC_LBRC,
        CTL_T(KC_ESC),  KC_A,       KC_O,           KC_E,       KC_U,       KC_I,
        KC_LSFT,        KC_SCLN,    KC_Q,           KC_J,       KC_K,       KC_X,       KC_LPRN,
        LCTL(KC_X),     LCTL(KC_C), LCTL(KC_V),     KC_LALT,    KC_LGUI,
                                                                            KC_NO,      KC_NO,
                                                                                        KC_NO,
                                                                KC_BSPC,    KC_DELT,    KC_NO,
        // right hand
        KC_NO,          KC_6,       KC_7,           KC_8,       KC_9,       KC_10,      KC_GRV,
        KC_RBRC,        KC_F,       KC_G,           KC_C,       KC_R,       KC_L,       KC_BSLS,
                        KC_D,       KC_H,           KC_T,       KC_N,       KC_S,       LT(MEDIA, KC_MINS),
        KC_RPRN,        KC_B,       KC_M,           KC_W,       KC_V,       KC_Z,       KC_SLSH,
                                    LALT(KC_LGUI),  KC_NO,      KC_NO,      KC_NO,      TO(OSX, 1),
        KC_NO,          KC_NO,
        KC_NO,
        KC_NO,          KC_ENT,     KC_SPC
    ),

/* Keymap 1: Media and mouse keys
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * |        |  F1  |  F2  |  F3  |  F4  |  F5  |      |           |      |  F6  |  F7  |  F8  |  F9  |  F10 |  F11   |
 * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
 * |        |      | Home |  Up  | PgUp |      |      |           |Scroll|      |MsUpL | MsUp |MnUpR |      |        |
 * |--------+------+------+------+------+------|      |           |Up    |------+------+------+------+------+--------|
 * |        |      | Left | Down | Right|      |------|           |------|      |MsLeft| MsDn |MsRght|      |        |
 * |--------+------+------+------+------+------|      |           |Scroll|------+------+------+------+------+--------|
 * |        |      | End  | Down | PgDn |      |      |           |Down  |      |MsDnL | MsDn |MsDnR |      |        |
 * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
 *   |      |      |      |      |      |                                       |      |      |      |      |      |
 *   `----------------------------------'                                       `----------------------------------'
 *                                        ,-------------.       ,-------------.
 *                                        |      | Mute |       | Prev | Next |
 *                                 ,------|------|------|       |------+------+------.
 *                                 |Play/ |Stop  |VolUp |       |      |Mouse |Mouse |
 *                                 |Pause |      |------|       |------|Left  |Right |
 *                                 |      |      |VolDn |       |      |Click |Click |
 *                                 `--------------------'       `--------------------'
 */
// MEDIA AND MOUSE
[MEDIA] = KEYMAP(
       KC_TRNS, KC_F1,      KC_F2,      KC_F3,      KC_F4,      KC_F5,      KC_TRNS,
       KC_TRNS, KC_TRNS,    KC_HOME,    KC_UP,      KC_PGUP,    KC_TRNS,    KC_TRNS,
       KC_TRNS, KC_TRNS,    KC_LEFT,    KC_DOWN,    KC_RGHT,    KC_TRNS,
       KC_TRNS, KC_TRNS,    KC_END,     KC_DOWN,    KC_PGDN,    KC_TRNS,    KC_TRNS,
       KC_TRNS, KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                                KC_TRNS,    KC_MUTE,
                                                                KC_VOLU,
                                                    KC_MPLY,    KC_MSTP,    KC_VOLD,
       // right hand
       KC_TRNS, KC_F6,      KC_F7,      KC_F8,      KC_F9,      KC_F10,     KC_F11,
       KC_WH_U, KC_TRNS,    M(A_MUL),   KC_MS_U,    M(A_MUR),   KC_TRNS,    KC_TRNS,
                KC_TRNS,    KC_MS_L,    KC_MS_D,    KC_MS_R,    KC_TRNS,    KC_TRNS,
       KC_WH_D, KC_TRNS,    M(A_MDL),   KC_MS_D,    M(A_MDR),   KC_TRNS,    KC_TRNS,
                            KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
       KC_MPRV, KC_MNXT,
       KC_TRNS,
       KC_TRNS, KC_BTN1,    KC_BTN2
),

/* Keymap 2: MAC OSX layer
 *
 * ,---------------------------------------------------.           ,---------------------------------------------------.
 * |   =    |   1  |   2   |   3  |   4  |   5  |      |           |      |   6  |   7   |   8  |   9  |   0  |   `    |
 * |--------+------+-------+------+------+-------------|           |------+------+-------+------+------+------+--------|
 * | Tab    |   "  |   ,   |   .  |   P  |   Y  |  [   |           |  ]   |   Y  |   G   |   C  |   R  |   L  |   \    |
 * |--------+------+-------+------+------+------|      |           |      |------+-------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   O   |   E  |   U  |   I  |------|           |------|   D  |   H   |   T  |   N  |   S  |-/MEDIA |
 * |--------+------+-------+------+------+------|  (   |           |  )   |------+-------+------+------+------+--------|
 * | LShift |   ;  |   Q   |   J  |   K  |   X  |      |           |      |   B  |   M   |   W  |   V  |   Z  |   /    |
 * `--------+------+-------+------+------+-------------'           `-------------+-------+------+------+------+--------'
 *   | Cut  | Copy | Paste | Alt  | LGui |                                       |LGuiSPC|      |      |      | ~PC  |
 *   `-----------------------------------'                                       `-----------------------------------'
 *                                        ,-------------.       ,---------------.
 *                                        |      |      |       |      |        |
 *                                 ,------|------|------|       |------+--------+------.
 *                                 |      |      |      |       |      |        |      |
 *                                 | Back | Del  |------|       |------|  Enter |Space |
 *                                 | Space|      |      |       |      |        |      |
 *                                 `--------------------'       `----------------------'
 */

[OSX] = KEYMAP(  // layer 0 : default
        // left hand
        KC_EQL,         KC_1,       KC_2,           KC_3,       KC_4,       KC_5,       KC_NO,
        KC_TAB,         KC_QUOT,    KC_COMM,        KC_DOT,     KC_P,       KC_Y,       KC_LBRC,
        CTL_T(KC_ESC),  KC_A,       KC_O,           KC_E,       KC_U,       KC_I,
        KC_LSFT,        KC_SCLN,    KC_Q,           KC_J,       KC_K,       KC_X,       KC_LPRN,
        LGUI(KC_X),     LGUI(KC_C), LGUI(KC_V),     KC_LALT,    KC_LGUI,
                                                                            KC_NO,      KC_NO,
                                                                                        KC_NO,
                                                                KC_BSPC,    KC_DELT,    KC_NO,
        // right hand
        KC_NO,      KC_6,       KC_7,           KC_8,       KC_9,       KC_10,      KC_GRV,
        KC_RBRC,    KC_F,       KC_G,           KC_C,       KC_R,       KC_L,       KC_BSLS,
                    KC_D,       KC_H,           KC_T,       KC_N,       KC_S,       LT(OSXMEDIA, KC_MINS),
        KC_RPRN,    KC_B,       KC_M,           KC_W,       KC_V,       KC_Z,       KC_SLSH,
                                LGUI(KC_SPC),   KC_NO,      KC_NO,      KC_NO,      TO(BASE, 1),
        KC_NO,      KC_NO,
        KC_NO,
        KC_NO,      KC_ENT,     KC_SPC
    ),

/* Keymap 3: Media and mouse keys (MAC OSX)
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * |        |  F1  |  F2  |  F3  |  F4  |  F5  |      |           |      |  F6  |  F7  |  F8  |  F9  |  F10 |  F11   |
 * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
 * |        |      | Home |  Up  | PgUp |      |      |           |Scroll|      |MsUpL | MsUp |MnUpR |      |        |
 * |--------+------+------+------+------+------|      |           |Up    |------+------+------+------+------+--------|
 * |        |      | Left | Down | Right|      |------|           |------|      |MsLeft| MsDn |MsRght|      |        |
 * |--------+------+------+------+------+------|      |           |Scroll|------+------+------+------+------+--------|
 * |        |      | End  | Down | PgDn |      |      |           |Down  |      |MsDnL | MsDn |MsDnR |      |        |
 * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
 *   |      |      |      |      |      |                                       |      |      |      |      |      |
 *   `----------------------------------'                                       `----------------------------------'
 *                                        ,-------------.       ,-------------.
 *                                        |      | Mute |       | Prev | Next |
 *                                 ,------|------|------|       |------+------+------.
 *                                 |Play/ |Stop  |VolUp |       |      |Mouse |Mouse |
 *                                 |Pause |      |------|       |------|Left  |Right |
 *                                 |      |      |VolDn |       |      |Click |Click |
 *                                 `--------------------'       `--------------------'
 */
// MEDIA AND MOUSE for OSX
[OSXMEDIA] = KEYMAP(
       KC_TRNS, KC_F1,      KC_F2,      KC_F3,      KC_F4,      KC_F5,      KC_TRNS,
       KC_TRNS, KC_TRNS,    KC_HOME,    KC_UP,      KC_PGUP,    KC_TRNS,    KC_TRNS,
       KC_TRNS, KC_TRNS,    KC_LEFT,    KC_DOWN,    KC_RGHT,    KC_TRNS,
       KC_TRNS, KC_TRNS,    KC_END,     KC_DOWN,    KC_PGDN,    KC_TRNS,    KC_TRNS,
       KC_TRNS, KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
                                                                KC_TRNS,    KC_MUTE,
                                                                            KC_VOLU,
                                                    KC_MPLY,    KC_MSTP,    KC_VOLD,
       // right hand
       KC_TRNS, KC_F6,      KC_F7,      KC_F8,      KC_F9,      KC_F10,     KC_F11,
       KC_WH_U, KC_TRNS,    M(A_MUL),   KC_MS_U,    M(A_MUR),   KC_TRNS,    KC_TRNS,
                KC_TRNS,    KC_MS_L,    KC_MS_D,    KC_MS_R,    KC_TRNS,    KC_TRNS,
       KC_WH_D, KC_TRNS,    M(A_MDL),   KC_MS_D,    M(A_MDR),   KC_TRNS,    KC_TRNS,
                            KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,
       KC_MPRV, KC_MNXT,
       KC_TRNS,
       KC_TRNS, KC_BTN1,    KC_BTN2
),

};

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
        /* Mouse movement */
        case A_MUL:
            if (record->event.pressed) {
                mousekey_on(KC_MS_UP);
                mousekey_on(KC_MS_LEFT);
            } else {
                mousekey_off(KC_MS_UP);
                mousekey_off(KC_MS_LEFT);
            }
            mousekey_send();
            break;

        case A_MUR:
            if (record->event.pressed) {
                mousekey_on(KC_MS_UP);
                mousekey_on(KC_MS_RIGHT);
            } else {
                mousekey_off(KC_MS_UP);
                mousekey_off(KC_MS_RIGHT);
            }
            mousekey_send();
            break;

        case A_MDL:
            if (record->event.pressed) {
                mousekey_on(KC_MS_DOWN);
                mousekey_on(KC_MS_LEFT);
            } else {
                mousekey_off(KC_MS_DOWN);
                mousekey_off(KC_MS_LEFT);
            }
            mousekey_send();
            break;

        case A_MDR:
            if (record->event.pressed) {
                mousekey_on(KC_MS_DOWN);
                mousekey_on(KC_MS_RIGHT);
            } else {
                mousekey_off(KC_MS_DOWN);
                mousekey_off(KC_MS_RIGHT);
            }
            mousekey_send();
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

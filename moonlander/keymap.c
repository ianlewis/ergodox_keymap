/*
 * Copyright 2025 Ian Lewis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include QMK_KEYBOARD_H
#include "version.h"

enum layers {
  BASE,  // default layer
  MEDIA, // media keys
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [BASE] = LAYOUT(
    KC_EQUAL,                   KC_1,           KC_2,           KC_3,           KC_4,                   KC_5,               _______,                                        _______,                KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           KC_GRAVE,       
    KC_TAB,                     KC_QUOTE,       KC_COMMA,       KC_DOT,         KC_P,                   KC_Y,               KC_LBRC,                                        KC_RBRC,                KC_F,           KC_G,           KC_C,           KC_R,           KC_L,           KC_BSLS,      
    MT(MOD_LCTL, KC_ESCAPE),    KC_A,           KC_O,           KC_E,           KC_U,                   KC_I,               KC_LPRN,                                        KC_RPRN,                KC_D,           KC_H,           KC_T,           KC_N,           KC_S,           LT(MEDIA, KC_MINUS), 
    KC_LSFT,                    KC_SCLN,        KC_Q,           KC_J,           KC_K,                   KC_X,                                                                                       KC_B,           KC_M,           KC_W,           KC_V,           KC_Z,           KC_SLASH,       
    _______,                    _______,        _______,        KC_LALT,        KC_LGUI,                                    _______,                                        _______,                                LCTL(KC_SPACE), _______,        _______,        _______,        _______, 
                                                                                KC_BSPC,                KC_DELETE,          _______,                                        _______,                KC_ENTER,       KC_SPACE
  ),
  [MEDIA] = LAYOUT(
    _______,                    KC_F1,          KC_F2,          KC_F3,          KC_F4,                  KC_F5,              _______,                                        _______,                KC_F6,          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_F11,         
    _______,                    _______,        KC_HOME,        KC_UP,          KC_PGUP,                _______,            KC_WWW_BACK,                                    KC_WWW_FORWARD,         KC_MS_WH_UP,    _______,        KC_MS_UP,       _______,        _______,        _______, 
    _______,                    _______,        KC_LEFT,        KC_DOWN,        KC_RIGHT,               _______,            KC_WWW_SEARCH,                                  KC_WWW_HOME,            KC_MS_WH_DOWN,  KC_MS_LEFT,     KC_MS_DOWN,     KC_MS_RIGHT,    _______,        _______, 
    _______,                    _______,        KC_END,         KC_DOWN,        KC_PGDN,                _______,                                                                                    _______,        _______,        _______,        _______,        _______,        _______, 
    _______,                    _______,        _______,        _______,        _______,                                    KC_AUDIO_VOL_UP,                                KC_MEDIA_NEXT_TRACK,                    _______,        _______,        _______,        _______,        _______, 
                                                                                KC_MEDIA_PLAY_PAUSE,    KC_MEDIA_STOP,      KC_AUDIO_VOL_DOWN,                              KC_MEDIA_PREV_TRACK,    KC_MS_BTN1,     KC_MS_BTN2
  ),
};

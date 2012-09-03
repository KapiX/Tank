/*
    Copyright 2011, 2012 Kacper Kasper <kacperkasper@gmail.com>

    This file is part of Tank.

    Tank is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tank is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Tank.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _DEFINES_H_
#define _DEFINES_H_

// Game objects' metrics
#define BOAT_WIDTH                  32
#define BOAT_HEIGHT                 32
#define BONUS_WIDTH                 32
#define BONUS_HEIGHT                32
#define BULLET_WIDTH                8
#define BULLET_HEIGHT               8
#define BULLET_WIDTH_DIV_2          4
#define BULLET_HEIGHT_DIV_2         4
#define ENEMY_WIDTH                 32
#define ENEMY_HEIGHT                32
#define ENEMY_WIDTH_DIV_2           16
#define ENEMY_HEIGHT_DIV_2          16
#define EXPLOSION_ANIM_WIDTH        32
#define EXPLOSION_ANIM_HEIGHT       32
#define EXPLOSION_ANIM_WIDTH_DIV_2  16
#define EXPLOSION_ANIM_HEIGHT_DIV_2 16
#define EXPLOSION_ANIM_WIDTH_2      64
#define EXPLOSION_ANIM_HEIGHT_2     64
#define SPAWN_ANIM_WIDTH            32
#define SPAWN_ANIM_HEIGHT           32
#define PLAYER_WIDTH                32
#define PLAYER_HEIGHT               32
#define PLAYER_WIDTH_DIV_2          16
#define PLAYER_HEIGHT_DIV_2         16
#define PLAYER_WIDTH_DIV_4          8
#define PLAYER_HEIGHT_DIV_4         8
#define SHIELD_WIDTH                32
#define SHIELD_HEIGHT               32

// Graphics' offsets in atlas
#define BOAT_OFFSET_X               484
#define BOAT_OFFSET_Y               288
#define BONUS_OFFSET_X              960
#define BONUS_OFFSET_Y              0
#define BULLET_OFFSET_X             292
#define BULLET_OFFSET_Y             288
#define EXPLOSION_ANIM_OFFSET_X     228
#define EXPLOSION_ANIM_OFFSET_Y     256
#define SPAWN_ANIM_OFFSET_X         228
#define SPAWN_ANIM_OFFSET_Y         320
#define LOGO_OFFSET_X               0
#define LOGO_OFFSET_Y               256
#define PLAYER_OFFSET_X             640
#define PLAYER_OFFSET_Y             0
#define SHIELD_OFFSET_X             228
#define SHIELD_OFFSET_Y             288

#define TANK_TYPE_WIDTH             128
#define TANK_LEVEL_HEIGHT           64

// HUD
#define HUD_BACKGROUND_LAYER        1
#define HUD_LAYER                   2
#define HUD_TEXT_SCALE              2
#define HUD_TEXT_COLOR              0,0,0 // RGB
#define HUD_SCORE_DIGITS_COUNT      8
#define HUD_LIVES_DIGITS_COUNT      2
#define HUD_ENEMIES_DIGITS_COUNT    2
#define HUD_ICON_WIDTH              32
#define HUD_ICON_HEIGHT             32
#define HUD_SPACE                   4 // between elements
#define HUD_X                       0
#define HUD_Y                       568 // RES_HEIGHT - HUD_HEIGHT
#define HUD_WIDTH                   800
#define HUD_HEIGHT                  32
#define HUD_TOP_HEIGHT              24
#define HUD_P1_SCORE_X              6
#define HUD_P1_SCORE_Y              571
    // HUD_Y + (HUD_HEIGHT - (FONT_CHAR_HEIGHT * HUD_TEXT_SCALE)) / 2
#define HUD_P1_ICON_X               136
    // HUD_P1_SCORE_X + ((FONT_CHAR_WIDTH + 1) * HUD_TEXT_SCALE) * HUD_SCORE_DIGITS_COUNT + HUD_SPACE - 2 (extra space after last char)
#define HUD_P1_ICON_Y               568 // HUD_Y
#define HUD_P1_LIVES_X              172
    // HUD_P1_ICON_X + HUD_SPACE + HUD_ICON_WIDTH
#define HUD_P1_LIVES_Y              571 // HUD_P1_SCORE_Y
#define HUD_P2_SCORE_X              668
    // HUD_WIDTH - 6 - ((FONT_CHAR_WIDTH + 1) * HUD_TEXT_SCALE) * HUD_SCORE_DIGITS_COUNT + 2
#define HUD_P2_SCORE_Y              571
#define HUD_P2_ICON_X               632
    // HUD_P2_SCORE_X - HUD_SPACE - HUD_ICON_WIDTH
#define HUD_P2_ICON_Y               568
#define HUD_P2_LIVES_X              598
    // HUD_P2_ICON_X - HUD_SPACE - ((FONT_CHAR_WIDTH + 1) * HUD_TEXT_SCALE) * HUD_LIVES_DIGITS_COUNT + 2
#define HUD_P2_LIVES_Y              571
#define HUD_ENEMIES_COUNT_X         384
    // HUD_WIDTH / 2 - (HUD_ENEMIES_DIGITS_COUNT / 2 * (FONT_CHAR_WIDTH + 1) * HUD_TEXT_SCALE)
#define HUD_ENEMIES_COUNT_Y         571
#define HUD_ENEMIES_ICON_X          352
    // HUD_ENEMIES_COUNT_X - HUD_ICON_WIDTH
#define HUD_ENEMIES_ICON_Y          568

// Layers
#define SPAWN_ANIM_LAYER            2
#define TANK_LAYER                  3
#define BOAT_LAYER                  4
#define SHIELD_LAYER                5
#define BULLET_LAYER                6
#define EXPLOSION_ANIM_LAYER        44
#define BONUS_LAYER                 58
#define LOGO_LAYER                  61
#define TEXT_LAYER                  64

// Font metrics
#define FONT_CHAR_WIDTH             7
#define FONT_CHAR_HEIGHT            13
#define FONT_CHAR_WIDTH_1           8
#define FONT_CHAR_HEIGHT_1          14

// Time
#define BONUS_STOP_TIME             10 // how long enemies are stopped (in secs)
#define SPAWN_SHIELD_TIME           5
#define SHIELD_TIME                 10

// Miscellanous
#define BULLET_SPEED_MULTIPLIER     3
#define ENEMY_SPEED                 50
#define ENEMY_BULLET_SPEED          150 // speed * 3
#define ENEMY_LVL2_SPEED            100 // speed * 2
#define ENEMY_LVL3_BULLET_SPEED     300 // bulletspeed * 2
#define RES_WIDTH                   800
#define RES_HEIGHT                  600
#define MENU_POINTER_SPACE          3 // space between pointer and menu options
#define MENU_TITLE_SPACE            3 // space between menu title and options
#define PLAYER_SPEED                85
#define PLAYER_BULLET_SPEED         255 // speed * 3

#endif // _DEFINES_H_
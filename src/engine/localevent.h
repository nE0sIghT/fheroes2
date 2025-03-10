/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *   Copyright (C) 2008 by Josh Matthews <josh@joshmatthews.net>           *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef H2LOCALEVENT_H
#define H2LOCALEVENT_H

#include <string>

#include "math_base.h"
#include "timing.h"
#include "types.h"

#include <SDL.h>

enum KeySym
{
    KEY_NONE = -1,

    KEY_UNKNOWN = SDLK_UNKNOWN,

    KEY_BACKSPACE = SDLK_BACKSPACE,
    KEY_RETURN = SDLK_RETURN,
    KEY_ESCAPE = SDLK_ESCAPE,
    KEY_SPACE = SDLK_SPACE,
    KEY_EXCLAIM = SDLK_EXCLAIM,
    KEY_QUOTEDBL = SDLK_QUOTEDBL,
    KEY_HASH = SDLK_HASH,
    KEY_DOLLAR = SDLK_DOLLAR,
    KEY_AMPERSAND = SDLK_AMPERSAND,
    KEY_QUOTE = SDLK_QUOTE,
    KEY_LEFTPAREN = SDLK_LEFTPAREN,
    KEY_RIGHTPAREN = SDLK_RIGHTPAREN,
    KEY_ASTERISK = SDLK_ASTERISK,
    KEY_PLUS = SDLK_PLUS,
    KEY_COMMA = SDLK_COMMA,
    KEY_MINUS = SDLK_MINUS,
    KEY_PERIOD = SDLK_PERIOD,
    KEY_SLASH = SDLK_SLASH,
    KEY_COLON = SDLK_COLON,
    KEY_SEMICOLON = SDLK_SEMICOLON,
    KEY_LESS = SDLK_LESS,
    KEY_EQUALS = SDLK_EQUALS,
    KEY_GREATER = SDLK_GREATER,
    KEY_QUESTION = SDLK_QUESTION,
    KEY_AT = SDLK_AT,
    KEY_LEFTBRACKET = SDLK_LEFTBRACKET,
    KEY_BACKSLASH = SDLK_BACKSLASH,
    KEY_RIGHTBRACKET = SDLK_RIGHTBRACKET,
    KEY_CARET = SDLK_CARET,
    KEY_UNDERSCORE = SDLK_UNDERSCORE,
    KEY_ALT = SDLK_LALT,
    KEY_CONTROL = SDLK_LCTRL,
    KEY_SHIFT = SDLK_LSHIFT,
    KEY_TAB = SDLK_TAB,
    KEY_DELETE = SDLK_DELETE,
    KEY_PAGEUP = SDLK_PAGEUP,
    KEY_PAGEDOWN = SDLK_PAGEDOWN,
    KEY_F1 = SDLK_F1,
    KEY_F2 = SDLK_F2,
    KEY_F3 = SDLK_F3,
    KEY_F4 = SDLK_F4,
    KEY_F5 = SDLK_F5,
    KEY_F6 = SDLK_F6,
    KEY_F7 = SDLK_F7,
    KEY_F8 = SDLK_F8,
    KEY_F9 = SDLK_F9,
    KEY_F10 = SDLK_F10,
    KEY_F11 = SDLK_F11,
    KEY_F12 = SDLK_F12,
    KEY_LEFT = SDLK_LEFT,
    KEY_RIGHT = SDLK_RIGHT,
    KEY_UP = SDLK_UP,
    KEY_DOWN = SDLK_DOWN,
    KEY_0 = SDLK_0,
    KEY_1 = SDLK_1,
    KEY_2 = SDLK_2,
    KEY_3 = SDLK_3,
    KEY_4 = SDLK_4,
    KEY_5 = SDLK_5,
    KEY_6 = SDLK_6,
    KEY_7 = SDLK_7,
    KEY_8 = SDLK_8,
    KEY_9 = SDLK_9,
    KEY_a = SDLK_a,
    KEY_b = SDLK_b,
    KEY_c = SDLK_c,
    KEY_d = SDLK_d,
    KEY_e = SDLK_e,
    KEY_f = SDLK_f,
    KEY_g = SDLK_g,
    KEY_h = SDLK_h,
    KEY_i = SDLK_i,
    KEY_j = SDLK_j,
    KEY_k = SDLK_k,
    KEY_l = SDLK_l,
    KEY_m = SDLK_m,
    KEY_n = SDLK_n,
    KEY_o = SDLK_o,
    KEY_p = SDLK_p,
    KEY_q = SDLK_q,
    KEY_r = SDLK_r,
    KEY_s = SDLK_s,
    KEY_t = SDLK_t,
    KEY_u = SDLK_u,
    KEY_v = SDLK_v,
    KEY_w = SDLK_w,
    KEY_x = SDLK_x,
    KEY_y = SDLK_y,
    KEY_z = SDLK_z,

#if SDL_VERSION_ATLEAST( 2, 0, 0 )
    KEY_PRINT = SDLK_PRINTSCREEN,
    KEY_KP0 = SDLK_KP_0,
    KEY_KP1 = SDLK_KP_1,
    KEY_KP2 = SDLK_KP_2,
    KEY_KP3 = SDLK_KP_3,
    KEY_KP4 = SDLK_KP_4,
    KEY_KP5 = SDLK_KP_5,
    KEY_KP6 = SDLK_KP_6,
    KEY_KP7 = SDLK_KP_7,
    KEY_KP8 = SDLK_KP_8,
    KEY_KP9 = SDLK_KP_9,
#else
    KEY_PRINT = SDLK_PRINT,
    KEY_KP0 = SDLK_KP0,
    KEY_KP1 = SDLK_KP1,
    KEY_KP2 = SDLK_KP2,
    KEY_KP3 = SDLK_KP3,
    KEY_KP4 = SDLK_KP4,
    KEY_KP5 = SDLK_KP5,
    KEY_KP6 = SDLK_KP6,
    KEY_KP7 = SDLK_KP7,
    KEY_KP8 = SDLK_KP8,
    KEY_KP9 = SDLK_KP9,
#endif

    KEY_KP_PERIOD = SDLK_KP_PERIOD,
    KEY_KP_DIVIDE = SDLK_KP_DIVIDE,
    KEY_KP_MULTIPLY = SDLK_KP_MULTIPLY,
    KEY_KP_MINUS = SDLK_KP_MINUS,
    KEY_KP_PLUS = SDLK_KP_PLUS,
    KEY_KP_ENTER = SDLK_KP_ENTER,
    KEY_KP_EQUALS = SDLK_KP_EQUALS,

    KEY_HOME = SDLK_HOME,
    KEY_END = SDLK_END,

    KEY_LAST
};

const char * KeySymGetName( KeySym );

KeySym GetKeySym( int );

bool PressIntKey( u32 max, u32 & result );

size_t InsertKeySym( std::string & res, size_t pos, const KeySym sym, const int32_t mod );

class LocalEvent
{
public:
    static LocalEvent & Get( void );
    static LocalEvent & GetClean(); // reset all previous event statuses and return a reference for events

    void SetGlobalFilterMouseEvents( void ( *pf )( s32, s32 ) );
    void SetGlobalFilterKeysEvents( void ( *pf )( int, int ) );

    static void SetStateDefaults( void );
    static void SetState( u32 type, bool enable );

    bool HandleEvents( bool delay = true, bool allowExit = false );

    bool MouseMotion( void ) const;

    const fheroes2::Point & GetMouseCursor( void ) const
    {
        return mouse_cu;
    }

    const fheroes2::Point & GetMousePressLeft( void ) const;

    void ResetPressLeft();

    bool MouseClickLeft();
    bool MouseClickMiddle();
    bool MouseClickRight();

    bool MouseClickLeft( const fheroes2::Rect & rt );
    bool MouseClickRight( const fheroes2::Rect & rt );

    bool MouseWheelUp() const;
    bool MouseWheelDn() const;

    bool MousePressLeft() const;
    bool MousePressLeft( const fheroes2::Rect & rt ) const;
    bool MousePressRight() const;
    bool MousePressRight( const fheroes2::Rect & rt ) const;

    bool MouseReleaseLeft() const;
    bool MouseReleaseLeft( const fheroes2::Rect & rt ) const;
    bool MouseReleaseRight() const;
    bool MouseReleaseRight( const fheroes2::Rect & rt ) const;

    bool MouseWheelUp( const fheroes2::Rect & rt ) const;
    bool MouseWheelDn( const fheroes2::Rect & rt ) const;

    bool MouseCursor( const fheroes2::Rect & rt ) const;

    bool KeyPress() const;
    bool KeyPress( KeySym key ) const;

    bool KeyHold() const
    {
        return ( modes & KEY_HOLD ) != 0;
    }

    KeySym KeyValue() const;
    int KeyMod() const;

    void RegisterCycling( void ( *preRenderDrawing )() = nullptr, void ( *postRenderDrawing )() = nullptr ) const;

    // These two methods are useful for video playback
    void PauseCycling() const;
    void ResumeCycling() const;

    void OpenVirtualKeyboard();
    void CloseVirtualKeyboard();

#if SDL_VERSION_ATLEAST( 2, 0, 0 )
    void OpenController();
    void CloseController();
    void OpenTouchpad();
#endif

    void SetControllerPointerSpeed( const int newSpeed )
    {
        if ( newSpeed > 0 ) {
            _controllerPointerSpeed = newSpeed / CONTROLLER_SPEED_MOD;
        }
    }

private:
    LocalEvent();

    void HandleMouseMotionEvent( const SDL_MouseMotionEvent & );
    void HandleMouseButtonEvent( const SDL_MouseButtonEvent & );
    void HandleKeyboardEvent( const SDL_KeyboardEvent & );

    static void StopSounds();
    static void ResumeSounds();

#if SDL_VERSION_ATLEAST( 2, 0, 0 )
    static int GlobalFilterEvents( void *, SDL_Event * );

    void HandleMouseWheelEvent( const SDL_MouseWheelEvent & );
    void HandleControllerAxisEvent( const SDL_ControllerAxisEvent & motion );
    void HandleControllerButtonEvent( const SDL_ControllerButtonEvent & button );
    void ProcessControllerAxisMotion();
    void HandleTouchEvent( const SDL_TouchFingerEvent & event );

    void OnSdl2WindowEvent( const SDL_Event & event );
#else
    static int GlobalFilterEvents( const SDL_Event * );

    void OnActiveEvent( const SDL_Event & event );
#endif

    enum flag_t
    {
        KEY_PRESSED = 0x0001,
        MOUSE_MOTION = 0x0002,
        MOUSE_PRESSED = 0x0004, // mouse button is currently pressed
        MOUSE_RELEASED = 0x0008, // mouse button has just been released
        MOUSE_CLICKED = 0x0010, // mouse button has been clicked
        MOUSE_WHEEL = 0x0020,
        KEY_HOLD = 0x0040
    };

    void SetModes( flag_t );
    void ResetModes( flag_t );

    int modes;
    KeySym key_value;
    int mouse_button;

    fheroes2::Point mouse_pl; // press left
    fheroes2::Point mouse_pm; // press middle
    fheroes2::Point mouse_pr; // press right

    fheroes2::Point mouse_rl; // release left
    fheroes2::Point mouse_rm; // release middle
    fheroes2::Point mouse_rr; // release right

    fheroes2::Point mouse_cu; // point cursor

    fheroes2::Point mouse_wm; // wheel movement

    void ( *redraw_cursor_func )( s32, s32 );
    void ( *keyboard_filter_func )( int, int );

    uint32_t loop_delay;

    enum
    {
        CONTROLLER_L_DEADZONE = 4000,
        CONTROLLER_R_DEADZONE = 25000
    };

    // used to convert user-friendly pointer speed values into more useable ones
    const double CONTROLLER_SPEED_MOD = 2000000.0;
    double _controllerPointerSpeed = 10.0 / CONTROLLER_SPEED_MOD;
    double _emulatedPointerPosX = 0;
    double _emulatedPointerPosY = 0;

#if SDL_VERSION_ATLEAST( 2, 0, 0 )
    // bigger value correndsponds to faster pointer movement speed with bigger stick axis values
    const double CONTROLLER_AXIS_SPEEDUP = 1.03;

    SDL_GameController * _gameController = nullptr;
    SDL_FingerID _firstFingerId = 0;
    fheroes2::Time _controllerTimer;
    int16_t _controllerLeftXAxis = 0;
    int16_t _controllerLeftYAxis = 0;
    int16_t _controllerRightXAxis = 0;
    int16_t _controllerRightYAxis = 0;
    bool _controllerScrollActive = false;
    bool _touchpadAvailable = false;
    int16_t _numTouches = 0;
#endif
};

#endif

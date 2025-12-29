//
// Created by james on 29/12/25.
//

#include "typer.hpp"
using namespace jot;

//
// ut
//
#include <ut/log.hpp>

//
// std
//

#include <exception>
using namespace std;


//
// *nix
//

#include <fcntl.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <cstring>
#include <iostream>
#include <cctype>

//
// Utilities
//

static void emit(int fd, int type, int code, int val)
{
    input_event ie;
    ie.type = type;
    ie.code = code;
    ie.value = val;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;
    write(fd, &ie, sizeof(ie));
}

static int getKeycode(char c)
{

    switch(c)
    {
        // letters
        case 'a': case 'A': return KEY_A;
        case 'b': case 'B': return KEY_B;
        case 'c': case 'C': return KEY_C;
        case 'd': case 'D': return KEY_D;
        case 'e': case 'E': return KEY_E;
        case 'f': case 'F': return KEY_F;
        case 'g': case 'G': return KEY_G;
        case 'h': case 'H': return KEY_H;
        case 'i': case 'I': return KEY_I;
        case 'j': case 'J': return KEY_J;
        case 'k': case 'K': return KEY_K;
        case 'l': case 'L': return KEY_L;
        case 'm': case 'M': return KEY_M;
        case 'n': case 'N': return KEY_N;
        case 'o': case 'O': return KEY_O;
        case 'p': case 'P': return KEY_P;
        case 'q': case 'Q': return KEY_Q;
        case 'r': case 'R': return KEY_R;
        case 's': case 'S': return KEY_S;
        case 't': case 'T': return KEY_T;
        case 'u': case 'U': return KEY_U;
        case 'v': case 'V': return KEY_V;
        case 'w': case 'W': return KEY_W;
        case 'x': case 'X': return KEY_X;
        case 'y': case 'Y': return KEY_Y;
        case 'z': case 'Z': return KEY_Z;

        // numbers
        case '0': case ')': return KEY_0;
        case '1': case '!': return KEY_1;
        case '2': case '@': return KEY_2;
        case '3': case '#': return KEY_3;
        case '4': case '$': return KEY_4;
        case '5': case '%': return KEY_5;
        case '6': case '^': return KEY_6;
        case '7': case '&': return KEY_7;
        case '8': case '*': return KEY_8;
        case '9': case '(': return KEY_9;


        // special chars
        case ' ' :    return KEY_SPACE;
        case '\n':    return KEY_ENTER;
        case '.' :    return KEY_DOT;
        case ',' :    return KEY_COMMA;
        case '-' :    return KEY_MINUS;
        case '=' :    return KEY_EQUAL;
        case '[' :    return KEY_LEFTBRACE;
        case ']' :    return KEY_RIGHTBRACE;
        case ';' :    return KEY_SEMICOLON;
        case '\'':    return KEY_APOSTROPHE;
        case '/' :    return KEY_SLASH;
        case '\\':    return KEY_BACKSLASH;
    }


    throw runtime_error(format("bad char: '{}'", c));
}

static bool needsShift(char c)
{
    return isupper(c) || strchr("!@#$%^&*()_+{}|:\"<>?", c) != nullptr;
}

void waitForDeviceReady(int fd)
{

}

//
// Typer -> Implementation
//

Typer::Typer()
{
    //
    // open uinput
    //

    m_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (m_fd < 0)
    {
        throw runtime_error{"error opening /dev/uinput. try: sudo chmod 666 /dev/uinput"};
    }

    //
    // configure uinput
    //

    ioctl(m_fd, UI_SET_EVBIT, EV_KEY); // will send key events
    ioctl(m_fd, UI_SET_EVBIT, EV_SYN); // will send sync events

    for (int i = KEY_ESC; i <= KEY_F24; ++i)
    {
        ioctl(m_fd, UI_SET_KEYBIT, i); // will send these specific keycode events
    }

    //
    // register virtual device
    //

    uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB; // pretend to be a usb device
    usetup.id.vendor = 0x1234; // arbitrary vendor id
    usetup.id.product = 0x5678; // arbitrary product id
    strcpy(usetup.name, "jot virtual keyboard"); // give it a name

    ioctl(m_fd, UI_DEV_SETUP, &usetup);
    ioctl(m_fd, UI_DEV_CREATE);

    //
    // wait for virtual device to become ready
    //

    // constexpr int max_attempts = 100;
    // constexpr duration poll_interval = 20_milliseconds;
    //
    // for (int i = 0; i < max_attempts; ++i)
    // {
    //     // Try a simple write to verify the device is ready
    //     input_event test_event;
    //     memset(&test_event, 0, sizeof(test_event));
    //     test_event.type = EV_SYN;
    //     test_event.code = SYN_REPORT;
    //     test_event.value = 0;
    //
    //     if (write(m_fd, &test_event, sizeof(test_event)) == sizeof(test_event))
    //     {
    //         timer::sleep(50_milliseconds); // Small grace period
    //         return;
    //     }
    //     timer::sleep(poll_interval);
    // }
    //
    // throw runtime_error{"timeout while waiting for device to become ready."};
}

Typer::~Typer()
{
    ioctl(m_fd, UI_DEV_DESTROY);
    close(m_fd);
}

void Typer::typeChar(char c)
{
    int key = getKeycode(c);
    bool shift = needsShift(c);
    typeKey(key, shift);
}

void Typer::typeKey(int key_code, bool shift)
{
    if (shift)
    {
        emit(m_fd, EV_KEY, KEY_LEFTSHIFT, 1);
        emit(m_fd, EV_SYN, SYN_REPORT, 0);
    }

    emit(m_fd, EV_KEY, key_code, 1);
    emit(m_fd, EV_SYN, SYN_REPORT, 0);

    timer::sleep(delay_key);

    emit(m_fd, EV_KEY, key_code, 0);
    emit(m_fd, EV_SYN, SYN_REPORT, 0);

    if (shift)
    {
        emit(m_fd, EV_KEY, KEY_LEFTSHIFT, 0);
        emit(m_fd, EV_SYN, SYN_REPORT, 0);
    }

    timer::sleep(delay_char);
}

void Typer::typeString(strparam s)
{
    vector<pair<int, bool>> keys;

    // transform all chars to keys before typing to catch errors
    for (auto&& it : s)
        keys.emplace_back(getKeycode(it), needsShift(it));

    timer::sleep(delay_string);  // Add this line
    for (auto&& it : keys)
        typeKey(it.first, it.second);
}

void Typer::typeEnter()
{
    typeKey(KEY_ENTER, false);
}

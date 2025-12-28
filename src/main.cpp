//
// CL11
//
#include "lib/CLI11.hpp"

//
// ut
//

#include <ut/string.hpp>
#include <ut/term.hpp>
#include <ut/time.hpp>
using namespace ut;

//
// std
//
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

void emit(int fd, int type, int code, int val)
{
    input_event ie;
    ie.type = type;
    ie.code = code;
    ie.value = val;
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;
    write(fd, &ie, sizeof(ie));
}

int getKeycode(char c)
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

bool needsShift(char c)
{
    return isupper(c) || strchr("!@#$%^&*()_+{}|:\"<>?", c) != nullptr;
}



struct Cmd
{
    string text;

    double delay_start  = 1000;
    double delay_key    = 10;
    double delay_char   = 20;

    bool press_enter = false;
    bool verbose = false;
} cmd;

struct Typer
{
    int m_fd;

    Typer()
    {
        m_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
        if (m_fd < 0)
        {
            throw runtime_error{"error opening /dev/uinput. try: sudo chmod 666 /dev/uinput"};
        }

        ioctl(m_fd, UI_SET_EVBIT, EV_KEY);
        ioctl(m_fd, UI_SET_EVBIT, EV_SYN);

        for (int i = KEY_ESC; i <= KEY_KPDOT; ++i)
        {
            ioctl(m_fd, UI_SET_KEYBIT, i);
        }

        uinput_setup usetup;
        memset(&usetup, 0, sizeof(usetup));
        usetup.id.bustype = BUS_USB;
        usetup.id.vendor = 0x1234;
        usetup.id.product = 0x5678;
        strcpy(usetup.name, "jot virtual keyboard");

        ioctl(m_fd, UI_DEV_SETUP, &usetup);
        ioctl(m_fd, UI_DEV_CREATE);
    }

    ~Typer()
    {
        ioctl(m_fd, UI_DEV_DESTROY);
        close(m_fd);
    }

    void typeChar(char c, int delay_key, int delay_char)
    {
        int key = getKeycode(c);

        if (needsShift(c))
        {
            emit(m_fd, EV_KEY, KEY_LEFTSHIFT, 1);
            emit(m_fd, EV_SYN, SYN_REPORT, 0);
        }

        emit(m_fd, EV_KEY, key, 1);
        emit(m_fd, EV_SYN, SYN_REPORT, 0);

        timer::sleep(duration::milliseconds(delay_key));

        emit(m_fd, EV_KEY, key, 0);
        emit(m_fd, EV_SYN, SYN_REPORT, 0);

        if (needsShift(c))
        {
            emit(m_fd, EV_KEY, KEY_LEFTSHIFT, 0);
            emit(m_fd, EV_SYN, SYN_REPORT, 0);
        }

        timer::sleep(duration::milliseconds(delay_char));
    }

    void type(strparam s, int delay_start, int delay_key, int delay_char)
    {
        timer::sleep(duration::milliseconds(delay_start));
        for (auto&& it : s)
        {
            typeChar(it, delay_key, delay_char);
        }

    }
};

int main(int argc, char** argv)
{
    CLI::App app{"jot - Type stuff automatically on linux!"};

    string text;

    int delay_start = 1000;
    int delay_key   = 10;
    int delay_char  = 20;

    bool press_enter    = false;
    bool verbose        = false;

    app.add_option("text", text, "Text to type (use quotes for spaces)");
    app.add_option("-d,--delay", delay_start, "Initial delay in milliseconds before typing")->default_val(1000);
    app.add_option("-k,--key-delay", delay_key, "Key press duration in milliseconds")->default_val(10);
    app.add_option("-c,--char-delay", delay_char, "Delay between characters in milliseconds")->default_val(20);

    app.add_flag("-e,--enter", press_enter, "Press 'enter' key after typing");
    app.add_flag("-v,--verbose", verbose, "Verbose output");

    CLI11_PARSE(app, argc, argv);

    delay_start = clamp(delay_start, 0, 10000);
    delay_key   = clamp(delay_key  , 0, 10000);
    delay_char  = clamp(delay_char , 0, 10000);

#define LOG(...) if (verbose) { cout << format(__VA_ARGS__); }

    if (text.empty())
    {
        cout << app.help() << endl;
        return EXIT_FAILURE;
    }

    Typer typer;
    typer.type(text, delay_start, delay_key, delay_char);

#undef LOG

    return EXIT_SUCCESS;
}
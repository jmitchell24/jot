//
// Created by james on 29/12/25.
//

#include <ut/string.hpp>
#include <ut/time.hpp>
#include "common.hpp"

namespace jot
{
    struct Typer
    {
        int m_fd;

        duration delay_char     = 20_milliseconds;      ///< delay after key press (milliseconds)
        duration delay_key      = 20_milliseconds;      ///< duration of key press (milliseconds)
        duration delay_string   = 1000_milliseconds;    ///< delay before first key press (milliseconds)

        Typer();
        ~Typer();

        void typeKey(int key_code, bool shift=false);
        void typeChar(char c);
        void typeString(strparam s);

        void typeEnter();
    };

}
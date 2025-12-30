//
// Created by james on 30/12/25.
//

#include "common.hpp"
using namespace jot;
//
// App -> Implementation
//

App::App()
{ }

App& App::instance()
{
    static App x;
    return x;
}
//
// Created by james on 29/12/25.
//

#pragma once

#include <ut/string.hpp>
#include <ut/log.hpp>


#define jot_app ( jot::App::instance() )

#define jot_info(...) if (jot_app.verbose) { ut_info(__VA_ARGS__); }
#define jot_warning(...) if (jot_app.verbose) { ut_warning(__VA_ARGS__); }
#define jot_error(...) if (jot_app.verbose) { ut_error(__VA_ARGS__); }
#define jot_fatal(...) { ut_fatal(__VA_ARGS__); }


namespace jot
{
    using namespace ut;

    class App
    {
    public:
        bool verbose=false;

        static App& instance();

    private:
        App();
    };

}
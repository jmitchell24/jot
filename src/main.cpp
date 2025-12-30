

//
// CL11
//
#include "lib/CLI11.hpp"

//
// jot
//
#include "jot.hpp"
#include "common.hpp"
#include "typer.hpp"
using namespace jot;

//
// ut
//
#include <ut/log.hpp>

/*
 It keeps happening that there is a naming conflict here with 'check'
 Its a very commonly used name, and being a preprocessor macro means it punches
 through any other namespacing

 Possible solution:
 Put this undef behind a flag in UT's check.hpp file,
 to be used if neccessary.

 Or just keep adding the undef as needed. Its a kludge but
 this is C++, so compromise is sometimes needed.
 */
#undef check


//
// std
//
using namespace std;



int main(int argc, char** argv)
{
    CLI::App app{JOT_NAME " - " JOT_DESCRIPTION};

    string command_param;

    int delay_start = 500;

    int delay_key   = 10;
    int delay_char  = 20;

    bool press_enter    = false;
    bool verbose        = false;

    auto text_cmd = app.add_subcommand("text", "Type a specific sequence of text");
    auto date_cmd = app.add_subcommand("date", "Type a datecode given a specified format, default is 'YYYY-MM-DD' ");

    // Add options to the text subcommand
    text_cmd->add_option("text", command_param, "Text to type")->required();

    // Add options to the date subcommand
    date_cmd->add_option("format", command_param, "Date format string");

    // Global options (apply to both subcommands)
    app.add_option("-d,--delay", delay_start, "Initial delay in milliseconds before typing.")
        ->check(CLI::Range(0, 10000));

    app.add_option("-k,--key-delay", delay_key, "Key press duration in milliseconds.")
        ->check(CLI::Range(0, 10000));

    app.add_option("-c,--char-delay", delay_char, "Delay between characters in milliseconds.")
        ->check(CLI::Range(0, 10000));

    app.add_flag("-e,--enter", press_enter, "Press 'enter' key after typing");
    app.add_flag("-v,--verbose", verbose, "Verbose output");

    app.set_version_flag("--version", JOT_VERSION " - " JOT_RELEASE);

    CLI11_PARSE(app, argc, argv);

    delay_start = clamp(delay_start, 0, 10000);
    delay_key   = clamp(delay_key  , 0, 10000);
    delay_char  = clamp(delay_char , 0, 10000);


    try
    {
        Typer typer;
        typer.delay_char    = duration::milliseconds(delay_char);
        typer.delay_key     = duration::milliseconds(delay_key);
        typer.delay_string  = duration::milliseconds(delay_start);

        if (text_cmd->parsed())
        {
            jot_info("typing specified text: '{}'", command_param);
            typer.typeString(command_param);
        }
        else if (date_cmd->parsed())
        {
            jot_info("typing date with format: '{}'", command_param);
            command_param = local_datetime::now().str(command_param.c_str());
            typer.typeString(command_param);
        }
        else
        {
            jot_info("typing default date format: 'YYYY-MM-DD'");
            command_param = local_datetime::now().str("%Y-%m-%d");
            typer.typeString(command_param);
        }

        if (press_enter)
        {
            jot_info("typing enter now");
            typer.typeEnter();
        }

        jot_info("finished typing, now exiting...");
    }
    catch (exception const& ex)
    {
        ut_error("error: {}", ex.what());
    }

    return EXIT_SUCCESS;
}
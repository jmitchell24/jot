

//
// CL11
//
#include "lib/CLI11.hpp"

//
// jot
//
#include "typer.hpp"
using namespace jot;






//
// ut
//

#include <ut/log.hpp>
#include <ut/string.hpp>
#include <ut/term.hpp>
#include <ut/time.hpp>
using namespace ut;

//
// std
//
using namespace std;







int main(int argc, char** argv)
{
    CLI::App app{"jot - Type stuff automatically on linux!"};

    string text;

    int delay_start = 50;
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


    if (text.empty())
    {
        text = local_datetime::now().str("%Y-%m-%d");
    }

    try
    {
        Typer typer;
        typer.delay_char    = duration::milliseconds(delay_char);
        typer.delay_key     = duration::milliseconds(delay_key);
        typer.delay_string  = duration::milliseconds(delay_start);

        ut_trace("beginning to type: {}", text);
        typer.typeString(text);
        ut_trace("finished typing");

        if (press_enter) typer.typeEnter();
    }
    catch (exception const& ex)
    {
        ut_error("error: {}", ex.what());
    }




    return EXIT_SUCCESS;
}
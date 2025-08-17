// parsley experimental.development
//

#include <iostream>
#include <iomanip>
#include <parsley.h>

#define nl                '\n'

static const std::string shellDescription =
      "!ace used as shell interpretor, i.e. used in script file like this:\n"
      "\n"
      "    #!/usr/local/bin/ace -s\n"
      "    #\n"
      "    <ace commands>\n"
      "    %c\n"
      "    # end\n"
      "\n"
      "The source file is set to standard input, the target file is\n"
      "set to standard output. Commands are read from the script file,\n"
      "and all reports are sent to /dev/null.";


//-----------------------------------------------------------------------------
//
static const Parsley::OptionSpecifications optionsSpec = {
   Parsley::strSpec("command",    'c', "defines command input file, uses standard in if not specified."),
   Parsley::strSpec("report",     'r', "defines report output file, uses standard error if not specified."),
   Parsley::strSpec("option",     'o', "initial command string, e.g. '%Q'.")->
                                        defStr ("")->envVar ("ACE_OPTION"),
   Parsley::strSpec("backup",     'b', "defines command backup file. No default, "
                                       "there is no command backup if not specified."),
   Parsley::flagSpec("shell",     's', shellDescription),
   Parsley::flagSpec("quiet",     'q', "quiet, i.e. suppress output of copyright info on program start.")->
                                        envVar ("ACE_QUIET"),
   Parsley::flagSpec("license",   'l', "display licence information and exit.", true),
   Parsley::flagSpec("warranty ", 'w', "show warranty info and exit.", true),
   Parsley::version(),          // pre-defined singleton
   Parsley::help ()             // pre-defined singleton
};


//------------------------------------------------------------------------------
//
int main (int argc, char** argv)
{
   Parsley::Arguments args = Parsley::formArguments (argc, argv);

   Parsley parser (optionsSpec);
   parser.setOptionIncludeNoMore (true);
   parser.setOptionNewLineSeparator (true);

   bool status = parser.process (args, true);
   if (!status) {
      std::cerr << "error: " << parser.errorMessage() << nl;
      std::cerr << nl;
      parser.optionHelp (std::cerr);
      std::cerr << nl;
      return 2;
   }

   const Parsley::OptionValues options = parser.options();
   const Parsley::Arguments parameters = parser.parameters();

   Parsley::OptionValue value;

   value = options["help"];
   if (value.isDefined && value.flag) {
      parser.optionHelp (std::cout);
      return 0;
   }

   value = options["version"];
   if (value.isDefined && value.flag) {
      std::cout << PARSLEY_VERSION_STRING << nl;
      return 0;
   }

   std::cout << "params: " << Parsley::join (parameters) << nl;
   std::cout << "parsley exp complete" << nl;
   return 0;
}

// end

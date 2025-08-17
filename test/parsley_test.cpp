// parsley test
//

#include <iostream>
#include <iomanip>
#include <parsley.h>

#define nl                '\n'
#define FLAG(zz)          ((zz) ? "set" : "unset")
#define ARRAY_LENGTH(xx)  (int (sizeof (xx) /sizeof (xx [0])))
#define TEST_VERSION      "Test Version 1.2"

//-----------------------------------------------------------------------------
//
static const Parsley::EnumOptions enumChoice = {
   "aaa", "bbb", "ccc", "ddd", "eee", "fff"
};

//------------------------------------------------------------------------------
// This get and dumps an arbirary OptionValue
// Will rebadge SimpleValue, OptionValue => OptionValue, OptionValueHolder
//
static void dump (const Parsley::OptionValues& options,
                  const std::string& name)
{
   Parsley::OptionValue value = options[name];
   std::cout << std::left << std::setw (12) << name
             << std::setw (14) << (value.isDefined ? " defined" : " not defined")
             << " flag: " << std::setw (6) << FLAG(value.flag)
             << std::right
             << " ival: " << std::setw (10) << value.ival
             << " real: " << std::setw (10) << value.real
             << " str: '" << value.str << "'" << nl;
}

//------------------------------------------------------------------------------
// Null tests
static int group1 (const Parsley::Arguments& args)
{
   static const Parsley::OptionSpecifications optionsSpec = { };

   Parsley parser (optionsSpec);

   bool status = parser.process (args, true);
   if (!status) {
      std::cerr << "error: " << parser.errorMessage() << nl;
      std::cerr << nl;
      parser.optionHelp (std::cerr);
      std::cerr << nl;
      return 2;
   }

   const Parsley::OptionValues options = parser.options();

   Parsley::OptionValue value;

   value = options["help"];
   if (value.isDefined && value.flag) {
      parser.optionHelp (std::cout);
      return 0;
   }

   value = options["version"];
   if (value.isDefined && value.flag) {
      std::cout << TEST_VERSION << nl;
      return 0;
   }

   dump (options, "mistake");

   const Parsley::Arguments parameters = parser.parameters();
   std::cout << "params: " << Parsley::join (parameters) << nl;
   return 0;
}


//------------------------------------------------------------------------------
// Basic tests
static int group2 (const Parsley::Arguments& args)
{
   static const Parsley::OptionSpecifications optionsSpec = {
      Parsley::flagSpec ("flag", 'f',  "The flag option description."),
      Parsley::strSpec  ("string", 's', "The string option description."),
      Parsley::enumSpec ("mode", 'm', "The mode option description.", enumChoice),
      Parsley::intSpec  ("number", 'n', "The number option description."),
      Parsley::realSpec ("real", 'r', "The real option description."),
      Parsley::version(),  // pre-defined singleton
      Parsley::help ()     // pre-defined singleton
   };

   Parsley parser (optionsSpec);

   bool status = parser.process (args, true);
   if (!status) {
      std::cerr << "error: " << parser.errorMessage() << nl;
      std::cerr << nl;
      parser.optionHelp (std::cerr);
      std::cerr << nl;
      return 2;
   }

   const Parsley::OptionValues options = parser.options();

   Parsley::OptionValue value;

   value = options["help"];
   if (value.isDefined && value.flag) {
      parser.optionHelp (std::cout);
      return 0;
   }

   value = options["version"];
   if (value.isDefined && value.flag) {
      std::cout << TEST_VERSION << nl;
      return 0;
   }

   dump (options, "flag");
   dump (options, "string");
   dump (options, "mode");
   dump (options, "number");
   dump (options, "real");
   dump (options, "mistake");

   const Parsley::Arguments parameters = parser.parameters();
   std::cout << "params: " << Parsley::join (parameters) << nl;
   return 0;
}

//------------------------------------------------------------------------------
// Like group 2 but with program defined defaults
//
static int group3 (const Parsley::Arguments& args)
{
   static const Parsley::OptionSpecifications optionsSpec = {
      Parsley::flagSpec ("flag", 'f',  "The flag option description."),
      Parsley::strSpec  ("string", 's', "The string option description.")->defStr("one"),
      Parsley::enumSpec ("mode", 'm', "The mode option description.", enumChoice)->defStr("eee"),
      Parsley::intSpec  ("number", 'n', "The number option description.")->defInt(10),
      Parsley::realSpec ("real", 'r', "The real option description.")->defReal(31.6227),
      Parsley::version(),  // pre-defined singleton
      Parsley::help ()     // pre-defined singleton
   };

   Parsley parser (optionsSpec);

   bool status = parser.process (args, true);
   if (!status) {
      std::cerr << "error: " << parser.errorMessage() << nl;
      std::cerr << nl;
      parser.optionHelp (std::cerr);
      std::cerr << nl;
      return 2;
   }

   const Parsley::OptionValues options = parser.options();

   Parsley::OptionValue value;

   value = options["help"];
   if (value.isDefined && value.flag) {
      parser.optionHelp (std::cout);
      return 0;
   }

   value = options["version"];
   if (value.isDefined && value.flag) {
      std::cout << TEST_VERSION << nl;
      return 0;
   }

   dump (options, "flag");
   dump (options, "string");
   dump (options, "mode");
   dump (options, "number");
   dump (options, "real");
   dump (options, "mistake");

   const Parsley::Arguments parameters = parser.parameters();
   std::cout << "params: " << Parsley::join (parameters) << nl;
   return 0;
}

//------------------------------------------------------------------------------
// Like group 2 but with program defined defaults
//
static int group4 (const Parsley::Arguments& args)
{
   static const Parsley::OptionSpecifications optionsSpec = {
      Parsley::flagSpec ("flag", 'f',  "The flag option description.")->envVar("PARSLEY_FLAG"),
      Parsley::strSpec  ("string", 's', "The string option description.")->envVar("PARSLEY_STR"),
      Parsley::enumSpec ("mode", 'm', "The mode option description.", enumChoice)->envVar("PARSLEY_ENUM"),
      Parsley::intSpec  ("number", 'n', "The number option description.")->envVar("PARSLEY_INT"),
      Parsley::realSpec ("real", 'r', "The real option description.")->envVar("PARSLEY_REAL"),
      Parsley::version(),  // pre-defined singleton
      Parsley::help ()     // pre-defined singleton
   };

   Parsley parser (optionsSpec);

   bool status = parser.process (args, true);
   if (!status) {
      std::cerr << "error: " << parser.errorMessage() << nl;
      std::cerr << nl;
      parser.optionHelp (std::cerr);
      std::cerr << nl;
      return 2;
   }

   const Parsley::OptionValues options = parser.options();

   Parsley::OptionValue value;

   value = options["help"];
   if (value.isDefined && value.flag) {
      parser.optionHelp (std::cout);
      return 0;
   }

   value = options["version"];
   if (value.isDefined && value.flag) {
      std::cout << TEST_VERSION << nl;
      return 0;
   }

   dump (options, "flag");
   dump (options, "string");
   dump (options, "mode");
   dump (options, "number");
   dump (options, "real");
   dump (options, "mistake");

   const Parsley::Arguments parameters = parser.parameters();
   std::cout << "params: " << Parsley::join (parameters) << nl;
   return 0;
}

// Like group 2 but with both program defined and environment variable
// defined defaults



//------------------------------------------------------------------------------
//
int main (int argc, char** argv)
{
   std::cout << "parsley test: ";

   Parsley::Arguments args = Parsley::formArguments (argc, argv);
   std::cout << Parsley::join (args) << nl;

   // We use the last parameter to select which option specification
   // group we are interested in.
   //
   int groupNumber;
   if (!Parsley::str2int (argv[argc -1], groupNumber)) {
      std::cerr << "parsley test invalid group number: " << argv[argc -1] <<  nl;
      return 4;
   }

   int status;
   switch (groupNumber) {

      case 1:
         status = group1 (args);
         break;

      case 2:
         status = group2 (args);
         break;

      case 3:
         status = group3 (args);
         break;

      case 4:
         status = group4 (args);
         break;

      default:
         std::cerr << "parsley test group number invalid: "
                   << groupNumber <<  nl;
         status = 4;
         break;
   }

   std::cout << "parsley test complete" << nl;
   return status;
}

// end

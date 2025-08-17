/* parsley.h
 *
 * Description:
 * The Parsley class provides a C++ command line option parser.
 * Parsley – it sprinkles a little flavor on your parsing.
 *
 * Credit: inspired, in part at least, by the python click module.
 *
 * SPDX-FileCopyrightText: 2025  Andrew C. Starritt
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#ifndef PARSLEY_H
#define PARSLEY_H

#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#if defined(_WIN32)
#   if defined(BUILDING_PARSLEY_LIBRARY)
#      define PARSLEY_SHARED __declspec(dllexport)
#   else
#      define PARSLEY_SHARED __declspec(dllimport)
#   endif
#elif __GNUC__ >= 4
#   define PARSLEY_SHARED    __attribute__ ((visibility("default")))
#else
#warning "Unknown compiler environment - proceeding cautiously"
#endif

#define PARSLEY_MAJOR           1
#define PARSLEY_MINOR           1
#define PARSLEY_PATCH           1
#define PARSLEY_VERSION         0x010101
#define PARSLEY_VERSION_STRING  "Parsley 1.1.1"

/// \brief The Parsley class provides a class to parse command line options.
///
/// The Parsley class
///
class PARSLEY_SHARED Parsley {
public:
   /// Arguments defines a collection (std vector) of command line
   /// arguments (std strings).
   //
   typedef std::vector <std::string> Arguments;

   /// EnumOptions defines a collection (std vector) of command line
   /// enumeration options (std strings).
   //
   typedef std::vector <std::string> EnumOptions;
   //
   // Allthough defined the "same" this are considered different types.

   /// intp_t defines the Parsley integer option type.
   ///
   typedef int intp_t;   // parsely integer type

   //---------------------------------------------------------------------------
   // OptionSpec characterises/specifies an option.
   //
   class OptionSpec;

   /// OptionSpecPointer is a std shared pointer type referencing an OptionSpec.
   //
   typedef std::shared_ptr<OptionSpec> OptionSpecPointer;

   /// OptionSpecifications is a collection (std list) of OptionSpecPointers/
   //
   typedef std::list <OptionSpecPointer> OptionSpecifications;

   /// OptionSpec construction methods that return a OptionSpecPointer.
   /// OptionSpecPointer is a shared pointer, so no need to manually free these.
   ///
   /// Provides: -h, --help   with description: "Show this message and exit."
   //
   static OptionSpecPointer help ();     // help option - singleton

   /// Provides: -V, --version   with description: "Show version and exit."
   //
   static OptionSpecPointer version ();  // version option - singleton

   /// This constructs a flag option specification.
   /// This is implicitly optional, default false.
   //
   static OptionSpecPointer
   flagSpec (const std::string& longName,
             const char shortName,
             const std::string& description,
             const bool isSingleton = false);

   /// This constructs a string option specification.
   //
   static OptionSpecPointer
   strSpec (const std::string& longName,
            const char shortName,
            const std::string& description,
            const bool isRequired = false);

   /// This constructs an enumeration option specification.
   //
   static OptionSpecPointer
   enumSpec (const std::string& longName,
             const char shortName,
             const std::string& description,
             const EnumOptions& enumOptions,
             const bool isRequired = false);

   /// This constructs an integer (intp_t) option specification.
   //
   static OptionSpecPointer
   intSpec (const std::string& longName,
            const char shortName,
            const std::string& description,
            const bool isRequired = false);

   /// This constructs an real (double) option specification.
   //
   static OptionSpecPointer
   realSpec (const std::string& longName,
             const char shortName,
             const std::string& description,
             const bool isRequired = false);


   //---------------------------------------------------------------------------
   /// Options are specified using the flagSpec, strSpec etc. defined above.
   /// Options can be qualified with a range constraint, a default value and/or
   /// an evironment variable defined default, e.g.:
   ///
   /// OptionSpec opt = Parsley::integer ("number", 'n', "Number of widgets.")->
   ///                                    range(1, 20)->
   ///                                    defval(4)->
   ///                                    envVar("NUMBER_OF_WIDGETS"))
   ///
   /// Also look at ropo_top_dir/test/parsley_test.cpp
   ///
   class OptionSpec {
   public:
      virtual ~OptionSpec ();   // this needs to be public

      // Provides a default values.
      //
      /// \brief defStr adds a default value to a string or enumeration
      /// option specification.
      /// \param defValue - std::string - the default value.
      /// \return OptionSpecPointer
      ///
      OptionSpecPointer defStr (const std::string& defValue);

      /// \brief defInt adds a default value to an integer option specification.
      /// \param defValue - intp_t - the default value.
      /// \return  OptionSpecPointer
      ///
      OptionSpecPointer defInt (const intp_t defValue);

      /// \brief defInt adds a default value to a real option specification.
      /// \param defValue - double - the default value.
      /// \return  OptionSpecPointer
      ///
      OptionSpecPointer defReal (const double defValue);

      // Provided an allowed range - numeric options only.
      //
      /// \brief intRange adds a range constraint to an integer option specification.
      /// \param min - the minimum allowed value.
      /// \param max - the maximum allowed value.
      /// \return OptionSpecPointer
      ///
      OptionSpecPointer intRange (const intp_t min, const intp_t max);

      /// \brief realRange adds a range constraint to a real option specification.
      /// \param min - the minimum allowed value.
      /// \param max - the maximum allowed value.
      /// \return OptionSpecPointer
      ///
      OptionSpecPointer realRange (const double min, const double max);

      // Defines the name of an environment variable that can supply
      // the option value if not otherwise specified.
      //
      ///
      /// \brief envVar - defines the name of an environment variable that can
      /// supply the option value if not otherwise specified.
      /// \param envVarName - define the environment variable name
      /// \return OptionSpecPointer
      //
      OptionSpecPointer envVar (const std::string& envVarName);

   private:
      enum Kind {
         kFlag = 0,
         kStr,
         kEnum,
         kInt,
         kReal   // double
      };

      static std::string kindImage (const Kind kind);

      OptionSpec (const Kind kind,
                  const std::string& longName,
                  const char shortName,
                  const std::string& description,
                  const bool isRequired);

      OptionSpec (const OptionSpec& other);

      std::string name () const;      // Used for the error messages.
      std::string range () const;
      std::string enum_set () const;

      // supports optionHelp method.
      std::string info () const;
      std::string helpConstraint () const;
      std::string helpDefault () const;
      std::string helpEnvVar () const;

      const Kind m_kind;
      const std::string m_longName;
      const char m_shortName;
      const std::string m_description;
      const bool m_isRequired;

      bool m_isSingleton;
      Parsley::EnumOptions m_enumOptions;

      bool m_rangeIsDefined;
      intp_t m_minIntValue;
      intp_t m_maxIntValue;
      double m_minRealValue;
      double m_maxRealValue;

      // Environment variable
      bool m_evIsDefined;
      std::string m_evName;

      bool m_defaultIsDefined;
      std::string m_defaultStr;   // string or enum
      intp_t m_defaultInt;
      double m_defaultReal;

      friend class Parsley;
   };

   //---------------------------------------------------------------------------
   /// OptionValue - instances of this class are available to the user
   /// accessed from the OptionValues class.
   ///
   class OptionValue {
   public:
      explicit OptionValue ();
      ~OptionValue ();

      // I could have made these members private and added getter methods,
      // however I'm not sure if it would be worth it. Users can only mess
      // up their own copy, not the original.
      //
      /// \brief isDefined
      ///
      bool isDefined;    // either explicitly or by default

      /// \brief flag
      ///
      bool flag;

      /// \brief str
      ///
      std::string str;   // str or enum value

      /// \brief ival
      ///
      intp_t ival;       // int value or enum index

      /// \brief real - provides the read value (if and only if isDefined
      /// is set true and this is a real option),
      ///
      double real;
   };

   //---------------------------------------------------------------------------
   /// ProxyValue - this is a private/internal class.
   /// As well as holding the OptionValue values, it also hold
   /// private/internal house-keeping data.
   ///
   class ProxyValue;

   /// \brief ProxyValuePointer provides a shared pointer value to A
   /// ProxyValue instance.
   ///
   typedef std::shared_ptr<ProxyValue> ProxyValuePointer;

   //---------------------------------------------------------------------------
   /// A wrapper class around unordered_map of ProxyValues.
   /// This allows operator[] to be const, and therfore allows options to be
   /// declared const: e.g.:
   ///
   /// Parsley parser (...);
   /// const Parsley::OptionValues options = parser.options();
   /// const Parsley::OptionValue item = options["option_name"];
   ///
   class OptionValues {
   public:
      explicit OptionValues();
      ~OptionValues();

      /// \brief operator [] allows access to options["help"] and the like. Read-only.
      /// \param option - the option name
      /// \return OptionValue
      ///
      OptionValue operator[] (const std::string& option) const;

   private:
      void clear ();
      void set (const std::string& option, const ProxyValuePointer& value);

      typedef std::unordered_map <std::string, ProxyValuePointer> MapType;
      MapType theMap;

      friend class Parsley;
   };


   // Object instance methods.
   //
   /// \brief Parsley object constructor.
   /// \param specList - the collection of option specifications.
   ///
   /// Parsley is primarily to intended to interpret and validate user command
   /// line options. Note: there is some validation of the programmers
   /// option specifications.
   //
   explicit Parsley (const OptionSpecifications& specList);
   ~Parsley ();

   // Qualify how the auto generated option help information is generated.
   //
   /// \brief setOptionHelpCpl - sets number of charactres per line in
   /// the auto generated help information.
   /// The default is 92, the minimum is 40.
   /// \param cpl
   ///
   void setOptionHelpCpl (const int cpl);

   /// \brief setOptionNewLineSeparator - set/clear extra newline between
   /// options in the auto generated help information.
   /// The default is unset.
   /// \param extraNewLine
   ///
   void setOptionNewLineSeparator (const bool extraNewLine);

   /// \brief setOptionIncludeNoMore - controls if the '--' no more options option
   /// is described in the auto generated help information.
   /// The default is false.
   /// \param includeNoMore
   ///
   void setOptionIncludeNoMore (const bool includeNoMore);

   /// \brief optionHelp - provides auto generated option help information.
   /// \param stream - the output stream which the option help is written to.
   /// \return - the output stream.
   ///
   std::ostream& optionHelp (std::ostream& stream);

   // Utility function for process (and any other purpose).
   // It forms an Arguments vector from the standard argc/argv main parameters.
   //
   ///
   /// \brief formArguments - utility function to convert main argc and argv
   /// parameters into an Arguments collection.
   /// \param argc - number of arguments.
   /// \param argv - the char* style arguments
   /// \return Arguments
   ///
   static Arguments formArguments (const int argc, const char* const* argv);

   /// \brief process - processes the given arguments.
   /// \param arguments - the list of arguments to be analysed against the
   /// option specification list provided to the constructor.
   /// \param skipProgramName - when true, process skips over and ignore the zeroth argument.
   /// \return true if no error detected otherwise false.
   ///
   bool process (const Arguments& arguments, const bool skipProgramName);

   /// \brief errorMessage - returns the first error detected by the process
   /// mothod. Only applicable if/when Parsley::process returned false.
   /// \return std::string
   ///
   std::string errorMessage() const;

   /// \brief options - returns the set of option values.
   /// Only applicable if/when Parsley::process returned true.
   /// \return Parsley::OptionValues
   ///
   OptionValues options () const;

   /// \brief parameters - returns the arguments NOT consumed as options.
   /// __Note:__ Parsley does not parse the parameter arguments, only the options.
   /// \return Arguments
   ///
   Arguments parameters () const;

private:
   const OptionSpecifications m_specList;
   bool m_specListOkay;
   std::string m_errorMessage;
   OptionValues m_optionValues;
   Arguments m_parameters;

   // Qualifies optionHelp output.
   //
   int m_cpl;
   bool m_extraNewLine;
   bool m_includeNoMore;

/* bonus */ public:
   // Utility functions that may be usefull; not directly related to using
   // parsley, but do exist for internal use, so making available.
   //
   // Works for enums and arguments.
   // Concatinates the strings in args, separated by with.

   /// \brief join
   /// \param args
   /// \param with
   /// \return
   ///
   static std::string join (const Arguments& args, const std::string& with = " ");

   // The str2int/real functions do not throw an exceptiom on erroneous
   // input - they just return false.
   //
   /// \brief str2real
   /// \param str
   /// \param value
   /// \return
   ///
   static bool str2real (const std::string& str, double& value);

   /// \brief str2int
   /// \param str
   /// \param value
   /// \return
   ///
   static bool str2int (const std::string& str, intp_t& value);

   /// \brief real2str - converts a double to std::string.
   /// \param x the real value to be stringified.
   /// \return - the string representation of x.
   ///
   static std::string real2str (const double x);

   /// \brief int2str - converts an intp_t to std::string.
   /// \param i - the integer value to be stringified.
   /// \return - the string representation of i.
   ///
   static std::string int2str (const intp_t i);

   /// \brief indexOf - looks for an option within a collection of options.
   /// \param enumOptions - the collection of options.
   /// \param value - the test value.
   /// \return returns 0 .. length-1 if value is found, otherwise -1.
   ///
   static int indexOf (const EnumOptions& enumOptions,
                       const std::string& value);
};

#endif  // PARSLEY_H

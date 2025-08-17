/* parsley.cpp
 *
 * SPDX-FileCopyrightText: 2025  Andrew C. Starritt
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "parsley.h"
#include <cctype>
#include <cmath>    // for floor()
#include <limits>

#define nl        '\n'
#define dnl       "\n\n"


//------------------------------------------------------------------------------
// Utility functions
//------------------------------------------------------------------------------
//
static void warning (const std::string& message)
{
   std::cerr << "\033[33;1mwarning:\033[00m " << message << nl;
}

//------------------------------------------------------------------------------
// Trim: based on:
// https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring/60243752#60243752
//
static std::string stripString (const std::string& str)
{
   const size_t strSize = str.size();
   size_t start, end;

   for (start = 0; start < strSize && isspace(str[start]); start++);
   for (end = strSize; end > start && isspace(str[end - 1]); end--);

   return str.substr(start, end-start);
}

//-----------------------------------------------------------------------------
//
static std::list<std::string> splitString (const std::string& str,
                                           const std::string& splitter,
                                           const bool includeEmpty)
{
   const size_t len = splitter.length ();
   std::list<std::string> result;

   if (len > 0) {
      size_t pos = 0;  // start at the begining
      while (true) {
         std::size_t found = str.find (splitter, pos);
         if (found == std::string::npos) {
            std::string remaining = str.substr (pos);
            if (includeEmpty || remaining.size() > 0)
               result.push_back (remaining);
            break;
         }

         std::string item = str.substr (pos, found - pos);
         if (includeEmpty || item.size() > 0)
            result.push_back (item);
         pos = found + len;
      }

   } else {
      result.push_back (str);
   }

   return result;
}

//-----------------------------------------------------------------------------
//
static std::string formatLongLine (const std::string& indent,
                                   const std::string& name,
                                   const std::string& desc,
                                   const unsigned cpl)
{
   const size_t indentSize = indent.size();
   const std::list<std::string> ds = splitString (desc, " ", false);

   std::string result = "";

   std::string line = name + " ";   // always want at least one space
   while (line.size() < indentSize) line += " ";

   bool first = true;
   for (std::string item : ds) {
      // always add then test in case item very long
      if (first) {
         line += item;
      } else {
         line = line + " " + item;
      }
      first = false;

      if (line.size() >= cpl) {
         result = result + line + nl;
         line = indent;
         first = true;
      }
   }

   if (!first) {
      result = result + line + nl;
   }

   return result;
}


//------------------------------------------------------------------------------
// static
std::string Parsley::join (const Arguments& args, const std::string& with)
{
   std::string result = "";

   bool first = true;
   for (std::string item : args) {
      if (!first) result += with;
      result += item;
      first = false;
   }
   return result;
}

//------------------------------------------------------------------------------
//
bool Parsley::str2real (const std::string& str, double& value)
{
   // Does str contain !!
   //
   std::size_t found = str.find ("!!", 0);
   if (found != std::string::npos) return false;

   const std::string temp = stripString (str) + "!!x";

   char c = ' ';
   const int n = sscanf(temp.c_str(), "%lf!!%c", &value, &c);
   if (n != 2) return false;
   if (c != 'x') return false;
   return true;
}

//------------------------------------------------------------------------------
//
bool Parsley::str2int (const std::string& str, intp_t& value)
{
   // Try as real first so that we can do a range check.
   //
   double real;
   bool s = str2real (str, real);
   if (!s) return false;

   // Min / max long values expressed as doubles.
   //
   static constexpr double dmin = std::numeric_limits<intp_t>::min();
   static constexpr double dmax = std::numeric_limits<intp_t>::max();

   if (real < dmin) return false;
   if (real > dmax) return false;

   const std::string temp = stripString (str) + "!!x";

   char c = ' ';
   const int n = sscanf(temp.c_str(), "%d!!%c", &value, &c);
   if (n != 2) return false;
   if (c != 'x') return false;

   return true;
}

//------------------------------------------------------------------------------
//
std::string Parsley::real2str (const double x)
{
   char buffer [40];

   const bool wholeNumber = (floor(x) == x);
   if (wholeNumber) {
      // Ensure we get the '.0'
      snprintf (buffer, sizeof (buffer), "%.1f", x);
   } else {
      snprintf (buffer, sizeof (buffer), "%g", x);
   }

   return std::string (buffer);
}

//------------------------------------------------------------------------------
//
std::string Parsley::int2str (const intp_t i)
{
   char buffer [40];
   snprintf (buffer, 40, "%d", i);
   return std::string (buffer);
}

//------------------------------------------------------------------------------
// static
Parsley::OptionSpecPointer Parsley::help ()
{
   OptionSpec* spec = new Parsley::OptionSpec
         (OptionSpec::Kind::kFlag,
          "help",
          'h',
          "Show this message and exit.",
          false);  // flags are implicitly optional.

   // allows sucessful parsing even when normally required options are not provided.
   //
   spec->m_isSingleton = true;
   spec->m_defaultIsDefined = true;   // the default is implicitly defined as false.

   return OptionSpecPointer (spec);
}

//------------------------------------------------------------------------------
// static
Parsley::OptionSpecPointer Parsley::version ()
{
   OptionSpec* spec = new Parsley::OptionSpec
         (OptionSpec::Kind::kFlag,
          "version",
          'V',
          "Show version and exit.",
          false);  // flags are implicitly optional.

   // Allows sucessful parsing even when otherwise required options are not provided.
   //
   spec->m_isSingleton = true;
   spec->m_defaultIsDefined = true;  // the default is implicitly false

   return OptionSpecPointer (spec);
}

//------------------------------------------------------------------------------
// static
Parsley::OptionSpecPointer
Parsley::flagSpec (const std::string& longName,
               const char shortName,
               const std::string& description,
               const bool isSingleton)
{
   OptionSpec* spec = new Parsley::OptionSpec
         (OptionSpec::Kind::kFlag,
          longName,
          shortName,
          description,
          false);  // flags are implicitly optional.

   // flags are implicitly defined, defaulting to false.
   spec->m_defaultIsDefined = true;
   spec->m_isSingleton = isSingleton;
   return OptionSpecPointer (spec);
}

//------------------------------------------------------------------------------
// static
Parsley::OptionSpecPointer
Parsley::strSpec (const std::string& longName,
                  const char shortName,
                  const std::string& description,
                  const bool isRequired)
{
   OptionSpec* spec = new Parsley::OptionSpec
         (OptionSpec::Kind::kStr,
          longName,
          shortName,
          description,
          isRequired);

   return OptionSpecPointer (spec);
}

//------------------------------------------------------------------------------
// static
Parsley::OptionSpecPointer
Parsley::enumSpec (const std::string& longName,
                   const char shortName,
                   const std::string& description,
                   const EnumOptions& enumOptions,
                   const bool isRequired)
{
   OptionSpec* spec = new Parsley::OptionSpec
         (OptionSpec::Kind::kEnum,
          longName,
          shortName,
          description,
          isRequired);

   spec->m_enumOptions = enumOptions;
   return OptionSpecPointer (spec);
}

//------------------------------------------------------------------------------
// static
Parsley::OptionSpecPointer
Parsley::intSpec (const std::string& longName,
                  const char shortName,
                  const std::string& description,
                  const bool isRequired)
{
   OptionSpec* spec = new Parsley::OptionSpec
         (OptionSpec::Kind::kInt,
          longName,
          shortName,
          description,
          isRequired);

   return OptionSpecPointer (spec);
}

//------------------------------------------------------------------------------
// static
Parsley::OptionSpecPointer
Parsley::realSpec (const std::string& longName,
                   const char shortName,
                   const std::string& description,
                   const bool isRequired)
{
   OptionSpec* spec = new Parsley::OptionSpec
         (OptionSpec::Kind::kReal,
          longName,
          shortName,
          description,
          isRequired);

   return OptionSpecPointer (spec);
}


//==============================================================================
// Parsley::OptionSpec
//==============================================================================
// static
std::string Parsley::OptionSpec::kindImage (const Kind kind)
{
   static const std::string images[] = { "flag", "string", "enumSpec", "integer", "real" };
   return images[kind];
}

//------------------------------------------------------------------------------
//
Parsley::OptionSpec::OptionSpec (const Kind kindIn,
                                 const std::string& longNameIn,
                                 const char shortNameIn,
                                 const std::string& descriptionIn,
                                 const bool isRequiredIn):
   m_kind (kindIn),
   m_longName (longNameIn),
   m_shortName (shortNameIn),
   m_description (descriptionIn),
   m_isRequired (isRequiredIn)
{
   this->m_isSingleton = false;

   this->m_enumOptions.clear();

   this->m_rangeIsDefined = false;
   this->m_minIntValue = 0;
   this->m_maxIntValue = 0;
   this->m_minRealValue = 0.0;
   this->m_maxRealValue = 0.0;

   this->m_evIsDefined = false;
   this->m_evName = "";

   this->m_defaultIsDefined = false;
   this->m_defaultStr = "";
   this->m_defaultInt = 0;
   this->m_defaultReal = 0.0;
}

//------------------------------------------------------------------------------
// Clone/copy and existing spec
//
Parsley::OptionSpec::OptionSpec (const OptionSpec& other) :
   m_kind (other.m_kind),
   m_longName (other.m_longName),
   m_shortName (other.m_shortName),
   m_description (other.m_description),
   m_isRequired (other.m_isRequired)
{
   this->m_isSingleton = other.m_isSingleton;

   this->m_enumOptions = other.m_enumOptions;

   this->m_rangeIsDefined = other.m_rangeIsDefined;
   this->m_minIntValue = other.m_minIntValue;
   this->m_maxIntValue = other.m_maxIntValue;
   this->m_minRealValue = other.m_minRealValue;
   this->m_maxRealValue = other.m_maxRealValue;

   this->m_evIsDefined = other.m_evIsDefined;
   this->m_evName = other.m_evName;

   this->m_defaultIsDefined = other.m_defaultIsDefined;
   this->m_defaultStr = other.m_defaultStr;
   this->m_defaultInt = other.m_defaultInt;
   this->m_defaultReal = other.m_defaultReal;
}

//------------------------------------------------------------------------------
//
Parsley::OptionSpec::~OptionSpec () { }



//------------------------------------------------------------------------------
// Note on defval/range/envVar - we need to create a new object
// (which is a clone of the original) and we return a shared pointer.
//
//------------------------------------------------------------------------------
//
Parsley::OptionSpecPointer Parsley::OptionSpec::defStr (const std::string& defValue)
{
   OptionSpec* clone = new OptionSpec(*this);

   if (clone->m_kind != kStr && clone->m_kind != kEnum) {
      warning ("default string value for " + this->info() + " ignored.");
   } else if (clone->m_defaultIsDefined) {
      warning ("secondary default value for " + this->info() + " ignored.");
   } else if ((clone->m_kind == kEnum) &&
              (Parsley::indexOf(clone->m_enumOptions, defValue) == -1)) {
      warning ("the default value for " + this->info() + " is not an allowed value.");
   } else {
      clone->m_defaultStr = defValue;
      clone->m_defaultIsDefined = true;
   }

   return Parsley::OptionSpecPointer (clone);
}

//------------------------------------------------------------------------------
//
Parsley::OptionSpecPointer
Parsley::OptionSpec::defInt (const intp_t defValue)
{
   OptionSpec* clone = new OptionSpec(*this);

   if (clone->m_kind != kInt) {
      warning ("default integer value for " + this->info() + " ignored.");
   } else if (clone->m_defaultIsDefined) {
      warning ("secondary default value for " + this->info() + " ignored.");
   } else {
      if (clone->m_rangeIsDefined && (defValue < clone->m_minIntValue || defValue > clone->m_maxIntValue)) {
         warning ("the default value for " + this->info() + " is out of range.");
      }
      clone->m_defaultInt = defValue;
      clone->m_defaultIsDefined = true;
   }

   return Parsley::OptionSpecPointer (clone);
}

//------------------------------------------------------------------------------
//
Parsley::OptionSpecPointer
Parsley::OptionSpec::intRange (const intp_t min, const intp_t max)
{
   OptionSpec* clone = new OptionSpec(*this);

   if (clone->m_kind != kInt) {
      warning ("integer range constraint for " + this->info() + " ignored.");
   } else if (clone->m_rangeIsDefined) {
      warning ("secondary range constraint for " + this->info() + " ignored.");
   } else {
      if (clone->m_defaultIsDefined && (clone->m_defaultInt < min || clone->m_defaultInt > max)) {
         warning ("the default value for " + this->info() + " is out of range.");
      }
      clone->m_minIntValue = min;
      clone->m_maxIntValue = max;
      clone->m_rangeIsDefined = true;
   }

   return Parsley::OptionSpecPointer (clone);
}

//------------------------------------------------------------------------------
//
Parsley::OptionSpecPointer
Parsley::OptionSpec::defReal (const double defValue)
{
   OptionSpec* clone = new OptionSpec(*this);

   if (clone->m_kind != kReal) {
      warning ("default real value for " + this->info() + " ignored.");
   } else if (clone->m_defaultIsDefined) {
      warning ("secondary default value for " + this->info() + " ignored.");
   } else {
      if (clone->m_rangeIsDefined && (defValue < clone->m_minRealValue || defValue > clone->m_maxRealValue)) {
         warning ("the default value for " + this->info() + " is out of range.");
      }       clone->m_defaultReal = defValue;
      clone->m_defaultIsDefined = true;
   }

   return Parsley::OptionSpecPointer (clone);
}

//------------------------------------------------------------------------------
//
Parsley::OptionSpecPointer
Parsley::OptionSpec::realRange (const double min, const double max)
{
   OptionSpec* clone = new OptionSpec (*this);

   if (clone->m_kind != kReal) {
      warning ("real range constraint for " + this->info() + " ignored.");
   } else if (clone->m_rangeIsDefined) {
      warning ("secondary range constraint for " + this->info() + " ignored.");
   } else {
      if (clone->m_defaultIsDefined && (clone->m_defaultReal < min || clone->m_defaultReal > max)) {
         warning ("the default value for " + this->info() + " is out of range.");
      }
      clone->m_minRealValue = min;
      clone->m_maxRealValue = max;
      clone->m_rangeIsDefined = true;
   }

   return Parsley::OptionSpecPointer (clone);
}

//------------------------------------------------------------------------------
//
Parsley::OptionSpecPointer Parsley::OptionSpec::envVar (const std::string& envVarName)
{
   OptionSpec* clone = new OptionSpec(*this);

   if (clone->m_evIsDefined) {
      warning ("secondary environment variable for " + this->info() + " ignored.");
   } else {
      clone->m_evName = envVarName;
      clone->m_evIsDefined = (envVarName.length() > 0);
   }

   return Parsley::OptionSpecPointer (clone);
}

//------------------------------------------------------------------------------
// Used for the error message.
//
std::string Parsley::OptionSpec::name() const
{
   if (this->m_shortName != '\0') {
      return std::string ("-") + this->m_shortName + ", --" + this->m_longName;
   } else {
      return "--" + this->m_longName;
   }
}

//------------------------------------------------------------------------------
//
std::string Parsley::OptionSpec::range() const
{
   if ((this->m_kind != kInt) && (this->m_kind != kReal)) return "";
   if (!this->m_rangeIsDefined) return "";

   std::string v1;
   std::string v2;

   if (this->m_kind == kInt) {
      v1 = int2str (this->m_minIntValue);
      v2 = int2str (this->m_maxIntValue);
   } else if (this->m_kind == kReal) {
      v1 = real2str (this->m_minRealValue);
      v2 = real2str (this->m_maxRealValue);
   }

   char buffer [40];
   snprintf (buffer, sizeof(buffer), "%s to %s", v1.c_str(), v2.c_str());
   return std::string (buffer);
}

//------------------------------------------------------------------------------
//
std::string Parsley::OptionSpec::enum_set() const
{
   if (this->m_kind != kEnum) return "(nil)";
   return "(" + Parsley::join (m_enumOptions, ", ") + ")";
}

//------------------------------------------------------------------------------
//
std::string Parsley::OptionSpec::info () const
{
   return "the " + kindImage (this->m_kind) + " option '" + this->m_longName + '\'';
}

//------------------------------------------------------------------------------
//
std::string Parsley::OptionSpec::helpConstraint () const
{
   std::string result = "";

   switch (this->m_kind) {
      case kEnum:
         result = "Allowed values: " + this->enum_set() + ". ";
         break;

      case kReal:
      case kInt:
         if (this->m_rangeIsDefined) {
            result = "Range: " + this->range() + ". ";
         }
         break;

      default:
         break;
   }

   return result;
}

//------------------------------------------------------------------------------
//
std::string Parsley::OptionSpec::helpDefault () const
{
   if (!this->m_defaultIsDefined) return "";

   std::string result = "Default value: ";

   switch (this->m_kind) {
      case kFlag:
         result += "n/a";
         break;

      case kStr:
      case kEnum:
         result += "'" + this->m_defaultStr + "'";
         break;

      case kInt:
         result += int2str (this->m_defaultInt);
         break;

      case kReal:
         result += real2str (this->m_defaultReal);
         break;
   }
   result += ". ";

   return result;
}

//------------------------------------------------------------------------------
//
std::string Parsley::OptionSpec::helpEnvVar () const
{
   if (!this->m_evIsDefined) return "";

   std::string result = "Use the " + this->m_evName + " environment variable to ";

   if (this->m_defaultIsDefined) {
      result += "override the default value. ";
   } else {
      result += "provide a default value. ";
   }

   return result;
}


//==============================================================================
// Parsley::OptionValue
//==============================================================================
//
Parsley::OptionValue::OptionValue ()
{
   this->isDefined = false;
   this->flag = false;
   this->str = "";
   this->ival = 0;
   this->real = 0.0;
}

//------------------------------------------------------------------------------
//
Parsley::OptionValue::~OptionValue () {}


//==============================================================================
// Parsley::ProxyValue
//==============================================================================
//
class Parsley::ProxyValue {
public:
   ~ProxyValue();     // this needs to be public

private:
   explicit ProxyValue();

   // These members get copied to an OptionValue by the [] operator out of
   // the OptionValues class.
   //
   bool isDefined;    // either explicitly or by default
   bool flag;
   std::string str;   // str or enum value
   intp_t ival;       // int value or enum index
   double real;

   bool m_alreadySpecified;             // for inernal use to detect duplicates.
   Parsley::OptionSpecPointer m_spec;   // the associated option spec

   friend class Parsley;
};


//==============================================================================
//
Parsley::ProxyValue::ProxyValue ()
{
   this->isDefined = false;
   this->flag = false;
   this->str = "";
   this->ival = 0;
   this->real = 0.0;
   this->m_alreadySpecified = false;
}

//------------------------------------------------------------------------------
//
Parsley::ProxyValue::~ProxyValue () {}


//==============================================================================
// Parsley::OptionValues
//==============================================================================
//
Parsley::OptionValues::OptionValues () {}

//------------------------------------------------------------------------------
//
Parsley::OptionValues::~OptionValues ()
{
   this->theMap.clear();
}

//------------------------------------------------------------------------------
//
void Parsley::OptionValues::clear ()
{
   this->theMap.clear();
}

//------------------------------------------------------------------------------
//
void Parsley::OptionValues::set (const std::string& option,
                                 const ProxyValuePointer& value)
{
   this->theMap[option] = value;
}

//------------------------------------------------------------------------------
//
Parsley::OptionValue
Parsley::OptionValues::operator[] (const std::string& option) const
{
   auto entry = this->theMap.find(option);
   if (entry == this->theMap.end()) {
      OptionValue empty = OptionValue();
      return empty;
   } else {
      OptionValue item;
      // Recall entry->second is a ProxyValuePointer
      if (entry->second) {
         item.isDefined = entry->second->isDefined;
         item.flag = entry->second->flag;
         item.str = entry->second->str;
         item.ival = entry->second->ival;
         item.real = entry->second->real;
      }

      return item;
   }
}


//==============================================================================
// Parsley
//==============================================================================
// static
Parsley::Arguments Parsley::formArguments (const int argc,
                                           const char* const* argv)
{
   Arguments result;
   result.reserve (argc);

   for (int j = 0; j < argc; j++) {
      const std::string arg = argv[j];
      result.push_back (arg);
   }

   return result;
}

//------------------------------------------------------------------------------
// static
int Parsley::indexOf (const EnumOptions& opts,
                      const std::string& value)
{
   int index = -1;
   for (std::string item : opts) {
      index++;
      if (value == item) {
         return index;
      }
   }
   return -1;
}

//------------------------------------------------------------------------------
// constructor
Parsley::Parsley (const OptionSpecifications& specList) :
   m_specList (specList)
{
   // Set defaults.
   //
   this->m_cpl = 92;
   this->m_extraNewLine = false;
   this->m_includeNoMore = false;

   this->m_specListOkay = true;   // hypothesize ok

   // check for duplicates
   // Note: specList is a list not a vector so we have do own quazi indexing
   //
   int skip = 0;
   for (OptionSpecPointer specA : specList) {
      skip++;

      int count = 0;
      for (OptionSpecPointer specB : specList) {
         count++;
         if (count <= skip) continue;

         if ((specA->m_longName == specB->m_longName) ||
             ((specA->m_shortName != '\0') && (specA->m_shortName == specB->m_shortName))) {
             warning ("conflicting option names: " + specA->name() +
                      " and " + specB->name());
             this->m_specListOkay = false;
         }
      }
   }
}

//------------------------------------------------------------------------------
// destructor
Parsley::~Parsley () { }

//------------------------------------------------------------------------------
//
void Parsley::setOptionHelpCpl (const int cpl)
{
   this->m_cpl = cpl >= 40 ? cpl : 40;   // ensure sensible
}

//------------------------------------------------------------------------------
//
void Parsley::setOptionNewLineSeparator (const bool extraNewLine)
{
   this->m_extraNewLine = extraNewLine;
}

//------------------------------------------------------------------------------
//
void Parsley::setOptionIncludeNoMore (const bool includeNoMore)
{
   this->m_includeNoMore = includeNoMore;
}

//------------------------------------------------------------------------------
//
std::ostream& Parsley::optionHelp (std::ostream& os)
{
   static const std::string gap = "                    ";
   static const size_t gapSize = gap.size();

   os << "Options:" << nl;

   for (OptionSpecPointer spec : this->m_specList) {

      const bool literalDescription =
            (spec->m_description.size() >= 1) && (spec->m_description[0] == '!');

      if (literalDescription) {
         const std::string desc = spec->m_description.substr(1);   // Drop the '!'
         const std::list<std::string> ds = splitString (desc, "\n", true);

         std::string prefix = spec->name() + " ";   // always want at least one space
         while (prefix.size() < gapSize) prefix += " ";
         for (std::string part : ds) {
            os << prefix << part << nl;
            prefix = gap;
         }

      } else {
         // Just use regular long line formatting
         //
         os << formatLongLine (gap, spec->name(), spec->m_description, this->m_cpl);
      }

      std::string extra = "";
      if (spec->m_isRequired && !spec->m_defaultIsDefined) {
         // If a default is defined, then input is not required per se.
         extra += "Required. ";
      }

      switch (spec->m_kind) {
         case OptionSpec::Kind::kFlag:
            if (spec->m_evIsDefined) {
               extra += "Use the " + spec->m_evName +
                        " environment variable set to 'Y', 'YES' or '1' to set flag on. ";
            }
            break;

         case OptionSpec::Kind::kStr:
            extra += spec->helpDefault();
            extra += spec->helpEnvVar();
            break;

         case OptionSpec::Kind::kEnum:
         case OptionSpec::Kind::kInt:
         case OptionSpec::Kind::kReal:
            extra += spec->helpConstraint();
            extra += spec->helpDefault();
            extra += spec->helpEnvVar();
            break;

         default:
            break;
      }

      if (extra.length() > 0) {
         os << formatLongLine (gap, "", extra, this->m_cpl);
      }

      if (this->m_extraNewLine) os << nl;
   }

   static const std::string nullDecrption =
         "The null option indicating no more options. "
         "This is useful if/when the initial parameters \"look like\" options. ";

   if (this->m_includeNoMore) {
       os << formatLongLine (gap, "--", nullDecrption, this->m_cpl);
   }

   return os;
}

//------------------------------------------------------------------------------
//
bool Parsley::process (const Arguments& arguments,
                       const bool skipProgramName)
{
   this->m_errorMessage = "";
   this->m_optionValues.clear();
   this->m_parameters.clear();

   if (!this->m_specListOkay) {
      this->m_errorMessage = "option specification errors";
      return false;
   }

   // First create a map of options with default values.
   //
   for (auto iter = this->m_specList.cbegin();
        iter != this->m_specList.cend(); ++iter) {

      const OptionSpecPointer spec = *iter;

      ProxyValue value;

      value.m_alreadySpecified = false;
      value.isDefined = spec->m_defaultIsDefined;
      value.m_spec = spec;

      std::string source = spec->m_defaultIsDefined ? "default" : "";

      bool envVarValueAvalable = false;
      std::string evValue = "";
      if (spec->m_evIsDefined) {
         const char* envp = std::getenv (spec->m_evName.c_str());
         if (envp) {
            evValue = std::string (envp);
            envVarValueAvalable = true;
         }
      }

      // Note: we often just copy undefined default values as is
      // as opposed to doing a check - what would be the point?
      //
      switch (spec->m_kind) {
         case OptionSpec::Kind::kFlag:
            value.flag = false;
            if (envVarValueAvalable) {
               if ((evValue == "1") || (evValue == "Y") || (evValue == "YES")) {
                  value.flag = true;
               }
            }
            break;

         case OptionSpec::Kind::kStr:
            value.str = spec->m_defaultStr;
            if (envVarValueAvalable) {
               value.str = evValue;
               value.isDefined = true;
            }
            break;

         case OptionSpec::Kind::kEnum:
            value.str = spec->m_defaultStr;
            if (envVarValueAvalable) {
               source = "environment variable " + spec->m_evName;
               value.str = evValue;
               value.isDefined = true;
            }
            if (value.isDefined) {  // default or env var
               value.ival = indexOf (spec->m_enumOptions, value.str);
               if (value.ival < 0) {
                  this->m_errorMessage =
                        "invalid " + source + " value for " +
                        spec->name() + " : " + value.str +
                        " is not one of " +  spec->enum_set();
                  return false;
               }
            }
            break;

         case OptionSpec::Kind::kInt:
            value.ival = spec->m_defaultInt;
            if (envVarValueAvalable) {
               source = "environment variable " + spec->m_evName;
               bool status = str2int (evValue, value.ival);
               if (!status) {
                  this->m_errorMessage =
                        "invalid " + source + " value for " +
                        spec->name() + " : '" + evValue +
                        "' is not a valid integer.";
                  return false;
               }
               value.isDefined = true;
            }
            break;

         case OptionSpec::Kind::kReal:
            value.real = spec->m_defaultReal;
            if (envVarValueAvalable) {
               source = "environment variable " + spec->m_evName;
               bool status = str2real (evValue, value.real);
               if (!status) {
                  this->m_errorMessage =
                        "invalid " + source + " value for " +
                        spec->name() + " : '" + evValue +
                        "' is not a valid floating point number.";
                  return false;
               }
               value.isDefined = true;
            }
            break;

         default:
            this->m_errorMessage = "*** program error";
            return false;
      }


      ProxyValue* ref = new ProxyValue(value);  // Why does this work
      ProxyValuePointer ptr = ProxyValuePointer (ref);
      this->m_optionValues.set (spec->m_longName, ptr);
   }

   // Next process all arguments.
   //
   int index = -1;
   bool optionsComplete = false;

   // We have to use the iter format here (as opposed to for (item : container) {} )
   //
   for (auto iter = arguments.cbegin(); iter != arguments.cend(); ++iter) {

      index++;
      if ((index == 0) && skipProgramName) continue;

      const std::string arg = *iter;

      if (optionsComplete) {
         // Just add the the parameter list
         this->m_parameters.push_back (arg);
         continue;
      }

      if (arg == "--") {
         // "--" is the specual null option for no more options.
         // Useful for when a paramaeter starts with -
         //
         optionsComplete = true;
         continue;
      }

      if ((arg.length() == 0) || (arg[0] != '-')) {
         // Not an option - so must is first paramter.
         //
         this->m_parameters.push_back (arg);
         optionsComplete = true;
         continue;
      }

      // Start processing the options.
      //
      OptionSpecPointer spec = nullptr;
      if (arg.length() == 2) {
         // Must be short form, e.g. -h, -x.
         //
         for (OptionSpecPointer checkSpec : this->m_specList) {
            if (checkSpec->m_shortName == arg[1]) {
               spec = checkSpec;  // found it
               break;
            }
         }
      }

      else if ((arg.length() >= 3) && (arg.substr(0,2) == "--")) {
         // Must be long form, e.g. --help
         //
         std::string longName = arg.substr(2);

         for (OptionSpecPointer checkSpec : this->m_specList) {
            if (checkSpec->m_longName == longName) {
               spec = checkSpec;   // found it
               break;
            }
         }

      } else {
         // Is something like: -xxx
         //
         this->m_errorMessage = "invalid option format: " + arg;
         return false;
      }

      if (!spec) {
         this->m_errorMessage = "no such option: " + arg;
         return false;
      }

      ProxyValuePointer value = this->m_optionValues.theMap[spec->m_longName];

      if (value->m_alreadySpecified) {
         this->m_errorMessage = "duplicate option: " + spec->name();
         return false;
      }
      value->m_alreadySpecified = true;

      std::string argValue;

      // Macro function to check for an option argument.
      //
#define CHECK_ARGUMENT {                                   \
   ++iter;                                                 \
   if (iter == arguments.cend()) {                         \
      this->m_errorMessage = "option " + spec->name() +    \
                             " requires an argument.";     \
      return false;                                        \
   }                                                       \
   argValue = *iter;                                       \
}

      bool status;
      switch (spec->m_kind) {
         case OptionSpec::Kind::kFlag:
            value->flag = true;
            value->isDefined = true;
            break;

         case OptionSpec::Kind::kStr:
            CHECK_ARGUMENT;
            value->str = argValue;
            value->isDefined = true;
            break;

         case OptionSpec::Kind::kEnum:
            CHECK_ARGUMENT;
            value->ival = indexOf (spec->m_enumOptions, argValue);
            if (value->ival < 0) {
               this->m_errorMessage =
                     "invalid value for " + spec->name() + " : " + argValue +
                     " is not one of " +  spec->enum_set();
               return false;
            }
            value->str = argValue;
            value->isDefined = true;
            break;

         case OptionSpec::Kind::kInt:
            CHECK_ARGUMENT;
            status = str2int (argValue, value->ival);
            if (!status) {
               this->m_errorMessage =
                     "invalid value for " + spec->name() + " : '" + argValue +
                     "' is not a valid integer.";
               return false;
            }

            if (spec->m_rangeIsDefined) {
               if ((value->ival < spec->m_minIntValue) ||
                   (value->ival > spec->m_maxIntValue)) {
                  this->m_errorMessage =
                        "invalid value for " + spec->name() + " : " +
                        int2str (value->ival) +
                        " is out of range " + spec->range() + ".";
                  return false;
               }
            }
            value->isDefined = true;
            break;

         case OptionSpec::Kind::kReal:
            CHECK_ARGUMENT;
            status = str2real (argValue, value->real);
            if (!status) {
               this->m_errorMessage =
                     "invalid value for " + spec->name() + " : '" + argValue +
                     "' is not a valid floating point number.";
               return false;
            }

            if (spec->m_rangeIsDefined) {
               if ((value->real < spec->m_minRealValue) ||
                   (value->real > spec->m_maxRealValue)) {
                  this->m_errorMessage =
                        "invalid value for " + spec->name() + " : " +
                        real2str (value->real) +
                        " is out of range " + spec->range() + ".";
                  return false;
               }
            }

            value->isDefined = true;
            break;

         default:
            this->m_errorMessage = "*** program error";
            return false;
            break;
      }

#undef CHECK_ARGUMENT

      // A singleton option has been specified - this overrides all else.
      //
      if (spec->m_isSingleton) return true;
   }

   // Now check all the options to verify all values are required have been defined.
   // This is really for those that have no default.
   //
   for (auto const& node : this->m_optionValues.theMap)
   {
      // const std::string name = node.first;
      const ProxyValuePointer value = node.second;

      if (value->m_spec->m_isRequired && !value->isDefined) {
         this->m_errorMessage = "a value is required for: " + value->m_spec->name();
         return false;
      }
   }

   return true;
}

//------------------------------------------------------------------------------
//
std::string Parsley::errorMessage() const
{
   return this->m_errorMessage;
}

//------------------------------------------------------------------------------
//
Parsley::OptionValues Parsley::options () const
{
   return this->m_optionValues;
}

//------------------------------------------------------------------------------
//
Parsley::Arguments Parsley::parameters () const
{
   return this->m_parameters;
}

// end

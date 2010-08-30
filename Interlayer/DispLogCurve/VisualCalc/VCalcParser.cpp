/**
 * @file VCalcParser.cpp
 * @brief VisualCalc Parser class member functions definitions.
 *
 * Defines the CVCalcParser class member functions.
 */

#include "stdafx.h"
#include "VCalcParser.h"
#include <comdef.h>


/**
 * @brief VisualCalc Parser Constructor.
 *
 * Constructs an instance of the VisualCalc Parser.<BR>
 * It initializes the constant members <I>m_PI</I>, <I>m_E</I>
 *   and the Parser version string.<BR>
 * The Functions, Variables and Answers lists are also reset (cleared),
 *   as the other data members are also set to their default value.
 */
CVCalcParser::CVCalcParser()
		: m_PI(3.1415926535897932384626433832795),
		  m_E (2.7182818284590452353602874713527),
		  m_nRowId(0),
		  m_nColId(0),
		  m_strParserVersion("2.5") {
	this->ResetParserMembers("");
	this->ResetFunctions();
	this->ResetVariables();
	this->ResetAnswersHistory();
	m_pWndGrid = NULL;
}


/**
 * @brief VisualCalc Parser Destructor.
 * 
 * As in the Constructor, the lists are reset to an empty content,
 *   and the other data members are set to a default state.
 */
CVCalcParser::~CVCalcParser() {
	this->ResetParserMembers("");
	this->m_lstFunctions.clear();
	this->m_mapVariables.clear();
	this->m_dqeAnswersHistory.clear();
}


/**
 * @brief Resets the parser members state to default.
 * @param strSource String to initialize the source member with.
 *
 * Resets the parser data members ; it set them to a defaut value :
 *     - Source string with the parameter value,
 *     - default token (end of the string to be parsed),
 *     - empty identifier value (used for function and variable names),
 *     - zero number value (used for calculated results),
 *     - no warning (no message, no flag set),
 *     - reading index placed at the begin of the Source string.
 */
void CVCalcParser::ResetParserMembers(const std::string strSource) {
	this->m_tokCurrentToken		= TV_END;
	this->m_strSource			= strSource;
	this->m_strIdentifierValue	= "";
	this->m_strWarningMsg		= "";
	this->m_valNumberValue		= 0.0;
	this->m_bWarningFlag		= false;
	this->m_bEndEncountered		= false;
	this->m_iCurrentIndex		= 0;
}


/**
 * @brief Resets the functions list to its default state.
 *
 * It initializes the list of the functions exposed for and
 *   usable by the user (in the string to be parser) with the
 *   functions name as they have to be typed
 *   (the parser is case sensitive on identifiers).
 */
void CVCalcParser::ResetFunctions() {
	this->m_lstFunctions.clear();
	this->m_lstFunctions.push_back("abs");
	this->m_lstFunctions.push_back("Acos");
	this->m_lstFunctions.push_back("Ans");
	this->m_lstFunctions.push_back("Asin");
	this->m_lstFunctions.push_back("Atan");
	this->m_lstFunctions.push_back("cos");
	this->m_lstFunctions.push_back("cosh");
	this->m_lstFunctions.push_back("deg");
	this->m_lstFunctions.push_back("exp");
	this->m_lstFunctions.push_back("ln");
	this->m_lstFunctions.push_back("log");
	this->m_lstFunctions.push_back("logn");
	this->m_lstFunctions.push_back("nAp");
	this->m_lstFunctions.push_back("nCp");
	this->m_lstFunctions.push_back("product");
	this->m_lstFunctions.push_back("rad");
	this->m_lstFunctions.push_back("sin");
	this->m_lstFunctions.push_back("sinh");
	this->m_lstFunctions.push_back("sqrt");
	this->m_lstFunctions.push_back("sqrtn");
	this->m_lstFunctions.push_back("sum");
	this->m_lstFunctions.push_back("tan");
	this->m_lstFunctions.push_back("tanh");
	this->m_lstFunctions.push_back("row");
	this->m_lstFunctions.push_back("col");
	this->m_lstFunctions.push_back("cell");
}


/**
 * @brief Resets the variables list to its default state.
 *
 * The list of the variables used and defined by the user is
 *   cleared and reset to a default list.<BR>
 * The default list contains the mathematic constants <I>pi</I> and <I>e</I>
 *   (which cannot be removed from the variable list).
 */
void CVCalcParser::ResetVariables() {
	this->m_mapVariables.clear();
	this->m_mapVariables["e"]  = m_E;
	this->m_mapVariables["pi"] = m_PI;
	this->m_mapVariables["rowid"] = m_nRowId;
	this->m_mapVariables["colid"] = m_nColId;
}


/**
 * @brief Resets the answers history list to its default state.
 *
 * The history of the formulas typed by the user and their relative result is
 *   cleared and reset to aa empty list.
 */
void CVCalcParser::ResetAnswersHistory() {
	this->m_dqeAnswersHistory.clear();
}


/**
 * @brief Returns the functions list.
 * @return A reference to the functions list.
 *
 * This function is a part of the public members of the Parser and
 *   interfaces the list of the functions available for the user.
 */
const std::list<std::string>& CVCalcParser::GetFunctions() {
	return this->m_lstFunctions;
}


/**
 * @brief Returns the variables map.
 * @return A reference to the variables map.
 *
 * This function is a part of the public members of the Parser and
 *   interfaces the map of the variables typed by the user and 
 *   the constants, with their associated value.
 */
const std::map<std::string, VALUES_TYPE>& CVCalcParser::GetVariables() {
	return this->m_mapVariables;
}


/**
 * @brief Returns the answers history.
 * @return A reference to the answers history.
 *
 * This function is a part of the public members of the Parser and
 *   interfaces the deque of the last formulas typed by the user which
 *   resulted into a valid answer, with their associated result.
 */
const std::deque<AnswerItem>& CVCalcParser::GetAnswersHistory() {
	return this->m_dqeAnswersHistory;
}


/**
 * @brief Tells if the parsing throws a warning.
 * @return The warning flag.
 *
 * If the parsing of a formula causes a warning (non blocking error) due 
 *   to possible loss of data, then the flag can be tested with this
 *   member function of the public interface of the Parser.
 */
bool CVCalcParser::HasWarning() {
	return this->m_bWarningFlag;
}


/**
 * @brief Returns the warning message.
 * @return The warning message.
 *
 * If the parsing of a formula causes a warning (non blocking error) due 
 *   to possible loss of data, then the flag is set and a descritpion message
 *   is exposed through this member function of the public interface of the Parser.
 */
std::string CVCalcParser::GetWarningMsg() {
	return this->m_strWarningMsg;
}


/**
 * @brief Returns the version of the Parser.
 * @return The version of the Parser.
 *
 * It simply returns as an informative data member a string which contains
 *   the version of the Parser.
 */
std::string CVCalcParser::GetVersion() {
	return this->m_strParserVersion;
}


/**
 * @brief Returns the factorial of the operand.
 * @param valOperand The value on which the factorial is calculated.
 * @return The factorial of the operand.
 * @throw CFactorialDefForPositiveIntsException 
 *     If the operand is not greater than or equal to 0.
 * @throw CMathParameterMustBeAnIntegerException
 *     If the operand is not an integer.
 * 
 * It calculates the factorial of the operand as the following
 *   formula describes :<BR>
 * \f$ x! = x \times (x - 1) \f$<BR>
 * As the mathematic factorial operator implies, the operand must be
 *   a positive operand, otherwise, a consequent exception is thrown.
 */
VALUES_TYPE CVCalcParser::ffactor(VALUES_TYPE valOperand)
		throw(CFactorialDefForPositiveIntsException,
			  CMathParameterMustBeAnIntegerException) {
	if (valOperand < 0) {
		throw CFactorialDefForPositiveIntsException(this->m_iCurrentIndex);
	}
	else if (valOperand != ::floor(valOperand)) {
		throw CMathParameterMustBeAnIntegerException(this->m_iCurrentIndex);
	}
	else if (valOperand == 0) {
		valOperand = 1;
	}
	else if (valOperand > 0) {
		for (long i = valOperand-1; i > 1; i--) {
			valOperand *= i;
		}
	}
	return valOperand;
}


/**
 * @brief Returns the <I>p</I> combinations on <I>n</I>.
 * @param n The set on wich are calculated the combinations.
 * @param p The number of combinations on the n set.
 * @return The <I>p</I> combinations on <I>n</I>.
 * @throw CNMustBePositiveException
 *     If <I>n</I> is not greater than or equal to 0.
 * @throw CPMustBePositiveException
 *     If <I>p</I> is not greater than or equal to 0.
 * @throw CMathParameterMustBeAnIntegerException
 *     If <I>n</I> and/or <I>p</I> are not integers.
 * @throw CPMustBeSmallerThanNException
 *     If <I>p</I> is not smaller than or equal to <I>n</I>.
 *
 * It calculates the <I>p</I> combinations on <I>n</I> as the following
 *   formula describes :<BR>
 * \f$ nCp(n,p) = \frac{ n! } { (n-p)! \times p! } \f$<BR>
 * As the mathematic nCp operator implies, the <I>n</I> and <I>p</I> operands
 *   must be positive integers, plus <I>p</I> must be smaller than <I>n</I>,
 *   otherwise, a consequent exception is thrown.
 */
VALUES_TYPE CVCalcParser::nCp(VALUES_TYPE n, VALUES_TYPE p)
		throw(CNMustBePositiveException,
			  CPMustBePositiveException,
			  CPMustBeSmallerThanNException,
			  CMathParameterMustBeAnIntegerException) {
	if (n < 0) {
		throw CNMustBePositiveException(this->m_iCurrentIndex);
	}
	else if (p < 0) {
		throw CPMustBePositiveException(this->m_iCurrentIndex);
	}
	else if (n != ::floor(n)) {
		throw CMathParameterMustBeAnIntegerException(this->m_iCurrentIndex);
	}
	else if (p != ::floor(p)) {
		throw CMathParameterMustBeAnIntegerException(this->m_iCurrentIndex);
	}
	else if (p > n) {
		throw CPMustBeSmallerThanNException(this->m_iCurrentIndex);
	}
	//  n! / ((n-p)! * p!)
	return (this->ffactor(n)) / ( this->ffactor(n - p) * this->ffactor(p) );
}


/**
 * @brief Returns the <I>p</I> arrangements on <I>n</I>.
 * @param n The set on wich are calculated the arrangements.
 * @param p The number of arrangements on the n set.
 * @return The <I>p</I> arrangements on <I>n</I>.
 * @throw CNMustBePositiveException
 *     If <I>n</I> is not greater than or equal to 0.
 * @throw CPMustBePositiveException
 *     If <I>p</I> is not greater than or equal to 0.
 * @throw CMathParameterMustBeAnIntegerException
 *     If <I>n</I> and/or <I>p</I> are not integers.
 * @throw CPMustBeSmallerThanNException
 *     If <I>p</I> is not smaller than or equal to <I>n</I>.
 *
 * It calculates the <I>p</I> arrangements on <I>n</I> as the following
 *   formula describes :<BR>
 * \f$ nAp(n,p) = \frac{ n! } { (n-p)! } \f$<BR>
 * As the mathematic nAp operator implies, the <I>n</I> and <I>p</I> operands
 *   must be positive integers, plus <I>p</I> must be smaller than <I>n</I>,
 *   otherwise, a consequent exception is thrown.
 */
VALUES_TYPE CVCalcParser::nAp(VALUES_TYPE n, VALUES_TYPE p)
		throw(CNMustBePositiveException,
			  CPMustBePositiveException,
			  CPMustBeSmallerThanNException,
			  CMathParameterMustBeAnIntegerException) {
	if (n < 0) {
		throw CNMustBePositiveException(this->m_iCurrentIndex);
	}
	else if (p < 0) {
		throw CPMustBePositiveException(this->m_iCurrentIndex);
	}
	else if (n != ::floor(n)) {
		throw CMathParameterMustBeAnIntegerException(this->m_iCurrentIndex);
	}
	else if (p != ::floor(p)) {
		throw CMathParameterMustBeAnIntegerException(this->m_iCurrentIndex);
	}
	else if (p > n) {
		throw CPMustBeSmallerThanNException(this->m_iCurrentIndex);
	}
	//  n! / (n-p)!
	return (this->ffactor(n)) / ( this->ffactor(n - p) );
}


/**
 * @brief Returns the required Answer in the history.
 * @param valIndex Index of the answer in the history.
 * @return The Answer in the history at the position specified by the parameter.
 * @throw CDomParameterMustBePositiveException
 *     If the parameter is not strictly greater than 0.
 * @throw CDomParameterMustBeAnIntegerException
 *     If the parameter is not an integer.
 * @throw CAnswerRequestedNotFoundException
 *     If the parameter is not in the range [1 ; history_length].
 *
 * It returns the AnswerItem positionned in the answers history at the index
 *   specified by the parameter.<BR>
 * The Index parameter value must be between 1 and the number of elements
 *   stored in the history, otherwise, a consequent exception is thrown.
 */
AnswerItem CVCalcParser::Ans(VALUES_TYPE valIndex)
		throw(CDomParameterMustBePositiveException,
			  CDomParameterMustBeAnIntegerException,
			  CAnswerRequestedNotFoundException) {
	valIndex--;
	if (valIndex < 0) {
		throw CDomParameterMustBePositiveException(this->m_iCurrentIndex);
	}
	else if (valIndex != ::floor(valIndex)) {
		throw CDomParameterMustBeAnIntegerException(this->m_iCurrentIndex);
	}
	else if (valIndex >= this->m_dqeAnswersHistory.size()) {
		throw CAnswerRequestedNotFoundException(this->m_iCurrentIndex);
	}
	return m_dqeAnswersHistory.at((int)valIndex);
}


/**
 * @brief Returns the absolute value of the operand.
 * @param valOperand Angle on which the absolute value is calculated.
 * @return The absolute value of the operand.
 *
 * This functions calculates the absolute value of its operand,
 *   and don't cause any error.
 */
VALUES_TYPE CVCalcParser::abs(VALUES_TYPE valOperand) {
	return ::fabs(valOperand);
}


/**
 * @brief Returns the cosine of the operand.
 * @param valOperand Operand in radian on which the cosine is calculated.
 * @return The cosine of the operand.
 * @throw CParameterOutOfRangeException
 *     If an error occured during calculation.
 *
 * This functions calculates the cosine of its operand which is an angle
 *   in radians. If the angle is out of the range [-2^63 ; 2^63], the result
 *   may have a loss of significance.
 */
VALUES_TYPE CVCalcParser::cos(VALUES_TYPE valOperand)
		throw(CParameterOutOfRangeException) {
	VALUES_TYPE valResult;
	try {
		valResult = ::cos(valOperand);
	}
	catch (...) {
		throw CParameterOutOfRangeException(this->m_iCurrentIndex);
	}
	return valResult;
}


/**
 * @brief Returns the sine of the operand.
 * @param valOperand Angle in radian on which the sine is calculated.
 * @return The sine of the operand.
 * @throw CParameterOutOfRangeException
 *     If an error occured during calculation.
 *
 * This functions calculates the sine of its operand which is an angle
 *   in radians. If the angle is out of the range [-2^63 ; 2^63], the result
 *   may have a loss of significance.
 */
VALUES_TYPE CVCalcParser::sin(VALUES_TYPE valOperand)
		throw(CParameterOutOfRangeException) {
	VALUES_TYPE valResult;
	try {
		valResult = ::sin(valOperand);
	}
	catch (...) {
		throw CParameterOutOfRangeException(this->m_iCurrentIndex);
	}
	return valResult;
}


/**
 * @brief Returns the tangent of the operand.
 * @param valOperand Angle in radian on which the tangent is calculated.
 * @return The tangent of the operand.
 * @throw CParameterOutOfRangeException
 *     If an error occured during calculation.
 *
 * This functions calculates the tangent of its operand which is an angle
 *   in radians. If the angle is out of the range [-2^63 ; 2^63], the result
 *   may have a loss of significance.
 */
VALUES_TYPE CVCalcParser::tan(VALUES_TYPE valOperand)
		throw(CParameterOutOfRangeException) {
	VALUES_TYPE valResult;
	try {
		valResult = ::tan(valOperand);
	}
	catch (...) {
		throw CParameterOutOfRangeException(this->m_iCurrentIndex);
	}
	return valResult;
}


/**
 * @brief Returns the hyperbolic cosine of the operand.
 * @param valOperand Angle in radian on which the hyperbolic cosine is calculated.
 * @return The hyperbolic cosine of the operand.
 * @throw CParameterOutOfRangeException
 *     If an error occured during calculation.
 *
 * This functions calculates the hyperbolic cosine of its operand which is an angle
 *   in radians :<BR>
 * \f$ cosh(x) = \frac{ e^x + e^-x } { 2 } \f$<BR>
 *   If the angle is out of the range [-2^63 ; 2^63], the result
 *   may have a loss of significance.
 */
VALUES_TYPE CVCalcParser::cosh(VALUES_TYPE valOperand)
		throw(CParameterOutOfRangeException) {
	VALUES_TYPE valResult;
	try {
		valResult = ::cosh(valOperand);
	}
	catch (...) {
		throw CParameterOutOfRangeException(this->m_iCurrentIndex);
	}
	return valResult;
}


/**
 * @brief Returns the hyperbolic sine of the operand.
 * @param valOperand Angle in radian on which the hyperbolic sine is calculated.
 * @return The hyperbolic sine of the operand.
 * @throw CParameterOutOfRangeException
 *     If an error occured during calculation.
 *
 * This functions calculates the hyperbolic sine of its operand which is an angle
 *   in radians :<BR>
 * \f$ sinh(x) = \frac{ e^x - e^-x } { 2 } \f$<BR>
 *   If the angle is out of the range [-2^63 ; 2^63], the result
 *   may have a loss of significance.
 */
VALUES_TYPE CVCalcParser::sinh(VALUES_TYPE valOperand)
		throw(CParameterOutOfRangeException) {
	VALUES_TYPE valResult;
	try {
		valResult = ::sinh(valOperand);
	}
	catch (...) {
		throw CParameterOutOfRangeException(this->m_iCurrentIndex);
	}
	return valResult;
}


/**
 * @brief Returns the hyperbolic tangent of the operand.
 * @param valOperand Angle in radian on which the hyperbolic tangent is calculated.
 * @return The hyperbolic tangent of the operand.
 * @throw CParameterOutOfRangeException
 *     If an error occured during calculation.
 *
 * This functions calculates the hyperbolic tangent of its operand which is an angle
 *   in radians :<BR>
 * \f$ tanh(x) = \frac{ e^x - e^-x } { e^x + e^-x } \f$<BR>
 *   If the angle is out of the range [-2^63 ; 2^63], the result
 *   may have a loss of significance.
 */
VALUES_TYPE CVCalcParser::tanh(VALUES_TYPE valOperand)
		throw(CParameterOutOfRangeException) {
	VALUES_TYPE valResult;
	try {
		valResult = ::tanh(valOperand);
	}
	catch (...) {
		throw CParameterOutOfRangeException(this->m_iCurrentIndex);
	}
	return valResult;
}


/**
 * @brief Returns the arccosine of the operand.
 * @param valOperand Operand on which the arccosine is calculated.
 * @return The Angle in radian which is the arcosine of the operand.
 * @throw CParameterOutOfRangeException
 *     If an error occured during calculation.
 *
 * This functions calculates the arccosine of its operand which is
 *   a value in the range [-1 ; 1] and the resulting angle is in
 *   the interval [0 ; pi].
 *   Otherwise, a consequent exception is thrown.
 */
VALUES_TYPE CVCalcParser::Acos(VALUES_TYPE valOperand)
		throw(CParameterOutOfRangeException) {
	if ((valOperand < -1) || (valOperand > 1)) {
		throw CParameterOutOfRangeException(this->m_iCurrentIndex);
	}
	return ::acos(valOperand);
}


/**
 * @brief Returns the arcsine of the operand.
 * @param valOperand Operand on which the arcsine is calculated.
 * @return The Angle in radian which is the arcsine of the operand.
 * @throw CParameterOutOfRangeException
 *     If an error occured during calculation.
 *
 * This functions calculates the arcsine of its operand which is
 *   a value in the range [-1 ; 1] and the resulting angle is in
 *   the interval [-pi/2 ; pi/2].
 *   Otherwise, a consequent exception is thrown.
 */
VALUES_TYPE CVCalcParser::Asin(VALUES_TYPE valOperand)
		throw(CParameterOutOfRangeException) {
	if ((valOperand < -1) || (valOperand > 1)) {
		throw CParameterOutOfRangeException(this->m_iCurrentIndex);
	}
	return ::asin(valOperand);
}


/**
 * @brief Returns the arctangent of the operand.
 * @param valOperand Operand on which the arctangent is calculated.
 * @return The Angle in radian which is the arctangent of the operand.
 * @throw CParameterOutOfRangeException
 *     If an error occured during calculation.
 *
 * This functions calculates the arctangent of its operand and the
 *   resulting angle is in the interval [-pi/2 ; pi/2].
 *   Otherwise, a consequent exception is thrown.
 */
VALUES_TYPE CVCalcParser::Atan(VALUES_TYPE valOperand)
		throw(CParameterOutOfRangeException) {
	if ((valOperand < -1) || (valOperand > 1)) {
		throw CParameterOutOfRangeException(this->m_iCurrentIndex);
	}
	return ::atan(valOperand);
}


/**
 * @brief Returns a radian angle converted into degrees.
 * @param valOperand Radian Angle.
 * @return The equivalence of the angle in degree.
 *
 * This functions calculates the equivalent of its operand
 *   in degrees into radians.
 */
VALUES_TYPE CVCalcParser::deg(VALUES_TYPE valOperand) {
	return ( (180 * valOperand) / this->m_PI );
}


/**
 * @brief Returns a degrees angle converted into radians.
 * @param valOperand degrees Angle.
 * @return The equivalence of the angle in radians.
 *
 * This functions calculates the arctangent of its operand and the
 *   resulting angle is in the interval [-pi/2 ; pi/2].
 *   Otherwise, a consequent exception is thrown.
 */
VALUES_TYPE CVCalcParser::rad(VALUES_TYPE valOperand) {
	return ( (this->m_PI * valOperand) / 180 );
}


/**
 * @brief Returns the exponential of the operand.
 * @param valOperand Operand on which is calculated the exponential.
 * @return The exponential of the operand.
 *
 * This functions calculates the exponential of its operand and the
 *   result is in the interval ]0 ; +inf[.
 */
VALUES_TYPE CVCalcParser::exp(VALUES_TYPE valOperand) {
	return ::exp(valOperand);
}


/**
 * @brief Returns the natural logarithm of the operand.
 * @param valOperand Operand on which is calculated the natural logarithm.
 * @return The natural logarithm of the operand.
 * @throw CMathParameterMustBePositiveException
 *     If the parameter is not higher than or equal to 0.
 *
 * This functions calculates the natural logarithm of its operand which
 *   must be in the interval ]0 ; +inf[.<BR>
 * The natural logarithm is a base-<I>e</I> logarithm :
 *   \f$ ln(x) = \frac{ ln(x) } { ln(e) } | ln(e) = 1 \f$
 */
VALUES_TYPE CVCalcParser::ln(VALUES_TYPE valOperand)
		throw(CMathParameterMustBePositiveException) {
	if (valOperand < 0) {
		throw CMathParameterMustBePositiveException(this->m_iCurrentIndex);
	}
	return ::log(valOperand);
}


/**
 * @brief Returns the decimal logarithm of the operand.
 * @param valOperand Operand on which is calculated the decimal logarithm.
 * @return The decimal logarithm of the operand.
 * @throw CMathParameterMustBePositiveException
 *     If the parameter is not higher than or equal to 0.
 *
 * This functions calculates the decimal logarithm of its operand which
 *   must be in the interval ]0 ; +inf[.<BR>
 * The decimal logarithm is a base-<I>10</I> logarithm :
 *   \f$ log(x) = \frac{ ln(x) } { ln(10) } \f$
 */
VALUES_TYPE CVCalcParser::log(VALUES_TYPE valOperand)
		throw(CMathParameterMustBePositiveException) {
	if (valOperand < 0) {
		throw CMathParameterMustBePositiveException(this->m_iCurrentIndex);
	}
	return ::log10(valOperand);
}


/**
 * @brief Returns the base-n logarithm of the operand.
 * @param valOperand Operand on which is calculated the base-n logarithm.
 * @param valBase Operand which determinates the base of the logarithm.
 * @return The base-n logarithm of the operand.
 * @throw CMathParameterMustBePositiveException
 *     If the operand and/or the base parameter is/are not higher than or equal to 0.
 * @throw CDivisionByZeroException
 *     If the base parameter is equal to 0.
 *
 * This functions calculates the base-n logarithm of its operand which
 *   must be in the interval ]0 ; +inf[.<BR>
 * This logarithm, as its name tells, is a base-<I>n</I> logarithm :
 *   \f$ logn(x,n) = log_n(x) = \frac{ ln(x) } { ln(n) } \f$
 */
VALUES_TYPE CVCalcParser::logn(VALUES_TYPE valOperand, VALUES_TYPE valBase)
		throw(CMathParameterMustBePositiveException,
			  CDivisionByZeroException) {
	if (valOperand < 0) {
		throw CMathParameterMustBePositiveException(this->m_iCurrentIndex);
	}
	if (valBase == 0) {
		throw CDivisionByZeroException(this->m_iCurrentIndex);
	}
	if (valBase < 0) {
		throw CMathParameterMustBePositiveException(this->m_iCurrentIndex);
	}
	return ( ::log(valOperand) / ::log(valBase) );
}


VARIANT CVCalcParser::cell(int nRow, int nCol)
{
	if (nRow < 0 || nCol < 0) {
		throw CGridParameterMustBeUnInt(this->m_iCurrentIndex);
	}
	
	if( m_pWndGrid == NULL )
	{
		throw CGridParameterNotExist(this->m_iCurrentIndex);
	}

	CString strTmp = m_pWndGrid->GetItemText(nRow, nCol);

	VARIANT var;
	var.vt = VT_BSTR;
	_bstr_t bstrTemp((LPCTSTR)strTmp);
	
	var.bstrVal = bstrTemp;

	return var;
}

VARIANT CVCalcParser::row(int nRow)
{
	if (nRow < 0 ) {
		throw CGridParameterMustBeUnInt(this->m_iCurrentIndex);
	}
	
	if( m_pWndGrid == NULL )
	{
		throw CGridParameterNotExist(this->m_iCurrentIndex);
	}
	
	CString strTmp = m_pWndGrid->GetItemText(nRow, m_nColId);
	
	VARIANT var;
	var.vt = VT_BSTR;
	_bstr_t bstrTemp((LPCTSTR)strTmp);
	
	var.bstrVal = bstrTemp;
	
	return var;
}

VARIANT CVCalcParser::col(int nCol)
{
	if ( nCol < 0) {
		throw CGridParameterMustBeUnInt(this->m_iCurrentIndex);
	}
	
	if( m_pWndGrid == NULL )
	{
		throw CGridParameterNotExist(this->m_iCurrentIndex);
	}
	
	CString strTmp = m_pWndGrid->GetItemText(m_nRowId, nCol);
	
	VARIANT var;
	var.vt = VT_BSTR;
	_bstr_t bstrTemp((LPCTSTR)strTmp);
	
	var.bstrVal = bstrTemp;
	
	return var;
}



/**
 * @brief Returns the square root of the operand.
 * @param valOperand Operand on which is calculated the square root.
 * @return The square root of the operand.
 * @throw CMathParameterMustBePositiveException
 *     If the operand is not higher than or equal to 0.
 *
 * This functions calculates the square root of its operand which
 *   must be in the interval [0 ; +inf[ :<BR>
 *   \f$ sqrt(x) = sqrtn(x,2) = \sqrt(x) \f$
 */
VALUES_TYPE CVCalcParser::sqrt(VALUES_TYPE valOperand)
		throw(CMathParameterMustBePositiveException) {
	if (valOperand < 0) {
		throw CMathParameterMustBePositiveException(this->m_iCurrentIndex);
	}
	return ::sqrt(valOperand);
}


/**
 * @brief Returns the n-order root of the operand.
 * @param valOperand Operand on which is calculated the n-order root.
 * @param valOrder Operand which determinates the order of the root.
 * @return The n-order root of the operand.
 * @throw CMathParameterMustBePositiveException
 *     If the operand is not higher than or equal to 0,
 *      or if the order is not strictly higher than  0.
 * @throw CMathParameterMustBeAnIntegerException
 *     If the order is not an integer.
 *
 * This functions calculates the n-order root of its operand which
 *   must be in the interval [0 ; +inf[, with an integral order :<BR>
 *   \f$ sqrtn(x,n) = \sqrt[n](x) \f$
 */
VALUES_TYPE CVCalcParser::sqrtn(VALUES_TYPE valOperand, VALUES_TYPE valOrder)
		throw(CMathParameterMustBePositiveException,
			  CMathParameterMustBeAnIntegerException) {
	if (valOperand < 0.) {
		throw CMathParameterMustBePositiveException(this->m_iCurrentIndex);
	}
	if (valOrder <= 0.) {
		throw CMathParameterMustBePositiveException(this->m_iCurrentIndex);
	}
	if (valOrder != ::floor(valOrder)) {
		throw CMathParameterMustBeAnIntegerException(this->m_iCurrentIndex);
	}
	return ::pow(valOperand, 1/valOrder);
}


/**
 * @brief Returns the left operand raised to the power of the right operand.
 * @param valLeftOperand Operand which is raised to the power of the right operand.
 * @param valRightOperand Operand which raises the left operand to his power.
 * @return The power of the left operand by the right operand.
 * @throw CMathParameterMustBePositiveException
 *     If the function tries to compute <I>0^y</I>,
 *     where <I>y</I> is a floating point decimal strictly lower than 0.
 *
 * This functions computes the left operand raised to the power of the right operand.<BR>
 * If the function tries to compute <I>0^0</I>, the following warning is sent :<BR>
 *   <I>Warning: 0^0 replaced by 1</I>.
 */
VALUES_TYPE CVCalcParser::pow(VALUES_TYPE valLeftOperand, VALUES_TYPE valRightOperand)
		throw(CMathParameterMustBePositiveException) {
	if ((valLeftOperand == 0.) && (valRightOperand < 0.)) {
		throw CMathParameterMustBePositiveException(this->m_iCurrentIndex);
	}
	if ((valLeftOperand == 0.) && (valRightOperand == 0.)) {
		this->m_strWarningMsg = "Warning: 0^0 replaced by 1";
		this->m_bWarningFlag  = true;
		return 1.;
	}
	return ::pow(valLeftOperand, valRightOperand);
}


/**
 * @brief Returns the reminder of the first operand out of the second operand.
 * @param valLeftOperand Numerator of the division of which the reminder is calculated.
 * @param valRightOperand Denominator of the division of which the reminder is calculated.
 * @return Reminder of the first operand out of the second operand.
 * @throw CDivisionByZeroException
 *     If the right operand is equal to 0.
 *
 * This functions calculates the reminder of the first operand out of the second operand.
 */
VALUES_TYPE CVCalcParser::mod(VALUES_TYPE valLeftOperand, VALUES_TYPE valRightOperand)
		throw(CDivisionByZeroException) {
	if (valRightOperand == 0.) {
		throw CDivisionByZeroException(this->m_iCurrentIndex);
	}
	return ::fmod(valLeftOperand, valRightOperand);
}


/**
 * @brief Returns the incremental sum of the expression.
 * @param expr Expression of which the sum is to be avaluated.
 * @param var Variable in the expression which varies by increments.
 * @param low The integral lower bound of the sum.
 * @param high The integral higher bound of the sum.
 * @return the incremental sum of the expression.
 * @throw CFunctionNotSupportedException
 *     The function is not implemented yet.
 * @warning Not implemented yet.
 *
 * This functions calculates the incremental sum of the expression in which the variable
 *   takes integral values from the lower to the higher bound :<BR>
 *   \f$ \sum_x=low^high (expr(x))\f$
 */
VALUES_TYPE CVCalcParser::sum(std::string expr, std::string var, VALUES_TYPE low, VALUES_TYPE high)
		throw(CFunctionNotSupportedException) {
	throw CFunctionNotSupportedException("sum", this->m_iCurrentIndex);
	// Supposed behavior :
	//    VALUES_TYPE vResult = 0;
	//    for (var = low; var < high; var++) {
	//        vResult += expr(var);
	//    }
	return (VALUES_TYPE)0;
}


/**
 * @brief Returns the incremental product of the expression.
 * @param expr Expression of which the product is to be avaluated.
 * @param var Variable in the expression which varies by increments.
 * @param low The integral lower bound of the sum.
 * @param high The integral higher bound of the sum.
 * @return the incremental product of the expression.
 * @throw CFunctionNotSupportedException
 *     The function is not implemented yet.
 * @warning Not implemented yet.
 *
 * This functions calculates the incremental product of the expression in which the variable
 *   takes integral values from the lower to the higher bound :<BR>
 *   \f$ \product_x=low^high (expr(x))\f$
 */
VALUES_TYPE CVCalcParser::product(std::string expr, std::string var, VALUES_TYPE low, VALUES_TYPE high)
		throw(CFunctionNotSupportedException) {
	throw CFunctionNotSupportedException("product", this->m_iCurrentIndex);
	// Supposed behavior :
	//    VALUES_TYPE vResult = 0;
	//    for (var = low; var < high; var++) {
	//        vResult *= expr(var);
	//    }
	return (VALUES_TYPE)0;
}


/**
 * @brief Returns the result of the parsed formula.
 * @param strSource Formula to parse.
 * @return The result of formula parsed.
 * @throw CVCalcParserException
 *     Whatever exception inherited from VisualCalc Parser Exceptions base class.
 *
 * This function calculates the value of the formula passed in the strSource parameter.<BR>
 * If an error occured during the calculation, a meaningful exception is thrown describing
 *   the condition why this happend, and where it happened in the formula.<BR>
 * If the calculation succeed, the formula and its associated result are pushed in the
 *   Answers history.
 */
VARIANT CVCalcParser::Evaluate(const std::string& strSource)
		throw(CVCalcParserException) {
	this->ResetParserMembers(strSource);
	try {
		VARIANT valResult = this->Level_1();
		AnswerItem AnsItem;
		AnsItem.m_strFormula = this->m_strSource;
		AnsItem.m_valResult  = valResult;
		this->m_dqeAnswersHistory.push_front(AnsItem);
		return valResult;
	}
	catch (CVCalcParserException) {
		throw;
	}
	catch (...) {
		throw CUnknownException(0);
	}
}


/**
 * @brief Computes the 'addition' and 'subtraction' operators.
 * @return The result of the current expression parsed.
 * @throw CVCalcParserException
 *     Whatever exception inherited from VisualCalc Parser Exceptions base class.
 * @see Level_2(), Level_3(), Level_4(), Level_5(), Primary(), GetToken()
 *
 * This function is the first of 5 of the recursive descending levels of the Parser.<BR>
 * It computes the following operators :
 *     - Addition    : &lt;expr&gt; + &lt;expr&gt;
 *     - Subtraction : &lt;expr&gt; - &lt;expr&gt;<BR>
 *
 * If the current operator is not in the ones below,
 *   then the result of Level_2() is returned.
 */
VARIANT CVCalcParser::Level_1(void)
		throw(CVCalcParserException) {
	VARIANT valLeftOperand = this->Level_2();
	while (true) {
		
		switch (this->m_tokCurrentToken) {
		case TV_PLUS:
			// Addition : 'expr + expr'
			{
				if( valLeftOperand.vt == VT_BSTR )
				{
					CString tmp = valLeftOperand.bstrVal;
					valLeftOperand.vt = VT_I8;
					valLeftOperand.dblVal = atof(tmp);
				}

				VARIANT var = this->Level_2();
				if( var.vt == VT_BSTR )
				{
					CString tmp = var.bstrVal;
					var.vt = VT_I8;
					var.dblVal = atof(tmp);
				}			

				valLeftOperand.dblVal += var.dblVal;
			}
			break;

		case TV_MINUS:
			// Subtraction : 'expr - expr'
			{
				if( valLeftOperand.vt == VT_BSTR )
				{
					CString tmp = valLeftOperand.bstrVal;
					valLeftOperand.vt = VT_I8;
					valLeftOperand.dblVal = atof(tmp);
				}

				VARIANT var = this->Level_2();
				if( var.vt == VT_BSTR )
				{
					CString tmp = var.bstrVal;
					var.vt = VT_I8;
					var.dblVal = atof(tmp);
				}				
				
				valLeftOperand.dblVal -= var.dblVal;
			}
			break;

		case TV_ASSIGN:
			// Assignation : 'expr = expr'
			throw CLiteralAssignationException(this->m_iCurrentIndex);

		default:
			return valLeftOperand;
		}
	}
}


/**
 * @brief Computes the 'multiplication' and 'division' operators.
 * @return The result of the current expression parsed.
 * @throw CVCalcParserException
 *     Whatever exception inherited from VisualCalc Parser Exceptions base class.
 * @see Level_1(), Level_3(), Level_4(), Level_5(), Primary(), GetToken()
 *
 * This function is the second of 5 of the recursive descending levels of the Parser.<BR>
 * It computes the following operators :
 *     - Multiplication : &lt;expr&gt; * &lt;expr&gt;
 *     - Division       : &lt;expr&gt; / &lt;expr&gt;<BR>
 *
 * If the current operator is not in the ones below,
 *   then the result of Level_3() is returned.<BR>
 * If the Parser tries to resolve a division which dividend is equal to 0,
 *   then the relative exception is thrown.
 */
VARIANT CVCalcParser::Level_2(void)
		throw(CVCalcParserException) {
	VARIANT valLeftOperand = this->Level_3();
	while (true) {
		
		switch (this->m_tokCurrentToken) {
		case TV_MUL:
			// Multiplication : 'expr * expr'
			{
				if( valLeftOperand.vt == VT_BSTR )
				{
					CString tmp = valLeftOperand.bstrVal;
					valLeftOperand.vt = VT_I8;
					valLeftOperand.dblVal = atof(tmp);
				}
				VARIANT var = this->Level_2();
				if( var.vt == VT_BSTR )
				{
					CString tmp = var.bstrVal;
					var.vt = VT_I8;
					var.dblVal = atof(tmp);
				}
				valLeftOperand.dblVal *= var.dblVal;
			}
			break;

		case TV_DIV:
			// Division : 'expr / expr'
			{
				if( valLeftOperand.vt == VT_BSTR )
				{
					CString tmp = valLeftOperand.bstrVal;
					valLeftOperand.vt = VT_I8;
					valLeftOperand.dblVal = atof(tmp);
				}

				VARIANT var = this->Level_2();
				if( var.vt == VT_BSTR )
				{
					CString tmp = var.bstrVal;
					var.vt = VT_I8;
					var.dblVal = atof(tmp);
				}

				if (var.dblVal != 0.0) {
					valLeftOperand.dblVal /= var.dblVal;
				}
				else {
					throw CDivisionByZeroException(this->m_iCurrentIndex);
				}
			}
			break;

		default:
			return valLeftOperand;
		}
	}
}


/**
 * @brief Computes the 'power' operator.
 * @return The result of the current expression parsed.
 * @throw CVCalcParserException
 *     Whatever exception inherited from VisualCalc Parser Exceptions base class.
 * @see Level_1(), Level_2(), Level_4(), Level_5(), Primary(), GetToken()
 * @see CVCalcParser::pow()
 *
 * This function is the third of 5 of the recursive descending levels of the Parser.<BR>
 * It computes the following operators :
 *     - Power : &lt;expr&gt; ^ &lt;expr&gt;<BR>
 *
 * If the current operator is not in the ones below,
 *   then the result of Level_4() is returned.<BR>
 * In the case of a Power evaluation, the parameters read from the input stream are
 *   passed to the pow() member function, which can result in an exception
 *   if the conditions required for this calculation are not filled.
 */
VARIANT CVCalcParser::Level_3(void)
		throw(CVCalcParserException) {
	VARIANT valLeftOperand = this->Level_4();
	while (true) {
		switch(this->m_tokCurrentToken) {
		case TV_POW:
			// Power : 'expr ^ expr'
			{
				if( valLeftOperand.vt == VT_BSTR )
				{
					CString tmp = valLeftOperand.bstrVal;
					valLeftOperand.vt = VT_I8;
					valLeftOperand.dblVal = atof(tmp);
				}
				VARIANT var = this->Level_3();
				if( var.vt == VT_BSTR )
				{
					CString tmp = var.bstrVal;
					var.vt = VT_I8;
					var.dblVal = atof(tmp);
				}
				valLeftOperand.dblVal = this->pow(valLeftOperand.dblVal, var.dblVal);
			}
			break;

		default:
			return valLeftOperand;
		}
	}
}


/**
 * @brief Computes the 'modulus' operator.
 * @return The result of the current expression parsed.
 * @throw CVCalcParserException
 *     Whatever exception inherited from VisualCalc Parser Exceptions base class.
 * @see Level_1(), Level_2(), Level_3(), Level_5(), Primary(), GetToken()
 * @see CVCalcParser::mod()
 *
 * This function is the fourth of 5 of the recursive descending levels of the Parser.<BR>
 * It computes the following operators :
 *     - Modulus : &lt;expr&gt; % &lt;expr&gt;<BR>
 *
 * If the current operator is not in the ones below,
 *   then the result of Level_5() is returned.<BR>
 * In the case of a Modulus evaluation, the parameters read from the input stream are
 *   passed to the mod() member function, which can result in an exception
 *   if the conditions required for this calculation are not filled.
 */
VARIANT CVCalcParser::Level_4(void)
		throw(CVCalcParserException) {
	VARIANT valLeftOperand = this->Level_5();
	while (true) {
		switch (this->m_tokCurrentToken) {
		case TV_MOD:
			// Modulus : 'expr % expr'
			{
				if( valLeftOperand.vt == VT_BSTR )
				{
					CString tmp = valLeftOperand.bstrVal;
					valLeftOperand.vt = VT_I8;
					valLeftOperand.dblVal = atof(tmp);
				}
				VARIANT var = this->Level_3();
				if( var.vt == VT_BSTR )
				{
					CString tmp = var.bstrVal;
					var.vt = VT_I8;
					var.dblVal = atof(tmp);
				}
				valLeftOperand.dblVal = this->mod(valLeftOperand.dblVal, var.dblVal);
			}
			break;

		default:
			return valLeftOperand;
		}
	}
}


/**
 * @brief Computes the 'factorial' and the "degree to radians" operators.
 * @return The result of the current expression parsed.
 * @throw CVCalcParserException
 *     Whatever exception inherited from VisualCalc Parser Exceptions base class.
 * @see Level_1(), Level_2(), Level_3(), Level_4(), Primary(), GetToken()
 * @see CVCalcParser::ffactor(), CVCalcParser::rad()
 *
 * This function is the fifth of 5 of the recursive descending levels of the Parser.<BR>
 * It computes the following operators :
 *     - Factorial         : &lt;expr&gt; !
 *     - Degree to radians : &lt;expr&gt; ?BR>
 *
 * If the current operator is not in the ones below,
 *   then the result of Primary() is returned.<BR>
 * In the case of a factorial evaluation, the only parameter read from the input stream is
 *   passed to the ffactor() member function.<BR>
 * In the case of a Degree to radians evaluation,
 *   the only parameter read from the input stream is passed to the rad() member function.<BR>
 * Both cases can result in an exception if the conditions required
 *   for this calculation are not filled.
 */
VARIANT CVCalcParser::Level_5(void)
		throw(CVCalcParserException) {
	VARIANT valLeftOperand = this->Primary();
	while (true) {
		switch (this->m_tokCurrentToken) {
		case TV_NUMBER:
			this->GetToken();
			break;

		case TV_FACT:
			// Factorial : 'expr !'
			this->GetToken();
			if( valLeftOperand.vt == VT_BSTR )
			{
				CString tmp = valLeftOperand.bstrVal;
				valLeftOperand.vt = VT_I8;
				valLeftOperand.dblVal = atof(tmp);
			}
			valLeftOperand.dblVal = this->ffactor(valLeftOperand.dblVal);
			break;

		case TV_DEG:
			// Degree to radian : 'expr?
			this->GetToken();
			if( valLeftOperand.vt == VT_BSTR )
			{
				CString tmp = valLeftOperand.bstrVal;
				valLeftOperand.vt = VT_I8;
				valLeftOperand.dblVal = atof(tmp);
			}
			valLeftOperand.dblVal = this->rad(valLeftOperand.dblVal);
			break;

		default:
			return valLeftOperand;
		}
	}
}


/**
 * @brief Computes the 'parenthesis', 'Unary +' and 'Unary -' operators.
 * @return The result of the current expression parsed.
 * @throw CVCalcParserException
 *     Whatever exception inherited from VisualCalc Parser Exceptions base class.
 * @see Level_1(), Level_2(), Level_3(), Level_4(), Level_5(), GetToken()
 * @see CVCalcParser::ffactor(), CVCalcParser::rad()
 *
 * This function intervenes after the 5th level function of the Parser.<BR>
 * It computes the following operators :
 *     - Parenthesis : ( &lt;expr&gt; )
 *     - Unary +     : + &lt;expr&gt;
 *     - Unary -     : - &lt;expr&gt;<BR>
 *
 * It also perform the following additional actions :
 *     - Calls to the user functions
 *     - Assignations/Recoverage of user variables<BR>
 *
 * If the current operator is not in the ones below, then an exception is
 *   thrown saying that a mathematic expression is expected.<BR>
 * In the case of a user function handling, then the parameters read from the input 
 *   are passed to the relative member function (each function presented in the
 *   user interface have its own private implementation within the parser class).<BR>
 * In the case of a user-defined variable handling, then either the data is retrieved
 *   if the variable aleady exists, otherwise it is assigned with a new value.<BR>
 * All cases can result in an exception if the conditions required
 *   for the handling are not filled.
 */
VARIANT CVCalcParser::Primary(void)
		throw(CVCalcParserException) {
	this->GetToken();
	switch (this->m_tokCurrentToken) {
	case TV_NUMBER:
		{
			VARIANT var;
			var.vt = VT_I8;
			var.dblVal = this->m_valNumberValue;

			return var;
		}
		

	case TV_IDENTIFIER:
		{
			std::string strIdentifierName = this->m_strIdentifierValue;
			std::list<std::string>::iterator IterList =
				std::find(this->m_lstFunctions.begin(),
						  this->m_lstFunctions.end(),
						  strIdentifierName
						 );
			if (IterList != this->m_lstFunctions.end()) {
				// The identifier is a function name
				if (this->GetToken() != TV_LP) {
					throw CIllegalUseOfFonctionNameException(strIdentifierName, this->m_iCurrentIndex);
				}
				else {
					VARIANT valFunctionResult;
					if (strIdentifierName == "abs") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->abs(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "Acos") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->Acos(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "Ans") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->Ans(this->Level_1().dblVal).m_valResult.dblVal;
					}
					else if (strIdentifierName == "Asin") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->Asin(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "Atan") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->Atan(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "cos") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->cos(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "cosh") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->cosh(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "deg") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->deg(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "exp") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->exp(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "ln") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->ln(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "log") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->log(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "row") {
						valFunctionResult = this->row(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "col") {
						valFunctionResult = this->col(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "logn") {
						VALUES_TYPE valFirstParameter = this->Level_1().dblVal;
						if (this->m_tokCurrentToken != TV_SEQ) {
							throw CTooFewParametersException(this->m_iCurrentIndex);
						}
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->logn(valFirstParameter, this->Level_1().dblVal);
					}
					else if (strIdentifierName == "cell") {
						long valFirstParameter = this->Level_1().dblVal;
						if (this->m_tokCurrentToken != TV_SEQ) {
							throw CTooFewParametersException(this->m_iCurrentIndex);
						}
						valFunctionResult = this->cell(valFirstParameter, this->Level_1().dblVal);
					}
					else if (strIdentifierName == "nAp") {
						VALUES_TYPE valFirstParameter = this->Level_1().dblVal;
						if (this->m_tokCurrentToken != TV_SEQ) {
							throw CTooFewParametersException(this->m_iCurrentIndex);
						}
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->nAp(valFirstParameter, this->Level_1().dblVal);
					}
					else if (strIdentifierName == "nCp") {
						VALUES_TYPE valFirstParameter = this->Level_1().dblVal;
						if (this->m_tokCurrentToken != TV_SEQ) {
							throw CTooFewParametersException(this->m_iCurrentIndex);
						}
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->nCp(valFirstParameter, this->Level_1().dblVal);
					}
					else if (strIdentifierName == "product") {
						VALUES_TYPE valFirstParameter = this->Level_1().dblVal;
						if (this->m_tokCurrentToken != TV_SEQ) {
							throw CTooFewParametersException(this->m_iCurrentIndex);
						}
						VALUES_TYPE valSecondParameter = this->Level_1().dblVal;
						if (this->m_tokCurrentToken != TV_SEQ) {
							throw CTooFewParametersException(this->m_iCurrentIndex);
						}
						VALUES_TYPE valThirdParameter = this->Level_1().dblVal;
						if (this->m_tokCurrentToken != TV_SEQ) {
							throw CTooFewParametersException(this->m_iCurrentIndex);
						}
						
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->product("",
														  "",
														  valThirdParameter,
														  this->Level_1().dblVal);
					}
					else if (strIdentifierName == "rad") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->rad(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "sin") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->sin(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "sinh") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->sinh(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "sqrt") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->sqrt(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "sqrtn") {
						VALUES_TYPE valFirstParameter = this->Level_1().dblVal;
						if (this->m_tokCurrentToken != TV_SEQ) {
							throw CTooFewParametersException(this->m_iCurrentIndex);
						}
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->sqrtn(valFirstParameter, this->Level_1().dblVal);
					}
					else if (strIdentifierName == "sum") {
						VALUES_TYPE valFirstParameter = this->Level_1().dblVal;
						if (this->m_tokCurrentToken != TV_SEQ) {
							throw CTooFewParametersException(this->m_iCurrentIndex);
						}
						VALUES_TYPE valSecondParameter = this->Level_1().dblVal;
						if (this->m_tokCurrentToken != TV_SEQ) {
							throw CTooFewParametersException(this->m_iCurrentIndex);
						}
						VALUES_TYPE valThirdParameter = this->Level_1().dblVal;
						if (this->m_tokCurrentToken != TV_SEQ) {
							throw CTooFewParametersException(this->m_iCurrentIndex);
						}
						
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->sum("",
													  "",
													  valThirdParameter,
													  this->Level_1().dblVal);
					}
					else if (strIdentifierName == "tan") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->tan(this->Level_1().dblVal);
					}
					else if (strIdentifierName == "tanh") {
						valFunctionResult.vt = VT_I8;
						valFunctionResult.dblVal = this->tanh(this->Level_1().dblVal);
					}
					else {
						throw CUnknownFunctionCalledException(strIdentifierName, this->m_iCurrentIndex);
					}
					if (m_tokCurrentToken != TV_RP) {
						if (m_tokCurrentToken == TV_SEQ) {
							throw CTooManyParametersException(this->m_iCurrentIndex);
						}
						else {
							throw CClosingParenthesisExpectedException(this->m_iCurrentIndex);
						}
					}
					this->GetToken();
					return valFunctionResult;
				}
			}
			else {
				// The identifier is not a function name but a user-defined variable
				this->GetToken();
				if (this->m_tokCurrentToken == TV_LP) {
					if (this->m_mapVariables.find(strIdentifierName) == this->m_mapVariables.end()) {
						// The identifier isn't either a variable and is
						//   called like a function, with a '()' operator.
						throw CUnknownFunctionCalledException(strIdentifierName, this->m_iCurrentIndex);
					}
					else {
						// The variable exists but is called like
						//   a function, with a '()' operator.
						throw CImplicitMultiplicationException(strIdentifierName, this->m_iCurrentIndex);
					}
				}
				
				VARIANT valResult;
				// The user assigns a value to the variable
				if (this->m_tokCurrentToken == TV_ASSIGN) 
				{
					if ((strIdentifierName == "pi") || (strIdentifierName == "e")) 
					{
						throw CConstantAssignationException(strIdentifierName, this->m_iCurrentIndex);
					}
					valResult = this->Level_1();
					// Adds the variable in the map if not existing yet
					this->m_mapVariables[strIdentifierName] = valResult.dblVal;
				}
				else {
					// The identifier is neither a function nor a variable
					if (this->m_mapVariables.find(strIdentifierName) == this->m_mapVariables.end()) {
						throw CUndefinedVariableException(strIdentifierName, this->m_iCurrentIndex);
					}
					valResult.vt = VT_I8;
					valResult.dblVal = this->m_mapVariables[strIdentifierName];
				}
				return valResult;
			}
		}
		break;

	case TV_PLUS:
		// Unary Plus '+expr'
		{
			VARIANT var;
			var = this->Primary();
			if( var.vt == VT_BSTR )
			{
				CString tmp = var.bstrVal;
				var.vt = VT_I8;
				var.dblVal = atof(tmp);
			}
			return var;
		}

	case TV_MINUS:
		// Unary Minus '-expr'
		{
			VARIANT var;
			var = this->Primary();
			if( var.vt == VT_BSTR )
			{
				CString tmp = var.bstrVal;
				var.vt = VT_I8;
				var.dblVal = atof(tmp);
			}
			var.dblVal *= -1;
			return var;
		}

	case TV_LP:
		// Opening parenthesis '('
		{
			VARIANT valExpression = this->Level_1();
			if (this->m_tokCurrentToken != TV_RP) {
				throw CClosingParenthesisExpectedException(this->m_iCurrentIndex);
			}
			this->GetToken();
			return valExpression;
		}

	default:
		throw CMathematicExpressionExpectedException(this->m_iCurrentIndex);
	}
}


/**
 * @brief Reads the tokens from the input stream.
 * @return The token extracted from the input stream.
 * @throw CVCalcParserException
 *     Whatever exception inherited from VisualCalc Parser Exceptions base class.
 * @see Level_1(), Level_2(), Level_3(), Level_4(), Level_5(), Primary()
 *
 * The main purpose of this function is to read the input stream character by
 *   character to extract the relative tokens.<BR>
 * The white spaces between 2 tokens are not taken in account and don't change
 *   at all the meaning of the formula parsed.<BR>
 *
 * It recognizes the following tokens :
 *     - Operators   : Whatever operator stored in TokenValue
 *     - Numbers     : Whenever a digit or a period ('.') is read
 *     - Identifiers : When a letter or an underscore  ('_') is read<BR>
 *
 * Depending on the cases above, the relative token is stored whether in
 *   m_tokCurrentToken for any operator, m_valNumberValue for numbers and
 *   m_strIdentifierValue for identifiers.
 *
 * If an unknown character is read or if a number is being extracted with more than
 *   1 period character ('.'), then a relative exception is thrown.
 */
TokenValue CVCalcParser::GetToken(void)
		throw(CVCalcParserException) {
	if (this->m_strSource.empty()) {
		return this->m_tokCurrentToken = TV_END;
	}
	if (this->m_bEndEncountered) {
		return this->m_tokCurrentToken = TV_END;
	}
	// Eats the white spaces...
	while (::isspace(this->m_strSource[this->m_iCurrentIndex])) {
		if (this->m_iCurrentIndex <= this->m_strSource.length()-1) {
			if (this->StepIndexForward()) {
				return this->m_tokCurrentToken = TV_END;
			}
		}
		/*
		else {
			return m_tokCurrentToken = TV_END;
		}
		*/
	}
	unsigned char ch = this->m_strSource[m_iCurrentIndex];
	this->StepIndexForward();
	switch (ch) {
	case '\0':
	case ';':
		// End of the expression
		return this->m_tokCurrentToken = TV_END;

	case ',':
	case '*':
	case '/':
	case '+':
	case '-':
	case '^':
	case '%':
	case '!':
	case '?':
	case '(':
	case ')':
	case '=':
		// A token is found on the input stream
		return this->m_tokCurrentToken = (TokenValue)ch;
		
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		// A digit is found on the input stream...
		{
			int iStartPos = this->m_iCurrentIndex - 1;
			if (!this->m_bEndEncountered) {
				// A valid number must contain at most one period ('.')
				bool bDotAlreadyThere = ( (this->m_strSource[iStartPos] == '.') ? true : false );
				while ( (!this->m_bEndEncountered) &&
						((::isdigit(this->m_strSource[this->m_iCurrentIndex])) ||
						 (this->m_strSource[this->m_iCurrentIndex] == '.')) ) {
					if (this->m_strSource[m_iCurrentIndex] == '.') {
						if (bDotAlreadyThere) {
							throw CPeriodUnexpectedException(this->m_iCurrentIndex);
						}
						else {
							bDotAlreadyThere = true;
						}
					}
					if (this->StepIndexForward()) {
						break;
					}
				}
			}
			// Extracts all the digits which compound the number
			const std::string strNumber = this->m_strSource.substr  (iStartPos,
																	 (this->m_iCurrentIndex - iStartPos)
																	).c_str();
			const char* pszStrNumber = strNumber.c_str();
			this->m_valNumberValue = ::atof(pszStrNumber);
			return this->m_tokCurrentToken = TV_NUMBER;
		}

	case '_':
	case 'a':	case 'A':
	case 'b':	case 'B':
	case 'c':	case 'C':
	case 'd':	case 'D':
	case 'e':	case 'E':
	case 'f':	case 'F':
	case 'g':	case 'G':
	case 'h':	case 'H':
	case 'i':	case 'I':
	case 'j':	case 'J':
	case 'k':	case 'K':
	case 'l':	case 'L':
	case 'm':	case 'M':
	case 'n':	case 'N':
	case 'o':	case 'O':
	case 'p':	case 'P':
	case 'q':	case 'Q':
	case 'r':	case 'R':
	case 's':	case 'S':
	case 't':	case 'T':
	case 'u':	case 'U':
	case 'v':	case 'V':
	case 'w':	case 'W':
	case 'x':	case 'X':
	case 'y':	case 'Y':
	case 'z':	case 'Z':
	
		// A letter is found on the input stream...
		{
			int iStartPos = this->m_iCurrentIndex - 1;
			if (!this->m_bEndEncountered) {
				while ( (!this->m_bEndEncountered) &&
						((::isalnum(this->m_strSource[this->m_iCurrentIndex])) ||
						 (this->m_strSource[this->m_iCurrentIndex] == '_')) ) {
					if (this->StepIndexForward()) {
						break;
					}
				}
			}
			// Extracts all the characters which compound the identifier
			this->m_strIdentifierValue = this->m_strSource.substr(iStartPos,
																  (this->m_iCurrentIndex - iStartPos)
																 );
			return this->m_tokCurrentToken = TV_IDENTIFIER;
		}

	default:
		this->m_tokCurrentToken = TV_END;
		throw CUnexpectedCharacterException(ch, this->m_iCurrentIndex);
	}
}


/**
 * @brief Steps the stream reading index of one position further.
 * @return A flag saying if the end of the formula was reached.
 *
 * This function was made to factorize some code of the GetToken() function.<BR>
 * It increments the index on the stream and set the m_bEndEncountered flag to
 *   true if the end of the formula is encountered.
 */
bool CVCalcParser::StepIndexForward(void) {
	this->m_iCurrentIndex++;
	if (m_iCurrentIndex > m_strSource.length() - 1) {
		return m_bEndEncountered = true;
	}
	return false;
}

void CVCalcParser::TraceGrid(int nRow, int nCol)
{
	this->m_mapVariables["rowid"] = nRow;
	this->m_mapVariables["colid"] = nCol;

	m_nRowId = nRow;
	m_nColId = nCol;
}
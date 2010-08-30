/**
 * @file VCalcParserException.cpp
 * @brief VisualCalc Parser exceptions implementations.
 *
 * Implements the constructors of the classes defined for the Parser exceptions.
 */

#include "stdafx.h"
#include "VCalcParserException.h"


/**
 * @brief Constructor of the CVCalcParserException class.
 * @param enExceptionNumber The number of the error, according to the ExceptionNumbers enumeration.
 * @param strExceptionMsg The description message of the error.
 * @param iErrorPos The position where the error occured in the formula typed by the user to be parsed.
 *
 * The constructor is set as a protected member to make the class abstract.<BR>
 * It must not be possible to instantiate the class ;
 *   it only provides an interface for the inheritance hierarchy.
 */
CVCalcParserException::CVCalcParserException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos) {
	this->m_enExceptionNumber = enExceptionNumber;
	this->m_strExceptionMsg	  = strExceptionMsg;
	this->m_iErrorPos		  = iErrorPos;
}

/**
 * @brief Destructor of the CVCalcParserException class.
 *
 * As there is no dynamically allocated memory within a parser exception,
 *   the destructor is quite useless here.<BR>
 * Moreover, it is set <I>virtual</I> for an inheritance purpose.
 */
CVCalcParserException::~CVCalcParserException() {
	this->m_enExceptionNumber = ENB_NONE;
	this->m_strExceptionMsg	  = "";
	this->m_iErrorPos		  = -1;
}

/**
 * @brief Returns the code number of an exception.
 * @return the exception code number corresponding to the real exception thrown.
 *
 * This function is from the public interface of the exceptions base class.<BR>
 * It returns the correct ExceptionNumbers value corresponding to the exception that is thrown.<BR>
 * Such a number is no longer hard-coded through a constructor parameter.
 *   Each Exception class constructor now internaly initializes this value.
 */
CVCalcParserException::ExceptionNumbers CVCalcParserException::GetExceptionNumber() {
	return this->m_enExceptionNumber;
}

/**
 * @brief Returns the description message of an exception.
 * @return the exception description message corresponding to the exception thrown.
 *
 * This function is from the public interface of the exceptions base class.<BR>
 * It returns the correct descrption message corresponding to the exception that is thrown.<BR>
 * Such a message is templated, whose text is internaly initialized by each Exception class constructor.
 *   Sometimes, messages may have to insert an additional information to complete the description.
 *   Such an information is passed as parameter to the parameter.
 */
std::string CVCalcParserException::GetMessage() {
	return this->m_strExceptionMsg;
}

/**
 * @brief Returns the error position of an exception.
 * @return the error position for the exception thrown.
 *
 * This function is from the public interface of the exceptions base class.<BR>
 * It returns the error position where the exception occured.<BR>
 * Each Exception class constructor always initializes this value from a received parameter.
 */
long CVCalcParserException::GetErrorPos() {
	return this->m_iErrorPos;
}



/************************************************************************/
/* CSyntaxException derived classes                                     */
/************************************************************************/

/**
 * @brief Constructor of the Syntax errors category.
 * @param enExceptionNumber The exception code number.
 * @param strExceptionMsg The exception description message.
 * @param iErrorPos The position of the error.
 *
 * The constructor only get the parameters and pass them to the base class constructor through the initializer list.<BR>
 * The constructor is set as a protected member to make the class abstract.<BR>
 * It must not be possible to instantiate the class.
 */
CSyntaxException::CSyntaxException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos)
		: CVCalcParserException(enExceptionNumber, strExceptionMsg, iErrorPos) {
}

/**
 * @brief Destructor of the Syntax exceptions category.
 */
CSyntaxException::~CSyntaxException() {
}

/**
 * @brief Constructor for an Unexpected Character exception
 * @param cCharacter Character which is unexpected.
 * @param iErrorPos The position of the error.
 *
 * Creates an Unexpected Character exception.
 */
CUnexpectedCharacterException::CUnexpectedCharacterException(char cCharacter, int iErrorPos) 
		: CSyntaxException(ENB_SYN_UNEXPECTED_CHARACTER,
						   std::string("Unexpected character '") + cCharacter + "'",
						   iErrorPos) {
}

/**
 * @brief Destructor of an Unexpected Character exception.
 */
CUnexpectedCharacterException::~CUnexpectedCharacterException() {
}


/**
 * @brief Constructor for a Period Character Unexpected exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Period Character Unexpected exception.
 */
CPeriodUnexpectedException::CPeriodUnexpectedException(int iErrorPos)
		: CSyntaxException(ENB_SYN_DIGIT_EXPECTED,
						   "Period character unexpected ('.')",
						   iErrorPos) {
}

/**
 * @brief Destructor of a Period Character Unexpected exception.
 */
CPeriodUnexpectedException::~CPeriodUnexpectedException() {
}


/**
 * @brief Constructor for a Closing Parenthesis Expected exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Closing Parenthesis Expected exception.
 */
CClosingParenthesisExpectedException::CClosingParenthesisExpectedException(int iErrorPos)
		: CSyntaxException(ENB_SYN_CLOSING_PARENTHESIS_EXPECTED,
						   "')' expected",
						   iErrorPos) {
}

/**
 * @brief Destructor of a Closing Parenthesis Expected exception.
 */
CClosingParenthesisExpectedException::~CClosingParenthesisExpectedException() {
}


/**
 * @brief Constructor for a Mathematic Expression Expected exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Mathematic Expression Expected exception.
 */
CMathematicExpressionExpectedException::CMathematicExpressionExpectedException(int iErrorPos)
		: CSyntaxException(ENB_SYN_MATHEMATIC_EXPRESSION_EXPECTED,
						   "Mathematic expression expected",
						   iErrorPos) {
}

/**
 * @brief Destructor of a Mathematic Expression Expected exception.
 */
CMathematicExpressionExpectedException::~CMathematicExpressionExpectedException() {
}


/************************************************************************/
/* CMathematicException derived classes                                 */
/************************************************************************/

/**
 * @brief Constructor of the Mathematic errors category.
 * @param enExceptionNumber The exception code number.
 * @param strExceptionMsg The exception description message.
 * @param iErrorPos The Position of the error.
 *
 * The constructor only get the parameters and pass them to the base class constructor through the initializer list.<BR>
 * The constructor is set as a protected member to make the class abstract.<BR>
 * It must not be possible to instantiate the class.
 */
CMathematicException::CMathematicException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos)
		: CVCalcParserException(enExceptionNumber, strExceptionMsg, iErrorPos) {
}

/**
 * @brief Destructor of the Mathematic exceptions category.
 */
CMathematicException::~CMathematicException() {
}


/**
 * @brief Constructor for a Division by Zero exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Division by Zero exception.
 */
CDivisionByZeroException::CDivisionByZeroException(int iErrorPos)
		: CMathematicException(ENB_MTH_DIVISION_BY_ZERO,
							   "Division by 0",
							   iErrorPos) {
}

/**
 * @brief Destructor of a Division by Zero exception.
 */
CDivisionByZeroException::~CDivisionByZeroException() {
}


/**
 * @brief Constructor for a N Must Be Positive exception
 * @param iErrorPos The position of the error.
 *
 * Creates a N Must Be Positive exception.
 */
CNMustBePositiveException::CNMustBePositiveException(int iErrorPos)
		: CMathematicException(ENB_MTH_N_MUST_BE_POSITIVE,
							   "'n' must be positive",
							   iErrorPos) {
}

/**
 * @brief Destructor of a N Must Be Positive exception.
 */
CNMustBePositiveException::~CNMustBePositiveException() {
}


/**
 * @brief Constructor for a P Must Be Positive exception
 * @param iErrorPos The position of the error.
 *
 * Creates a P Must Be Positive exception.
 */
CPMustBePositiveException::CPMustBePositiveException(int iErrorPos)
		: CMathematicException(ENB_MTH_P_MUST_BE_POSITIVE,
							   "'p' must be positive",
							   iErrorPos) {
}

/**
 * @brief Destructor of a P Must Be Positive exception.
 */
CPMustBePositiveException::~CPMustBePositiveException() {
}


/**
 * @brief Constructor for a P Must Be Smaller than N exception
 * @param iErrorPos The position of the error.
 *
 * Creates a P Must Be Smaller than N exception.
 */
CPMustBeSmallerThanNException::CPMustBeSmallerThanNException(int iErrorPos)
		: CMathematicException(ENB_MTH_P_MUST_BE_SMALLER_THAN_N,
							   "'p' must be smaller than 'n'",
							   iErrorPos) {
}

/**
 * @brief Destructor of a P Must Be Smaller than N exception.
 */
CPMustBeSmallerThanNException::~CPMustBeSmallerThanNException() {
}


/**
 * @brief Constructor for a Factorial Is Defined For Positive Integers exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Factorial Is Defined For Positive Integers exception.
 */
CFactorialDefForPositiveIntsException::CFactorialDefForPositiveIntsException(int iErrorPos)
		: CMathematicException(ENB_MTH_FACTORIAL_DEF_FOR_POSITIVE_INTS,
							   "A factorial is defined for positive integers",
							   iErrorPos) {
}

/**
 * @brief Destructor of a Factorial Is Defined For Positive Integers exception.
 */
CFactorialDefForPositiveIntsException::~CFactorialDefForPositiveIntsException() {
}


/**
 * @brief Constructor for a Parameter Out Of Range exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Parameter Out Of Range exception.
 */
CParameterOutOfRangeException::CParameterOutOfRangeException(int iErrorPos)
		: CMathematicException(ENB_MTH_PARAMETER_OUT_OF_RANGE,
							   "Parameter out of range",
							   iErrorPos) {
}

/**
 * @brief Destructor of a Parameter Out Of Range exception.
 */
CParameterOutOfRangeException::~CParameterOutOfRangeException() {
}


/**
 * @brief Constructor for a Parameter Must Be Positive exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Parameter Must Be Positive exception.
 */
CMathParameterMustBePositiveException::CMathParameterMustBePositiveException(int iErrorPos)
		: CMathematicException(ENB_MTH_PARAMETER_MUST_BE_POSITIVE,
							   "Parameter must be positive",
							   iErrorPos) {
}

/**
 * @brief Destructor of a Parameter Must Be Positive exception.
 */
CMathParameterMustBePositiveException::~CMathParameterMustBePositiveException() {
}

//////////////////////////////////////////////////////////////////////////
// 张军增加
CGridParameterMustBeUnInt::CGridParameterMustBeUnInt(int iErrorPos)
		: CMathematicException(ENB_GRID_PARAMETERS_MUST_BE_UNINT,
							   "行列参数必须是大于－1的数！",
							   iErrorPos) {
}

/**
 * @brief Destructor of a Parameter Must Be Positive exception.
 */
CGridParameterMustBeUnInt::~CGridParameterMustBeUnInt() {
}


CGridParameterNotExist::CGridParameterNotExist(int iErrorPos)
		: CMathematicException(ENB_GRID_PARAMETERS_GRID_NOT_EXIST,
							   "表格没有设置",
							   iErrorPos) {
}

/**
 * @brief Destructor of a Parameter Must Be Positive exception.
 */
CGridParameterNotExist::~CGridParameterNotExist() {
}


/**
 * @brief Constructor for a Parameter Must Be An Integer exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Parameter Must Be An Integer exception.
 */
CMathParameterMustBeAnIntegerException::CMathParameterMustBeAnIntegerException(int iErrorPos)
		: CMathematicException(ENB_MTH_PARAMETER_MUST_BE_AN_INTEGER,
							   "Parameter must be an integer",
							   iErrorPos) {
}

/**
 * @brief Destructor of a Parameter Must Be An Integer exception.
 */
CMathParameterMustBeAnIntegerException::~CMathParameterMustBeAnIntegerException() {
}



/************************************************************************/
/* CFunctionException derived classes                                   */
/************************************************************************/

/**
 * @brief Constructor of the Functions errors category.
 * @param enExceptionNumber The exception code number.
 * @param strExceptionMsg The exception description message.
 * @param iErrorPos The Position of the error.
 *
 * The constructor only get the parameters and pass them to the base class constructor through the initializer list.<BR>
 * The constructor is set as a protected member to make the class abstract.<BR>
 * It must not be possible to instantiate the class.
 */
CFunctionException::CFunctionException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos)
		: CVCalcParserException(enExceptionNumber, strExceptionMsg, iErrorPos) {
}

/**
 * @brief Destructor of the Functions exceptions category.
 */
CFunctionException::~CFunctionException() {
}


/**
 * @brief Constructor for an Unknown Function Called exception
 * @param strFunctionName Name of the function.
 * @param iErrorPos The position of the error.
 *
 * Creates an Unknown Function Called exception.
 */
CUnknownFunctionCalledException::CUnknownFunctionCalledException(const std::string& strFunctionName, int iErrorPos)
		: CFunctionException(ENB_FCT_UNKNOWN_FUNCTION_CALLED,
							 "Unknown function called ('" + strFunctionName + "')",
							 iErrorPos) {
}

/**
 * @brief Destructor of an Unknown Function Called exception.
 */
CUnknownFunctionCalledException::~CUnknownFunctionCalledException() {
}


/**
 * @brief Constructor for an Implicit Multiplication Not Supported exception
 * @param strVariableName Name of the variable.
 * @param iErrorPos The position of the error.
 *
 * Creates an Implicit Multiplication Not Supported exception.
 */
CImplicitMultiplicationException::CImplicitMultiplicationException(const std::string& strVariableName, int iErrorPos)
		: CFunctionException(ENB_FCT_IMPLICIT_MULTIPLICATION,
							 "Implicit multiplication not supported ('" + strVariableName + "')",
							 iErrorPos) {
}

/**
 * @brief Destructor of an Implicit Multiplication Not Supported exception.
 */
CImplicitMultiplicationException::~CImplicitMultiplicationException() {
}


/**
 * @brief Constructor for an Illegal Use Of Function Name exception
 * @param strFunctionName Name of the function.
 * @param iErrorPos The position of the error.
 *
 * Creates an Illegal Use Of Function Name exception.
 */
CIllegalUseOfFonctionNameException::CIllegalUseOfFonctionNameException(const std::string& strFunctionName, int iErrorPos)
		: CFunctionException(ENB_FCT_ILLEGAL_USE_OF_FUNCTION_NAME,
							 "Illegal use of function name ('" + strFunctionName + "')",
							 iErrorPos) {
}

/**
 * @brief Destructor of an Illegal Use Of Function Name exception.
 */
CIllegalUseOfFonctionNameException::~CIllegalUseOfFonctionNameException() {
}



/************************************************************************/
/* CParameterException derived classes                                  */
/************************************************************************/

/**
 * @brief Constructor of the Parameters errors category.
 * @param enExceptionNumber The exception code number.
 * @param strExceptionMsg The exception description message.
 * @param iErrorPos The Position of the error.
 *
 * The constructor only get the parameters and pass them to the base class constructor through the initializer list.<BR>
 * The constructor is set as a protected member to make the class abstract.<BR>
 * It must not be possible to instantiate the class.
 */
CParameterException::CParameterException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos)
		: CVCalcParserException(enExceptionNumber, strExceptionMsg, iErrorPos) {
}

/**
 * @brief Destructor of the Parameters exceptions category.
 */
CParameterException::~CParameterException() {
}


/**
 * @brief Constructor for a Too Few Parameters exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Too Few Parameters exception.
 */
CTooFewParametersException::CTooFewParametersException(int iErrorPos)
		: CParameterException(ENB_PRM_TOO_FEW_PARAMETERS,
							  "Too few parameters. ',' expected",
							  iErrorPos) {
}

/**
 * @brief Destructor of a Too Few Parameters exception.
 */
CTooFewParametersException::~CTooFewParametersException() {
}


/**
 * @brief Constructor for a Too Many Parameters exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Too Many Parameters exception.
 */
CTooManyParametersException::CTooManyParametersException(int iErrorPos)
		: CParameterException(ENB_PRM_TOO_MANY_PARAMETERS,
							  "Too many parameters. ')' expected",
							  iErrorPos) {
}

/**
 * @brief Destructor of a Too Many Parameters exception.
 */
CTooManyParametersException::~CTooManyParametersException() {
}



/************************************************************************/
/* CVariableException derived classes                                   */
/************************************************************************/

/**
 * @brief Constructor of the Variables errors category.
 * @param enExceptionNumber The exception code number.
 * @param strExceptionMsg The exception description message.
 * @param iErrorPos The Position of the error.
 *
 * The constructor only get the parameters and pass them to the base class constructor through the initializer list.<BR>
 * The constructor is set as a protected member to make the class abstract.<BR>
 * It must not be possible to instantiate the class.
 */
CVariableException::CVariableException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos)
		: CVCalcParserException(enExceptionNumber, strExceptionMsg, iErrorPos) {
}

/**
 * @brief Destructor of the Variables exceptions category.
 */
CVariableException::~CVariableException() {
}


/**
 * @brief Constructor for a Cannot Assign A Constant exception
 * @param strConstantName Name of the constant.
 * @param iErrorPos The position of the error.
 *
 * Creates a Cannot Assign A Constant exception.
 */
CConstantAssignationException::CConstantAssignationException(const std::string& strConstantName, int iErrorPos)
		: CVariableException(ENB_VAR_CONSTANTASSIGNATION,
							 "Cannot assign a constant ('" + strConstantName + "')",
							 iErrorPos) {
}

/**
 * @brief Destructor of a Cannot Assign A Constant exception.
 */
CConstantAssignationException::~CConstantAssignationException() {
}


/**
 * @brief Constructor for a Cannot Assign a Literal exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Cannot Assign a Literal exception.
 */
CLiteralAssignationException::CLiteralAssignationException(int iErrorPos)
		: CVariableException(ENB_VAR_LITERALASSIGNATION,
							 "Cannot assign a literal",
							 iErrorPos) {
}

/**
 * @brief Destructor of a Cannot Assign a Literal exception.
 */
CLiteralAssignationException::~CLiteralAssignationException() {
}


/**
 * @brief Constructor for an Undefined Variable exception
 * @param strVariableName Name of the variable.
 * @param iErrorPos The position of the error.
 *
 * Creates an Undefined Variable exception.
 */
CUndefinedVariableException::CUndefinedVariableException(const std::string& strVariableName, int iErrorPos)
		: CVariableException(ENB_VAR_UNDEFINED_VARIABLE,
							 "Undefined variable '" + strVariableName + "'",
							 iErrorPos) {
}

/**
 * @brief Destructor of an Undefined Variable exception.
 */
CUndefinedVariableException::~CUndefinedVariableException() {
}



/************************************************************************/
/* CDomainException derived classes                                     */
/************************************************************************/

/**
 * @brief Constructor of the Domain errors category.
 * @param enExceptionNumber The exception code number.
 * @param strExceptionMsg The exception description message.
 * @param iErrorPos The Position of the error.
 *
 * The constructor only get the parameters and pass them to the base class constructor through the initializer list.<BR>
 * The constructor is set as a protected member to make the class abstract.<BR>
 * It must not be possible to instantiate the class.
 */
CDomainException::CDomainException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos)
		: CVCalcParserException(enExceptionNumber, strExceptionMsg, iErrorPos) {
}

/**
 * @brief Destructor of the Domain errors category.
 */
CDomainException::~CDomainException() {
}


/**
 * @brief Constructor for a The Answer Requested Was Not Found exception
 * @param iErrorPos The position of the error.
 *
 * Creates a The Answer Requested Was Not Found exception.
 */
CAnswerRequestedNotFoundException::CAnswerRequestedNotFoundException(int iErrorPos)
		: CDomainException(ENB_DMN_ANSWER_REQUESTED_NOT_FOUND,
						   "The answer requested was not found",
						   iErrorPos) {
}

/**
 * @brief Destructor of a The Answer Requested Was Not Found exception.
 */
CAnswerRequestedNotFoundException::~CAnswerRequestedNotFoundException() {
}


/**
 * @brief Constructor for a Parameter Must Be Positive exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Parameter Must Be Positive exception.
 */
CDomParameterMustBePositiveException::CDomParameterMustBePositiveException(int iErrorPos)
		: CDomainException(ENB_DMN_PARAMETER_MUST_BE_POSITIVE,
						   "Parameter must be positive",
						   iErrorPos) {
}

/**
 * @brief Destructor of a Parameter Must Be Positive exception.
 */
CDomParameterMustBePositiveException::~CDomParameterMustBePositiveException() {
}


/**
 * @brief Constructor for a Parameter Must Be An Integer exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Parameter Must Be An Integer exception.
 */
CDomParameterMustBeAnIntegerException::CDomParameterMustBeAnIntegerException(int iErrorPos)
		: CDomainException(ENB_DMN_PARAMETER_MUST_BE_AN_INTEGER,
						   "Parameter must be an integer",
						   iErrorPos) {
}

/**
 * @brief Destructor of a Parameter Must Be An Integer exception.
 */
CDomParameterMustBeAnIntegerException::~CDomParameterMustBeAnIntegerException() {
}


/**
 * @brief Constructor for a Parameters Must Be Positive exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Parameters Must Be Positive exception.
 */
CDomParametersMustBePositiveException::CDomParametersMustBePositiveException(int iErrorPos)
		: CDomainException(ENB_DMN_PARAMETERS_MUST_BE_POSITIVE,
						   "Parameters must be positive",
						   iErrorPos) {
}

/**
 * @brief Destructor of a Parameters Must Be Positive exception.
 */
CDomParametersMustBePositiveException::~CDomParametersMustBePositiveException() {
}


/**
 * @brief Constructor for a Parameters Must Be Integers exception
 * @param iErrorPos The position of the error.
 *
 * Creates a Parameters Must Be Integers exception.
 */
CDomParametersMustBeIntegersException::CDomParametersMustBeIntegersException(int iErrorPos)
		: CDomainException(ENB_DMN_PARAMETERS_MUST_BE_INTEGERS,
						   "Parameters must be integers",
						   iErrorPos) {
}

/**
 * @brief Destructor of a Parameters Must Be Integers exception.
 */
CDomParametersMustBeIntegersException::~CDomParametersMustBeIntegersException() {
}



/************************************************************************/
/* CParserException derived classes                                     */
/************************************************************************/

/**
 * @brief Constructor of the Parser errors category.
 * @param enExceptionNumber The exception code number.
 * @param strExceptionMsg The exception description message.
 * @param iErrorPos The Position of the error.
 *
 * The constructor only get the parameters and pass them to the base class constructor through the initializer list.<BR>
 * The constructor is set as a protected member to make the class abstract.<BR>
 * It must not be possible to instantiate the class.
 */
CParserException::CParserException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos)
		: CVCalcParserException(enExceptionNumber, strExceptionMsg, iErrorPos) {
}

/**
 * @brief Destructor of the Parser exceptions category.
 */
CParserException::~CParserException() {
}


/**
 * @brief Constructor for a Function Not Supported Yet Warning exception.
 * @param strFunctionName Name of the Function.
 * @param iErrorPos The position of the error.
 *
 * Creates a Function Not Supported Yet Warning exception.
 */
CFunctionNotSupportedException::CFunctionNotSupportedException(const std::string& strFunctionName, int iErrorPos)
		: CParserException(ENB_PRS_FUNCTION_NOT_SUPPORTED,
						   "Warning: Function not supported yet ('" + strFunctionName + "')",
						   iErrorPos) {
}

/**
 * @brief Destructor of a Function Not Supported Yet Warning exception.
 */
CFunctionNotSupportedException::~CFunctionNotSupportedException() {
}


/**
 * @brief Constructor for an Unknown Exception exception
 * @param iErrorPos The position of the error.
 *
 * Creates an Unknown Exception exception.
 */
CUnknownException::CUnknownException(int iErrorPos)
		: CParserException(ENB_PRS_UNKNOWN_EXCEPTION,
						   "Unknown exception",
						   iErrorPos) {
}

/**
 * @brief Destructor of an Unknown Exception exception.
 */
CUnknownException::~CUnknownException() {
}

/**
 * @file VCalcParserException.h
 * @brief VisualCalc Parser exceptions definitions.
 *
 * Defines the types that are thrown by the Parser as exceptions.<BR>
 * Every exception inherit from CVCalcParserException. 
 *   Then, each exception category is factorized with its parent.<BR>
 * Only the "younger" child classes in the exceptions hierarchy can be
 *   instanciated, as the parents are abstract classes.
 */

#if !defined(__AFX_VCALCPARSEREXCEPTION_H_INCLUDED__)
#define		 __AFX_VCALCPARSEREXCEPTION_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif


/***************************************************************
 *  VisualCalc Parser Exceptions hierarchy                     *
 * ----------------------------------------------------------- *
 *  - CVCalcParserException                                    *
 *      - CSyntaxException                                     *
 *          - CUnexpectedCharacterException                    *
 *          - CPeriodUnexpectedException                       *
 *          - CClosingParenthesisExpectedException             *
 *          - CMathematicExpressionExpectedException           *
 *      - CMathematicException                                 *
 *          - CDivisionByZeroException                         *
 *          - CNMustBePositiveException                        *
 *          - CPMustBePositiveException                        *
 *          - CPMustBeSmallerThanNException                    *
 *          - CFactorialDefForPositiveIntsException            *
 *          - CParameterOutOfRangeException                    *
 *          - CMathParameterMustBePositiveException            *
 *          - CMathParameterMustBeAnIntegerException           *
 *      - CFunctionException                                   *
 *          - CUnknownFunctionCalledException                  *
 *          - CImplicitMultiplicationException                 *
 *          - CIllegalUseOfFonctionNameException               *
 *      - CParameterException                                  *
 *          - CTooFewParametersException                       *
 *          - CTooManyParametersException                      *
 *      - CVariableException                                   *
 *          - CConstantAssignationException                    *
 *          - CLiteralAssignationException                     *
 *          - CUndefinedVariableException                      *
 *      - CDomainException                                     *
 *          - CAnswerRequestedNotFoundException                *
 *          - CDomParameterMustBePositiveException             *
 *          - CDomParameterMustBeAnIntegerException            *
 *          - CDomParametersMustBePositiveException            *
 *          - CDomParametersMustBeIntegersException            *
 *      - CParserException                                     *
 *          - CFunctionNotSupportedException                   *
 *          - CUnknownException                                *
 ***************************************************************/


#include <STRING>	// std::string


/**
 * @class CVCalcParserException
 * @brief Base class for VisualCalc Parser Exceptions.
 * @see CSyntaxException,
 *		CMathematicException,
 *		CFunctionException,
 *		CParameterException,
 *		CVariableException,
 *		CDomainException,
 *		CParserException
 *
 * Every exception category inherit from CVCalcParserException.<BR>
 * This abstract base class provides :
 *     - a uniform error codes numbering (ExceptionNumbers),
 *     - a function to get the error description message (getMessage()),
 *     - a function to get the error code number (getExceptionNumber()),
 *     - a function to get the position in the formula where the error occured (getErrorPos()).
 */
class AFX_EXT_CLASS CVCalcParserException {
protected:
	/**
	 * @enum ExceptionNumbers
	 * @brief Uniformizes the exceptions numbers.
	 *
	 * This enumeration provides an error code for each exception thrown by the Parser.<BR>
	 * It is used by the instanciable derived classes.
	 */
	enum ExceptionNumbers {
		ENB_SYN_UNEXPECTED_CHARACTER			= 1001,	/**< Code for the <I>Syntax error : Unexpected character</I>.				*/
		ENB_SYN_DIGIT_EXPECTED					= 1002,	/**< Code for the <I>Syntax error : Digit expected</I>.						*/
		ENB_SYN_CLOSING_PARENTHESIS_EXPECTED	= 1003,	/**< Code for the <I>Syntax error : Closing parenthesis expected</I>.		*/
		ENB_SYN_MATHEMATIC_EXPRESSION_EXPECTED	= 1004,	/**< Code for the <I>Syntax error : Mathematic expression expected</I>.		*/
		ENB_MTH_DIVISION_BY_ZERO				= 2001,	/**< Code for the <I>Mathematic error : Division by zero</I>.				*/
		ENB_MTH_N_MUST_BE_POSITIVE				= 2002,	/**< Code for the <I>Mathematic error : N must be positive</I>.				*/
		ENB_MTH_P_MUST_BE_POSITIVE				= 2003,	/**< Code for the <I>Mathematic error : P must be positive</I>.				*/
		ENB_MTH_P_MUST_BE_SMALLER_THAN_N		= 2004,	/**< Code for the <I>Mathematic error : P must be smaller than N</I>.		*/
		ENB_MTH_FACTORIAL_DEF_FOR_POSITIVE_INTS	= 2005,	/**< Code for the <I>Mathematic error : Factorial defined for positive integers</I>.	*/
		ENB_MTH_PARAMETER_OUT_OF_RANGE			= 2006,	/**< Code for the <I>Mathematic error : Parameter out of range</I>.			*/
		ENB_MTH_PARAMETER_MUST_BE_POSITIVE		= 2007,	/**< Code for the <I>Mathematic error : Parameter must be positive</I>.		*/
		ENB_MTH_PARAMETER_MUST_BE_AN_INTEGER	= 2008,	/**< Code for the <I>Mathematic error : Parameter must be an integer</I>.	*/
		ENB_FCT_UNKNOWN_FUNCTION_CALLED			= 3001,	/**< Code for the <I>Function error : Unkown function called</I>.			*/
		ENB_FCT_IMPLICIT_MULTIPLICATION			= 3002,	/**< Code for the <I>Function error : Implicit Multiplication</I>.			*/
		ENB_FCT_ILLEGAL_USE_OF_FUNCTION_NAME	= 3003,	/**< Code for the <I>Function error : Illegal use of function name</I>.		*/
		ENB_PRM_TOO_FEW_PARAMETERS				= 4001,	/**< Code for the <I>Parameters error : Too few parameters</I>.				*/
		ENB_PRM_TOO_MANY_PARAMETERS				= 4002,	/**< Code for the <I>Parameters error : Too many parameters</I>.			*/
		ENB_VAR_CONSTANTASSIGNATION				= 5001,	/**< Code for the <I>Variable error : Trying to Assigning a constant</I>.	*/
		ENB_VAR_LITERALASSIGNATION				= 5002,	/**< Code for the <I>Variable error : Trying to Assigning a literal</I>.	*/
		ENB_VAR_UNDEFINED_VARIABLE				= 5003,	/**< Code for the <I>Variable error : Undefined Variable</I>.				*/
		ENB_DMN_ANSWER_REQUESTED_NOT_FOUND		= 6001,	/**< Code for the <I>Domain error : Answer requested not found</I>.			*/
		ENB_DMN_PARAMETER_MUST_BE_POSITIVE		= 6002,	/**< Code for the <I>Domain error : Parameter must be positive</I>.			*/
		ENB_DMN_PARAMETER_MUST_BE_AN_INTEGER	= 6003,	/**< Code for the <I>Domain error : Parameter must be an integer</I>.		*/
		ENB_DMN_PARAMETERS_MUST_BE_POSITIVE		= 6004,	/**< Code for the <I>Domain error : Parameters must be positive</I>.		*/
		ENB_DMN_PARAMETERS_MUST_BE_INTEGERS		= 6005,	/**< Code for the <I>Domain error : Parameters must be integers</I>.		*/
		ENB_GRID_PARAMETERS_MUST_BE_UNINT		= 7001,	/**< Code for the <I>Domain error : Parameters must be integers</I>.		*/
		ENB_GRID_PARAMETERS_GRID_NOT_EXIST		= 7002,	/**< Code for the <I>Domain error : Parameters must be integers</I>.		*/

		ENB_PRS_FUNCTION_NOT_SUPPORTED			= 9001,	/**< Code for the <I>Parser error : Function not supported</I>.				*/
		ENB_PRS_UNKNOWN_EXCEPTION				= 9009,	/**< Code for the <I>Parser error : Unknown exception</I>.					*/
		ENB_NONE								= -1	/**< No error. This code is typically used by the destructor to reset the state of the exception.	*/
	};

	ExceptionNumbers	m_enExceptionNumber;	/**< Member that stores the exception number, according to the ExceptionNumbers enumeration.	*/
	std::string			m_strExceptionMsg;		/**< Member that stores the exception description message. 				*/
	long				m_iErrorPos;			/**< Member that stores the position of the error in the source stream.	*/

	CVCalcParserException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos);
	
public:
	virtual ~CVCalcParserException();
	ExceptionNumbers GetExceptionNumber();	// Returns the exception number
	std::string		 GetMessage();			// Returns the exception description message
	long			 GetErrorPos();			// Returns the Position of the error in the input stream
};



/************************************************************************/
/* CSyntaxException derived classes                                     */
/************************************************************************/

/**
 * @class CSyntaxException
 * @brief Parent class for VisualCalc Syntax Exceptions.
 * @see CVCalcParserException
 * @see CUnexpectedCharacterException,
 *		CPeriodUnexpectedException,
 *		CClosingParenthesisExpectedException,
 *		CMathematicExpressionExpectedException
 *
 * This class inherits from CVCalcParserException but remains abstract as it is a base class for every Syntax errors.<BR>
 * Only the derived classes can be instanciated.
 */
class AFX_EXT_CLASS CSyntaxException : public CVCalcParserException {
protected:
	CSyntaxException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos);
	
public:
	virtual ~CSyntaxException();
};


/**
 * @class CUnexpectedCharacterException
 * @brief Exception for when an unexpected character is found.
 * @see CVCalcParserException
 * @see CSyntaxException
 *
 * This exception is thrown when an unexpected character is read on the input stream.<BR>
 *   For instance : <I>2 + $</I><BR><BR>
 * This class inherits from CSyntaxException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception waits for the character in question.
 */
class AFX_EXT_CLASS CUnexpectedCharacterException : public CSyntaxException {
public:
	CUnexpectedCharacterException(char cCharacter, int iErrorPos);
	~CUnexpectedCharacterException();
};


/**
 * @class CPeriodUnexpectedException
 * @brief Exception for when an unexpected period character is found.
 * @see CVCalcParserException
 * @see CSyntaxException
 *
 * This exception is thrown when a period character ('.') is read in a floating point number which already have a period.<BR>
 *   For instance : <I>2.354.12</I><BR><BR>
 * This class inherits from CSyntaxException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CPeriodUnexpectedException : public CSyntaxException {
public:
	CPeriodUnexpectedException(int iErrorPos);
	~CPeriodUnexpectedException();
};


/**
 * @class CClosingParenthesisExpectedException
 * @brief Exception for when a closing parenthesis is missing.
 * @see CVCalcParserException
 * @see CSyntaxException
 *
 * This exception is thrown when a parenthesis has been open without been closed.<BR>
 *   For instance : <I>cos(pi/2</I><BR><BR>
 * This class inherits from CSyntaxException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CClosingParenthesisExpectedException : public CSyntaxException {
public:
	CClosingParenthesisExpectedException(int iErrorPos);
	~CClosingParenthesisExpectedException();
};


/**
 * @class CMathematicExpressionExpectedException
 * @brief Exception for when a mathematic expression was expected.
 * @see CVCalcParserException
 * @see CSyntaxException
 *
 * This exception is thrown when a parenthesis has been open without been closed.<BR>
 *   For instance : <I>7 * </I><BR><BR>
 * This class inherits from CSyntaxException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CMathematicExpressionExpectedException : public CSyntaxException {
public:
	CMathematicExpressionExpectedException(int iErrorPos);
	~CMathematicExpressionExpectedException();
};



/************************************************************************/
/* CMathematicException derived classes                                 */
/************************************************************************/

/**
 * @class CMathematicException
 * @brief Parent class for VisualCalc Mathematic Exceptions.
 * @see CVCalcParserException
 * @see CDivisionByZeroException,
 *		CNMustBePositiveException,
 *		CPMustBePositiveException,
 *		CPMustBeSmallerThanNException,
 *		CFactorialDefForPositiveIntsException,
 *		CParameterOutOfRangeException,
 *		CMathParameterMustBePositiveException,
 *		CMathParameterMustBeAnIntegerException
 *
 * This class inherits from CVCalcParserException but remains abstract as it is a base class for every Mathematic errors.<BR>
 * Only the derived classes can be instanciated.
 */
class AFX_EXT_CLASS CMathematicException : public CVCalcParserException {
protected:
	CMathematicException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos);

public:
	virtual ~CMathematicException();
};


/**
 * @class CDivisionByZeroException
 * @brief Exception for when the user makes a division by 0.
 * @see CVCalcParserException
 * @see CMathematicException
 *
 * This exception is thrown when the dividend of a fraction/division is evaluated to 0.<BR>
 *   For instance : <I>1 / sin(0)</I>, <I>logn(2, 0)</I>, <I>mod(3, 0)</I><BR><BR>
 * This class inherits from CMathematicException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CDivisionByZeroException : public CMathematicException {
public:
	CDivisionByZeroException(int iErrorPos);
	virtual ~CDivisionByZeroException();
};


/**
 * @class CNMustBePositiveException
 * @brief Exception for when 'n' is a negative parameter.
 * @see CVCalcParserException
 * @see CMathematicException
 *
 * This exception is thrown when the 'n' parameter is evaluated to a strictly negative number.<BR>
 *   For instance : <I>nCp(-4, 2)</I>, <I>nAp(-6, 3)</I><BR><BR>
 * This class inherits from CMathematicException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CNMustBePositiveException : public CMathematicException {
public:
	CNMustBePositiveException(int iErrorPos);
	virtual ~CNMustBePositiveException();
};


/**
 * @class CPMustBePositiveException
 * @brief Exception for when 'p' is a negative parameter.
 * @see CVCalcParserException
 * @see CMathematicException
 *
 * This exception is thrown when the 'p' parameter is evaluated to a strictly negative number.<BR>
 *   For instance : <I>nCp(4, -2)</I>, <I>nAp(6, -3)</I><BR><BR>
 * This class inherits from CMathematicException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CPMustBePositiveException : public CMathematicException {
public:
	CPMustBePositiveException(int iErrorPos);
	virtual ~CPMustBePositiveException();
};


/**
 * @class CPMustBeSmallerThanNException
 * @brief Exception for when 'p' is smaller than 'n'.
 * @see CVCalcParserException
 * @see CMathematicException
 *
 * This exception is thrown when the 'p' parameter is evaluated to be higher stricly than 'n'.<BR>
 *   For instance : <I>nCp(-2, 4)</I>, <I>nAp(3, 6)</I><BR><BR>
 * This class inherits from CMathematicException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CPMustBeSmallerThanNException : public CMathematicException {
public:
	CPMustBeSmallerThanNException(int iErrorPos);
	virtual ~CPMustBeSmallerThanNException();
};


/**
 * @class CFactorialDefForPositiveIntsException
 * @brief Exception for when the user asks for a negative factorial.
 * @see CVCalcParserException
 * @see CMathematicException
 *
 * This exception is thrown when the factorial operator ('!') is applied on a strictly negative number.<BR>
 *   For instance : <I>(-1)!</I><BR><BR>
 * This class inherits from CMathematicException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CFactorialDefForPositiveIntsException : public CMathematicException {
public:
	CFactorialDefForPositiveIntsException(int iErrorPos);
	virtual ~CFactorialDefForPositiveIntsException();
};


/**
 * @class CParameterOutOfRangeException
 * @brief Exception for when a parameter is out of range.
 * @see CVCalcParserException
 * @see CMathematicException
 *
 * This exception is thrown when a parameter is out of the range of a function.<BR>
 *   For instance : <I>Acos(2)</I><BR><BR>
 * This class inherits from CMathematicException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CParameterOutOfRangeException : public CMathematicException {
public:
	CParameterOutOfRangeException(int iErrorPos);
	virtual ~CParameterOutOfRangeException();
};


/**
 * @class CMathParameterMustBePositiveException
 * @brief Exception for when a parameter have to be positive.
 * @see CVCalcParserException
 * @see CMathematicException
 *
 * This exception is thrown when a parameter have to be positive (strictly or not, depending on the case).<BR>
 *   For instance : <I>ln(-1)</I>, <I>logn(10, -2)</I>, <I>sqrt(-9)</I><BR><BR>
 * This class inherits from CMathematicException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CMathParameterMustBePositiveException : public CMathematicException {
public:
	CMathParameterMustBePositiveException(int iErrorPos);
	virtual ~CMathParameterMustBePositiveException();
};


// 张军增加
class AFX_EXT_CLASS CGridParameterMustBeUnInt : public CMathematicException {
public:
	CGridParameterMustBeUnInt(int iErrorPos);
	virtual ~CGridParameterMustBeUnInt();
};


class AFX_EXT_CLASS CGridParameterNotExist : public CMathematicException {
public:
	CGridParameterNotExist(int iErrorPos);
	virtual ~CGridParameterNotExist();
};
//////////////////////////////////////////////////////////////////////////

/**
 * @class CMathParameterMustBeAnIntegerException
 * @brief Exception for when a parameter have to be an integer.
 * @see CVCalcParserException
 * @see CMathematicException
 *
 * This exception is thrown when a parameter have to be an integer.<BR>
 *   For instance : <I>2.1!</I>, <I>nCp(10, 3.7)</I>, <I>nAp(4.1, 3)</I><BR><BR>
 * This class inherits from CMathematicException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CMathParameterMustBeAnIntegerException : public CMathematicException {
public:
	CMathParameterMustBeAnIntegerException(int iErrorPos);
	virtual ~CMathParameterMustBeAnIntegerException();
};



/************************************************************************/
/* CFunctionException derived classes                                   */
/************************************************************************/

/**
 * @class CFunctionException
 * @brief Parent class for VisualCalc Function Exceptions.
 * @see CVCalcParserException
 * @see CUnknownFunctionCalledException,
 *		CImplicitMultiplicationException,
 *		CIllegalUseOfFonctionNameException
 *
 * This class inherits from CVCalcParserException but remains abstract as it is a base class for every Function errors.<BR>
 * Only the derived classes can be instanciated.
 */
class AFX_EXT_CLASS CFunctionException : public CVCalcParserException {
protected:
	CFunctionException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos);
	
public:
	virtual ~CFunctionException();
};


/**
 * @class CUnknownFunctionCalledException
 * @brief Exception for when an unknown function is called.
 * @see CVCalcParserException
 * @see CFunctionException
 *
 * This exception is thrown when an unknown function is called.<BR>
 * This happens only if the identifier is neither a variable name nor a known function name.<BR>
 *   For instance : <I>foo( 2 + 2 )</I><BR>
 * (See also CImplicitMultiplicationException).<BR><BR>
 * This class inherits from CFunctionException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception waits for the function name in question.
 */
class AFX_EXT_CLASS CUnknownFunctionCalledException : public CFunctionException {
public:
	CUnknownFunctionCalledException(const std::string& strFunctionName, int iErrorPos);
	virtual ~CUnknownFunctionCalledException();
};


/**
 * @class CImplicitMultiplicationException
 * @brief Exception for when a variable name is used as a function.
 * @see CVCalcParserException
 * @see CFunctionException
 *
 * This exception is thrown when the user uses a variable name as a function.<BR>
 * This happens only if the identifier is an existing variable,
 *   that is assigned with a value, and that has not been deleted yet.<BR>
 *   For instance : <I>myVar = 1</I> then <I>myVar(1)</I><BR>
 * (See also CUnknownFunctionCalledException).<BR><BR>
 * This class inherits from CFunctionException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception waits for the variable name in question.
 */
class AFX_EXT_CLASS CImplicitMultiplicationException : public CFunctionException {
public:
	CImplicitMultiplicationException(const std::string& strVariableName, int iErrorPos);
	virtual ~CImplicitMultiplicationException();
};


/**
 * @class CIllegalUseOfFonctionNameException
 * @brief Exception for when a function name is used an illegal way.
 * @see CVCalcParserException
 * @see CFunctionException
 *
 * This exception is thrown when the user uses a function name without a list of parameters.<BR>
 *   For instance : <I>Ans + 2</I> instead of <I>Ans(1) + 2</I><BR><BR>
 * This class inherits from CFunctionException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception waits for the function name in question.
 */
class AFX_EXT_CLASS CIllegalUseOfFonctionNameException : public CFunctionException {
public:
	CIllegalUseOfFonctionNameException(const std::string& strFunctionName, int iErrorPos);
	virtual ~CIllegalUseOfFonctionNameException();
};



/************************************************************************/
/* CParameterException derived classes                                  */
/************************************************************************/

/**
 * @class CParameterException
 * @brief Parent class for VisualCalc Parameter Exceptions.
 * @see CVCalcParserException
 * @see CTooFewParametersException,
 *		CTooManyParametersException
 *
 * This class inherits from CVCalcParserException but remains abstract as it is a base class for every Parameter errors.<BR>
 * Only the derived classes can be instanciated.
 */
class AFX_EXT_CLASS CParameterException : public CVCalcParserException {
protected:
	CParameterException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos);

public:
	virtual ~CParameterException();
};


/**
 * @class CTooFewParametersException
 * @brief Exception for when a function parameters list is missing some parameters.
 * @see CVCalcParserException
 * @see CParameterException
 *
 * This exception is thrown when not all of the parameters expected are passed.<BR>
 *   For instance : <I>logn(10)</I><BR><BR>
 * This class inherits from CParameterException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CTooFewParametersException : public CParameterException {
public:
	CTooFewParametersException(int iErrorPos);
	virtual ~CTooFewParametersException();
};


/**
 * @class CTooManyParametersException
 * @brief Exception for when a function parameters list has too much parameters.
 * @see CVCalcParserException
 * @see CParameterException
 *
 * This exception is thrown when more than the parameters expected are passed.<BR>
 *   For instance : <I>cos(pi, 4)</I><BR><BR>
 * This class inherits from CParameterException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CTooManyParametersException : public CParameterException {
public:
	CTooManyParametersException(int iErrorPos);
	virtual ~CTooManyParametersException();
};



/************************************************************************/
/* CVariableException derived classes                                   */
/************************************************************************/

/**
 * @class CVariableException
 * @brief Parent class for VisualCalc Variable Exceptions.
 * @see CVCalcParserException
 * @see CConstantAssignationException,
 *		CLiteralAssignationException,
 *		CUndefinedVariableException
 *
 * This class inherits from CVCalcParserException but remains abstract as it is a base class for every Variable errors.<BR>
 * Only the derived classes can be instanciated.
 */
class AFX_EXT_CLASS CVariableException : public CVCalcParserException {
protected:
	CVariableException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos);

public:
	virtual ~CVariableException();
};


/**
 * @class CConstantAssignationException
 * @brief Exception for when the user tries to assign a constant.
 * @see CVCalcParserException
 * @see CVariableException
 *
 * This exception is thrown when the user is trying to assign a value to a constant.<BR>
 *   For instance : <I>pi = 2</I><BR><BR>
 * This class inherits from CVariableException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception waits for the constant name in question.
 */
class AFX_EXT_CLASS CConstantAssignationException : public CVariableException {
public:
	CConstantAssignationException(const std::string& strConstantName, int iErrorPos);
	virtual ~CConstantAssignationException();
};


/**
 * @class CLiteralAssignationException
 * @brief Exception for when the user tries to assign a literal.
 * @see CVCalcParserException
 * @see CVariableException
 *
 * This exception is thrown when the user is trying to assign a value to a literal.<BR>
 *   For instance : <I>5 = 2</I><BR><BR>
 * This class inherits from CVariableException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CLiteralAssignationException : public CVariableException {
public:
	CLiteralAssignationException(int iErrorPos);
	virtual ~CLiteralAssignationException();
};


/**
 * @class CUndefinedVariableException
 * @brief Exception for when the user uses an undefined variable.
 * @see CVCalcParserException
 * @see CVariableException
 *
 * This exception is thrown when the user is using a variable name which don't exist
 *   (either never created or erased).<BR>
 *   For instance : <I>myVar + 2</I><BR><BR>
 * This class inherits from CVariableException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception waits for the variable name in question.
 */
class AFX_EXT_CLASS CUndefinedVariableException : public CVariableException {
public:
	CUndefinedVariableException(const std::string& strVariableName, int iErrorPos);
	virtual ~CUndefinedVariableException();
};



/************************************************************************/
/* CDomainException derived classes                                     */
/************************************************************************/

/**
 * @class CDomainException
 * @brief Parent class for VisualCalc Domain Exceptions.
 * @see CVCalcParserException
 * @see CAnswerRequestedNotFoundException,
 *		CDomParameterMustBePositiveException,
 *		CDomParameterMustBeAnIntegerException,
 *		CDomParametersMustBePositiveException,
 *		CDomParametersMustBeIntegersException
 *
 * This class inherits from CVCalcParserException but remains abstract as it is a base class for every Domain errors.<BR>
 * Only the derived classes can be instanciated.
 */
class AFX_EXT_CLASS CDomainException : public CVCalcParserException {
protected:
	CDomainException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos);

public:
	virtual ~CDomainException();
};


/**
 * @class CAnswerRequestedNotFoundException
 * @brief Exception for when an answer requested cannot be found.
 * @see CVCalcParserException
 * @see CDomainException
 *
 * This exception is thrown when the <I>Ans()</I> function is called with an invalid parameter
 *   (index of the answer is higher than the number of answers stored in the history).<BR>
 *   For instance : <I>Ans(5000)</I> (when only 3 answers are known)<BR><BR>
 * This class inherits from CDomainException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CAnswerRequestedNotFoundException : public CDomainException {
public:
	CAnswerRequestedNotFoundException(int iErrorPos);
	virtual ~CAnswerRequestedNotFoundException();
};


/**
 * @class CDomParameterMustBePositiveException
 * @brief Exception for when a parameter must be positive.
 * @see CVCalcParserException
 * @see CDomainException
 *
 * This exception is thrown when a function of the parser (not a mathematic one) is called with a negative parameter.<BR>
 *   For instance : <I>Ans(-2)</I><BR><BR>
 * This class inherits from CDomainException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CDomParameterMustBePositiveException : public CDomainException {
public:
	CDomParameterMustBePositiveException(int iErrorPos);
	virtual ~CDomParameterMustBePositiveException();
};


/**
 * @class CDomParameterMustBeAnIntegerException
 * @brief Exception for when a parameter must be an integer.
 * @see CVCalcParserException
 * @see CDomainException
 *
 * This exception is thrown when a function of the parser (not a mathematic one) is called with a floating point parameter.<BR>
 *   For instance : <I>Ans(1.7)</I><BR><BR>
 * This class inherits from CDomainException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CDomParameterMustBeAnIntegerException : public CDomainException {
public:
	CDomParameterMustBeAnIntegerException(int iErrorPos);
	virtual ~CDomParameterMustBeAnIntegerException();
};


/**
 * @class CDomParametersMustBePositiveException
 * @brief Exception for when several parameters of a function must be positive.
 * @see CVCalcParserException
 * @see CDomainException
 *
 * This exception is thrown when several parameters of a function of the parser (not a mathematic one) must be positive.<BR>
 *   Currently, this exception is not thrown from nowhere in the VisualCalc Parser.<BR><BR>
 * This class inherits from CDomainException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CDomParametersMustBePositiveException : public CDomainException {
public:
	CDomParametersMustBePositiveException(int iErrorPos);
	virtual ~CDomParametersMustBePositiveException();
};


/**
 * @class CDomParametersMustBeIntegersException
 * @brief Exception for when several parameters of a function must be integers.
 * @see CVCalcParserException
 * @see CDomainException
 *
 * This exception is thrown when several parameters of a function of the parser (not a mathematic one) must be integers.<BR>
 *   Currently, this exception is not thrown from nowhere in the VisualCalc Parser.<BR><BR>
 * This class inherits from CDomainException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CDomParametersMustBeIntegersException : public CDomainException {
public:
	CDomParametersMustBeIntegersException(int iErrorPos);
	virtual ~CDomParametersMustBeIntegersException();
};



/************************************************************************/
/* CParserException derived classes                                     */
/************************************************************************/

/**
 * @class CParserException
 * @brief Parent class for VisualCalc general Exceptions.
 * @see CVCalcParserException
 * @see CFunctionNotSupportedException,
 *		CUnknownException
 *
 * This class inherits from CVCalcParserException but remains abstract as it is a base class for general errors.<BR>
 * Only the derived classes can be instanciated.
 */
class AFX_EXT_CLASS CParserException : public CVCalcParserException {
protected:
	CParserException(ExceptionNumbers enExceptionNumber, const std::string& strExceptionMsg, int iErrorPos);

public:
	virtual ~CParserException();
};


/**
 * @class CFunctionNotSupportedException
 * @brief Exception for when a function called is not supported yet.
 * @see CVCalcParserException
 * @see CParserException
 *
 * This exception is thrown when a the user is referring to a function not implemented yet.<BR>
 *   For instance : <I>product(x^2-2x, x, 1, 5)</I><BR><BR>
 * This class inherits from CParserException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception waits for the function name in question.
 */
class AFX_EXT_CLASS CFunctionNotSupportedException : public CParserException {
public:
	CFunctionNotSupportedException(const std::string& strFunctionName, int iErrorPos);
	virtual ~CFunctionNotSupportedException();
};


/**
 * @class CUnknownException
 * @brief Exception for for an unknown internal crash.
 * @see CVCalcParserException
 * @see CParserException
 *
 * This exception is thrown when a the user is referring to a function not implemented yet.<BR>
 *   Currently, this exceptionis not thrown from nowhere in the VisualCalc Parser.<BR><BR>
 * This class inherits from CParserException and is at a level lower enough for being instanciated.<BR>
 * The description message for such an exception don't need additional parameter to be formated.
 */
class AFX_EXT_CLASS CUnknownException : public CParserException {
public:
	CUnknownException(int iErrorPos);
	virtual ~CUnknownException();
};


#endif // !defined(__AFX_VCALCPARSEREXCEPTION_H_INCLUDED__)

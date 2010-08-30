/**
 * @file VCalcParserTypes.h
 * @brief VisualCalc Parser Types.
 *
 * Defines some types used by the VisualCalc Parser.
 */

#if !defined(__AFX_VCALCPARSERTYPES_H_INCLUDED__)
#define		 __AFX_VCALCPARSERTYPES_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif


#include "VCalcParserException.h"


/**
 * @typedef VALUES_TYPE
 * @brief Type of the results.
 *
 * Type defined to handle operands and results.
 */
typedef	long double VALUES_TYPE;
//typedef	VARIANT  VALUES_TYPE;


/**
 * @enum TokenValue
 * @brief Type of the Tokens.
 *
 * Type defined for recognized separation tokens on the input stream.
 */
typedef enum {
	TV_NUMBER,			/**< If a number is read (integer of floating point)		*/
	TV_IDENTIFIER,		/**< If an identifier is read (variable or function)		*/
	TV_END,				/**< If the end of the expression was reached				*/
	TV_SEQ		= ',',	/**< If a '<B>,</B>' is read (function parameters list)		*/
	TV_PLUS		= '+',	/**< If a '<B>+</B>' is read (Addition or unary plus)		*/
	TV_MINUS	= '-',	/**< If a '<B>-</B>' is read (Subtraction or unary minus)	*/
	TV_MUL		= '*',	/**< If a '<B>*</B>' is read (multiplication)				*/
	TV_DIV		= '/',	/**< If a '<B>/</B>' is read (Division)						*/
	TV_POW		= '^',	/**< If a '<B>^</B>' is read (Power)						*/
	TV_MOD		= '%',	/**< If a '<B>%</B>' is read (Modulus)						*/
	TV_FACT		= '!',	/**< If a '<B>!</B>' is read (Factorial)					*/
	TV_DEG		= '?',	 /**< If a '<B>?/B>' is read (degrees to radians)			*/
	TV_LP		= '(',	/**< If a '<B>(</B>' is read (opening parenthesis)			*/
	TV_RP		= ')',	/**< If a '<B>)</B>' is read (closing parenthesis)			*/
	TV_ASSIGN	= '='	/**< If a '<B>=</B>' is read (Assignation)					*/
} TokenValue;


/**
 * @struct AnswerItem
 * @brief Type for an entry in the answers history.
 *
 * Type defined to store a formula typed by the user (which resulted in a valid answer)
 *   and its relating value.<br>
 * An invalid formula (which by definition throws an error) isn't stored until the user
 *   corrects his mistake.
 */
typedef struct {
	std::string	m_strFormula;
	VARIANT	m_valResult;
} AnswerItem;


#endif // !defined(__AFX_VCALCPARSERTYPES_H_INCLUDED__)

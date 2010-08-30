#if !defined(__AFX_VCALCPARSER_H_INCLUDED__)
#define		 __AFX_VCALCPARSER_H_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif

// Identifier was truncated to '255' characters in the debug information
#pragma warning (disable: 4786)

#include <ALGORITHM>	// std::find()
#include <STRING>		// std::string
#include <LIST>			// std::list<>
#include <MAP>			// std::map<>
#include <DEQUE>		// std::deque<>
#include <CMATH>		// ::abs(), ::acos(), ::asin(), ::atan(), ::cos(),   ::cosh()
						// ::exp(), ::fabs(), ::floor() ::log(),  ::log10(), ::fmod()
						// ::pow(), ::sin(),  ::sinh(), ::sqrt(), ::tan(),   ::tanh()
#include "VCalcParserTypes.h"


/**
 * @class CVCalcParser
 * @brief VisualCalc Parser.
 *
 * This is the VisualCalc Parser class which take a formula as input,
 *   and returns either the floating point result or an error describing
 *   what is wrong in the string to parse.<BR>
 *
 * The VisualCalc Parser is able to make the following actions, because of
 *   its data members :
 *     - know what is the current Token during the parsing process,
 *     - know depending on the case either the Function/Variable/Constant name,
 *         the number value or the current operator,
 *     - raise a warning flag (non-blocking error during parsing),
 *     - store the mathematic functions defined to be used in the formula to be parsed,
 *     - store each variable name associated to its stored value,
 *     - store each formula correctly parsed and the related result.
 *
 * There is a public interface compound of :
 *     - the constructor/Desctructor,
 *     - the Functions/Variables/Answers lists handling member functions,
 *     - the Warnings handling member functions.
 *     - the Evaluate() member function, which launches the parsing process.<BR>
 *
 * All the other member functions are private :
 *     - the mathematic functions presented as the functions that can be
 *         used in the formula to be parsed,
 *     - the 5 level functions and the 2 low level functions that actually
 *         make the parsing process.
 */
class AFX_EXT_CLASS CVCalcParser 
{
public:
	CGridCtrl *m_pWndGrid;
	void TraceGrid(int nRow, int nCol);
private:
	// Mathematic constants
	const VALUES_TYPE	m_PI;	/**< 32 digits <I>pi</I> value : 3.1415926535897932384626433832795 */
	const VALUES_TYPE	m_E;	/**< 32 digits <I>e</I> value :  2.7182818284590452353602874713527 */
	
	// 张军增加
	int   m_nRowId;
	int   m_nColId;

	// Members for parsing treatment
	TokenValue					m_tokCurrentToken;		/**< Current token extracted from the input stream		*/
	std::string					m_strSource;			/**< Reference to the input stream						*/
	std::string					m_strIdentifierValue;	/**< Identifier extracted as an IDENTIFIER token		*/
	std::string					m_strWarningMsg;		/**< To describe the warning							*/
	VALUES_TYPE					m_valNumberValue;		/**< Number extracted as a NUMBER token					*/
	bool						m_bWarningFlag;			/**< Non-interrupting low level message					*/
	bool						m_bEndEncountered;		/**< The end of the input stream is reached				*/
	int							m_iCurrentIndex;		/**< Index in the input stream							*/
	std::list<std::string>		m_lstFunctions;			/**< Supported functions								*/
	std::map<std::string, VALUES_TYPE>
								m_mapVariables;			/**< User defined variables table mapping the identifier with a value	*/
	std::deque<AnswerItem>		m_dqeAnswersHistory;	/**< Answers History mapping a formula with a result	*/
	std::string					m_strParserVersion;		/**< Version of the Parser								*/

	// Locally defined/redefined mathematic functions
	VALUES_TYPE	ffactor	(VALUES_TYPE);
	VALUES_TYPE	nCp		(VALUES_TYPE, VALUES_TYPE);
	VALUES_TYPE	nAp		(VALUES_TYPE, VALUES_TYPE);
	AnswerItem	Ans		(VALUES_TYPE valIndex);

	VALUES_TYPE abs		(VALUES_TYPE);
	VALUES_TYPE cos		(VALUES_TYPE);
	VALUES_TYPE sin		(VALUES_TYPE);
	VALUES_TYPE tan		(VALUES_TYPE);
	VALUES_TYPE cosh	(VALUES_TYPE);
	VALUES_TYPE sinh	(VALUES_TYPE);
	VALUES_TYPE tanh	(VALUES_TYPE);
	VALUES_TYPE Acos	(VALUES_TYPE);
	VALUES_TYPE Asin	(VALUES_TYPE);
	VALUES_TYPE Atan	(VALUES_TYPE);
	VALUES_TYPE deg		(VALUES_TYPE);
	VALUES_TYPE rad		(VALUES_TYPE);
	VALUES_TYPE exp		(VALUES_TYPE);
	VALUES_TYPE ln		(VALUES_TYPE);
	VALUES_TYPE log		(VALUES_TYPE);
	VALUES_TYPE logn	(VALUES_TYPE, VALUES_TYPE);
	VALUES_TYPE sqrt	(VALUES_TYPE);
	VALUES_TYPE sqrtn	(VALUES_TYPE, VALUES_TYPE);
	VALUES_TYPE pow		(VALUES_TYPE, VALUES_TYPE);
	VALUES_TYPE mod		(VALUES_TYPE, VALUES_TYPE);
	VALUES_TYPE sum		(std::string expr, std::string var, VALUES_TYPE low, VALUES_TYPE high);
	VALUES_TYPE product	(std::string expr, std::string var, VALUES_TYPE low, VALUES_TYPE high);

	VARIANT		row		(int);
	VARIANT		col		(int);
	VARIANT		cell	(int, int);

public:
	CVCalcParser();
	virtual ~CVCalcParser();
	void ResetParserMembers(const std::string);
	void ResetFunctions();
	void ResetVariables();
	void ResetAnswersHistory();
	const std::list<std::string>&				GetFunctions();
	const std::map<std::string, VALUES_TYPE>&	GetVariables();
	const std::deque<AnswerItem>&				GetAnswersHistory();
	bool HasWarning();
	std::string GetWarningMsg();
	std::string GetVersion();

	//Parsing functions following (in the recursive-descending order)...
	VARIANT Evaluate(const std::string& Source) throw(CVCalcParserException);

private:
	VARIANT	Level_1			(void);		// + , -
	VARIANT	Level_2			(void);		// * , /
	VARIANT	Level_3			(void);		// ^
	VARIANT	Level_4			(void);		// %
	VARIANT	Level_5			(void);		// ! , ?
	VARIANT	Primary			(void);		// ( ) , Unary + , Unary -
	TokenValue	GetToken		(void);
	bool		StepIndexForward(void);
};


#endif // !defined(__AFX_VCALCPARSER_H_INCLUDED__)

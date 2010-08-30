/**********************************************************************
 *
 * autocbox.h
 *
 * programmed by e.e.l.mitchell, fort myers, fl (2003)
 * $Date: 2008/03/17 04:03:08 $
 *
 * version 1.2
 *
 * looks at the contents of the list box and when the line given matches
 * the beginning of only one of the rows, then the item number is
 * returned that the matchedLine string is filled as well.
 *
 * based on the autocompleting combobox of chris maunder. see
 * www.codeproject.com
 *
 * the characters can be entered in reverse in which case the match
 * is made on the right rather than on the left
 *
***********************************************************************/
#ifndef autocombobox_h
#define autocombobox_h
#include <vector>
//using namespace std;
using std::vector;
typedef vector<int> IntVec;
typedef vector<CString> CStringsVec;

////////////////////////////////////////////////////////////////////////
//
// CAutoComboBox window
//
class CAutoComboBox : public CComboBox {

public:
    // define the two types to emulate
    enum AutoComboBoxType {
        ACB_DROP_DOWN,
        ACB_DROP_LIST
    };
    CAutoComboBox(AutoComboBoxType acbt = ACB_DROP_LIST);
    virtual ~CAutoComboBox();

    // ClassWizard generated virtual function overrides
private:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

public:
    int GetSelectedRow();
    int AddRow(const TCHAR* pString, const CStringsVec& extraCols);
    int AddRow(const CStringsVec& cols);
    int AddRow(const TCHAR* pString);
    int DeleteRow(int iRow);
    void SetReverseEntry(bool bReverse) { m_bReverseEntry = bReverse; }
    void GetLBText(int nIndex, CString& rString) const;
    void ResetSelection();
    void ResetContent();
    // the following routines are deprecated!!
    int DeleteString(UINT uRow);        // use DeleteRow(...)
    void SetColCount(int iCols);        // not needed any more

protected:
    afx_msg void OnEditUpdate();
    afx_msg void OnSelChange();
    afx_msg void OnCbnDropdown();

    DECLARE_MESSAGE_MAP()
private:
    int FindUniqueMatchedItem(const CString& line, CString& matchedLine);
    int SetDroppedWidthForTextLengths();
    bool LineInString(const CString& rsLine, const CString& rsString) const;
    int m_iSelectedRow;
    int m_iHorizontalExtent;    // list box width for all columns
    bool m_bAutoComplete;       // false after delete or backspace
    bool m_bReverseEntry;       // enter characters in reverse
    bool m_bEditHeightSet;      // true when we've set the height of the box
    AutoComboBoxType m_autoComboBoxType;
    IntVec m_vecEndPixels;      // ending positions for each column
    vector<CStringsVec> m_vecRows;// row text for each col after the first
};

#endif // autocombobox_h

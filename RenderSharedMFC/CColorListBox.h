#pragma once


// CColorListBox

class CColorListBox : public CListBox
{
	DECLARE_DYNAMIC(CColorListBox)

public:
	CColorListBox();
	virtual ~CColorListBox();


	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};



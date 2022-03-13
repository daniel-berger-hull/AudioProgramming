// CColorListBox.cpp : implementation file
//

#include "pch.h"
#include "WavPlayerRenderShared.h"
#include "CColorListBox.h"


// CColorListBox

IMPLEMENT_DYNAMIC(CColorListBox, CListBox)

CColorListBox::CColorListBox()
{

}

CColorListBox::~CColorListBox()
{
}


BEGIN_MESSAGE_MAP(CColorListBox, CListBox)
    ON_WM_CREATE()
    ON_WM_DRAWITEM()
    ON_BN_CLICKED(IDC_CLEAR_WAV_FILE_BUTTON_VALUE, &CColorListBox::OnBnClickedClearWavFileButtonValue)
END_MESSAGE_MAP()


int CColorListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    return 0;
}

											  
void CColorListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CString itemString;
    CDC dc;


    dc.Attach(lpDrawItemStruct->hDC);


    //Save these value to restore them when done drawing.
    COLORREF crOldTextColor = dc.GetTextColor();
    COLORREF crOldBkColor = RGB(0, 255, 255);


    //If this item is selected, set the background color 
   //and the text color to appropriate values. Also, erase
    //rect by filling it with the background color.
    if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
        (lpDrawItemStruct->itemState & ODS_SELECTED))
    {
        dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
        dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
        dc.FillSolidRect(&lpDrawItemStruct->rcItem,
            RGB(255, 0, 0));


    }
    else
        dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);

    GetText(lpDrawItemStruct->itemID, itemString);


    //Change the text color on every other line
    if (lpDrawItemStruct->itemID % 2)
        dc.SetTextColor(RGB(255, 255, 0));
    else
        dc.SetTextColor(RGB(150, 0, 200));


    //Draw text color
   /* dc.DrawText(
        itemString,
        strlen(itemString),
        &lpDrawItemStruct->rcItem,
        DT_CENTER | DT_SINGLELINE | DT_VCENTER);*/

    dc.SetTextColor(crOldTextColor);//Restore the original dc
    dc.Detach();

}

// CColorListBox message handlers




void CColorListBox::OnBnClickedClearWavFileButtonValue()
{
    int a = 2;
    // TODO: Add your control notification handler code here
}

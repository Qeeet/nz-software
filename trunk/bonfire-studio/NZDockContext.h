#ifndef __NZDOCKCONTEXT_H_
#define __NZDOCKCONTEXT_H_

class CNZDockContext : public CDockContext
{
public:
// Construction
	CNZDockContext(CControlBar* pBar);

// Attributes
	CPoint m_ptLast;            // last mouse position during drag
	CRect m_rectLast;
	CSize m_sizeLast;
	BOOL m_bDitherLast;

	// Rectangles used during dragging or resizing
	CRect m_rectDragHorz;
	CRect m_rectDragVert;
	CRect m_rectFrameDragHorz;
	CRect m_rectFrameDragVert;

	CControlBar* m_pBar;        // the toolbar that created this context
	CFrameWnd* m_pDockSite;     // the controlling frame of the CControlBar
	DWORD m_dwDockStyle;        // allowable dock styles for bar
	DWORD m_dwOverDockStyle;    // style of dock that rect is over
	DWORD m_dwStyle;            // style of control bar
	BOOL m_bFlip;               // if shift key is down
	BOOL m_bForceFrame;         // if ctrl key is down

	CDC* m_pDC;                 // where to draw during drag
	BOOL m_bDragging;
	int m_nHitTest;

	UINT m_uMRUDockID;
	CRect m_rectMRUDockPos;

	DWORD m_dwMRUFloatStyle;
	CPoint m_ptMRUFloatPos;

// Drag Operations
	virtual void StartDrag(CPoint pt);
	void Move(CPoint pt);       // called when mouse has moved
	void EndDrag();             // drop
	void OnKey(int nChar, BOOL bDown);

// Resize Operations
	virtual void StartResize(int nHitTest, CPoint pt);
	void Stretch(CPoint pt);
	void EndResize();

// Double Click Operations
	virtual void ToggleDocking();

// Operations
	void InitLoop();
	void CancelLoop();

// Implementation
public:
	~CNZDockContext();
	BOOL Track();
	void DrawFocusRect(BOOL bRemoveRect = FALSE);
		// draws the correct outline
	void UpdateState(BOOL* pFlag, BOOL bNewValue);
	DWORD CanDock();
	CDockBar* GetDockBar(DWORD dwOverDockStyle);
};

#endif

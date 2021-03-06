#pragma once

#include "ChartDataView.h"
#include "ChartMaster.h"
#include "GUIFrameBase.h"
#include "GeneratePeriodicRefresh.h"

// CChartViewPort is the window used to hold the master chart

class CChartViewPort : public CGUIFrameBase {
  DECLARE_DYNAMIC(CChartViewPort)
public:
  CChartViewPort(CChartDataView *cdv, CWnd* pParent = NULL);
  virtual ~CChartViewPort( void );
  void SetChartDataView( CChartDataView *cdv ) { m_cm.SetChartDataView( cdv ); };
  CChartDataView *GetChartDataView( void ) { return m_cm.GetChartDataView(); };
protected:
  CChartMaster m_cm;
  //CScrollBar m_hscroll;
  void SetChartMasterSize( void );

  afx_msg void OnViewPortChanged();
  afx_msg void OnChartViewer();

  afx_msg int OnCreate( LPCREATESTRUCT );
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
  afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
  afx_msg void OnInitMenuPopup( CMenu *, UINT, BOOL );
  afx_msg void OnKeyDown( UINT, UINT, UINT );
  afx_msg void OnKeyUp( UINT, UINT, UINT );
  afx_msg void OnSizing( UINT, LPRECT ); 
  afx_msg void OnMouseMove( UINT, CPoint );
  afx_msg BOOL OnSetCursor( CWnd*, UINT, UINT );

  CGeneratePeriodicRefresh m_refresh;
  void HandlePeriodicRefresh( CGeneratePeriodicRefresh *pMsg );

private:
	DECLARE_MESSAGE_MAP()
  bool m_bKeyOnShift;
  bool m_bKeyOnControl;
  void SetMouseUsage( void );
};

// have the refresh on only when there is a DataView available
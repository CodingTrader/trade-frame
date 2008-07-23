#pragma once

#include <string>
#include "ChartDirector\chartviewer.h"
#include "GeneratePeriodicRefresh.h"
#include "ChartDataView.h"

class CChartMaster :  public CChartViewer {
public:
  CChartMaster(void);
  CChartMaster( unsigned int width, unsigned int height );
  virtual ~CChartMaster(void);
  void SetChartDimensions( unsigned int width, unsigned int height);
  //void SetChartTitle( std::string sChartTitle ) { m_sChartTitle = sChartTitle; };
  void SetChartDataView( CChartDataView *pcdv ) { m_pCdv = pcdv; };
  CChartDataView *GetChartDataView( void ) { return m_pCdv; };
protected:
  //std::string m_sChartTitle;  // data view has this information
  unsigned int m_nChartWidth;
  unsigned int m_nChartHeight;
  CChartDataView *m_pCdv;
  void Initialize( void );
private:
  CGeneratePeriodicRefresh m_refresh;
  void HandlePeriodicRefresh( CGeneratePeriodicRefresh *pMsg );
	DECLARE_MESSAGE_MAP()
};
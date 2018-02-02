// Copyright 2017 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "PointHighlighter.h"

#include "ToolResourceProvider.h"

#include "GeoView.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "Point.h"
#include "SimpleMarkerSceneSymbol.h"

#include <QTimer>

using namespace Esri::ArcGISRuntime;

PointHighlighter::PointHighlighter(QObject* parent):
  QObject(parent)
{
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged,
          this, &PointHighlighter::onGeoViewChanged);

  onGeoViewChanged();
}

PointHighlighter::~PointHighlighter()
{

}

void PointHighlighter::onPointChanged(const Point& point)
{
  m_point = point;
}

void PointHighlighter::startHighlight()
{
  if (m_point.isEmpty())
    return;

  if (!m_highlightOverlay || !m_highlightSymbol)
    return;

  Graphic* highlightGraphic = new Graphic(m_point, m_highlightSymbol, this);
  m_highlightOverlay->graphics()->append(highlightGraphic);

  m_highlightTimer = new QTimer(this);
  connect(m_highlightTimer, &QTimer::timeout, this, [this]()
  {
    if (!m_highlightSymbol)
    {
      m_highlightTimer->stop();
      return;
    }

    if (!m_highlightOverlay || !m_highlightOverlay->graphics() || m_highlightOverlay->graphics()->isEmpty())
    {
      m_highlightTimer->stop();
      return;
    }

    Graphic* graphic = m_highlightOverlay->graphics()->first();
    if (!graphic)
    {
      m_highlightTimer->stop();
      return;
    }

    graphic->setGeometry(m_point);

    const int currDim = m_highlightSymbol->width();
    constexpr int maxDimension = 1000;
    int newDimension = currDim;
    float newOpacity = m_highlightOverlay->opacity();
    if (currDim < maxDimension)
    {
      newDimension += 10;
      newOpacity -= 0.01f;
    }
    else
    {
      newDimension = 1;
    }

    if (currDim > 10 && currDim < 30)
      newOpacity = 1.0f;

    m_highlightSymbol->setWidth(newDimension);
    m_highlightSymbol->setHeight(newDimension);
    m_highlightSymbol->setDepth(newDimension);
    m_highlightOverlay->setOpacity(newOpacity);
  });

  m_highlightTimer->start(10);
}

void PointHighlighter::stopHighlight()
{
  if (!m_highlightOverlay || !m_highlightOverlay->graphics() || m_highlightOverlay->graphics()->isEmpty())
    return;

  Graphic* graphic = m_highlightOverlay->graphics()->first();
  if (graphic)
  {
    delete graphic;
    m_highlightOverlay->graphics()->clear();
  }

  if (m_highlightTimer)
  {
    m_highlightTimer->stop();
    delete m_highlightTimer;
    m_highlightTimer = nullptr;
  }
}

void PointHighlighter::onGeoViewChanged()
{
  if (m_highlightOverlay)
  {
    delete m_highlightOverlay;
    m_highlightOverlay = nullptr;
  }

  if (m_highlightSymbol)
  {
    delete m_highlightSymbol;
    m_highlightSymbol = nullptr;
  }

  GeoView* geoview = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoview)
    return;

  m_highlightOverlay = new GraphicsOverlay(this);
  geoview->graphicsOverlays()->append(m_highlightOverlay);

  m_highlightSymbol = new SimpleMarkerSceneSymbol(
        SimpleMarkerSceneSymbolStyle::Sphere, Qt::red, 1.0, 1.0, 1.0, SceneSymbolAnchorPosition::Center, this);
}

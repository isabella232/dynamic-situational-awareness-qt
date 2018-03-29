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

// PCH header
#include "pch.hpp"

// example app headers
#include "AddLocalDataController.h"
#include "AlertConditionsController.h"
#include "AlertListController.h"
#include "AnalysisListController.h"
#include "AppInfo.h"
#include "BasemapPickerController.h"
#include "ContactReportController.h"
#include "ContextMenuController.h"
#include "DsaResources.h"
#include "FollowPositionController.h"
#include "Handheld.h"
#include "HandheldStyles.h"
#include "IdentifyController.h"
#include "LineOfSightController.h"
#include "LocationController.h"
#include "LocationTextController.h"
#include "MarkupController.h"
#include "MessageFeedsController.h"
#include "NavigationController.h"
#include "OptionsController.h"
#include "TableOfContentsController.h"
#include "ViewedAlertsController.h"
#include "ViewshedController.h"

// toolkit headers
#include "ArcGISCompassController.h"
#include "ArcGISRuntimeToolkit.h"

// C++ API headers
#include "ArcGISRuntimeEnvironment.h"
#include "PopupManager.h"
#include "SceneQuickView.h"

// Qt headers
#include <QCommandLineParser>
#include <QDir>
#include <QGuiApplication>
#include <QMessageBox>
#include <QObject>
#include <QQmlEngine>
#include <QQuickView>
#include <QSettings>

// STL headers
#ifdef Q_OS_WIN
#include <Windows.h>
#endif

//------------------------------------------------------------------------------

#define kSettingsFormat                 QSettings::IniFormat

//------------------------------------------------------------------------------

#define kArgShowName                    "show"
#define kArgShowValueName               "showOption"
#define kArgShowDescription             "Show option maximized | minimized | fullscreen | normal | default"
#define kArgShowDefault                 "show"

#define kShowMaximized                  "maximized"
#define kShowMinimized                  "minimized"
#define kShowFullScreen                 "fullscreen"
#define kShowNormal                     "normal"
#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

//------------------------------------------------------------------------------

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Toolkit;

QObject* dsaStylesProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
QObject* dsaResourcesProvider(QQmlEngine* engine, QJSEngine* scriptEngine);

int main(int argc, char *argv[])
{
#ifndef Q_OS_WIN
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication app(argc, argv);

  QCoreApplication::setApplicationName(kApplicationName);
  QCoreApplication::setApplicationVersion(kApplicationVersion);
  QCoreApplication::setOrganizationName(kOrganizationName);
#ifdef Q_OS_MAC
  QCoreApplication::setOrganizationDomain(kOrganizationName);
#else
  QCoreApplication::setOrganizationDomain(kOrganizationDomain);
#endif
  QSettings::setDefaultFormat(kSettingsFormat);

#ifdef Q_OS_WIN
  // Force usage of OpenGL ES through ANGLE on Windows
  QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif

  // Before initializing ArcGIS Runtime, first set the
  // ArcGIS Runtime license setting required for your application.

  // ArcGISRuntimeEnvironment::setLicense("Place license string in here");

  //  use this code to check for initialization errors
  //  QObject::connect(ArcGISRuntimeEnvironment::instance(), &ArcGISRuntimeEnvironment::errorOccurred, [](const Error& error){
  //    QMessageBox msgBox;
  //    msgBox.setText(error.message);
  //    msgBox.exec();
  //  });

  //  if (ArcGISRuntimeEnvironment::initialize() == false)
  //  {
  //    application.quit();
  //    return 1;
  //  }

  // Register the map view for QML
  qmlRegisterType<SceneQuickView>("Esri.Handheld", 1, 0, "SceneView");
  qRegisterMetaType<PopupManager*>("PopupManager*");

  // Register the Handheld (QQuickItem) for QML
  qmlRegisterType<Handheld>("Esri.Handheld", 1, 0, "Handheld");
  qmlRegisterType<BasemapPickerController>("Esri.DSA", 1, 0, "BasemapPickerController");
  qmlRegisterType<AddLocalDataController>("Esri.DSA", 1, 0, "AddLocalDataController");
  qmlRegisterType<LocationController>("Esri.DSA", 1, 0, "LocationController");
  qmlRegisterType<MessageFeedsController>("Esri.DSA", 1, 0, "MessageFeedsController");
  qmlRegisterType<FollowPositionController>("Esri.DSA", 1, 0, "FollowPositionController");
  qmlRegisterType<TableOfContentsController>("Esri.DSA", 1, 0, "TableOfContentsController");
  qmlRegisterType<NavigationController>("Esri.DSA", 1, 0, "NavigationController");
  qmlRegisterType<MarkupController>("Esri.DSA", 1, 0, "MarkupController");
  qmlRegisterType<ViewshedController>("Esri.DSA", 1, 0, "ViewshedController");
  qmlRegisterType<OptionsController>("Esri.DSA", 1, 0, "OptionsController");
  qmlRegisterSingletonType<HandheldStyles>("Esri.DSA", 1, 0, "DsaStyles", &dsaStylesProvider);
  qmlRegisterSingletonType<DsaResources>("Esri.DSA", 1, 0, "DsaResources", &dsaResourcesProvider);
  qmlRegisterType<IdentifyController>("Esri.DSA", 1, 0, "IdentifyController");
  qmlRegisterType<AlertListController>("Esri.DSA", 1, 0, "AlertListController");
  qmlRegisterType<ViewedAlertsController>("Esri.DSA", 1, 0, "ViewedAlertsController");
  qmlRegisterType<LocationTextController>("Esri.DSA", 1, 0, "LocationTextController");
  qmlRegisterType<AlertConditionsController>("Esri.DSA", 1, 0, "AlertConditionsController");
  qmlRegisterType<LineOfSightController>("Esri.DSA", 1, 0, "LineOfSightController");
  qmlRegisterType<ContextMenuController>("Esri.DSA", 1, 0, "ContextMenuController");
  qmlRegisterType<AnalysisListController>("Esri.DSA", 1, 0, "AnalysisListController");
  qmlRegisterType<ContactReportController>("Esri.DSA", 1, 0, "ContactReportController");

  // Register Toolkit Component Types
  ArcGISRuntimeToolkit::registerToolkitTypes();

  // Intialize application view
  QQuickView view;
  view.setResizeMode(QQuickView::SizeRootObjectToView);

#ifndef DEPLOYMENT_BUILD
  // Add the import Path
  view.engine()->addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));
  QString arcGISRuntimeImportPath = QUOTE(ARCGIS_RUNTIME_IMPORT_PATH);
  QString arcGISToolkitImportPath = QUOTE(ARCGIS_TOOLKIT_IMPORT_PATH);

#if defined(LINUX_PLATFORM_REPLACEMENT)
  // on some linux platforms the string 'linux' is replaced with 1
  // fix the replacement paths which were created
  QString replaceString = QUOTE(LINUX_PLATFORM_REPLACEMENT);
  arcGISRuntimeImportPath = arcGISRuntimeImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
  arcGISToolkitImportPath = arcGISToolkitImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
#endif

  // Add the Runtime and Extras path
  view.engine()->addImportPath(arcGISRuntimeImportPath);
  // Add the Toolkit path
  view.engine()->addImportPath(arcGISToolkitImportPath);
#endif // DEPLOYMENT_BUILD

  // To quit via Qt.quit() from QML, you must connect the QQmlEngine::quit()
  // signal to the QCoreApplication::quit() slot
  QObject::connect(view.engine(), &QQmlEngine::quit, &QCoreApplication::quit);

  // Set the source
  view.setSource(QUrl(kApplicationSourceUrl));

#if !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID)
  // Process command line
  QCommandLineOption showOption(kArgShowName, kArgShowDescription, kArgShowValueName, kArgShowDefault);

  QCommandLineParser commandLineParser;

  commandLineParser.setApplicationDescription(kApplicationDescription);
  commandLineParser.addOption(showOption);
  commandLineParser.addHelpOption();
  commandLineParser.addVersionOption();
  commandLineParser.process(app);

  // Show app window

  auto showValue = commandLineParser.value(kArgShowName).toLower();

  if (showValue.compare(kShowMaximized) == 0)
  {
    view.showMaximized();
  }
  else if (showValue.compare(kShowMinimized) == 0)
  {
    view.showMinimized();
  }
  else if (showValue.compare(kShowFullScreen) == 0)
  {
    view.showFullScreen();
  }
  else if (showValue.compare(kShowNormal) == 0)
  {
    view.showNormal();
  }
  else
  {
    view.show();
  }

#else
  view.show();
#endif

  return app.exec();
}

//------------------------------------------------------------------------------

// qml dsa styles provider
QObject* dsaStylesProvider(QQmlEngine* engine, QJSEngine*)
{
  static HandheldStyles* dsaStyles = new HandheldStyles(engine);
  return dsaStyles;
}

QObject* dsaResourcesProvider(QQmlEngine* engine, QJSEngine*)
{
  static DsaResources* dsaResources = new DsaResources(engine);
  return dsaResources;
}

Project(FiberViewerLight)
set(LOCAL_PROJECT_NAME FiberViewerLight)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FIND_PACKAGE(VTK REQUIRED)
IF (VTK_FOUND)
  SET(VTK_USE_QVTK TRUE)
  SET(VTK_USE_GUISUPPORT TRUE)
  INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
   MESSAGE(FATAL_ERROR, "VTK not found. Please set VTK_DIR.")
ENDIF (VTK_FOUND)


FIND_PACKAGE(Qt4 REQUIRED)
IF(QT_USE_FILE)
  SET(QT_USE_ALL)
  SET(QT_USE_QTOPENGL TRUE)
  SET(QT_USE_QTXMLPATTERNS TRUE)
  INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
   MESSAGE(FATAL_ERROR, "QT not found. Please set QT_DIR.")
ENDIF(QT_USE_FILE)


INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${FVLight_BINARY_DIR}
  ${FVLight_SOURCE_DIR}
)

QT4_WRAP_CPP(MOC_FILES FiberViewerLightGUI.h)

SET(FVLight_source FiberViewerLight.cxx FiberViewerLightGUI.cxx ${MOC_FILES})
SET(FVLight_header FiberViewerLightGUI.h)

# GENERATECLP(FiberViewerLight.cxx FiberViewerLight.xml)
add_executable( FiberViewerLight ${FVLight_source} ${FVLight_header})

TARGET_LINK_LIBRARIES(FiberViewerLight
   vtkWidgets
  vtkRendering
  vtkGraphics
  vtkImaging
  vtkIO
  vtkFiltering
  vtkCommon
  vtkHybrid
  vtksys
  QVTK
  vtkQtChart
  vtkViews
  vtkInfovis
  vtklibxml2
  vtkDICOMParser
  vtkpng
  vtkzlib
  vtkjpeg
  vtkalglib
  vtkexpat
  vtkverdict
  vtkmetaio
  vtkNetCDF
  vtksqlite
  vtkexoIIc
  vtkftgl
  vtkfreetype
  ${QT_LIBRARIES}
)
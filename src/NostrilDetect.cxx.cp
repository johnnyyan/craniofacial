#include <vtkSmartPointer.h>
#include <vtkPLYReader.h>
#include <vtkClipPolyData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWindowToImageFilter.h>
#include <vtkJPEGWriter.h>
#include <vtkPointsProjectedHull.h>
#include <vtkPolyLine.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>

#include "Utility.h"
#include "Detector.h"

#define PI 3.14159265
//#define SHOW 

void Usage(const char *prog) {
  std::cout << "Usage: " << prog
            << " [-a|-i]  Filename(.ply) Landmark(.txt)" << std::endl;
  std::cout << "  -a only use area to pick the cells" << std::endl;
  std::cout << "  -i only use incircle to pick the cells" << std::endl;
  std::cout << "  no flags use both" << std::endl;
  exit(EXIT_FAILURE);
}

int main ( int argc, char *argv[] )
{
  std::string inputFilename;
  std::string landmarkFilename;
  bool flag_area = false, flag_incircle = false;
  if (argc == 3) {
    inputFilename = argv[1];
    landmarkFilename = argv[2];
  } else if (argc == 4) {
    std::string flag = argv[1];
    if (flag.compare("-a") == 0)
      flag_area = true;
    else if (flag.compare("-i") == 0)
      flag_incircle = true;
    else
      Usage(argv[0]);
    inputFilename = argv[2];
    landmarkFilename = argv[3];
  } else {
    Usage(argv[0]);
  }

  std::string outputFilename = inputFilename; 
  std::size_t index = outputFilename.find("ply");
  if (index != std::string::npos)
    outputFilename.replace(index, 3, "jpg"); 

  vtkSmartPointer<vtkPLYReader> reader =
    vtkSmartPointer<vtkPLYReader>::New();
  reader->SetFileName( inputFilename.c_str() );
  reader->Update();

  double prn[3], sn[3], se[3], acl[3], acr[3], al[3], ar[3], cl[3], cr[3], sal[3], sar[3];
  if (!GetLandmarksFromFile(landmarkFilename, prn, sn, se, acl, acr, al, ar, cl, cr, sal, sar)) {
    std::cerr << "Failed to read in landmarks!" << std::endl;
    return EXIT_FAILURE;
  }

  // TODO: using the superaurale and chelion on the side of cleft lip
  // Define the normal direction of the projection plane
  double n[3] = {0.0, sal[1] - cl[1], sal[2] - cl[2]};
  double norm = sqrt(pow(n[1], 2.0) + pow(n[2], 2.0));
  double theta = acos(-n[2] / norm) * 180 / PI;
  //std::cout << theta << std::endl;

  vtkSmartPointer<vtkPoints> nostrilPoints;
  if (flag_area) {
    nostrilPoints = findNostrilPointsByArea(reader, prn, sn, al, ar);
  } else if (flag_incircle) {
    nostrilPoints = findNostrilPointsByIncircle(reader, prn, sn, al, ar);
  } else {
    nostrilPoints = findNostrilPointsDefault(reader, prn, sn, al, ar, n);
  }

  vtkSmartPointer<vtkPolyData> nostrilPoly =
    vtkSmartPointer<vtkPolyData>::New();
  nostrilPoly->SetPoints(nostrilPoints);
  nostrilPoly->Update();

  vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  glyphFilter->SetInputConnection(nostrilPoly->GetProducerPort());
  glyphFilter->Update();

  vtkSmartPointer<vtkTransform> transform = 
    vtkSmartPointer<vtkTransform>::New();
  transform->RotateWXYZ(90.0-theta, 1, 0, 0);

  vtkSmartPointer<vtkTransformPolyDataFilter> nostrilFilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  nostrilFilter->SetTransform(transform);
  nostrilFilter->SetInputConnection(glyphFilter->GetOutputPort());
  nostrilFilter->Update();

  //setup actor and mapper
  vtkSmartPointer<vtkPolyDataMapper> pointMapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  pointMapper->SetInputConnection(nostrilFilter->GetOutputPort());

  vtkSmartPointer<vtkActor> pointActor = 
    vtkSmartPointer<vtkActor>::New();
  pointActor->SetMapper(pointMapper);
  
  vtkSmartPointer<vtkPointsProjectedHull> points = 
    vtkSmartPointer<vtkPointsProjectedHull>::New();
  points->DeepCopy(nostrilFilter->GetOutput()->GetPoints());
  
  int xSize = points->GetSizeCCWHullX();
  std::cout << "xSize: " << xSize << std::endl;
  
  double* pts = new double[xSize*2];
  
  points->GetCCWHullY(pts,xSize);
  
  vtkSmartPointer<vtkPoints> xHullPoints = 
    vtkSmartPointer<vtkPoints>::New();
  for(int i = 0; i < xSize; i++)
    {
    double yval = pts[2*i];
    double zval = pts[2*i + 1];
    std::cout << "(y,z) value of point " << i << " : ("
              << yval << " , " << zval << ")" << std::endl;
    xHullPoints->InsertNextPoint(0.0, yval, zval);
    }
  // Insert the first point again to close the loop
  xHullPoints->InsertNextPoint(0.0, pts[0], pts[1]);
    
  // Display the x hull
  vtkSmartPointer<vtkPolyLine> xPolyLine = 
    vtkSmartPointer<vtkPolyLine>::New();
  xPolyLine->GetPointIds()->SetNumberOfIds(xHullPoints->GetNumberOfPoints());
  
  for(vtkIdType i = 0; i < xHullPoints->GetNumberOfPoints(); i++)
    {
    xPolyLine->GetPointIds()->SetId(i,i);
    }
    
  // Create a cell array to store the lines in and add the lines to it
  vtkSmartPointer<vtkCellArray> cells = 
    vtkSmartPointer<vtkCellArray>::New();
  cells->InsertNextCell(xPolyLine);

  // Create a polydata to store everything in
  vtkSmartPointer<vtkPolyData> polyData = 
    vtkSmartPointer<vtkPolyData>::New();

  // Add the points to the dataset
  polyData->SetPoints(xHullPoints);

  // Add the lines to the dataset
  polyData->SetLines(cells);
  
  // Setup actor and mapper
  vtkSmartPointer<vtkPolyDataMapper> xHullMapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
  xHullMapper->SetInput(polyData);
#else
  xHullMapper->SetInputData(polyData);
#endif
 
  vtkSmartPointer<vtkActor> xHullActor = 
    vtkSmartPointer<vtkActor>::New();
  xHullActor->SetMapper(xHullMapper);
 
  // Setup render window, renderer, and interactor
  vtkSmartPointer<vtkRenderer> renderer = 
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = 
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = 
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->SetBackground(.1, .2, .3);
  renderWindow->SetSize(800, 800);
  
  xHullActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
  pointActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
  renderer->AddActor(xHullActor);
//  renderer->AddActor(pointActor);
 
  // Rotate camera
  renderer->GetActiveCamera()->Azimuth(90);
  renderer->ResetCamera();

  renderWindow->Render();
  vtkSmartPointer<vtkWindowToImageFilter> writeFilter =
    vtkSmartPointer<vtkWindowToImageFilter>::New();
  writeFilter->SetInput(renderWindow);
  writeFilter->Update();

  vtkSmartPointer<vtkJPEGWriter> writer = 
    vtkSmartPointer<vtkJPEGWriter>::New();
  writer->SetFileName(outputFilename.c_str());
  writer->SetInputConnection(writeFilter->GetOutputPort());
  writer->Write();

#ifdef SHOW 
  renderWindowInteractor->Start();
#endif

  return EXIT_SUCCESS;
}

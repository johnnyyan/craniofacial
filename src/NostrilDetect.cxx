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

#include "Utility.h"
#include "Detector.h"

#define PI 3.14159265

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
// TODO: to be removed
#if defined (CENTER_IN_RECT)
  std::cout << "CENTER_IN_RECT" << std::endl;
#elif defined (TRIANGLE_IN_RECT)
  std::cout << "TRIANGLE_IN_RECT" << std::endl;
#elif defined (CENTER_IN_DIAM)
  std::cout << "CENTER_IN_DIAM" << std::endl;
#elif defined (TRIANGLE_IN_DIAM)
  std::cout << "TRIANGLE_IN_DIAM" << std::endl;
#endif
#if defined (CENTER_IN_CIRCLE)
  std::cout << "CENTER_IN_CIRCLE" << std::endl;
#endif
#if defined (SHOW)
  std::cout << "SHOW" << std::endl;
#endif

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
  double n[3] = {0.0, abs(sal[1] - cl[1]), abs(sal[2] - cl[2])};
  double norm = sqrt(pow(n[1], 2.0) + pow(n[2], 2.0));
  double theta = acos(-n[2] / norm) * 180 / PI;

  vtkSmartPointer<vtkClipPolyData> nose = 
    getNoseClipper(reader, se, cl, cr, al, ar);

  vtkSmartPointer<vtkPoints> nostrilPoints;
  if (flag_area) {
    nostrilPoints = findNostrilPointsByArea(reader, prn, sn, al, ar);
  } else if (flag_incircle) {
    nostrilPoints = findNostrilPointsByIncircle(reader, prn, sn, al, ar);
  } else {
    nostrilPoints = findNostrilPointsDefault(reader, prn, sn, al, ar);
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
  transform->RotateWXYZ(90-theta, 1, 0, 0);

  vtkSmartPointer<vtkTransformPolyDataFilter> noseFilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  noseFilter->SetTransform(transform);
  noseFilter->SetInputConnection(nose->GetOutputPort());
  noseFilter->Update();

  vtkSmartPointer<vtkTransformPolyDataFilter> nostrilFilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  nostrilFilter->SetTransform(transform);
  nostrilFilter->SetInputConnection(glyphFilter->GetOutputPort());
  nostrilFilter->Update();

  // Visualize
  // Mapping nose 
  vtkSmartPointer<vtkPolyDataMapper> noseMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  noseMapper->SetInputConnection(noseFilter->GetOutputPort()); 
  // Mapping nostril
  vtkSmartPointer<vtkPolyDataMapper> nostrilMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  nostrilMapper->SetInputConnection(nostrilFilter->GetOutputPort());

  vtkSmartPointer<vtkActor> noseActor =
    vtkSmartPointer<vtkActor>::New();
  noseActor->SetMapper(noseMapper);

  vtkSmartPointer<vtkActor> nostrilActor =
    vtkSmartPointer<vtkActor>::New();
  nostrilActor->SetMapper(nostrilMapper);
  nostrilActor->GetProperty()->SetPointSize(4);
  nostrilActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(noseActor);
  renderer->AddActor(nostrilActor);
  renderer->SetBackground(.1, .2, .3);
  renderWindow->SetSize(800, 800);

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

#if defined (SHOW) 
  renderWindowInteractor->Start();
#endif

  return EXIT_SUCCESS;
}

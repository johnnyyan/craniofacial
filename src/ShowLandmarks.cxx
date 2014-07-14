#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkCellData.h>
#include <vtkTriangle.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPLYReader.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCellArray.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkAxesActor.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include "Utility.h"

int main ( int argc, char *argv[] )
{
  if(argc != 3) {
    std::cout << "Usage: " << argv[0] << "  Filename(.ply) Landmark(.txt)" << std::endl;
    return EXIT_FAILURE;
  }

  std::string inputFilename = argv[1];

  vtkSmartPointer<vtkPLYReader> reader =
    vtkSmartPointer<vtkPLYReader>::New();
  reader->SetFileName( inputFilename.c_str() );
  reader->Update();

  vtkSmartPointer<vtkPolyData> pd = reader->GetOutput();
  vtkSmartPointer<vtkCellArray> ca = pd->GetPolys();
  vtkSmartPointer<vtkPoints> points = pd->GetPoints();

  double prn[3], sn[3], se[3], acl[3], acr[3], al[3], ar[3], cl[3], cr[3], sal[3], sar[3];
  if (!GetLandmarksFromFile(argv[2], prn, sn, se, acl, acr, al, ar, cl, cr, sal, sar)) {
    std::cerr << "Failed to read in landmarks!" << std::endl;
    return EXIT_FAILURE;
  }
/*
  double scl[3] = {sal[0], cl[1], cl[2]};
  std::cout << "pronasale: " << prn[0] << "," << prn[1] << "," << prn[2] << std::endl;
  std::cout << "subnasale: " << sn[0] << "," << sn[1] << "," << sn[2] << std::endl;
  std::cout << "left alare: " << al[0] << "," << al[1] << "," << al[2] << std::endl;
  std::cout << "right alare: " << ar[0] << "," << ar[1] << "," << ar[2] << std::endl;
  std::cout << "left chelion: " << cl[0] << "," << cl[1] << "," << cl[2] << std::endl;
  std::cout << "left superaurale: " << sal[0] << "," << sal[1] << "," << sal[2] << std::endl;
  std::cout << "shifted chelion: " << scl[0] << "," << scl[1] << "," << scl[2] << std::endl;
*/
  vtkSmartPointer<vtkPoints> nose = 
    vtkSmartPointer<vtkPoints>::New();
  // Add the landmarks to be shown here
  nose->InsertNextPoint(prn);
  nose->InsertNextPoint(sn);
  nose->InsertNextPoint(ar);
  nose->InsertNextPoint(al);
  nose->InsertNextPoint(cr);
  nose->InsertNextPoint(cl);
  nose->InsertNextPoint(sal);
  nose->InsertNextPoint(sar);
  vtkSmartPointer<vtkPolyData> nosepd =
    vtkSmartPointer<vtkPolyData>::New();
  nosepd->SetPoints(nose);
  
  vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  glyphFilter->SetInputConnection(nosepd->GetProducerPort());
  glyphFilter->Update();

  vtkSmartPointer<vtkTransform> transform = 
    vtkSmartPointer<vtkTransform>::New();
  transform->RotateWXYZ(0, 1, 0, 0);

  vtkSmartPointer<vtkTransformPolyDataFilter> headFilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  headFilter->SetTransform(transform);
  headFilter->SetInputConnection(pd->GetProducerPort());
  headFilter->Update();

  vtkSmartPointer<vtkTransformPolyDataFilter> landmarkFilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  landmarkFilter->SetTransform(transform);
  landmarkFilter->SetInputConnection(glyphFilter->GetOutputPort());
  landmarkFilter->Update();

  // Visualize
  // Mapping original head
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(headFilter->GetOutputPort());
  // Mapping landmarks
  vtkSmartPointer<vtkPolyDataMapper> landmarkMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  landmarkMapper->SetInputConnection(landmarkFilter->GetOutputPort()); 

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  vtkSmartPointer<vtkActor> landmarkActor =
    vtkSmartPointer<vtkActor>::New();
  landmarkActor->SetMapper(landmarkMapper);
  landmarkActor->GetProperty()->SetPointSize(10);
  landmarkActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  
  vtkSmartPointer<vtkAxesActor> axes =
    vtkSmartPointer<vtkAxesActor>::New();
  axes->SetTotalLength(200., 200., 200.);

  renderer->AddActor(axes);
  renderer->ResetCamera();
  renderer->AddActor(actor);
  renderer->AddActor(landmarkActor);
  renderer->SetBackground(.1, .2, .3);

  renderWindow->SetSize(800, 800);
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}

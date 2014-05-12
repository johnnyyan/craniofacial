#include <vtkLine.h>
#include <vtkMath.h>
#include <vtkSmartPointer.h>
#include <vtkPlane.h>

#include <fstream>
#include "Utility.h"

double tarea(double x[3], double y[3], double z[3]) {
  // x as point A
  // y as point B
  // z as point C
  double AB[3] = {x[0] - y[0], x[1] - y[1], x[2] - y[2]};
  double AC[3] = {x[0] - z[0], x[1] - z[1], x[2] - z[2]};
  double area = 0.0;
  area += pow((AB[1] * AC[2] - AB[2] * AC[1]), 2.0); 
  area += pow((AB[2] * AC[0] - AB[0] * AC[2]), 2.0); 
  area += pow((AB[0] * AC[1] - AB[1] * AC[0]), 2.0); 
  area = sqrt(area) / 2.0;
  return area;
}

double tarea2(double x[3], double y[3], double z[3]) {
  double altitude = sqrt(vtkLine::DistanceToLine(x, y, z));
  double sidelen = sqrt(vtkMath::Distance2BetweenPoints(y, z));
  return sidelen * altitude / 2.0;
}

void getCenter(double x[3], double y[3], double z[3], double center[3]) {
  for (int i = 0; i < 3; i++)
    center[i] = (x[i] + y[i] + z[i]) / 3.0;
}

double getIncircleCenter(double x[3], double y[3], double z[3], double incenter[3]) {
  double a = sqrt(vtkMath::Distance2BetweenPoints(y, z));
  double b = sqrt(vtkMath::Distance2BetweenPoints(z, x));
  double c = sqrt(vtkMath::Distance2BetweenPoints(x, y));
  double total = a + b + c;
  incenter[0] = (a*x[0] + b*y[0] + c*z[0]) / total;
  incenter[1] = (a*x[1] + b*y[1] + c*z[1]) / total;
  incenter[2] = (a*x[2] + b*y[2] + c*z[2]) / total;
  return sqrt(vtkLine::DistanceToLine(incenter, x, y));
}

bool onLeft(double x[3], double prn[3], double sn[3]) {
  double dir[3] = {sn[0]-prn[0], sn[1]-prn[1], sn[2]-prn[2]};
  double xdir[3] = {x[0]-prn[0], x[1]-prn[1], x[2]-prn[2]};
  double zvalue = xdir[0]*dir[1] - xdir[1]*dir[0];
  return zvalue < 0.0;
}

bool GetLandmarksFromFile(std::string file, double prn[3], 
                          double sn[3], double se[3], 
                          double acl[3], double acr[3], 
                          double al[3], double ar[3], 
                          double cl[3], double cr[3],
                          double sal[3], double sar[3]){
  std::ifstream input(file.c_str());
  if (!input) {
    std::cerr << "Error: failed to open file " << file << std::endl;
    return false;
  }

  double tmp[3];

  // Read in pronasale
  for (int i = 0; i < 3; i++) {
    input >> prn[i];
    input.ignore(1, ',');
  }

  // Read in subnasale
  for (int i = 0; i < 3; i++) {
    input >> sn[i];
    input.ignore(1, ',');
  }

  // Read in sellion 
  for (int i = 0; i < 3; i++) {
    input >> se[i];
    input.ignore(1, ',');
  }

  // Read in right alare curvature
  for (int i = 0; i < 3; i++) {
    input >> acr[i];
    input.ignore(1, ',');
  }

  // Read in left alare curvature
  for (int i = 0; i < 3; i++) {
    input >> acl[i];
    input.ignore(1, ',');
  }

  // Read in right alare
  for (int i = 0; i < 3; i++) {
    input >> ar[i];
    input.ignore(1, ',');
  }

  // Read in left alare
  for (int i = 0; i < 3; i++) {
    input >> al[i];
    input.ignore(1, ',');
  }

  // Read in right chelion 
  for (int i = 0; i < 3; i++) {
    input >> cr[i];
    input.ignore(1, ',');
  }

  // Read in left chelion 
  for (int i = 0; i < 3; i++) {
    input >> cl[i];
    input.ignore(1, ',');
  }

  // Read in right superaurale 
  for (int i = 0; i < 3; i++) {
    input >> sar[i];
    input.ignore(1, ',');
  }

  // Skip right postaurale 
  for (int i = 0; i < 3; i++) {
    input >> tmp[i];
    input.ignore(1, ',');
  }

  // Read in left superaurale 
  for (int i = 0; i < 3; i++) {
    input >> sal[i];
    input.ignore(1, ',');
  }

  // Skip left postaurale 
  for (int i = 0; i < 3; i++) {
    input >> tmp[i];
    input.ignore(1, ',');
  }

  input.close();
  return true;
}

vtkSmartPointer<vtkClipPolyData> 
getNoseClipper(vtkSmartPointer<vtkPLYReader> reader, double se[3],
               double cl[3], double cr[3], double al[3], double ar[3]) {
  vtkSmartPointer<vtkPlane> midPlane =
    vtkSmartPointer<vtkPlane>::New();
  midPlane->SetNormal(0.0, 0.0, 1.0);
  midPlane->SetOrigin(0.0, 0.0, 0.0);

  vtkSmartPointer<vtkClipPolyData> frontFace =
    vtkSmartPointer<vtkClipPolyData>::New();
  frontFace->SetInputConnection(reader->GetOutputPort());
  frontFace->SetClipFunction(midPlane);

  vtkSmartPointer<vtkPlane> sellionPlane =
    vtkSmartPointer<vtkPlane>::New();
  sellionPlane->SetNormal(0.0, -1.0, 0.0);
  sellionPlane->SetOrigin(se);

  vtkSmartPointer<vtkClipPolyData> belowSellionFace =
    vtkSmartPointer<vtkClipPolyData>::New();
  belowSellionFace->SetInputConnection(frontFace->GetOutputPort());
  belowSellionFace->SetClipFunction(sellionPlane);

  // pseudo labiale superius which is the middle of the left and right chelion 
  double pls[3] = {(cr[0] + cl[0])/2.0,(cr[1] + cl[1])/2.0,(cr[2] + cl[2])/2.0};  
  vtkSmartPointer<vtkPlane> plsPlane =
    vtkSmartPointer<vtkPlane>::New();
  plsPlane->SetNormal(0.0, 1.0, 0.0);
  plsPlane->SetOrigin(pls);

  vtkSmartPointer<vtkClipPolyData> abovePlsFace =
    vtkSmartPointer<vtkClipPolyData>::New();
  abovePlsFace->SetInputConnection(belowSellionFace->GetOutputPort());
  abovePlsFace->SetClipFunction(plsPlane);

  vtkSmartPointer<vtkPlane> leftAlarePlane =
    vtkSmartPointer<vtkPlane>::New();
  leftAlarePlane->SetNormal(-1.0, 0.0, 0.0);
  leftAlarePlane->SetOrigin(al[0] + 3.0, al[1], al[2]);

  vtkSmartPointer<vtkClipPolyData> rightToLeftAlareFace =
    vtkSmartPointer<vtkClipPolyData>::New();
  rightToLeftAlareFace->SetInputConnection(abovePlsFace->GetOutputPort());
  rightToLeftAlareFace->SetClipFunction(leftAlarePlane);

  vtkSmartPointer<vtkPlane> rightAlarePlane =
    vtkSmartPointer<vtkPlane>::New();
  rightAlarePlane->SetNormal(1.0, 0.0, 0.0);
  rightAlarePlane->SetOrigin(ar[0] - 3.0, ar[1], ar[2]);

  vtkSmartPointer<vtkClipPolyData> leftToRightAlareFace =
    vtkSmartPointer<vtkClipPolyData>::New();
  leftToRightAlareFace->SetInputConnection(rightToLeftAlareFace->GetOutputPort());
  leftToRightAlareFace->SetClipFunction(rightAlarePlane);

  return leftToRightAlareFace;
}

void getProjection(double *normal, double *original, double *projected) {
  double norm = pow(normal[1], 2.0) + pow(normal[2], 2.0);
  projected[0] = original[0];
  projected[1] = normal[2] * (normal[2]*original[1] - normal[1]*original[2]) / norm;
  projected[2] = normal[1] * (normal[1]*original[2] - normal[2]*original[1]) / norm;
}

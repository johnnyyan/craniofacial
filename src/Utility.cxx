#include <vtkLine.h>
#include <vtkMath.h>
#include <vtkSmartPointer.h>
#include <vtkPlane.h>

#include <fstream>
#include "Utility.h"

double
tarea(
  const double A[3],
  const double B[3],
  const double C[3]
)
{
  double AB[3] = {A[0] - B[0], A[1] - B[1], A[2] - B[2]};
  double AC[3] = {A[0] - C[0], A[1] - C[1], A[2] - C[2]};
  double area = 0.0;
  area += pow((AB[1] * AC[2] - AB[2] * AC[1]), 2.0); 
  area += pow((AB[2] * AC[0] - AB[0] * AC[2]), 2.0); 
  area += pow((AB[0] * AC[1] - AB[1] * AC[0]), 2.0); 
  area = sqrt(area) / 2.0;
  return area;
}

double
tarea2(
  double A[3],
  double B[3],
  double C[3]
)
{
  double altitude = sqrt(vtkLine::DistanceToLine(A, B, C));
  double sidelen = sqrt(vtkMath::Distance2BetweenPoints(B, C));
  return sidelen * altitude / 2.0;
}

void
getCenter(
  const double A[3],
  const double B[3],
  const double C[3],
  double center[3]
)
{
  for (int i = 0; i < 3; i++)
    center[i] = (A[i] + B[i] + C[i]) / 3.0;
}

double
getIncircleCenter(
  double A[3],
  double B[3],
  double C[3],
  double incenter[3]
)
{
  double a = sqrt(vtkMath::Distance2BetweenPoints(B, C));
  double b = sqrt(vtkMath::Distance2BetweenPoints(C, A));
  double c = sqrt(vtkMath::Distance2BetweenPoints(A, B));
  double total = a + b + c;
  incenter[0] = (a*A[0] + b*B[0] + c*C[0]) / total;
  incenter[1] = (a*A[1] + b*B[1] + c*C[1]) / total;
  incenter[2] = (a*A[2] + b*B[2] + c*C[2]) / total;
  return sqrt(vtkLine::DistanceToLine(incenter, A, B));
}

bool onLeft(
  const double x[3],
  const double pt1[3],
  const double pt2[3]
)
{
  double dir[3] = {pt2[0]-pt1[0], pt2[1]-pt1[1], pt2[2]-pt1[2]};
  double xdir[3] = {x[0]-pt1[0], x[1]-pt1[1], x[2]-pt1[2]};
  double zvalue = xdir[0]*dir[1] - xdir[1]*dir[0];
  return zvalue < 0.0;
}

bool
GetLandmarksFromFile(
  const std::string file,
  double prn[3],
  double sn[3],
  double se[3],
  double acl[3],
  double acr[3],
  double al[3],
  double ar[3],
  double cl[3],
  double cr[3],
  double sal[3],
  double sar[3]
)
{
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
getNoseClipper(
  vtkSmartPointer<vtkPLYReader> reader,
  double se[3],
  double cl[3],
  double cr[3],
  double al[3],
  double ar[3]
)
{
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

void
getProjection(
  const double normal[3],
  const double original[3],
  double projected[3]
)
{
  double norm = pow(normal[1], 2.0) + pow(normal[2], 2.0);
  projected[0] = original[0];
  projected[1] = normal[2] * (normal[2]*original[1] - normal[1]*original[2]) / norm;
  projected[2] = normal[1] * (normal[1]*original[2] - normal[2]*original[1]) / norm;
}

bool
isCenterInRect(
  const double center[3],
  const double left,
  const double right,
  const double bottom,
  const double top
)
{
  return center[2] > 0.0
         && center[0] > left && center[0] < right
         && center[1] > bottom && center[1] < top;
}

bool
isTriangleInRect(
  const double pts[3][3],
  const double left,
  const double right,
  const double bottom,
  const double top
)
{
  return isCenterInRect(pts[0], left, right, bottom, top) 
         && isCenterInRect(pts[1], left, right, bottom, top) 
         && isCenterInRect(pts[2], left, right, bottom, top);
}

int
isCenterInDiam(
  const double center[3],
  const double left[3],
  const double right[3],
  const double bottom[3],
  const double top[3])
{
  if ( isProjectionInTriangle(center, left, bottom, top) )
    return 1;
  if ( isProjectionInTriangle(center, right, bottom, top) )
    return 2;
  return 0;
}

bool
isProjectionInTriangle(
  const double pt[3],
  const double A[3],
  const double B[3],
  const double C[3])
{
  double proj[3];
  getProjectionPoint(pt, A, B, C, proj);
  return isPointInTriangle(proj, A, B, C);
}

void
getProjectionPoint(
  const double pt[3],
  const double A[3],
  const double B[3],
  const double C[3],
  double proj[3])
{
  double AB[3], AC[3];
  vtkMath::Subtract(B, A, AB);
  vtkMath::Subtract(C, A, AC);

  // Get the normal from AB X AC
  double normal[3];
  vtkMath::Cross(AB, AC, normal);
  vtkMath::Normalize(normal);

  // Project the point pt onto the plane defined by A and normal
  vtkSmartPointer<vtkPlane> plane = 
    vtkSmartPointer<vtkPlane>::New();
  plane->SetOrigin(const_cast<double *>(A));
  plane->SetNormal(normal);

  plane->ProjectPoint(const_cast<double *>(pt), const_cast<double *>(A), normal, proj);
}

bool
isPointInTriangle(
  const double pt[3],
  const double A[3],
  const double B[3],
  const double C[3])
{
  double AB[3], BC[3], CA[3];
  vtkMath::Subtract(B, A, AB);
  vtkMath::Subtract(C, B, BC);
  vtkMath::Subtract(A, C, CA);

  double AP[3], BP[3], CP[3];
  vtkMath::Subtract(pt, A, AP);
  vtkMath::Subtract(pt, B, BP);
  vtkMath::Subtract(pt, C, CP);

  double crossA[3], crossB[3], crossC[3];
  vtkMath::Cross(AB, AP, crossA);
  vtkMath::Cross(BC, BP, crossB);
  vtkMath::Cross(CA, CP, crossC);

  double abDot = vtkMath::Dot(crossA, crossB);
  double bcDot = vtkMath::Dot(crossB, crossC);
  double caDot = vtkMath::Dot(crossC, crossA);

  return (abDot > 0 && bcDot > 0 && caDot > 0) || (abDot < 0 && bcDot < 0 && caDot < 0); 
}

int
isTriangleInDiam(
  const double pts[3][3],
  const double left[3],
  const double right[3],
  const double bottom[3],
  const double top[3]
)
{
  if (  isProjectionInTriangle(pts[0], left, bottom, top) 
     && isProjectionInTriangle(pts[1], left, bottom, top)
     && isProjectionInTriangle(pts[2], left, bottom, top) )
    return 1;
  if (  isProjectionInTriangle(pts[0], right, bottom, top) 
     && isProjectionInTriangle(pts[1], right, bottom, top)
     && isProjectionInTriangle(pts[2], right, bottom, top) )
    return 2;
  return 0;
}

int
isCenterInCircle(
  const double center[3],
  const double left[3],
  const double right[3],
  const double bottom[3],
  const double top[3],
  const double lOrigin[3],
  const double lRadius,
  const double rOrigin[3],
  const double rRadius
)
{
  if ( isProjectionInTriangle(center, left, bottom, top) ){
    // We could have used the distance between origin and the projection of 
    // center. But we decided to use the circle radius as a sphere radius
    if ( sqrt(vtkMath::Distance2BetweenPoints(center, lOrigin)) < lRadius )
      return 1;
    else
      return 0;
  }
  if ( isProjectionInTriangle(center, right, bottom, top) ){
    if ( sqrt(vtkMath::Distance2BetweenPoints(center, rOrigin)) < rRadius )
      return 2;
    else
      return 0;
  }

  return 0;
}

int
isTriangleInCircle(
  const double pts[3][3],
  const double left[3],
  const double right[3],
  const double bottom[3],
  const double top[3],
  const double lOrigin[3],
  const double lRadius,
  const double rOrigin[3],
  const double rRadius
)
{
  if (  isProjectionInTriangle(pts[0], left, bottom, top) 
     && isProjectionInTriangle(pts[1], left, bottom, top)
     && isProjectionInTriangle(pts[2], left, bottom, top) ){
    if (  sqrt(vtkMath::Distance2BetweenPoints(pts[0], lOrigin)) < lRadius 
       && sqrt(vtkMath::Distance2BetweenPoints(pts[1], lOrigin)) < lRadius
       && sqrt(vtkMath::Distance2BetweenPoints(pts[2], lOrigin)) < lRadius )
      return 1;
    else
      return 0;
  }
  if (  isProjectionInTriangle(pts[0], right, bottom, top) 
     && isProjectionInTriangle(pts[1], right, bottom, top)
     && isProjectionInTriangle(pts[2], right, bottom, top) ){
    if (  sqrt(vtkMath::Distance2BetweenPoints(pts[0], rOrigin)) < rRadius 
       && sqrt(vtkMath::Distance2BetweenPoints(pts[1], rOrigin)) < rRadius
       && sqrt(vtkMath::Distance2BetweenPoints(pts[2], rOrigin)) < rRadius )
      return 2;
    else
      return 0;
  }

  return 0;
}

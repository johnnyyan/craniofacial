#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkMath.h>

#include <assert.h>
#include <vector>
#include <map>
#include "Utility.h"
#include "Detector.h"

#define LEFT  1
#define RIGHT 2

vtkSmartPointer<vtkPoints>
findNostrilPointsByArea(
  vtkSmartPointer<vtkPLYReader> reader,
  double prn[3],
  double sn[3],
  double al[3],
  double ar[3]
)
{
  vtkSmartPointer<vtkPolyData> pd = reader->GetOutput();
  vtkSmartPointer<vtkCellArray> ca = pd->GetPolys();
  vtkSmartPointer<vtkPoints> points = pd->GetPoints();

  // Candidate nostril points
  vtkSmartPointer<vtkPoints> nostrilPoints =
    vtkSmartPointer<vtkPoints>::New();

  ca->InitTraversal();
  vtkIdType npts, *pts;
  int res;
  double maxArea = 0.0, minArea = 1.0E12, total = 0.0;
  while((res = ca->GetNextCell(npts, pts))) {
    assert(npts == 3);
    double x[3][3];
    for (vtkIdType i = 0; i < 3; i++) {
      points->GetPoint(pts[i],x[i]);
    }

#if defined (CENTER_IN_RECT)
    //-- Center is in the rect
    double center[3];
    getCenter(x[0], x[1], x[2], center);
    if ( isCenterInRect(center, ar[0], al[0], sn[1], prn[1]) ) {
#elif defined (TRIANGLE_IN_RECT)
    //-- All points are in the rect
    if ( isTriangleInRect(x, ar[0], al[0], sn[1], prn[1]) ) {
#elif defined (CENTER_IN_DIAM)
    //-- Center is in the diamond 
    double center[3];
    getCenter(x[0], x[1], x[2], center);
    if ( isCenterInDiam(center, al, ar, sn, prn) ) {
#elif defined (TRIANGLE_IN_DIAM)
    //-- All points are in the diamond
    if ( isTriangleInDiam(x, al, ar, sn, prn) ) {
#endif
      double area = tarea2(x[0], x[1], x[2]);
      // TODO: replace hard-coded number 4.0 with a dynamic one
      if (area > 4.0 ){ //&& x[0][2] > 72. && x[1][2] > 72. && x[2][2] > 72.) {
        std::cout << "(" << x[0][0] << "," << x[0][1] << "," << x[0][2] << "); ";
std::cout << "(" << x[1][0] << "," << x[1][1] << "," << x[1][2] << "); ";
std::cout << "(" << x[2][0] << "," << x[2][1] << "," << x[2][2] << ")" << std::endl;
        nostrilPoints->InsertNextPoint(x[0]);
        nostrilPoints->InsertNextPoint(x[1]);
        nostrilPoints->InsertNextPoint(x[2]);
      }
      total += area;
      if (area > maxArea)
        maxArea = area;
      if (area < minArea)
        minArea = area;
    }
  }

  return nostrilPoints;
}

vtkSmartPointer<vtkPoints>
findNostrilPointsByIncircle(
  vtkSmartPointer<vtkPLYReader> reader,
  double prn[3],
  double sn[3],
  double al[3],
  double ar[3]
)
{
  vtkSmartPointer<vtkPolyData> pd = reader->GetOutput();
  vtkSmartPointer<vtkCellArray> ca = pd->GetPolys();
  vtkSmartPointer<vtkPoints> points = pd->GetPoints();

  // Candidate nostril points
  vtkSmartPointer<vtkPoints> nostrilPoints =
    vtkSmartPointer<vtkPoints>::New();

  // TODO: What's the proper value of the adjustment? Currently 2.0
  double xmin = ar[0]+2.0, xmax = al[0]-2.0;
  double ymin = sn[1], ymax = prn[1]-2.0;
  
  // Centers of the left and right incircles respectively
  double lc[3], rc[3];  
  double lradius = getIncircleCenter(prn, sn, al, lc);
  double rradius = getIncircleCenter(prn, sn, ar, rc);

  ca->InitTraversal();
  vtkIdType npts, *pts;
  int res;
  while((res = ca->GetNextCell(npts, pts))) {
    assert(npts == 3);
    double x[3][3];
    for (vtkIdType i = 0; i < 3; i++) {
      points->GetPoint(pts[i],x[i]);
    }

#if defined (CENTER_IN_CIRCLE)
    double center[3];
    getCenter(x[0], x[1], x[2], center);
    if ( isCenterInCircle(center, al, ar, sn, prn, lc, lradius, rc, rradius) ) {
#elif defined (TRIANGLE_IN_CIRCLE)
    if ( isTriangleInCircle(x, al, ar, sn, prn, lc, lradius, rc, rradius) ) {
#endif
      nostrilPoints->InsertNextPoint(x[0]);
      nostrilPoints->InsertNextPoint(x[1]);
      nostrilPoints->InsertNextPoint(x[2]);
    }
  }
  return nostrilPoints;
}

vtkSmartPointer<vtkPoints>
findNostrilPointsDefault(
  vtkSmartPointer<vtkPLYReader> reader,
  double prn[3],
  double sn[3],
  double al[3],
  double ar[3]
)
{
  vtkSmartPointer<vtkPolyData> pd = reader->GetOutput();
  vtkSmartPointer<vtkCellArray> ca = pd->GetPolys();
  vtkSmartPointer<vtkPoints> points = pd->GetPoints();

  // Candidate nostril points
  vtkSmartPointer<vtkPoints> nostrilPoints =
    vtkSmartPointer<vtkPoints>::New();

  double xmin = ar[0]+2.0, xmax = al[0]-2.0;
  double ymin = sn[1], ymax = prn[1]-2.0;
  
  // Centers of the left and right incircles respectively
  double lc[3], rc[3];  
  double lradius = getIncircleCenter(prn, sn, al, lc);
  double rradius = getIncircleCenter(prn, sn, ar, rc);

  ca->InitTraversal();
  vtkIdType npts, *pts;
  int res;

#if defined (DEFAULT_AVERAGE_AREA)
  double leftTotal = 0.0, rightTotal = 0.0;
  double leftMaxArea = 0.0, leftMinArea = 1.0E12;
  double rightMaxArea = 0.0, rightMinArea = 1.0E12;

  std::map<vtkIdType *, double> leftCells, rightCells;
#endif

  while((res = ca->GetNextCell(npts, pts))) {
    assert(npts == 3);
    double x[3][3];
    for (vtkIdType i = 0; i < 3; i++) {
      points->GetPoint(pts[i], x[i]);
    }

#if defined (CENTER_IN_CIRCLE)
    double center[3];
    getCenter(x[0], x[1], x[2], center);
    int side = isCenterInCircle(center, al, ar, sn, prn, lc, lradius, rc, rradius);
#elif defined (TRIANGLE_IN_CIRCLE)
    int side = isTriangleInCircle(x, al, ar, sn, prn, lc, lradius, rc, rradius);
#endif
    if (side) {
      double area = tarea2(x[0], x[1], x[2]);
#if defined (DEFAULT_FIXED_AREA)
      if (area > 4.0) {
        nostrilPoints->InsertNextPoint(x[0]);
        nostrilPoints->InsertNextPoint(x[1]);
        nostrilPoints->InsertNextPoint(x[2]);
      }
#elif defined (DEFAULT_AVERAGE_AREA)
      if ( side == 1 ) {
        leftCells[pts] = area;
        leftTotal += area;
        if (area > leftMaxArea)
          leftMaxArea = area;
        if (area < leftMinArea)
          leftMinArea = area;
      } else {
        rightCells[pts] = area;
        rightTotal += area;
        if (area > rightMaxArea)
          rightMaxArea = area;
        if (area < rightMinArea)
          rightMinArea = area;
      }
#endif
    }
  }

#if defined (DEFAULT_AVERAGE_AREA)
  // TODO: Get a more proper and reasonable parameter other than 1.2
  double leftAverageArea = leftTotal/leftCells.size()*1.2;
  double rightAverageArea = rightTotal/rightCells.size()*1.2;

  std::map<vtkIdType *, double>::const_iterator it;
  for (it = leftCells.begin(); it != leftCells.end(); ++it) {
    if (it->second > leftAverageArea) {
      double x[3][3];
      for (vtkIdType i = 0; i < 3; i++)
        points->GetPoint(it->first[i], x[i]);
      nostrilPoints->InsertNextPoint(x[0]);
      nostrilPoints->InsertNextPoint(x[1]);
      nostrilPoints->InsertNextPoint(x[2]);
    }
  }

  for (it = rightCells.begin(); it != rightCells.end(); ++it) {
    if (it->second > rightAverageArea) {
      double x[3][3];
      for (vtkIdType i = 0; i < 3; i++)
        points->GetPoint(it->first[i], x[i]);
      nostrilPoints->InsertNextPoint(x[0]);
      nostrilPoints->InsertNextPoint(x[1]);
      nostrilPoints->InsertNextPoint(x[2]);
    }
  }

  std::cout << "left meshes  : " << leftCells.size() << std::endl;
  std::cout << "left max area: " << leftMaxArea << std::endl;
  std::cout << "left min area: " << leftMinArea << std::endl;
  std::cout << "left average : " << leftTotal / leftCells.size() << std::endl; 
  std::cout << "right meshes  : " << rightCells.size() << std::endl;
  std::cout << "right max area: " << rightMaxArea << std::endl;
  std::cout << "right min area: " << rightMinArea << std::endl;
  std::cout << "right average : " << rightTotal / rightCells.size() << std::endl;

#endif

  return nostrilPoints;
}

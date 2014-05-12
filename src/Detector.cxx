#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkMath.h>

#include <assert.h>
#include "Utility.h"
#include "Detector.h"

vtkSmartPointer<vtkPoints>
findNostrilPointsByArea(vtkSmartPointer<vtkPLYReader> reader,
                        double prn[3], double sn[3], 
                        double al[3], double ar[3]) {

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
    double center[3];
    getCenter(x[0], x[1], x[2], center);
    if (center[0] > ar[0] && center[0] < al[0] &&
        center[1] > sn[1] && center[1] < prn[1] &&
        center[2] > 0.0) {
      double area = tarea2(x[0], x[1], x[2]);
      // TODO: replace hard-coded number 4.0 with a dynamic one
      if (area > 4.0) {
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
  std::cout << "max area: " << maxArea << std::endl;
  std::cout << "min area: " << minArea << std::endl;
  //std::cout << "average : " << total / pd->GetNumberOfCells() << std::endl; 

  return nostrilPoints;
}

vtkSmartPointer<vtkPoints>
findNostrilPointsByIncircle(vtkSmartPointer<vtkPLYReader> reader,
                            double prn[3], double sn[3], 
                            double al[3], double ar[3]) {

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

  std::cout << "prn: " << prn[0] << "," << prn[1] << "," << prn[2] << std::endl;
  std::cout << "sn: " << sn[0] << "," << sn[1] << "," << sn[2] << std::endl;
  std::cout << "ar: " << ar[0] << "," << ar[1] << "," << ar[2] << std::endl;
  std::cout << "al: " << al[0] << "," << al[1] << "," << al[2] << std::endl;
  std::cout << "left center: " << lc[0] << "," << lc[1] << "," << lc[2] << std::endl;
  std::cout << "right center: " << rc[0] << "," << rc[1] << "," << rc[2] << std::endl;
  std::cout << "lradius: " << lradius << std::endl;
  std::cout << "rradius: " << rradius << std::endl;

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
    double center[3];
    getCenter(x[0], x[1], x[2], center);

    if (center[2] > 0.0) {
      // TODO: clean up code
      if ((onLeft(x[0], prn, sn) && onLeft(x[1], prn, sn) && onLeft(x[2], prn, sn)) ||
          (!onLeft(x[0], prn, sn) && !onLeft(x[1], prn, sn) && !onLeft(x[2], prn, sn))) {
        bool left = onLeft(center, prn, sn);
        bool inside = false;
        double dist;
        if (left) {
          dist = sqrt(vtkMath::Distance2BetweenPoints(center, lc));
          if (dist < lradius)
            inside = true;
          else
            inside = false;
        } else {
          dist = sqrt(vtkMath::Distance2BetweenPoints(center, rc));
          if (dist < rradius)
            inside = true;
          else
            inside = false;
        }
        if (inside) {
          nostrilPoints->InsertNextPoint(x[0]);
          nostrilPoints->InsertNextPoint(x[1]);
          nostrilPoints->InsertNextPoint(x[2]);
        }
      }
    }
  }
  return nostrilPoints;
}

vtkSmartPointer<vtkPoints>
findNostrilPointsDefault(vtkSmartPointer<vtkPLYReader> reader,
                         double prn[3], double sn[3], 
                         double al[3], double ar[3]) {

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
/*
  std::cout << "prn: " << prn[0] << "," << prn[1] << "," << prn[2] << std::endl;
  std::cout << "sn: " << sn[0] << "," << sn[1] << "," << sn[2] << std::endl;
  std::cout << "ar: " << ar[0] << "," << ar[1] << "," << ar[2] << std::endl;
  std::cout << "al: " << al[0] << "," << al[1] << "," << al[2] << std::endl;
  std::cout << "left center: " << lc[0] << "," << lc[1] << "," << lc[2] << std::endl;
  std::cout << "right center: " << rc[0] << "," << rc[1] << "," << rc[2] << std::endl;
  std::cout << "lradius: " << lradius << std::endl;
  std::cout << "rradius: " << rradius << std::endl;
*/
  ca->InitTraversal();
  vtkIdType npts, *pts;
  int res;
  int lcount = 0, rcount = 0;  // number of meshes in left/right nostril
  double ltotal = 0.0, rtotal = 0.0;
  double lmaxArea = 0.0, lminArea = 1.0E12;
  double rmaxArea = 0.0, rminArea = 1.0E12;

  std::vector<double> larea, rarea;

  while((res = ca->GetNextCell(npts, pts))) {
    assert(npts == 3);
    double x[3][3];
    for (vtkIdType i = 0; i < 3; i++) {
      points->GetPoint(pts[i],x[i]);
    }
    double center[3];
    getCenter(x[0], x[1], x[2], center);

    if (center[2] > 0.0) {
      // TODO: clean up code
      bool left = onLeft(center, prn, sn);
      bool inside = false;
      double dist;
      if (left) {
        dist = sqrt(vtkMath::Distance2BetweenPoints(center, lc));
        if (dist < lradius)
          inside = true;
        else
          inside = false;
      } else {
        dist = sqrt(vtkMath::Distance2BetweenPoints(center, rc));
        if (dist < rradius)
          inside = true;
        else
          inside = false;
      }
      if (inside) {
        double area = tarea2(x[0], x[1], x[2]);
        if (left) {
          larea.push_back(area);
          lcount++;
          ltotal += area;
          if (area > lmaxArea)
            lmaxArea = area;
          if (area < lminArea)
            lminArea = area;
        }
        else {
          rarea.push_back(area);
          rcount++;
          rtotal += area;
          if (area > rmaxArea)
            rmaxArea = area;
          if (area < rminArea)
            rminArea = area;
        }
      }
    }
  }

  ca->InitTraversal();
  while((res = ca->GetNextCell(npts, pts))) {
    assert(npts == 3);
    double x[3][3];
    for (vtkIdType i = 0; i < 3; i++) {
      points->GetPoint(pts[i],x[i]);
    }
    double center[3];
    getCenter(x[0], x[1], x[2], center);

    if (center[2] > 0.0) {
      // TODO: clean up code
      bool left = onLeft(center, prn, sn);
      bool inside = false;
      double dist;
      if (left) {
        dist = sqrt(vtkMath::Distance2BetweenPoints(center, lc));
        if (dist < lradius)
          inside = true;
        else
          inside = false;
      } else {
        dist = sqrt(vtkMath::Distance2BetweenPoints(center, rc));
        if (dist < rradius)
          inside = true;
        else
          inside = false;
      }

      if (inside) {
        double area = tarea2(x[0], x[1], x[2]);
        if (left && area > ltotal/lcount*1.2) {
          nostrilPoints->InsertNextPoint(x[0]);
          nostrilPoints->InsertNextPoint(x[1]);
          nostrilPoints->InsertNextPoint(x[2]);
        }
        else if (!left && area > rtotal/rcount*1.2) {
          nostrilPoints->InsertNextPoint(x[0]);
          nostrilPoints->InsertNextPoint(x[1]);
          nostrilPoints->InsertNextPoint(x[2]);
        }
      }
    }
  }

  std::cout << "left areas: " << std::endl;
  for (std::vector<double>::const_iterator it = larea.begin();
       it != larea.end(); it++) {
    std::cout << *it << std::endl;
  }
  std::cout << "right areas: " << std::endl;
  for (std::vector<double>::const_iterator it = rarea.begin();
       it != rarea.end(); it++) {
    std::cout << *it << std::endl;
  }

  std::cout << "left meshes  : " << lcount << std::endl;
  std::cout << "left max area: " << lmaxArea << std::endl;
  std::cout << "left min area: " << lminArea << std::endl;
  std::cout << "left average : " << ltotal / lcount << std::endl; 
  std::cout << "right meshes  : " << rcount << std::endl;
  std::cout << "right max area: " << rmaxArea << std::endl;
  std::cout << "right min area: " << rminArea << std::endl;
  std::cout << "right average : " << rtotal / rcount << std::endl; 

  return nostrilPoints;
}

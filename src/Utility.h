#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <vtkSmartPointer.h>
#include <vtkClipPolyData.h>
#include <vtkPLYReader.h>

// TODO: Add on const 

// Calculate the area of a triangle determined by points x, y, z,
// using ad hoc methods
//
// Arguments
// - x, y, z: the three points of a triangle
//
// Return
// - the area of the triangle
double tarea(double x[3], double y[3], double z[3]);

// Calculate the area of a triangle determined by points x, y, z,
// using VTK built-in functions
//
// Arguments
// - x, y, z: the three points of a triangle
//
// Return
// - the area of the triangle
double tarea2(double x[3], double y[3], double z[3]);

// Calculate the center of a triangle determined by points x, y, z
//
// Arguments
// - x, y, z: the three points of a triangle
// - center: the return parameter of the triangle center
void getCenter(double x[3], double y[3], double z[3], double center[3]);

// Calculate the center of an incircle in triangle xyz
//
// Arguments
// - x, y, z: the three points of a triangle
// - incenter: the return parameter of the incircle center
//
// Return
// - the radius of the circle
double getIncircleCenter(double x[3], double y[3], double z[3], double incenter[3]);

// Decide point x is on the left/right side of line prn-sn
//
// Arguments
// - x: the point
// - prn, sn: pronasale and sbunasale points
//
// Return
// - true, if the point is on the left side; false otherwise
bool onLeft(double x[3], double prn[3], double sn[3]);

// Get the landmarks from file. It's the user's responsibility to guarantee the
// correctness of the input file format.
//
// Arguments
// - file: the file name from which the landmarks is read out
// - prn, sn, se, acl, acr, al, ar, cl, cr: the return parameters of the landmarks
//
// Return
// - true, if the file opened successfully; false otherwise
bool GetLandmarksFromFile(std::string file, double prn[3], 
                          double sn[3], double se[3], 
                          double acl[3], double acr[3], 
                          double al[3], double ar[3], 
                          double cl[3], double cr[3],
                          double sal[3], double sar[3]);

// Get the nose area clip
//
// Arguments
// - reader: the vtkPLYReader of the 3D face
// - se, cl, cr, al, ar: landmarks used
//
// Return
// - the vtkClipPolyData of the nose area
vtkSmartPointer<vtkClipPolyData> 
getNoseClipper(vtkSmartPointer<vtkPLYReader> reader, double se[3],
               double cl[3], double cr[3], double al[3], double ar[3]);

// Get the projection of a point on the plane given normal
//
// Arguments
// - normal: the normal direction of the projection plane
// - original: the point to be projected
// - projected: return parameter, the projected point onto the plane
void getProjection(const double normal[3], const double original[3], double projected[3]);

// Is the center point in the rectangle and the z > 0 bisector?
//
// Arguments
// - center: the center point of a triangle 
// - left, right, bottom, top: the boundaries of the rectangle
//
// Return
// - true if the center point is in the rectangle and the z > 0 bisector; 
// - false otherwise
bool
isCenterInRect(
  const double center[3],
  const double left,
  const double right,
  const double bottom,
  const double top);

// Are all points in the rectangle and the z > 0 bisector?
//
// Arguments
// - pts: all points in a triangle we want to decide
// - left, right, bottom, top: the boundaries of the rectangle
//
// Return
// - true if all points are in the rectangle and the z > 0 bisector; 
// - false otherwise
bool
isAllPointsInRect(
  const double pts[3][3],
  const double left,
  const double right,
  const double bottom,
  const double top);

#endif  // _UTILITY_H_

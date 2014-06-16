#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <vtkSmartPointer.h>
#include <vtkClipPolyData.h>
#include <vtkPLYReader.h>

// NOTE: BE CAREFUL ABOUT THE LEFT AND RIGHT MEANING

// Calculate the area of a triangle determined by points x, y, z,
// using ad hoc methods
//
// Arguments
// - A, B, C: the vertices of a triangle
//
// Return
// - the area of the triangle
double
tarea(
  const double A[3],
  const double B[3],
  const double C[3]
);

// Calculate the area of a triangle determined by points x, y, z,
// using VTK built-in functions
//
// Arguments
// - A, B, C: the vertices of a triangle
//
// Return
// - the area of the triangle
double
tarea2(
  double A[3],
  double B[3],
  double C[3]
);

// Calculate the center of a triangle determined by points x, y, z
//
// Arguments
// - A, B, C: the vertices of a triangle
// - center: the return parameter of the triangle center
void
getCenter(
  const double A[3],
  const double B[3],
  const double C[3],
  double center[3]
);

// Calculate the center of an incircle in triangle xyz
//
// Arguments
// - A, B, C: the vertices of a triangle
// - incenter: the return parameter of the incircle center
//
// Return
// - the radius of the circle
double
getIncircleCenter(
  double A[3],
  double B[3],
  double C[3],
  double incenter[3]
);

// Decide point x is on the left/right side of line pt1->pt2
//
// Arguments
// - x: the point
// - pt1, pt2: two points on the line, which is pointing from pt1 to pt2
//
// Return
// - true, if the point is on the left side; false otherwise
bool onLeft(
  const double x[3],
  const double pt1[3],
  const double pt2[3]
);

// Get the landmarks from file. It's the user's responsibility to guarantee the
// correctness of the input file format.
//
// Arguments
// - file: the file name from which the landmarks is read out
// - prn, sn, se, acl, acr, al, ar, cl, cr: the return parameters of the landmarks
//
// Return
// - true, if the file opened successfully; false otherwise
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
);

// Get the nose area clip
//
// Arguments
// - reader: the vtkPLYReader object of the 3D face
// - se, cl, cr, al, ar: landmarks used
//
// Return
// - the vtkClipPolyData of the nose area
vtkSmartPointer<vtkClipPolyData> 
getNoseClipper(
  vtkSmartPointer<vtkPLYReader> reader,
  double se[3],
  double cl[3],
  double cr[3],
  double al[3],
  double ar[3]
);

// Get the projection of a point on the plane given normal
//
// Arguments
// - normal: the normal direction of the projection plane
// - original: the point to be projected
// - projected: return parameter, the projected point onto the plane
void
getProjection(
  const double normal[3],
  const double original[3],
  double projected[3]
);

// Is the center point in the rectangle and the z > 0 bisector?
//
// Arguments
// - center: the center point of a triangle mesh
// - left, right, bottom, top: the boundaries of the rectangle
//
// Return
// - true if the center point is in the rectangle and the z > 0 bisector
// - false otherwise
bool
isCenterInRect(
  const double center[3],
  const double left,
  const double right,
  const double bottom,
  const double top
);

// Is the triangle in the rectangle and the z > 0 bisector?
//
// Arguments
// - pts: the vertices of the triangle we want to decide
// - left, right, bottom, top: the boundaries of the rectangle
//
// Return
// - true if the triangle is in the rectangle and the z > 0 bisector
// - false otherwise
bool
isTriangleInRect(
  const double pts[3][3],
  const double left,
  const double right,
  const double bottom,
  const double top
);

// Is the center point "in the diamond" defined by ar, al, sn, prn
//
// Arguments
// - center: the center point of a triangle 
// - left, right, bottom, top: the boundaries of the diamond
//
// Return
// - true if the center point is in the diamond
// - false otherwise
bool
isCenterInDiam(
  const double center[3],
  const double left[3],
  const double right[3],
  const double bottom[3],
  const double top[3]
);

// A triangle defined by points A, B, C is on a plane. Is the projection of a
// point on the plane inside the triangle?
//
// Arguments
// - pt: the point 
// - A, B, C: the vertices of the triangle
//
// Return
// - true if the point projection is inside the triangle
// - false otherwise
bool
isProjectionInTriangle(
  const double pt[3],
  const double A[3],
  const double B[3],
  const double C[3]
);

// Calculate the projection of a point on the plane of a triangle
//
// Arguments
// - pt: the point
// - A, B, C: the vertices of the triangle
// - proj: the return parameter of the projected point
void
getProjectionPoint(
  const double pt[3],
  const double A[3],
  const double B[3],
  const double C[3],
  double proj[3]
);

// Is the point inside the triangle?
//
// Arguments
// - pt: the point 
// - A, B, C: the vertices of the triangle
//
// Return
// - true if the point projection is inside the triangle
// - false otherwise
bool
isPointInTriangle(
  const double pt[3],
  const double A[3],
  const double B[3],
  const double C[3]
);

// Is the triangle "in the diamond" defined by ar, al, sn, prn
//
// Arguments
// - pts: the vertices of the triangle we want to decide
// - left, right, bottom, top: the boundaries of the diamond
//
// Return
// - true if the triangle is in the diamond 
// - false otherwise
bool
isTriangleInDiam(
  const double pts[3][3],
  const double left[3],
  const double right[3],
  const double bottom[3],
  const double top[3]
);

// Is the center point inside either the left or the right incircle
//
// Arguments
// - center: the center point of a triangle 
// - left, right, bottom, top: the boundaries of the diamond
// - lOrigin, lRadius, rOrigin, rRadius: origin and radius of the left and the 
//   right incircle
//
// Return
// - true if the center point is in the incircle 
// - false otherwise
bool
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
);

// Is the triangle inside either the left or the right incircle
//
// Arguments
// - pts: the vertices of the triangle we want to decide
// - left, right, bottom, top: the boundaries of the diamond
// - lOrigin, lRadius, rOrigin, rRadius: origin and radius of the left and the 
//   right incircle
//
// Return
// - true if the triangle is in the incircle 
// - false otherwise
bool
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
);

#endif  // _UTILITY_H_

#ifndef _DETECTOR_H_
#define _DETECTOR_H_

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPLYReader.h>

// Find the vertices located in the nostril area via area threshold
//
// Arguments
// - reader: the vtkPLYReader of the 3D face
//
// Return
// - the points in the nostril area
vtkSmartPointer<vtkPoints>
findNostrilPointsByArea(vtkSmartPointer<vtkPLYReader> reader,
                        double prn[3], double sn[3], 
                        double al[3], double ar[3]);

// Find the vertices located in the nostril area via incircle method
//
// Arguments
// - reader: the vtkPLYReader of the 3D face
// - prn, sn, al, ar: landmarks used
//
// Return
// - the points in the nostril area
vtkSmartPointer<vtkPoints>
findNostrilPointsByIncircle(vtkSmartPointer<vtkPLYReader> reader,
                            double prn[3], double sn[3], 
                            double al[3], double ar[3]);

// Find the vertices located in the nostril area via incircle method 
// and area threshold
//
// Arguments
// - reader: the vtkPLYReader of the 3D face
// - prn, sn, al, ar: landmarks used
//
// Return
// - the points in the nostril area
vtkSmartPointer<vtkPoints>
findNostrilPointsDefault(vtkSmartPointer<vtkPLYReader> reader,
                         double prn[3], double sn[3], 
                         double al[3], double ar[3], double n[3]);

#endif  // _DETECTOR_H_

/*=========================================================================

 Copyright (c) ProxSim ltd., Kwun Tong, Hong Kong. All Rights Reserved.

 See COPYRIGHT.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
 and development was supported by ProxSim ltd.

=========================================================================*/

/**
 * @class   vtkSlicerMarkupsWidgetRepresentation
 * @brief   Class for rendering a markups node
 *
 * This class can display a markups node in the scene.
 * It plays a similar role to vtkWidgetRepresentation, but it is
 * simplified and specialized for optimal use in Slicer.
 * It state is stored in the associated MRML display node to
 * avoid extra synchronization mechanisms.
 * The representation only observes MRML node changes,
 * it does not directly process any interaction events directly
 * (interaction events are processed by vtkMRMLAbstractWidget,
 * which then modifies MRML nodes).
 *
 * This class (and subclasses) are a type of
 * vtkProp; meaning that they can be associated with a vtkRenderer end
 * embedded in a scene like any other vtkActor.
*
 * @sa
 * vtkSlicerMarkupsWidgetRepresentation vtkMRMLAbstractWidget vtkPointPlacer
*/

#ifndef vtkSlicerMarkupsRepresentation_h
#define vtkSlicerMarkupsRepresentation_h

#include "vtkSlicerMarkupsModuleVTKWidgetsExport.h"
#include "vtkMRMLAbstractWidgetRepresentation.h"

#include "vtkMRMLMarkupsDisplayNode.h"
#include "vtkMRMLMarkupsNode.h"

class vtkMapper;
class vtkMarkupsGlyphSource2D;
class vtkPolyData;
class vtkPoints;

class vtkPointPlacer;
class vtkPolyData;
class vtkIdList;
class vtkPointSetToLabelHierarchy;
class vtkSphereSource;
class vtkStringArray;
class vtkTextProperty;

#include "vtkBoundingBox.h"

class ControlPointsPipeline;
class vtkMRMLAbstractViewNode;

class VTK_SLICER_MARKUPS_MODULE_VTKWIDGETS_EXPORT vtkSlicerMarkupsWidgetRepresentation : public vtkMRMLAbstractWidgetRepresentation
{
public:
  /// Standard methods for instances of this class.
  vtkTypeMacro(vtkSlicerMarkupsWidgetRepresentation, vtkMRMLAbstractWidgetRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  // Update the representation from markups node
  void UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData = NULL) VTK_OVERRIDE;

  /// Get the nth node's display position. Will return
  /// 1 on success, or 0 if there are not at least
  /// (n+1) nodes (0 based counting).
  virtual int GetNthNodeDisplayPosition(int n, double pos[2]);

  /// Get the nth node.
  virtual vtkMRMLMarkupsNode::ControlPoint *GetNthNode(int n);

  /// Set/Get the vtkMRMLMarkipsNode connected with this representation
  virtual void SetMarkupsDisplayNode(vtkMRMLMarkupsDisplayNode *markupsDisplayNode);
  virtual vtkMRMLMarkupsDisplayNode* GetMarkupsDisplayNode();
  virtual vtkMRMLMarkupsNode* GetMarkupsNode();

  /// Compute the center by sampling the points along
  /// the polyline of the widget at equal distances.
  /// and it also updates automatically the center pos stored in the Markups node
  virtual void UpdateCenter();

  /// Translation, rotation, scaling will happen around this position
  virtual bool GetTransformationReferencePoint(double referencePointWorld[3]);

  /// Return found component type (as vtkMRMLMarkupsDisplayNode::ComponentType).
  /// closestDistance2 is the squared distance in display coordinates from the closest position where interaction is possible.
  /// componentIndex returns index of the found component (e.g., if control point is found then control point index is returned).
  virtual void CanInteract(const int displayPosition[2], const double worldPosition[3],
    int &foundComponentType, int &foundComponentIndex, double &closestDistance2);

  virtual int FindClosestPointOnWidget(const int displayPos[2], double worldPos[3], int *idx);

  vtkPointPlacer* GetPointPlacer();

protected:
  vtkSlicerMarkupsWidgetRepresentation();
  ~vtkSlicerMarkupsWidgetRepresentation() VTK_OVERRIDE;

  class ControlPointsPipeline
  {
  public:
    ControlPointsPipeline();
    virtual ~ControlPointsPipeline();

    /// Specify the glyph that is displayed at each control point position.
    /// Keep in mind that the shape will be
    /// aligned with the constraining plane by orienting it such that
    /// the x axis of the geometry lies along the normal of the plane.
    //vtkSmartPointer<vtkPolyData> PointMarkerShape;
    vtkSmartPointer<vtkMarkupsGlyphSource2D> GlyphSource2D;
    vtkSmartPointer<vtkSphereSource> GlyphSourceSphere;

    vtkSmartPointer<vtkPolyData> ControlPointsPolyData;
    vtkSmartPointer<vtkPoints> ControlPoints;
    vtkSmartPointer<vtkPolyData> LabelControlPointsPolyData;
    vtkSmartPointer<vtkPoints> LabelControlPoints;
    vtkSmartPointer<vtkPointSetToLabelHierarchy> PointSetToLabelHierarchyFilter;
    vtkSmartPointer<vtkStringArray> Labels;
    vtkSmartPointer<vtkStringArray> LabelsPriority;
    vtkSmartPointer<vtkTextProperty> TextProperty;
  };

  double ControlPointSize;

  virtual void SetMarkupsNode(vtkMRMLMarkupsNode *markupsNode);

  vtkWeakPointer<vtkMRMLMarkupsDisplayNode> MarkupsDisplayNode;
  vtkWeakPointer<vtkMRMLMarkupsNode> MarkupsNode;

  vtkSmartPointer<vtkPointPlacer> PointPlacer;

  vtkTypeBool ClosedLoop;

  /// Convenience method.
  virtual bool GetAllControlPointsVisible();

  /// Convenience method.
  bool GetAllControlPointsSelected();

  // Utility function to build straight lines between control points.
  // If displayPosition is true then positions will be computed in display coordinate system,
  // otherwise in world coordinate system.
  // displayPosition is normally set to true in 2D, and to false in 3D representations.
  void BuildLine(vtkPolyData* linePolyData, bool displayPosition);

  vtkTimeStamp MarkupsTransformModifiedTime;

  enum
  {
    Unselected,
    Selected,
    Active,
    NumberOfControlPointTypes
  };

  double* GetWidgetColor(int controlPointType);

  ControlPointsPipeline* ControlPoints[3]; // Unselected, Selected, Active

private:
  vtkSlicerMarkupsWidgetRepresentation(const vtkSlicerMarkupsWidgetRepresentation&) = delete;
  void operator=(const vtkSlicerMarkupsWidgetRepresentation&) = delete;
};

#endif

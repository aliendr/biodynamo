/**
 * This file contains code generation customizations for class Edge.
 * At the top of this file it defines partial macro applications by fixing the
 * value of a number of arguments. Therefore, this macros are easier to use.
 * https://en.wikipedia.org/wiki/Partial_application
 * At the bottom it executes the customizations, based on two preprocessor
 * variables. (EDGE_NATIVE and EDGE_DEBUG)
 * Documentation of macros and their arguments can be found in the included
 * files!
 *
 * SWIG modules that use the class simply include it using:
 * %include "class_customization/edge.i"
 */

%include "util.i"
%include "cx3d_shared_ptr.i"

%define %Edge_cx3d_shared_ptr()
  %cx3d_shared_ptr(EdgeT_PhysicalNode,
                   ini/cx3d/spatialOrganization/interfaces/Edge,
                   cx3d::spatial_organization::Edge<cx3d::PhysicalNode>);
%enddef

%define %Edge_java()
  %java_defined_class(cx3d::spatial_organization::Edge<cx3d::PhysicalNode>,
                      EdgeT_PhysicalNode,
                      Edge,
                      ini.cx3d.spatialOrganization.interfaces.Edge,
                      ini/cx3d/spatialOrganization/interfaces/Edge);
  %typemap(javainterfaces) cx3d::spatial_organization::Edge<cx3d::PhysicalNode> "ini.cx3d.spatialOrganization.interfaces.Edge<ini.cx3d.physics.PhysicalNode>"
%enddef

%define %Edge_native()
  %native_defined_class(cx3d::spatial_organization::Edge<cx3d::PhysicalNode>,
                        EdgeT_PhysicalNode,
                        ini.cx3d.spatialOrganization.interfaces.Edge,
                        Edge,
                        public EdgeT_PhysicalNode(){});
  %typemap(javainterfaces) cx3d::spatial_organization::Edge<cx3d::PhysicalNode> "ini.cx3d.spatialOrganization.interfaces.Edge<ini.cx3d.physics.PhysicalNode>"
%enddef

/**
 * apply customizations
 */
%Edge_cx3d_shared_ptr();
#ifdef EDGE_NATIVE
  %Edge_native();
#else
  %Edge_java();
#endif
#ifdef EDGE_DEBUG
  %setJavaDebugSwitch(Edge, true);
#else
  %setJavaDebugSwitch(Edge, false);
#endif
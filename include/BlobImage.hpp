#ifndef _BLOBIMAGE_H_
#define _BLOBIMAGE_H_

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <Eigen/Core>

#include "Tools.hpp"
#include "Blob.hpp"
#include "Grid.hpp"
#include "Types.hpp"
#include "GPUKernel.hpp"
#include "SpAlgo.hpp"

using namespace std;
using namespace Eigen;

//! Blob-Image model
/*!  The blob image is an image represented by the translation of
 blobs on grids. This class contains a collection of blob functions
 and corresponding grids. It's the base class for blob based linear
 operator. The name "scale" refers to a given translation invariant
 space. The grid (or lattice) is generated by two independant vectors
 with a fixed sampling step for two directions. Fixing the index
 range, the nodes are bounded in a large parallelogram, and the
 "active nodes" are those in the square/circular ROI. Only these nodes
 contribute to the formation of image.
 */

class BlobImage {

protected :
  int nbNode;		//!< Total number of active nodes of all grids
  int nbScale;		//!< Number of scales, or the number of different blobs
  Array2d sizeObj;	//!< Real object (ROI) size, rectangular
  double diamROI;	//!< Real object (ROI) diameter, together with sizeObj determines the active nodes
  double scaling;	//!< Dilation factor between scales, meaningful for multi-scale model. Not a multi-scale model if scaling <= 1.

public :
  vector<const Blob *> blob;  //!< standard blob basis for each scale
  vector<const Grid *> bgrid;	//!< Grid for each scale. All grids must have the same sizeObj.

  //! Constructor for a single scale blob image, typically the Gaussian blob image.
  BlobImage(const Blob *, const Grid *, const Array2d &sizeObj, double diamROI);

  //! Constructor for multiple scale blob image, typically the Gaussian+DiffGauss, or +MexHat blob image. 
  BlobImage(const vector<const Blob *> &, const vector<const Grid *> &, const Array2d &sizeObj, double diamROI, double scaling);

  //! Copy constructor
  BlobImage(const BlobImage &BI) : blob(BI.blob), bgrid(BI.bgrid), nbNode(BI.nbNode), nbScale(BI.nbScale), sizeObj(BI.sizeObj), diamROI(BI.diamROI), scaling(BI.scaling) {}

  template<class T>  
  vector<T> separate(const T &X) const;
  template<class T> 
  T joint(const vector<T> &Y) const;
  // vector<ArrayXd> separate(const ArrayXd &X) const;
  // ArrayXd joint(const vector<ArrayXd> &Y) const;

  ArrayXd scalewise_NApprx(const ArrayXd &Xr, double beta=0, double spa=1) const;


  //! Interpolation from blob image to pixel image, for visualization only
  ImageXXd blob2pixel(const ArrayXd &X, const Array2i &) const;

  //! Interpolation to gradient pixel image, returns Yx, Yy, for visualization only
  vector<ImageXXd> blob2pixelgrad(const ArrayXd &X, const Array2i &) const;

  //! Interpolation to second order derivate pixel image, returns Yxx, Yxy, Yyx, Yyy for visualization only
  //vector<ArrayXXd> blob2pixelderv2(const ArrayXd &X, const Array2i &) const;

  //! Interpolation  from blob image to multiple pixel images (image of each scale), for visualization only
  vector<ImageXXd> blob2multipixel(const ArrayXd &X, const Array2i &dimObj) const;

  //! Interscale product threshold
  vector<ArrayXd> interscale_product(const ArrayXd &X) const;
  void firstscale_product(const ArrayXd &X, ArrayXd &P) const;
  void interscale_product(const ArrayXd &X, ArrayXd &P) const;

  ArrayXi scalewise_prodmask(const ArrayXd &X, double beta=0, double spa=1) const;
  ArrayXi prodmask(const ArrayXd &X, double spa) const;
  //ArrayXi prod_mask(const ArrayXd &X, double nz) const;

  //! Return the number of active node (all scales included) in a BlobImage
  int get_nbNode() const { return this->nbNode; }

  //! Return the number of total scales
  int get_nbScale() const { return this->nbScale; }

  //! Return the rectangular spatial support of a BlobImage
  Array2d get_sizeObj() const { return this->sizeObj; }

  //! Return the circular spatial support radius of a BlobImage
  double get_diamROI() const { return this->diamROI; }

  //! Return the constant dilation (i.e. scaling) factor between scales (bigger than 1, from coarse to fine)
  double get_scaling() const { return this->scaling; }

  //! Print sparsity information of vector X 
  void sparsity(const ArrayXd &X) const;

  //! Make a L1 norm reweighting mask from given vector and thresh, useful for reweighted L1 reconstruction.
  ArrayXd reweighting_mask(const ArrayXd &X, double rwEpsilon);

  //template<class T> friend T& operator<<(T& out, const BlobImage& BI);
  friend ostream & operator<<(ostream &, const BlobImage &);
  friend ofstream & operator<<(ofstream &, const BlobImage &);

  friend class BlobInterpl;
  friend class BlobProjector;
};

#endif
#include "BezierPatchMesh.hpp"
#include <functional>

namespace rt
{

  BezierPatchMesh::BezierPatchMesh(size_t m,    size_t n,
    size_t resu, size_t resv) : BVHIndexedTriangleMesh(),
  mM(m), mN(n), mResU(resu), mResV(resv)
  {
    // Allocate memory for Bezier points
    mControlPoints.resize(m*n);
  }

  void BezierPatchMesh::initialize()
  {
    //this function samples the underlying continuous patch and tessellates it
    //regularly with triangles

    //sample at triangle vertices at uniform uv parameters
    std::vector<BezierPatchSample> samples; samples.reserve(mResU*mResV);

    for(size_t j=0; j<mResV; ++j)
      for(size_t i=0; i<mResU; ++i)
        samples.push_back(this->sample(double(i) / (mResU-1), double(j) / (mResV-1)));

    //construct triangle strips based on the computed samples
    for(size_t j=0; j<mResV-1; ++j)
    {
      for(size_t i=0; i<mResU-1; ++i)
      {
        const size_t i00 = mResU* j    +  i,
          i10 = mResU* j    + (i+1),
          i01 = mResU*(j+1) +  i,
          i11 = mResU*(j+1) + (i+1);

        {
          //construct lower triangle
          const Vec3d &v0 = samples[i00].position,
            &v1 = samples[i10].position,
            &v2 = samples[i01].position;
          const Vec2d &p0 = samples[i00].uv,
            &p1 = samples[i10].uv,
            &p2 = samples[i01].uv;
          Vec3d  n0 = samples[i00].normal,
            n1 = samples[i10].normal,
            n2 = samples[i01].normal;

          if(!n0.lengthSquared() || !n1.lengthSquared() || !n2.lengthSquared())
          {
            //fall back to triangle normals if normals are not defined
            const Vec3d normal = cross((v1-v0),(v2-v0)).normalize();
            n0 = normal; n1 = normal; n2 = normal;
          }

          int i0=this->addVertex(v0,n0,Vec3d(p0,0));
          int i1=this->addVertex(v1,n1,Vec3d(p1,0));
          int i2=this->addVertex(v2,n2,Vec3d(p2,0));
          this->addTriangle(i0,i1,i2);
        }

        {
          //construct upper triangle
          const Vec3d &v0 = samples[i10].position,
            &v1 = samples[i11].position,
            &v2 = samples[i01].position;
          const Vec2d &p0 = samples[i10].uv,
            &p1 = samples[i11].uv,
            &p2 = samples[i01].uv;
          Vec3d n0 = samples[i10].normal,
            n1 = samples[i11].normal,
            n2 = samples[i01].normal;

          if(!n0.lengthSquared() || !n1.lengthSquared() || !n2.lengthSquared())
          {
            //fall back to triangle normals if normals are not defined
            const Vec3d normal = cross((v1-v0),(v2-v0)).normalize();
            n0 = normal; n1 = normal; n2 = normal;
          }
          int i0=this->addVertex(v0,n0,Vec3d(p0,0));
          int i1=this->addVertex(v1,n1,Vec3d(p1,0));
          int i2=this->addVertex(v2,n2,Vec3d(p2,0));
          this->addTriangle(i0,i1,i2);
        }
      }
    }
    BVHIndexedTriangleMesh::initialize();
  }

  BoundingBox BezierPatchMesh::computeBoundingBox() const
  {
    BoundingBox bbox;
    for (size_t i=0;i<mControlPoints.size();++i)
      bbox.expandByPoint(mControlPoints[i]);
    return bbox;
  }

  BezierPatchMesh::BezierPatchSample BezierPatchMesh::sample(double u, double v) const
  {

    // samples a Bezier curve and curve tangent at parameter t using
    // DeCasteljau algorithm

    BezierPatchSample ret;
    ret.normal = Vec3d();
    ret.uv = Vec2d(u,v);

    //perform DeCasteljau algorithm in both directions
    std::vector<Vec3d> uControlPoints(mM), vControlPoints(mN);
    Vec3d utangent, vtangent;

    //compute v tangent
    {
      for(size_t j=0; j<mN; ++j)
      {
        //fill u curve
        for(size_t i=0; i<mM; ++i)
          uControlPoints[i] = controlPoint(i,j);

        //sample u curve
        vControlPoints[j] = this->deCasteljau(uControlPoints, u).first;
      }

      //sample v tangent
      vtangent = this->deCasteljau(vControlPoints, v).second;
    }

    //compute u tangent and surface point
    {
      for(size_t i=0; i<mM; ++i)
      {
        //fill v curve
        for(size_t j=0; j<mN; ++j)
          vControlPoints[j] = this->controlPoint(i,j);

        //sample u curve
        uControlPoints[i] = this->deCasteljau(vControlPoints, v).first;
      }

      //sample u tangent and surface point
      std::pair<Vec3d,Vec3d> point_tangent = this->deCasteljau(uControlPoints, u);
      ret.position = point_tangent.first;
      utangent  = point_tangent.second;
    }

    //compute normal vector from surface tangents
    ret.normal = cross(utangent,vtangent).normalize();

    return ret;
  }


  std::pair<Vec3d,Vec3d> BezierPatchMesh::deCasteljau(const std::vector<Vec3d> &curvePoints, double t) const
  {
    //copy control points to workspace for recursion
    std::vector<Vec3d> workSpace=curvePoints;

    //call recursive method for final two points
    this->deCasteljauRec(workSpace, t);

    Vec3d point   = (1-t) * workSpace[0] + t * workSpace[1];
    Vec3d tangent = (workSpace[1] - workSpace[0]) * double(curvePoints.size());

    return std::make_pair(point, tangent);
  }

  void BezierPatchMesh::deCasteljauRec(std::vector<Vec3d> &points, double t) const
  {
    const size_t n = points.size();

    if (n == 2)
      return;

    for(size_t i=0; i<n-1; ++i)
    {
      points[i] *= (1-t);
      points[i] += t * points[i+1];
    }

    points.resize(n-1);

    return this->deCasteljauRec(points, t);
  }

} //namespace rt

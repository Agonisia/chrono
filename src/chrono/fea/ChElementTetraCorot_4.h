// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Andrea Favali, Alessandro Tasora
// =============================================================================

#ifndef CH_ELEMENT_TETRA_COROT_4_H
#define CH_ELEMENT_TETRA_COROT_4_H

#include <cmath>

#include "chrono/fea/ChElementTetrahedron.h"
#include "chrono/fea/ChElementGeneric.h"
#include "chrono/fea/ChElementCorotational.h"
#include "chrono/fea/ChContinuumPoisson3D.h"
#include "chrono/fea/ChNodeFEAxyz.h"
#include "chrono/fea/ChNodeFEAxyzP.h"
#include "chrono/core/ChTensors.h"

namespace chrono {
namespace fea {

/// @addtogroup fea_elements
/// @{

/// Tetrahedron FEA element with 4 nodes.
/// This is a classical element with linear displacement, hence with constant stress and constant strain. It can be
/// easily used for 3D FEA problems.
class ChApi ChElementTetraCorot_4 : public ChElementTetrahedron,
                                    public ChElementGeneric,
                                    public ChElementCorotational,
                                    public ChLoadableUVW {
  public:
    using ShapeVector = ChMatrixNM<double, 1, 4>;

    ChElementTetraCorot_4();
    ~ChElementTetraCorot_4();

    virtual unsigned int GetNumNodes() override { return 4; }
    virtual unsigned int GetNumCoordsPosLevel() override { return 4 * 3; }
    virtual unsigned int GetNodeNumCoordsPosLevel(unsigned int n) override { return 3; }

    double GetVolume() { return Volume; }

    virtual std::shared_ptr<ChNodeFEAbase> GetNode(unsigned int n) override { return nodes[n]; }

    /// Return the specified tetrahedron node (0 <= n <= 3).
    virtual std::shared_ptr<ChNodeFEAxyz> GetTetrahedronNode(unsigned int n) override { return nodes[n]; }

    virtual void SetNodes(std::shared_ptr<ChNodeFEAxyz> nodeA,
                          std::shared_ptr<ChNodeFEAxyz> nodeB,
                          std::shared_ptr<ChNodeFEAxyz> nodeC,
                          std::shared_ptr<ChNodeFEAxyz> nodeD);

    //
    // FEM functions
    //

    /// Update element at each time step.
    virtual void Update() override;

    /// Fills the N shape function matrix with the
    /// values of shape functions at r,s,t 'volume' coordinates, where
    /// r=1 at 2nd vertex, s=1 at 3rd, t = 1 at 4th. All ranging in [0...1].
    /// The last (u, u=1 at 1st vertex) is computed form the first 3 as 1.0-r-s-t.
    /// NOTE! actually N should be a 3row, 12 column sparse matrix,
    /// as  N = [n1*eye(3) n2*eye(3) n3*eye(3) n4*eye(3)]; ,
    /// but to avoid wasting zero and repeated elements, here
    /// it stores only the n1 n2 n3 n4 values in a 1 row, 4 columns matrix.
    void ShapeFunctions(ShapeVector& N, double r, double s, double t);

    /// Fills the D vector (displacement) with the current field values at the nodes of the element, with proper
    /// ordering. If the D vector has not the size of this->GetNumCoordsPosLevel(), it will be resized.For corotational
    /// elements, field is assumed in local reference!
    virtual void GetStateBlock(ChVectorDynamic<>& mD) override;

    double ComputeVolume();

    /// Computes the local STIFFNESS MATRIX of the element:
    /// K = Volume * [B]' * [D] * [B]
    virtual void ComputeStiffnessMatrix();

    /// compute large rotation of element for corotational approach
    virtual void UpdateRotation() override;

    /// Sets H as the global stiffness matrix K, scaled  by Kfactor. Optionally, also
    /// superimposes global damping matrix R, scaled by Rfactor, and global mass matrix M multiplied by Mfactor.
    virtual void ComputeKRMmatricesGlobal(ChMatrixRef H,
                                          double Kfactor,
                                          double Rfactor = 0,
                                          double Mfactor = 0) override;

    /// Computes the internal forces (ex. the actual position of nodes is not in relaxed reference position) and set
    /// values in the Fi vector.
    virtual void ComputeInternalForces(ChVectorDynamic<>& Fi) override;

    //
    // Custom properties functions
    //

    /// Set the material of the element
    void SetMaterial(std::shared_ptr<ChContinuumElastic> my_material) { Material = my_material; }
    std::shared_ptr<ChContinuumElastic> GetMaterial() { return Material; }

    /// Get the partial derivatives matrix MatrB and the StiffnessMatrix
    const ChMatrixDynamic<>& GetMatrB() const { return MatrB; }
    const ChMatrixDynamic<>& GetStiffnessMatrix() const { return StiffnessMatrix; }

    /// Returns the strain tensor (note that the tetrahedron 4 nodes is a linear
    /// element, thus the strain is constant in the entire volume).
    /// The tensor is in the original undeformed unrotated reference.
    ChStrainTensor<> GetStrain();

    /// Returns the stress tensor (note that the tetrahedron 4 nodes is a linear
    /// element, thus the stress is constant in the entire volume).
    /// The tensor is in the original undeformed unrotated reference.
    ChStressTensor<> GetStress();

    /// This function computes and adds corresponding masses to ElementBase member m_TotalMass
    void ComputeNodalMass() override;

    //
    // Functions for interfacing to the solver
    //            (***not needed, thank to bookkeeping in parent class ChElementGeneric)

    //
    // Functions for ChLoadable interface
    //

    /// Gets the number of DOFs affected by this element (position part)
    virtual unsigned int GetLoadableNumCoordsPosLevel() override { return 4 * 3; }

    /// Gets the number of DOFs affected by this element (speed part)
    virtual unsigned int GetLoadableNumCoordsVelLevel() override { return 4 * 3; }

    /// Gets all the DOFs packed in a single vector (position part)
    virtual void LoadableGetStateBlockPosLevel(int block_offset, ChState& mD) override;

    /// Gets all the DOFs packed in a single vector (speed part)
    virtual void LoadableGetStateBlockVelLevel(int block_offset, ChStateDelta& mD) override;

    /// Increment all DOFs using a delta.
    virtual void LoadableStateIncrement(const unsigned int off_x,
                                        ChState& x_new,
                                        const ChState& x,
                                        const unsigned int off_v,
                                        const ChStateDelta& Dv) override;

    /// Number of coordinates in the interpolated field: here the {x,y,z} displacement
    virtual unsigned int GetNumFieldCoords() override { return 3; }

    /// Get the number of DOFs sub-blocks.
    virtual unsigned int GetNumSubBlocks() override { return 4; }

    /// Get the offset of the specified sub-block of DOFs in global vector.
    virtual unsigned int GetSubBlockOffset(unsigned int nblock) override {
        return nodes[nblock]->NodeGetOffsetVelLevel();
    }

    /// Get the size of the specified sub-block of DOFs in global vector.
    virtual unsigned int GetSubBlockSize(unsigned int nblock) override { return 3; }

    /// Check if the specified sub-block of DOFs is active.
    virtual bool IsSubBlockActive(unsigned int nblock) const override { return !nodes[nblock]->IsFixed(); }

    /// Get the pointers to the contained ChVariables, appending to the mvars vector.
    virtual void LoadableGetVariables(std::vector<ChVariables*>& mvars) override;

    /// Evaluate N'*F , where N is some type of shape function
    /// evaluated at U,V,W coordinates of the volume, each ranging in 0..+1
    /// F is a load, N'*F is the resulting generalized load
    /// Returns also det[J] with J=[dx/du,..], that might be useful in gauss quadrature.
    virtual void ComputeNF(const double U,              ///< parametric coordinate in volume
                           const double V,              ///< parametric coordinate in volume
                           const double W,              ///< parametric coordinate in volume
                           ChVectorDynamic<>& Qi,       ///< Return result of N'*F  here, maybe with offset block_offset
                           double& detJ,                ///< Return det[J] here
                           const ChVectorDynamic<>& F,  ///< Input F vector, size is = n.field coords.
                           ChVectorDynamic<>* state_x,  ///< if != 0, update state (pos. part) to this, then evaluate Q
                           ChVectorDynamic<>* state_w   ///< if != 0, update state (speed part) to this, then evaluate Q
                           ) override;

    /// This is needed so that it can be accessed by ChLoaderVolumeGravity
    virtual double GetDensity() override { return this->Material->GetDensity(); }

    /// If true, use quadrature over u,v,w in [0..1] range as tetrahedron volumetric coords, with z=1-u-v-w
    /// otherwise use quadrature over u,v,w in [-1..+1] as box isoparametric coords.
    virtual bool IsTetrahedronIntegrationNeeded() override { return true; }

  private:
    /// Initial setup: set up the element's parameters and matrices
    virtual void SetupInitial(ChSystem* system) override;

    std::vector<std::shared_ptr<ChNodeFEAxyz> > nodes;
    std::shared_ptr<ChContinuumElastic> Material;
    ChMatrixDynamic<> MatrB;            // matrix of shape function's partial derivatives
    ChMatrixDynamic<> StiffnessMatrix;  // undeformed local stiffness matrix
    ChMatrixNM<double, 4, 4> mM;        // for speeding up corotational approach
    double Volume;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

// -----------------------------------------------------------------------------

/// Tetrahedron FEM element with 4 nodes for scalar fields (for Poisson-like problems).
/// This is a classical element with linear displacement.
/// ***EXPERIMENTAL***
class ChApi ChElementTetraCorot_4_P : public ChElementGeneric, public ChElementCorotational, public ChLoadableUVW {
  public:
    using ShapeVector = ChMatrixNM<double, 1, 4>;

    ChElementTetraCorot_4_P();
    ~ChElementTetraCorot_4_P() {}

    virtual unsigned int GetNumNodes() override { return 4; }
    virtual unsigned int GetNumCoordsPosLevel() override { return 4 * 1; }
    virtual unsigned int GetNodeNumCoordsPosLevel(unsigned int n) override { return 1; }

    double GetVolume() { return Volume; }

    virtual std::shared_ptr<ChNodeFEAbase> GetNode(unsigned int n) override { return nodes[n]; }

    virtual void SetNodes(std::shared_ptr<ChNodeFEAxyzP> nodeA,
                          std::shared_ptr<ChNodeFEAxyzP> nodeB,
                          std::shared_ptr<ChNodeFEAxyzP> nodeC,
                          std::shared_ptr<ChNodeFEAxyzP> nodeD);

    //
    // FEM functions
    //

    /// Update element at each time step.
    virtual void Update() override;

    /// Fills the N shape function matrix with the
    /// values of shape functions at zi parametric coordinates, where
    /// z0=1 at 1st vertex, z1=1 at second, z2 = 1 at third (volumetric shape functions).
    /// The 4th is computed form the first 3.  All ranging in [0...1].
    /// NOTE! actually N should be a 3row, 12 column sparse matrix,
    /// as  N = [n1*eye(3) n2*eye(3) n3*eye(3) n4*eye(3)]; ,
    /// but to avoid wasting zero and repeated elements, here
    /// it stores only the n1 n2 n3 n4 values in a 1 row, 4 columns matrix!
    virtual void ShapeFunctions(ShapeVector& N, double z0, double z1, double z2);

    /// Fills the D vector with the current field values at the nodes of the element, with proper ordering. If the D
    /// vector has not the size of this->GetNumCoordsPosLevel(), it will be resized. For corotational elements, field is
    /// assumed in local reference!
    virtual void GetStateBlock(ChVectorDynamic<>& D) override;

    double ComputeVolume();

    /// Computes the local STIFFNESS MATRIX of the element:
    /// K = Volume * [B]' * [D] * [B]
    virtual void ComputeStiffnessMatrix();

    // compute large rotation of element for corotational approach
    // Btw: NOT really needed for Poisson problems
    virtual void UpdateRotation() override;

    /// Sets H as the global stiffness matrix K, scaled  by Kfactor. Optionally, also
    /// superimposes global damping matrix R, scaled by Rfactor, and global mass matrix M multiplied by Mfactor.
    virtual void ComputeKRMmatricesGlobal(ChMatrixRef H,
                                          double Kfactor,
                                          double Rfactor = 0,
                                          double Mfactor = 0) override;

    /// Computes the internal 'pseudo-forces' and set values in the Fi vector.
    virtual void ComputeInternalForces(ChVectorDynamic<>& Fi) override;

    //
    // Custom properties functions
    //

    /// Set the material of the element
    void SetMaterial(std::shared_ptr<ChContinuumPoisson3D> my_material) { Material = my_material; }
    std::shared_ptr<ChContinuumPoisson3D> GetMaterial() { return Material; }

    /// Get the partial derivatives matrix MatrB and the StiffnessMatrix
    const ChMatrixDynamic<>& GetMatrB() const { return MatrB; }
    const ChMatrixDynamic<>& GetStiffnessMatrix() const { return StiffnessMatrix; }

    /// Returns the gradient of P (note that the tetrahedron 4 nodes is a linear
    /// element, thus the gradient is constant in the entire volume).
    /// It is in the original undeformed unrotated reference.
    ChVectorN<double, 3> GetPgradient();

    //
    // Functions for interfacing to the solver
    //            (***not needed, thank to bookkeeping in parent class ChElementGeneric)

    //
    // Functions for ChLoadable interface
    //

    /// Gets the number of DOFs affected by this element (position part)
    virtual unsigned int GetLoadableNumCoordsPosLevel() override { return 4 * 3; }

    /// Gets the number of DOFs affected by this element (speed part)
    virtual unsigned int GetLoadableNumCoordsVelLevel() override { return 4 * 3; }

    /// Gets all the DOFs packed in a single vector (position part)
    virtual void LoadableGetStateBlockPosLevel(int block_offset, ChState& mD) override;

    /// Gets all the DOFs packed in a single vector (speed part)
    virtual void LoadableGetStateBlockVelLevel(int block_offset, ChStateDelta& mD) override;

    /// Increment all DOFs using a delta.
    virtual void LoadableStateIncrement(const unsigned int off_x,
                                        ChState& x_new,
                                        const ChState& x,
                                        const unsigned int off_v,
                                        const ChStateDelta& Dv) override;

    /// Number of coordinates in the interpolated field: here the {t} temperature
    virtual unsigned int GetNumFieldCoords() override { return 1; }

    /// Get the number of DOFs sub-blocks.
    virtual unsigned int GetNumSubBlocks() override { return 4; }

    /// Get the offset of the specified sub-block of DOFs in global vector.
    virtual unsigned int GetSubBlockOffset(unsigned int nblock) override {
        return nodes[nblock]->NodeGetOffsetVelLevel();
    }

    /// Get the size of the specified sub-block of DOFs in global vector.
    virtual unsigned int GetSubBlockSize(unsigned int nblock) override { return 1; }

    /// Check if the specified sub-block of DOFs is active.
    virtual bool IsSubBlockActive(unsigned int nblock) const override { return !nodes[nblock]->IsFixed(); }

    /// Get the pointers to the contained ChVariables, appending to the mvars vector.
    virtual void LoadableGetVariables(std::vector<ChVariables*>& mvars) override;

    /// Evaluate N'*F , where N is some type of shape function
    /// evaluated at U,V,W coordinates of the volume, each ranging in 0..+1
    /// F is a load, N'*F is the resulting generalized load
    /// Returns also det[J] with J=[dx/du,..], that might be useful in gauss quadrature.
    virtual void ComputeNF(const double U,              ///< parametric coordinate in volume
                           const double V,              ///< parametric coordinate in volume
                           const double W,              ///< parametric coordinate in volume
                           ChVectorDynamic<>& Qi,       ///< Return result of N'*F  here, maybe with offset block_offset
                           double& detJ,                ///< Return det[J] here
                           const ChVectorDynamic<>& F,  ///< Input F vector, size is = n.field coords.
                           ChVectorDynamic<>* state_x,  ///< if != 0, update state (pos. part) to this, then evaluate Q
                           ChVectorDynamic<>* state_w   ///< if != 0, update state (speed part) to this, then evaluate Q
                           ) override;

    /// Return 0 if not supportable by ChLoaderVolumeGravity
    virtual double GetDensity() override { return 0; }

    /// If true, use quadrature over u,v,w in [0..1] range as tetrahedron volumetric coords, with z=1-u-v-w
    /// otherwise use quadrature over u,v,w in [-1..+1] as box isoparametric coords.
    virtual bool IsTetrahedronIntegrationNeeded() override { return true; }

  private:
    /// Initial setup: set up the element's parameters and matrices
    virtual void SetupInitial(ChSystem* system) override;

    std::vector<std::shared_ptr<ChNodeFEAxyzP> > nodes;
    std::shared_ptr<ChContinuumPoisson3D> Material;
    ChMatrixDynamic<> MatrB;            // matrix of shape function's partial derivatives
    ChMatrixDynamic<> StiffnessMatrix;  // local stiffness matrix
    ChMatrixNM<double, 4, 4> mM;        // for speeding up corotational approach
    double Volume;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/// @} fea_elements

}  // end namespace fea
}  // end namespace chrono

#endif

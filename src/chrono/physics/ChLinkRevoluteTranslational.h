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
// Authors: Radu Serban
// =============================================================================

#ifndef CH_LINK_REVOLUTE_TRANSLATIONAL_H
#define CH_LINK_REVOLUTE_TRANSLATIONAL_H

#include "chrono/physics/ChLink.h"
#include "chrono/physics/ChBody.h"
#include "chrono/solver/ChConstraintTwoBodies.h"

namespace chrono {

/// Class for modeling a composite revolute-translational joint between two
/// ChBodyFrame objects.  This joint is defined through a point and
/// direction on the first body (the revolute side), a point and two mutually
/// orthogonal directions on the second body (the translational side),
/// and a distance.
class ChApi ChLinkRevoluteTranslational : public ChLink {
  public:
    ChLinkRevoluteTranslational();
    ChLinkRevoluteTranslational(const ChLinkRevoluteTranslational& other);
    ~ChLinkRevoluteTranslational() {}

    /// "Virtual" copy constructor (covariant return type).
    virtual ChLinkRevoluteTranslational* Clone() const override { return new ChLinkRevoluteTranslational(*this); }

    /// Get the number of (bilateral) constraints introduced by this joint.
    virtual unsigned int GetNumConstraintsBilateral() override { return 4; }

    /// Get the imposed distance (length of massless connector).
    double GetImposedDistance() const { return m_dist; }

    /// Get the current distance between the two points.
    double GetCurrentDistance() const { return m_cur_dist; }

    /// Get the point on m_body1 (revolute side), expressed in absolute coordinate system.
    ChVector3d GetPoint1Abs() const { return m_body1->TransformPointLocalToParent(m_p1); }

    /// Get the direction of the revolute joint, expressed in absolute coordinate system.
    ChVector3d GetDirZ1Abs() const { return m_body1->TransformDirectionLocalToParent(m_z1); }

    /// Get the point on m_body2 (translational side), expressed in absolute coordinate system.
    ChVector3d GetPoint2Abs() const { return m_body2->TransformPointLocalToParent(m_p2); }

    /// Get the first direction of the translational joint, expressed in absolute coordinate system.
    /// The translational axis is orthogonal to the direction.
    ChVector3d GetDirX2Abs() const { return m_body2->TransformDirectionLocalToParent(m_x2); }

    /// Get the second direction of the translational joint, expressed in absolute coordinate system.
    /// The translational axis is orthogonal to the direction.
    ChVector3d GetDirY2Abs() const { return m_body2->TransformDirectionLocalToParent(m_y2); }

    /// Get the link frame 1, relative to body 1.
    /// This frame, defined on body 1 (the revolute side), is centered at the revolute joint location, has its X axis
    /// along the joint connector, and its Z axis aligned with the revolute axis.
    virtual ChFrame<> GetFrame1Rel() const override;

    /// Get the link frame 2, relative to body 2.
    virtual ChFrame<> GetFrame2Rel() const override;

    /// Get the reaction force and torque on the 1st body, expressed in the link frame 1.
    /// NOT YET IMPLEMENTED!
    virtual ChWrenchd GetReaction1() const override;

    /// Get the reaction force and torque on the 2nd body, expressed in the link frame 2.
    /// NOT YET IMPLEMENTED!
    virtual ChWrenchd GetReaction2() const override;

    /// Get the joint violation (residuals of the constraint equations)
    virtual ChVectorDynamic<> GetConstraintViolation() const override { return m_C; }

    /// Initialize this joint by specifying the two bodies to be connected, a
    /// coordinate system specified in the absolute frame, and the distance of
    /// the massless connector.  The composite joint is constructed such that the
    /// revolute joint is centered at the origin of the specified coordinate system.
    /// The revolute joint rotates about the z axis, the translational joint moves
    /// along the y axis, and the translation axis is at the specified distance
    /// along the x axis.
    void Initialize(std::shared_ptr<ChBody> body1,  ///< first frame (revolute side)
                    std::shared_ptr<ChBody> body2,  ///< second frame (translational side)
                    const ChCoordsys<>& csys,       ///< joint coordinate system (in absolute frame)
                    double distance                 ///< imposed distance
    );

    /// Initialize this joint by specifying the two bodies to be connected, a point
    /// and a direction on body1 defining the revolute joint, and two directions and
    /// a point on the second body defining the translational joint. If local = true,
    /// it is assumed that these quantities are specified in the local body frames.
    /// Otherwise, it is assumed that they are specified in the absolute frame. The
    /// imposed distance between the two points can be either inferred from the provided
    /// configuration (auto_distance = true) or specified explicitly.
    void Initialize(std::shared_ptr<ChBody> body1,  ///< first frame (revolute side)
                    std::shared_ptr<ChBody> body2,  ///< second frame (spherical side)
                    bool local,                     ///< true if data given in body local frames
                    const ChVector3d& p1,           ///< point on first frame (revolute side)
                    const ChVector3d& dirZ1,        ///< direction of revolute on first frame
                    const ChVector3d& p2,           ///< point on second frame (translational side)
                    const ChVector3d& dirX2,        ///< first direction of translational joint
                    const ChVector3d& dirY2,        ///< second direction of translational joint
                    bool auto_distance = true,      ///< true if imposed distance equal to distance between axes
                    double distance = 0             ///< imposed distance (used only if auto_distance = false)
    );

    /// Perform the update of this joint at the specified time: compute jacobians,
    /// constraint violations, etc. and cache in internal structures
    virtual void Update(double time, bool update_assets) override;

    /// Method to allow serialization of transient data to archives.
    virtual void ArchiveOut(ChArchiveOut& archive_out) override;

    /// Method to allow deserialization of transient data from archives.
    virtual void ArchiveIn(ChArchiveIn& archive_in) override;

  private:
    ChVector3d m_p1;  ///< point on first frame (in local frame)
    ChVector3d m_p2;  ///< point on second frame (in local frame)
    ChVector3d m_z1;  ///< direction of revolute on first frame (in local frame)
    ChVector3d m_x2;  ///< first direction of translational on second frame (in local frame)
    ChVector3d m_y2;  ///< first direction of translational on second frame (in local frame)
    double m_dist;    ///< imposed distance between rotational and translation axes

    double m_cur_par1;  ///< actual value of par1 constraint
    double m_cur_par2;  ///< actual value of par2 constraint
    double m_cur_dot;   ///< actual value of dot constraint
    double m_cur_dist;  ///< actual distance between pos1 and pos2

    ChConstraintTwoBodies m_cnstr_par1;  ///< constraint: z1 perpendicualr to x2
    ChConstraintTwoBodies m_cnstr_par2;  ///< constraint: z1 perpendicular to y2
    ChConstraintTwoBodies m_cnstr_dot;   ///< constraint: d12 perpendicular to z1
    ChConstraintTwoBodies m_cnstr_dist;  ///< constraint: distance between axes

    ChVectorN<double, 4> m_C;  ///< current constraint violations

    // Note that the order of the Lagrange multipliers corresponds to the following
    // order of the constraints: par1, par2, dot, dist.
    double m_multipliers[4];  ///< Lagrange multipliers

    // Solver and integrator interface functions

    virtual void IntStateGatherReactions(const unsigned int off_L, ChVectorDynamic<>& L) override;
    virtual void IntStateScatterReactions(const unsigned int off_L, const ChVectorDynamic<>& L) override;
    virtual void IntLoadResidual_CqL(const unsigned int off_L,
                                     ChVectorDynamic<>& R,
                                     const ChVectorDynamic<>& L,
                                     const double c) override;
    virtual void IntLoadConstraint_C(const unsigned int off,
                                     ChVectorDynamic<>& Qc,
                                     const double c,
                                     bool do_clamp,
                                     double recovery_clamp) override;
    virtual void IntToDescriptor(const unsigned int off_v,
                                 const ChStateDelta& v,
                                 const ChVectorDynamic<>& R,
                                 const unsigned int off_L,
                                 const ChVectorDynamic<>& L,
                                 const ChVectorDynamic<>& Qc) override;
    virtual void IntFromDescriptor(const unsigned int off_v,
                                   ChStateDelta& v,
                                   const unsigned int off_L,
                                   ChVectorDynamic<>& L) override;

    virtual void InjectConstraints(ChSystemDescriptor& descriptor) override;
    virtual void ConstraintsBiReset() override;
    virtual void ConstraintsBiLoad_C(double factor = 1, double recovery_clamp = 0.1, bool do_clamp = false) override;
    virtual void LoadConstraintJacobians() override;
    virtual void ConstraintsFetch_react(double factor = 1) override;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

CH_CLASS_VERSION(ChLinkRevoluteTranslational, 0)

}  // end namespace chrono

#endif

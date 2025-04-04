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
// Authors: Alessandro Tasora, Radu Serban
// =============================================================================

#ifndef CHSHAFTSPLANETARY_H
#define CHSHAFTSPLANETARY_H

#include "chrono/physics/ChPhysicsItem.h"
#include "chrono/physics/ChShaft.h"
#include "chrono/solver/ChConstraintThreeGeneric.h"

namespace chrono {

// Forward references (for parent hierarchy pointer)
class ChShaft;

/// Class for defining a planetary gear between three one-degree-of-freedom parts.
/// Planetary gears can be used to make, for instance, the differentials of cars. While traditional gear reducers have
/// one input and one output, the planetary gear have two inputs and one output (or, if you prefer, one input and two
/// outputs). Note that you can use this class also to make a gearbox if you are interested in knowing the reaction
/// torque transmitted to the truss (whereas the basic ChLinkLockGear cannot do this because it has only in and out); in
/// this case you just use the shaft n.1 as truss and fix it.
class ChApi ChShaftsPlanetary : public ChPhysicsItem {
  public:
    ChShaftsPlanetary();
    ChShaftsPlanetary(const ChShaftsPlanetary& other);
    ~ChShaftsPlanetary() {}

    /// "Virtual" copy constructor (covariant return type).
    virtual ChShaftsPlanetary* Clone() const override { return new ChShaftsPlanetary(*this); }

    /// Initialize this planetary gear, given three shafts to join.
    /// Although there's no special requirement, you may think of the three
    /// typical moving parts of an epicycloidal reducer: the carrier, the
    /// input gear, and the gear with inner teeth that usually is kept fixed (but the
    /// ChShaftsPlanetary does not require that one shaft is fixed - it's up to you)
    /// Both shafts must belong to the same ChSystem.
    bool Initialize(std::shared_ptr<ChShaft> shaft_1,  ///< first shaft to join (carrier wheel)
                    std::shared_ptr<ChShaft> shaft_2,  ///< second shaft to join (wheel)
                    std::shared_ptr<ChShaft> shaft_3   ///< third shaft to join (wheel)
    );

    /// Disable this element (disable constraints).
    void SetDisabled(bool val) { active = !val; }

    /// Get the first shaft (carrier wheel).
    ChShaft* GetShaft1() const { return shaft1; }

    /// Get the second shaft.
    ChShaft* GetShaft2() const { return shaft2; }

    /// Get the third shaft.
    ChShaft* GetShaft3() const { return shaft3; }

    /// Return the speed of the first shaft (carrier wheel).
    double GetSpeedShaft1() const { return shaft1->GetPosDt(); }

    /// Return the speed of the second shaft.
    double GetSpeedShaft2() const { return shaft2->GetPosDt(); }

    /// Return the speed of the third shaft.
    double GetSpeedShaft3() const { return shaft3->GetPosDt(); }

    /// Set the transmission ratios r1 r2 r3 as in r1*w1 + r2*w2 + r3*w3 = 0.
    /// For example, for the car differential, if you assume that shaft 1 is
    /// the carrier and shafts 2 and 3 go to the wheel hubs, you must use
    /// r1=-2, r2=1, r3=1 to satisfy the kinematics -2*w1+w2+w3=0 of the differential;
    /// equivalently, you may use r1=1, r2=-0.5, r3=-0.5 (the equation would hold the same).
    void SetTransmissionRatios(double r_1, double r_2, double r_3) {
        r1 = r_1;
        r2 = r_2;
        r3 = r_3;
    }

    /// Setting the transmission ratios r1 r2 r3 for  r1*w1 + r2*w2 + r3*w3 = 0
    /// may be cumbersome, but when you deal with typical planetary devices, this
    /// function provides a shortcut to setting them for you, given a single
    /// parameter t0, that is the speed ratio t'=w3'/w2' of the inverted planetary.
    /// That ratio is simple to get: to invert the planetary, imagine to hold fixed
    /// the carrier of shaft 1 (that is w1' =0), move the shaft 2 and see which is
    /// the speed of shaft 3, to get the ratio t0=w3'/w2'. Generally, shaft 1 is
    /// called the 'carrier'. For example, in normal operation of an epicycloidal
    /// reducer, the carrier (shaft 1) is used as output, shaft 2 is the input, and
    /// shaft 3 is hold fixed to get one degree of freedom only; but in 'inverted' operation
    /// imagine the carrier is fixed, so t0 can be easily got as t0=-z2/z3, with z=n.of teeth.
    /// In a car differential, again with shaft 1 as carrier, one can see that t0=w3'/w2'
    /// so t0=-1.     See the Willis theory for more details on these formulas.
    /// Note that t0 should be different from 1 (singularity).
    /// Once you get t0, simply use this function and it will set r1 r2 r3 automatically.
    void SetTransmissionRatioOrdinary(double t0) {
        r1 = (1. - t0);
        r2 = t0;
        r3 = -1.0;
    }

    /// Get the t0 transmission ratio of the equivalent ordinary gearbox (i.e., the inverted planetary).
    /// This is the ratio  t0=w3'/w2' assuming that the carrier (shaft 1) is held fixed.
    double GetTransmissionRatioOrdinary() const { return -r2 / r3; }

    /// Get the transmission ratio r1, as in  r1*w1+r2*w2+r3*w3 = 0.
    double GetTransmissionR1() const { return r1; }

    /// Get the transmission ratio r1, as in  r1*w1+r2*w2+r3*w3 = 0.
    double GetTransmissionR2() const { return r2; }

    /// Get the transmission ratio r1, as in  r1*w1+r2*w2+r3*w3 = 0.
    double GetTransmissionR3() const { return r3; }

    /// Enable phase drift avoidance (default: true).
    /// If true, phasing is always tracked and the constraint is satisfied also at the position level.
    /// If false, microslipping can accumulate (as in friction wheels).
    void AvoidPhaseDrift(bool avoid) { avoid_phase_drift = avoid; }

    /// Get the reaction torque considered as applied to the 1st axis.
    double GetReaction1() const { return (r1 * torque_react); }

    /// Get the reaction torque considered as applied to the 2nd axis.
    double GetReaction2() const { return (r2 * torque_react); }

    /// Get the reaction torque considered as applied to the 3rd axis.
    double GetTorqueReactionOn3() const { return (r3 * torque_react); }

    /// Method to allow serialization of transient data to archives.
    virtual void ArchiveOut(ChArchiveOut& archive_out) override;

    /// Method to allow deserialization of transient data from archives.
    virtual void ArchiveIn(ChArchiveIn& archive_in) override;

  private:
    bool active;

    double r1;  ///< transmission ratios  as in   r1*w1 + r2*w2 + r3*w3 = 0
    double r2;
    double r3;

    double torque_react;  ///< shaft reaction torque

    bool avoid_phase_drift;
    double phase1;
    double phase2;
    double phase3;

    ChConstraintThreeGeneric constraint;  ///< used as an interface to the solver

    ChShaft* shaft1;  ///< first connected shaft
    ChShaft* shaft2;  ///< second connected shaft
    ChShaft* shaft3;  ///< third connected shaft

    /// Get the number of scalar variables affected by constraints in this link
    virtual unsigned int GetNumAffectedCoords() const { return 3; }

    /// Number of scalar constraints
    virtual unsigned int GetNumConstraintsBilateral() override { return 1; }

    /// Update all auxiliary data of the gear transmission at given time
    virtual void Update(double time, bool update_assets) override;

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
    virtual void LoadConstraintJacobians() override;

    virtual void ConstraintsBiReset() override;
    virtual void ConstraintsBiLoad_C(double factor = 1, double recovery_clamp = 0.1, bool do_clamp = false) override;
    virtual void ConstraintsBiLoad_Ct(double factor = 1) override;
    virtual void ConstraintsFetch_react(double factor = 1) override;
};

CH_CLASS_VERSION(ChShaftsPlanetary, 0)

}  // end namespace chrono

#endif

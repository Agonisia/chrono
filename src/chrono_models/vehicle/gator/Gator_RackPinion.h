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
//
// Gator rack-pinion steering model.
//
// =============================================================================

#ifndef GATOR_RACKPINION_H
#define GATOR_RACKPINION_H

#include "chrono_vehicle/wheeled_vehicle/steering/ChRackPinion.h"

#include "chrono_models/ChApiModels.h"

namespace chrono {
namespace vehicle {
namespace gator {

/// @addtogroup vehicle_models_gator
/// @{

/// Rack-pinion steering subsystem for the Gator vehicle.
class CH_MODELS_API Gator_RackPinion : public ChRackPinion {
  public:
    Gator_RackPinion(const std::string& name);
    ~Gator_RackPinion() {}

    virtual double GetSteeringLinkMass() const override { return m_steeringLinkMass; }
    virtual ChVector3d GetSteeringLinkInertia() const override { return m_steeringLinkInertia; }
    virtual double GetSteeringLinkCOM() const override { return m_steeringLinkCOM; }
    virtual double GetSteeringLinkRadius() const override { return m_steeringLinkRadius; }
    virtual double GetSteeringLinkLength() const override { return m_steeringLinkLength; }

    virtual double GetPinionRadius() const override { return m_pinionRadius; }

    virtual double GetMaxAngle() const override { return m_maxAngle; }

  private:
    static const double m_steeringLinkMass;
    static const ChVector3d m_steeringLinkInertia;
    static const double m_steeringLinkCOM;
    static const double m_steeringLinkRadius;
    static const double m_steeringLinkLength;

    static const double m_pinionRadius;

    static const double m_maxAngle;
};

/// @} vehicle_models_gator

}  // end namespace gator
}  // end namespace vehicle
}  // end namespace chrono

#endif

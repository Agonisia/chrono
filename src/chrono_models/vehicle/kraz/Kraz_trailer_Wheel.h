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
// Authors: Radu Serban, Rainer Gericke
// =============================================================================
//
// Kraz trailer wheel subsystem
//
// =============================================================================

#ifndef KRAZ_TRAILER_WHEEL_H
#define KRAZ_TRAILER_WHEEL_H

#include "chrono/assets/ChVisualShapeTriangleMesh.h"

#include "chrono_vehicle/ChSubsysDefs.h"
#include "chrono_vehicle/ChVehicleModelData.h"
#include "chrono_vehicle/wheeled_vehicle/ChWheel.h"

#include "chrono_models/ChApiModels.h"

namespace chrono {
namespace vehicle {
namespace kraz {

/// @addtogroup vehicle_models_kraz
/// @{

/// Kraz trailer wheel (can be used on any axle, left or right).
class CH_MODELS_API Kraz_trailer_Wheel : public ChWheel {
  public:
    Kraz_trailer_Wheel(const std::string& name);
    ~Kraz_trailer_Wheel() {}

    virtual double GetWheelMass() const override { return m_mass; }
    virtual const ChVector3d& GetWheelInertia() const override { return m_inertia; }
    virtual double GetRadius() const override { return m_radius; }
    virtual double GetWidth() const override { return m_width; }

  protected:
    static const double m_radius;
    static const double m_width;
    static const double m_mass;
    static const ChVector3d m_inertia;
};

/// @} vehicle_models_kraz

}  // end namespace kraz
}  // end namespace vehicle
}  // end namespace chrono

#endif

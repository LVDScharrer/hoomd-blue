// Copyright (c) 2009-2021 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.

// Maintainer: joaander

/*! \file ZeroMomentumUpdater.h
    \brief Declares an updater that zeros the momentum of the system
*/

#ifdef __HIPCC__
#error This header cannot be compiled by nvcc
#endif

#include "hoomd/Updater.h"

#include <memory>
#include <pybind11/pybind11.h>
#include <vector>

#ifndef __ZEROMOMENTUMUPDATER_H__
#define __ZEROMOMENTUMUPDATER_H__

namespace hoomd {
namespace md {

//! Updates particle velocities to zero the momentum
/*! This simple updater just calculate the linear momentum of the system and subtracts it from every
   particle to zero it.

    \ingroup updaters
*/
class PYBIND11_EXPORT ZeroMomentumUpdater : public Updater
    {
    public:
    //! Constructor
    ZeroMomentumUpdater(std::shared_ptr<SystemDefinition> sysdef);
    virtual ~ZeroMomentumUpdater();

    //! Take one timestep forward
    virtual void update(uint64_t timestep);
    };

namespace detail {

//! Export the ZeroMomentumUpdater to python
void export_ZeroMomentumUpdater(pybind11::module& m);

} // end namespace detail
} // end namespace md
} // end namespace hoomd

#endif

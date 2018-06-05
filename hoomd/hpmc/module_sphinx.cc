// Copyright (c) 2009-2018 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.

// Include the defined classes that are to be exported to python
#include "IntegratorHPMC.h"
#include "IntegratorHPMCMono.h"
#include "IntegratorHPMCMonoImplicit.h"
#include "IntegratorHPMCMonoImplicitNew.h"
#include "ComputeFreeVolume.h"

#include "ShapeSphinx.h"
#include "AnalyzerSDF.h"
#include "ShapeUnion.h"

#include "ExternalField.h"
#include "ExternalFieldWall.h"
#include "ExternalFieldLattice.h"
#include "ExternalFieldComposite.h"
#include "ExternalCallback.h"

#include "UpdaterExternalFieldWall.h"
#include "UpdaterRemoveDrift.h"
#include "UpdaterMuVT.h"
#include "UpdaterMuVTImplicit.h"
#include "UpdaterClusters.h"
#include "UpdaterClustersImplicit.h"

#include "ShapeUtils.h"
#include "ShapeMoves.h"
#include "UpdaterShape.h"

#ifdef ENABLE_CUDA
#include "IntegratorHPMCMonoGPU.h"
#include "IntegratorHPMCMonoImplicitGPU.h"
#include "IntegratorHPMCMonoImplicitNewGPU.h"
#include "ComputeFreeVolumeGPU.h"
#endif

namespace py = pybind11;
using namespace hpmc;

using namespace hpmc::detail;

namespace hpmc
{

//! Export the base HPMCMono integrators
void export_sphinx(py::module& m)
    {
    export_IntegratorHPMCMono< ShapeSphinx >(m, "IntegratorHPMCMonoSphinx");
    export_IntegratorHPMCMonoImplicit< ShapeSphinx >(m, "IntegratorHPMCMonoImplicitSphinx");
    export_IntegratorHPMCMonoImplicitNew< ShapeSphinx >(m, "IntegratorHPMCMonoImplicitNewSphinx");
    export_ComputeFreeVolume< ShapeSphinx >(m, "ComputeFreeVolumeSphinx");
    export_AnalyzerSDF< ShapeSphinx >(m, "AnalyzerSDFSphinx");
    export_UpdaterMuVT< ShapeSphinx >(m, "UpdaterMuVTSphinx");
    export_UpdaterClusters< ShapeSphinx >(m, "UpdaterClustersSphinx");
    export_UpdaterClustersImplicit< ShapeSphinx, IntegratorHPMCMonoImplicit<ShapeSphinx> >(m, "UpdaterClustersImplicitSphinx");
    export_UpdaterClustersImplicit< ShapeSphinx, IntegratorHPMCMonoImplicitNew<ShapeSphinx> >(m, "UpdaterClustersImplicitNewSphinx");
    export_UpdaterMuVTImplicit< ShapeSphinx, IntegratorHPMCMonoImplicit<ShapeSphinx> >(m, "UpdaterMuVTImplicitSphinx");
    export_UpdaterMuVTImplicit< ShapeSphinx, IntegratorHPMCMonoImplicitNew<ShapeSphinx> >(m, "UpdaterMuVTImplicitNewSphinx");

    export_ExternalFieldInterface<ShapeSphinx>(m, "ExternalFieldSphinx");
    export_LatticeField<ShapeSphinx>(m, "ExternalFieldLatticeSphinx");
    export_ExternalFieldComposite<ShapeSphinx>(m, "ExternalFieldCompositeSphinx");
    export_RemoveDriftUpdater<ShapeSphinx>(m, "RemoveDriftUpdaterSphinx");
    export_ExternalFieldWall<ShapeSphinx>(m, "WallSphinx");
    export_UpdaterExternalFieldWall<ShapeSphinx>(m, "UpdaterExternalFieldWallSphinx");
    export_ExternalCallback<ShapeSphinx>(m, "ExternalCallbackSphinx");

    export_ShapeMoveInterface< ShapeSphinx >(m, "ShapeMoveSphinx");
    export_ShapeLogBoltzmann< ShapeSphinx >(m, "LogBoltzmannSphinx");
    export_AlchemyLogBoltzmannFunction< ShapeSphinx >(m, "AlchemyLogBoltzmannSphinx");
    export_UpdaterShape< ShapeSphinx >(m, "UpdaterShapeSphinx");
    export_PythonShapeMove< ShapeSphinx >(m, "PythonShapeMoveSphinx");
    export_ConstantShapeMove< ShapeSphinx >(m, "ConstantShapeMoveSphinx");

    #ifdef ENABLE_CUDA
    #ifdef ENABLE_SPHINX_GPU

    export_IntegratorHPMCMonoGPU< ShapeSphinx >(m, "IntegratorHPMCMonoGPUSphinx");
    export_IntegratorHPMCMonoImplicitGPU< ShapeSphinx >(m, "IntegratorHPMCMonoImplicitGPUSphinx");
    export_IntegratorHPMCMonoImplicitNewGPU< ShapeSphinx >(m, "IntegratorHPMCMonoImplicitNewGPUSphinx");
    export_ComputeFreeVolumeGPU< ShapeSphinx >(m, "ComputeFreeVolumeGPUSphinx");

    #endif
    #endif
    }

}

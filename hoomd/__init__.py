# Copyright (c) 2009-2021 The Regents of the University of Michigan
# This file is part of the HOOMD-blue project, released under the BSD 3-Clause
# License.

"""HOOMD-blue python API.

:py:mod:`hoomd` provides a high level user interface for defining and executing
simulations using HOOMD.
"""
import sys
import os

from hoomd import version
from hoomd import trigger
from hoomd import variant
from hoomd.box import Box
from hoomd import data
from hoomd import filter
from hoomd import device
from hoomd import update
from hoomd import integrate
from hoomd import communicator
from hoomd import util
from hoomd import write
from hoomd import _hoomd
if version.md_built:
    from hoomd import md
if version.hpmc_built:
    from hoomd import hpmc
if version.dem_built:
    from hoomd import dem
# if version.metal_built:
#     from hoomd import metal
# if version.mpcd_built:
#     from hoomd import mpcd

from hoomd.simulation import Simulation
from hoomd.state import State
from hoomd.operations import Operations
from hoomd.snapshot import Snapshot
from hoomd import tune
from hoomd import logging
from hoomd import custom

_default_excepthook = sys.excepthook


def _hoomd_sys_excepthook(type, value, traceback):
    """Override Python's excepthook to abort MPI runs."""
    _default_excepthook(type, value, traceback)
    sys.stderr.flush()
    _hoomd.abort_mpi(communicator._current_communicator.cpp_mpi_conf, 1)


sys.excepthook = _hoomd_sys_excepthook

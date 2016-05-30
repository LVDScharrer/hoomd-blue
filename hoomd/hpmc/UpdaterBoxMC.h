// This is an Updater class to apply NPT MC volume changes and shearing to an HPMC system

// inclusion guard
#ifndef _UPDATER_HPMC_BOX_MC_
#define _UPDATER_HPMC_BOX_MC_

/*! \file UpdaterBoxMC.h
    \brief Declaration of UpdaterBoxMC
*/

#include <hoomd/Updater.h>
#include <hoomd/Variant.h>
#include <hoomd/extern/saruprng.h>
#include <cmath>

// Need Moves.h for rand_select
#include "Moves.h"

#include "IntegratorHPMC.h"

namespace hpmc
{

//! Update box for HPMC simulation in the NPT ensemble, etc.
/*! The pressure parameter is beta*P. For a unitless reduced pressure, the user must adopt and apply the
    convention of their choice externally. E.g. \f$ P^* \equiv \frac{P \sigma^3}{k_B T} \f$ implies a user should pass
    \f$ P^* / \sigma^3 \f$ as the UpdaterBoxMC P parameter.
*/
class UpdaterBoxMC : public Updater
    {
    public:
        //! Constructor
        /*! \param sysdef System definition
            \param mc HPMC integrator object
            \param P Pressure times thermodynamic beta to apply in NPT ensemble
            \param frequency average number of box updates per particle super-move
            \param seed PRNG seed

            Variant parameters are possible, but changing MC parameters violates detailed balance.
        */
        UpdaterBoxMC(boost::shared_ptr<SystemDefinition> sysdef,
                      boost::shared_ptr<IntegratorHPMC> mc,
                      boost::shared_ptr<Variant> P,
                      Scalar frequency,
                      const unsigned int seed);

        //! Destructor
        virtual ~UpdaterBoxMC();

        //! Sets parameters
        /*! \param delta maximum size of volume change
            \param weight relative likelihood of volume move
        */
        void setVolumeMove(Scalar delta,
                           Scalar weight)
            {
            m_Volume_delta = delta;
            m_Volume_weight = weight;
            // Calculate aspect ratio
            computeAspectRatios();
            };

        //! Sets parameters
        /*! \param dLx Extent of length change distribution in first lattice vector for box resize moves
            \param dLy Extent of length change distribution in second lattice vector for box resize moves
            \param dLz Extent of length change distribution in third lattice vector for box resize moves
            \param weight relative likelihood of volume move
        */
        void setLengthMove(Scalar dLx,
                           Scalar dLy,
                           Scalar dLz,
                           Scalar weight)
            {
            m_Length_delta[0] = dLx;
            m_Length_delta[1] = dLy;
            m_Length_delta[2] = dLz;
            m_Length_weight = weight;
            };


        //! Sets parameters
        /*! \param dxy Extent of shear parameter distribution for shear moves in x,y plane
            \param dxz Extent of shear parameter distribution for shear moves in x,z plane
            \param dyz Extent of shear parameter distribution for shear moves in y,z plane
            \param reduce Maximum number of lattice vectors of shear to allow before applying lattice reduction.
                Shear of +/- 0.5 cannot be lattice reduced, so set to a value < 0.5 to disable (default 0)
                Note that due to precision errors, lattice reduction may introduce small overlaps which can be resolved,
                but which temporarily break detailed balance.
            \param weight relative likelihood of shear move
        */
        void setShearMove(Scalar dxy,
                          Scalar dxz,
                          Scalar dyz,
                          Scalar reduce,
                          Scalar weight)
            {
            m_Shear_delta[0] = dxy;
            m_Shear_delta[1] = dxz;
            m_Shear_delta[2] = dyz;
            m_Shear_reduce = reduce;
            m_Shear_weight = weight;
            };

        //! Calculate aspect ratios for use in isotropic volume changes
        void computeAspectRatios()
            {
            // when volume is changed, we want to set Ly = m_rLy * Lx, etc.
            BoxDim curBox = m_pdata->getGlobalBox();
            Scalar Lx = curBox.getLatticeVector(0).x;
            Scalar Ly = curBox.getLatticeVector(1).y;
            Scalar Lz = curBox.getLatticeVector(2).z;
            m_Volume_A1 = Lx / Ly;
            m_Volume_A2 = Lx / Lz;
            }

        //! Get pressure parameter
        /*! \returns pressure variant object
        */
        boost::shared_ptr<Variant> getP()
            {
            return m_P;
            }

        //! Print statistics about the NPT box update steps taken
        void printStats()
            {
            hpmc_npt_counters_t counters = getCounters(1);
            m_exec_conf->msg->notice(2) << "-- HPMC NPT box change stats:" << std::endl;

            if (counters.shear_accept_count + counters.shear_reject_count > 0)
                {
                m_exec_conf->msg->notice(2) << "Average shear acceptance: " << counters.getShearAcceptance() << "\n";
                }
            if (counters.volume_accept_count + counters.volume_reject_count > 0)
                {
                m_exec_conf->msg->notice(2) << "Average volume acceptance: " << counters.getVolumeAcceptance() << std::endl;
                }

            m_exec_conf->msg->notice(2) << "Total box changes:        " << counters.getNMoves() << std::endl;
            }

        //! Get a list of logged quantities
        virtual std::vector< std::string > getProvidedLogQuantities();

        //! Get the value of a logged quantity
        virtual Scalar getLogValue(const std::string& quantity, unsigned int timestep);

        //! Reset statistics counters
        void resetStats()
            {
            m_count_run_start = m_count_total;
            }

        //! Handle MaxParticleNumberChange signal
        /*! Resize the m_pos_backup array
        */
        void slotMaxNChange()
            {
            unsigned int MaxN = m_pdata->getMaxN();
            m_pos_backup.resize(MaxN);
            }

        //! Take one timestep forward
        /*! \param timestep timestep at which update is being evaluated
        */
        virtual void update(unsigned int timestep);

        //! Get the current counter values
        hpmc_npt_counters_t getCounters(unsigned int mode=0);

        //! Perform box update in NpT box length distribution
        /*! \param timestep timestep at which update is being evaluated
            \param rng psueudo random number generator instance
        */
        void update_L(unsigned int timestep, Saru& rng);

        //! Perform box update in NpT volume distribution
        /*! \param timestep timestep at which update is being evaluated
            \param rng psueudo random number generator instance
        */
        void update_V(unsigned int timestep, Saru& rng);

        //! Perform box update in NpT shear distribution
        /*! \param timestep timestep at which update is being evaluated
            \param rng psueudo random number generator instance
        */
        void update_shear(unsigned int timestep, Saru& rng);

    private:
        boost::shared_ptr<IntegratorHPMC> m_mc;     //!< HPMC integrator object
        boost::shared_ptr<Variant> m_P;             //!< Reduced pressure in NPT ensemble
        Scalar m_frequency;                         //!< Frequency of BoxMC moves versus HPMC integrator moves

        Scalar m_Volume_delta;                      //!< Amount by which to change parameter during box-change
        Scalar m_Volume_weight;                     //!< relative weight of volume moves
        Scalar m_Volume_A1;                         //!< Ratio of Lx to Ly to use in isotropic volume changes
        Scalar m_Volume_A2;                         //!< Ratio of Lx to Lz to use in isotropic volume changes

        Scalar m_Length_delta[3];                   //!< Max length change in each dimension
        Scalar m_Length_weight;                     //!< relative weight of length change moves

        Scalar m_Shear_delta[3];                    //!< Max tilt factor change in each dimension        Scalar m_Shear_reduce;                            //!< Threshold for lattice reduction
        Scalar m_Shear_weight;                      //!< relative weight of shear moves
        Scalar m_Shear_reduce;                      //!< Tolerance for automatic box lattice reduction

        GPUArray<Scalar4> m_pos_backup;             //!< hold backup copy of particle positions
        boost::signals2::connection m_maxparticlenumberchange_connection;
                                                    //!< Connection to MaxParticleNumberChange signal

        hpmc_npt_counters_t m_count_total;          //!< Accept/reject total count
        hpmc_npt_counters_t m_count_run_start;      //!< Count saved at run() start
        hpmc_npt_counters_t m_count_step_start;     //!< Count saved at the start of the last step

        unsigned int m_seed;                        //!< Seed for pseudo-random number generator

        inline bool is_oversheared();               //!< detect oversheared box
        inline bool remove_overshear();             //!< detect and remove overshear
        inline bool box_resize(Scalar Lx,
                               Scalar Ly,
                               Scalar Lz,
                               Scalar xy,
                               Scalar xz,
                               Scalar yz
                               );
                               //!< perform specified box change, if possible
        inline bool box_resize_trial(Scalar Lx,
                                     Scalar Ly,
                                     Scalar Lz,
                                     Scalar xy,
                                     Scalar xz,
                                     Scalar yz,
                                     unsigned int timestep,
                                     Scalar boltzmann,
                                     Saru& rng
                                     );
                                     //!< attempt specified box change and undo if overlaps generated
        inline bool safe_box(const Scalar newL[3], const unsigned int& Ndim);
                                                    //!< Perform appropriate checks for box validity
    };

//! Export UpdaterBoxMC to Python
void export_UpdaterBoxMC();

} // end namespace hpmc

#endif // _UPDATER_HPMC_BOX_MC_

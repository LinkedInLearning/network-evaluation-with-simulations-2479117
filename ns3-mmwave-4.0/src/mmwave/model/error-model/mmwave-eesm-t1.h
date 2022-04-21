/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Adapted from the release 1.0 of the 5G-LENA simulator, please refer to https://5g-lena.cttc.es/ for further details 
 *   and https://gitlab.com/cttc-lena/nr/-/tree/v1.0 for the reference code.
 * 
 *   Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef SRC_MMWAVE_EESM_T1_H
#define SRC_MMWAVE_EESM_T1_H

#include <vector>
#include "mmwave-eesm-error-model.h"

namespace ns3 {

namespace mmwave {

/**
 * \ingroup error-models
 * \brief The MmWaveEesmT1 struct
 *
 * This class provides the NR Tables to be used for MCS/CQI Table1, corresponding
 * to tables 5.1.3.1-1 and 5.2.2.1-2 in TS 38.214. It includes the beta values
 * (obtained from link-to-sytem mapping techniques) for EESM under MCS Table1, the
 * Effective Code Rate (ECR) corresponding to MCS Table1, the BLER-SINR tables
 * for different MCS, Block Sizes and LDPC base graph types, the M tables
 * (modulation order for each MCS index in MCS Table1), the Spectral Efficiency
 * (SE) for MCS indexes in MCS Table1, and the SE for CQI indexes in CQI Table1.
 *
 * Values used inside MmWaveEesmIrT1 and MmWaveEesmCcT1 classes
 *
 * \see MmWaveEesmIrT1
 * \see MmWaveEesmCcT1
 */
struct MmWaveEesmT1
{
  /**
   * \brief MmWaveEesmT1 constructor. Initialize the pointers
   */
  MmWaveEesmT1 ();

  const std::vector<double> *m_betaTable {nullptr};  //!< Beta table
  const std::vector<double> *m_mcsEcrTable {nullptr}; //!< MCS-ECR table
  const MmWaveEesmErrorModel::SimulatedBlerFromSINR *m_simulatedBlerFromSINR {nullptr}; //!< BLER from SINR table
  const std::vector<uint8_t> *m_mcsMTable {nullptr}; //!< MCS-M table
  const std::vector<double> *m_spectralEfficiencyForMcs {nullptr}; //!< Spectral-efficiency for MCS
  const std::vector<double> *m_spectralEfficiencyForCqi {nullptr}; //!< Spectral-efficiency for CQI
};

} // namespace ns3
} // namespace mmwave

#endif // SRC_MMWAVE_EESM_T1_H

/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter 3.1.0.202603111726!
 ***
 *** Name: PWM
 *** Description: Service Interface Function Block Type
 *** Version:
 ***     1.0: 2026-05-15/Tran Hung -  -
 *************************************************************************/

#pragma once

#include "forte/funcbloc.h"
#include "forte/datatypes/forte_bool.h"
#include "forte/datatypes/forte_int.h"
#include "forte/datatypes/forte_uint.h"
#include "forte/forte_st_util.h"

namespace forte {
  class FORTE_PWM final : public CFunctionBlock {
      DECLARE_FIRMWARE_FB(FORTE_PWM)

    private:
      static const TEventID scmEventINITOID = 0;
      static const TEventID scmEventCNFID = 1;
      static const TEventID scmEventINITID = 0;
      static const TEventID scmEventREQID = 1;

      void executeEvent(TEventID paEIID, CEventChainExecutionThread *const paECET) override;

      void readInputData(TEventID paEIID) override;
      void writeOutputData(TEventID paEIID) override;
      void setInitialValues() override;

    public:
      FORTE_PWM(StringId paInstanceNameId, CFBContainer &paContainer);

      CIEC_INT var_PIN;
      CIEC_INT var_FREQ;
      CIEC_INT var_DUTY;

      CIEC_BOOL var_QO;
      CIEC_UINT var_STATUS;

      CEventConnection conn_INITO;
      CEventConnection conn_CNF;

      CDataConnection *conn_PIN;
      CDataConnection *conn_FREQ;
      CDataConnection *conn_DUTY;

      COutDataConnection<CIEC_BOOL> conn_QO;
      COutDataConnection<CIEC_UINT> conn_STATUS;

      CIEC_ANY *getDI(size_t) override;
      CIEC_ANY *getDO(size_t) override;
      CEventConnection *getEOConUnchecked(TPortId) override;
      CDataConnection **getDIConUnchecked(TPortId) override;
      CDataConnection *getDOConUnchecked(TPortId) override;

      void evt_INIT(const CIEC_INT &paPIN, const CIEC_INT &paFREQ, const CIEC_INT &paDUTY, CAnyBitOutputParameter<CIEC_BOOL> paQO, COutputParameter<CIEC_UINT> paSTATUS) {
        COutputGuard guard_QO(paQO);
        COutputGuard guard_STATUS(paSTATUS);
        var_PIN = paPIN;
        var_FREQ = paFREQ;
        var_DUTY = paDUTY;
        executeEvent(scmEventINITID, nullptr);
        *paQO = var_QO;
        *paSTATUS = var_STATUS;
      }

      void evt_REQ(const CIEC_INT &paPIN, const CIEC_INT &paFREQ, const CIEC_INT &paDUTY, CAnyBitOutputParameter<CIEC_BOOL> paQO, COutputParameter<CIEC_UINT> paSTATUS) {
        COutputGuard guard_QO(paQO);
        COutputGuard guard_STATUS(paSTATUS);
        var_PIN = paPIN;
        var_FREQ = paFREQ;
        var_DUTY = paDUTY;
        executeEvent(scmEventREQID, nullptr);
        *paQO = var_QO;
        *paSTATUS = var_STATUS;
      }
  };
}


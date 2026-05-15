/*************************************************************************
 *** Name: PWM
 *** Version:
 ***     1.0: 2026-05-15/Tran Hung
 *************************************************************************/

#include "forte/PWM_fbt.h"

#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>

using namespace std::literals;
using namespace forte::literals;

namespace forte {

  namespace {

    constexpr std::string_view TypeHash =
      "v2:SHA3-512:qE1o3GJGvxe4BbuvZu2jNwIPt5wAWMyVL5BmUm7Hp4NuewTDvVkeAJHuP8SkyOa5n2v7jkBBsBQSStv4ZMChxg=="sv;

    const auto cEventInputNames =
      std::array{"INIT"_STRID, "REQ"_STRID};

    const auto cEventInputTypeIds =
      std::array{"EInit"_STRID, "Event"_STRID};

    const auto cEventOutputNames =
      std::array{"INITO"_STRID, "CNF"_STRID};

    const auto cEventOutputTypeIds =
      std::array{"EInit"_STRID, "Event"_STRID};

    const auto cDataInputNames =
      std::array{"PIN"_STRID, "FREQ"_STRID, "DUTY"_STRID};

    const auto cDataOutputNames =
      std::array{"QO"_STRID, "STATUS"_STRID};

    const SFBInterfaceSpec cFBInterfaceSpec = {
      .mEINames = cEventInputNames,
      .mEITypeNames = cEventInputTypeIds,
      .mEONames = cEventOutputNames,
      .mEOTypeNames = cEventOutputTypeIds,
      .mDINames = cDataInputNames,
      .mDONames = cDataOutputNames,
      .mDIONames = {},
      .mSocketNames = {},
      .mPlugNames = {},
    };

    // =====================================================
    // Helper functions
    // =====================================================

    static bool writeToFile(
        const std::string &path,
        const std::string &value) {

      std::ofstream fs(path);

      if(!fs.is_open()) {
        return false;
      }

      fs << value;

      return true;
    }

    static bool pathExists(
        const std::string &path) {

      return access(path.c_str(), F_OK) == 0;
    }

  }

  DEFINE_FIRMWARE_FB(
      FORTE_PWM,
      "PWM"_STRID,
      TypeHash)

  FORTE_PWM::FORTE_PWM(
      const StringId paInstanceNameId,
      CFBContainer &paContainer) :

      CFunctionBlock(
          paContainer,
          cFBInterfaceSpec,
          paInstanceNameId),

      var_PIN(0_INT),
      var_FREQ(0_INT),
      var_DUTY(0_INT),

      var_QO(0_BOOL),
      var_STATUS(0_UINT),

      conn_INITO(*this, 0),
      conn_CNF(*this, 1),

      conn_PIN(nullptr),
      conn_FREQ(nullptr),
      conn_DUTY(nullptr),

      conn_QO(*this, 0, var_QO),
      conn_STATUS(*this, 1, var_STATUS) {
  }

  void FORTE_PWM::setInitialValues() {

    CFunctionBlock::setInitialValues();

    var_PIN = 0_INT;
    var_FREQ = 0_INT;
    var_DUTY = 0_INT;

    var_QO = 0_BOOL;
    var_STATUS = 0_UINT;
  }

  void FORTE_PWM::executeEvent(
      const TEventID paEIID,
      CEventChainExecutionThread *const paECET) {

    int pin =
      var_PIN.getSignedValue();

    int freq =
      var_FREQ.getSignedValue();

    int duty =
      var_DUTY.getSignedValue();

    // ====================================
    // Validate input
    // ====================================

    if(freq <= 0) {
      freq = 1000;
    }

    if(duty < 0) {
      duty = 0;
    }

    if(duty > 100) {
      duty = 100;
    }

    // ====================================
    // PWM path
    // ====================================

    std::string pwmPath =
      "/sys/class/pwm/pwmchip0/pwm" +
      std::to_string(pin) + "/";

    switch(paEIID) {

      // ====================================
      // INIT EVENT
      // ====================================

      case scmEventINITID: {

        // export pwm if not exists
        if(!pathExists(pwmPath)) {

          bool exportOk =
            writeToFile(
              "/sys/class/pwm/pwmchip0/export",
              std::to_string(pin));

          if(!exportOk) {

            var_QO = false;
            var_STATUS = 1;

            sendOutputEvent(
                scmEventINITOID,
                paECET);

            return;
          }

          // wait kernel create pwm folder
          usleep(100000);
        }

        int period_ns =
          1000000000 / freq;

        int duty_ns =
          (period_ns * duty) / 100;

        // disable before config
        writeToFile(
            pwmPath + "enable",
            "0");

        bool ok1 =
          writeToFile(
            pwmPath + "period",
            std::to_string(period_ns));

        bool ok2 =
          writeToFile(
            pwmPath + "duty_cycle",
            std::to_string(duty_ns));

        bool ok3 =
          writeToFile(
            pwmPath + "enable",
            "1");

        if(ok1 && ok2 && ok3) {

          var_QO = true;
          var_STATUS = 0;

        } else {

          var_QO = false;
          var_STATUS = 2;
        }

        sendOutputEvent(
            scmEventINITOID,
            paECET);

        break;
      }

      // ====================================
      // REQ EVENT
      // ====================================

      case scmEventREQID: {

        int period_ns =
          1000000000 / freq;

        int duty_ns =
          (period_ns * duty) / 100;

        // disable before update
        writeToFile(
            pwmPath + "enable",
            "0");

        bool ok1 =
          writeToFile(
            pwmPath + "period",
            std::to_string(period_ns));

        bool ok2 =
          writeToFile(
            pwmPath + "duty_cycle",
            std::to_string(duty_ns));

        bool ok3 =
          writeToFile(
            pwmPath + "enable",
            "1");

        if(ok1 && ok2 && ok3) {

          var_STATUS = 0;

        } else {

          var_STATUS = 3;
        }

        sendOutputEvent(
            scmEventCNFID,
            paECET);

        break;
      }
    }
  }

  void FORTE_PWM::readInputData(TEventID) {
    // nothing to do
  }

  void FORTE_PWM::writeOutputData(TEventID) {
    // nothing to do
  }

  CIEC_ANY *FORTE_PWM::getDI(
      const size_t paIndex) {

    switch(paIndex) {

      case 0:
        return &var_PIN;

      case 1:
        return &var_FREQ;

      case 2:
        return &var_DUTY;
    }

    return nullptr;
  }

  CIEC_ANY *FORTE_PWM::getDO(
      const size_t paIndex) {

    switch(paIndex) {

      case 0:
        return &var_QO;

      case 1:
        return &var_STATUS;
    }

    return nullptr;
  }

  CEventConnection *FORTE_PWM::getEOConUnchecked(
      const TPortId paIndex) {

    switch(paIndex) {

      case 0:
        return &conn_INITO;

      case 1:
        return &conn_CNF;
    }

    return nullptr;
  }

  CDataConnection **FORTE_PWM::getDIConUnchecked(
      const TPortId paIndex) {

    switch(paIndex) {

      case 0:
        return &conn_PIN;

      case 1:
        return &conn_FREQ;

      case 2:
        return &conn_DUTY;
    }

    return nullptr;
  }

  CDataConnection *FORTE_PWM::getDOConUnchecked(
      const TPortId paIndex) {

    switch(paIndex) {

      case 0:
        return &conn_QO;

      case 1:
        return &conn_STATUS;
    }

    return nullptr;
  }

}
#ifndef INFERENCE_SLOPE_COLUMN_PARAMETER_CONTROLLER_H
#define INFERENCE_SLOPE_COLUMN_PARAMETER_CONTROLLER_H

#include <array>

#include "shared/store_parameter_controller.h"

namespace Inference {

class SlopeColumnParameterController : public Shared::StoreParameterController {
 public:
  SlopeColumnParameterController(Escher::Responder* parentResponder,
                                 Shared::StoreColumnHelper* storeColumnHelper)
      : Shared::StoreParameterController(parentResponder, storeColumnHelper) {
    // Do not display an option to hide series
    m_hideCell.hide();
  }
  Escher::ViewController::TitlesDisplay titlesDisplay() override {
    return m_titlesDisplay;
  }
  void setTitlesDisplay(Escher::ViewController::TitlesDisplay titlesDisplay) {
    m_titlesDisplay = titlesDisplay;
  }

 private:
  Escher::ViewController::TitlesDisplay m_titlesDisplay;
};

}  // namespace Inference

#endif

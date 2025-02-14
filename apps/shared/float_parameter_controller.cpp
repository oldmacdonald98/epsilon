#include "float_parameter_controller.h"

#include <assert.h>
#include <poincare/preferences.h>

#include <cmath>

#include "poincare_helpers.h"
#include "text_field_delegate_app.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

template <typename T>
FloatParameterController<T>::FloatParameterController(
    Responder *parentResponder)
    : ListWithTopAndBottomController(parentResponder),
      m_okButton(
          &m_selectableListView, I18n::Message::Ok,
          Invocation::Builder<FloatParameterController>(
              [](FloatParameterController *parameterController, void *sender) {
                parameterController->buttonAction();
                return true;
              },
              this)) {}

template <typename T>
bool FloatParameterController<T>::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    stackController()->pop();
    return true;
  }
  return false;
}

template <typename T>
int FloatParameterController<T>::typeAtRow(int index) const {
  if (index == numberOfRows() - 1) {
    return k_buttonCellType;
  }
  return k_parameterCellType;
}

template <typename T>
int FloatParameterController<T>::reusableCellCount(int type) {
  if (type == k_buttonCellType) {
    return 1;
  }
  return reusableParameterCellCount(type);
}

template <typename T>
HighlightCell *FloatParameterController<T>::reusableCell(int index, int type) {
  if (type == k_buttonCellType) {
    return &m_okButton;
  }
  return reusableParameterCell(index, type);
}

template <typename T>
void FloatParameterController<T>::fillCellForRow(HighlightCell *cell, int row) {
  if (typeAtRow(row) == k_buttonCellType ||
      textFieldOfCellAtIndex(cell, row)->isEditing()) {
    return;
  }
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int bufferSize =
      PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode<T>(
      parameterAtIndex(row), buffer, bufferSize, precision,
      Preferences::PrintFloatMode::Decimal);
  textFieldOfCellAtIndex(cell, row)->setText(buffer);
}

template <typename T>
KDCoordinate FloatParameterController<T>::nonMemoizedRowHeight(int row) {
  if (typeAtRow(row) == k_buttonCellType) {
    return m_okButton.minimalSizeForOptimalDisplay().height();
  }
  return ListViewDataSource::nonMemoizedRowHeight(row);
}

template <typename T>
bool FloatParameterController<T>::textFieldShouldFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down &&
          innerSelectedRow() < numberOfRows() - 1) ||
         (event == Ion::Events::Up && innerSelectedRow() > 0) ||
         TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

template <typename T>
bool FloatParameterController<T>::textFieldDidFinishEditing(
    AbstractTextField *textField, const char *text, Ion::Events::Event event) {
  T floatBody =
      textFieldDelegateApp()->template parseInputtedFloatValue<T>(text);
  if (hasUndefinedValue(text, floatBody) ||
      !setParameterAtIndex(innerSelectedRow(), floatBody)) {
    return false;
  }
  resetMemoization();
  m_selectableListView.reloadSelectedCell();
  m_selectableListView.reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    assert(innerSelectedRow() + 1 < numberOfRows());
    m_selectableListView.selectCell(selectedRow() + 1);
  } else {
    m_selectableListView.handleEvent(event);
  }
  return true;
}

template <typename T>
void FloatParameterController<T>::buttonAction() {
  StackViewController *stack = FloatParameterController<T>::stackController();
  stack->pop();
}

template <typename T>
bool FloatParameterController<T>::hasUndefinedValue(const char *text,
                                                    T floatValue) const {
  InfinityTolerance infTolerance = infinityAllowanceForRow(innerSelectedRow());
  return textFieldDelegateApp()->hasUndefinedValue(
      floatValue, infTolerance == InfinityTolerance::PlusInfinity,
      infTolerance == InfinityTolerance::MinusInfinity);
}

template class FloatParameterController<float>;
template class FloatParameterController<double>;

}  // namespace Shared

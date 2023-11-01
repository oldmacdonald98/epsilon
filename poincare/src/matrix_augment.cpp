#include <poincare/matrix.h>
#include <poincare/matrix_complex.h>
#include <poincare/matrix_augment.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

int MatrixAugmentNode::numberOfChildren() const {
    return MatrixAugment::s_functionHelper.numberOfChildren();
}

Expression MatrixAugmentNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return MatrixAugment(this).shallowReduce(reductionContext);
}

Layout MatrixAugmentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context* context) const {
    return LayoutHelper::Prefix(
        MatrixAugment(this), floatDisplayMode, numberOfSignificantDigits,
        MatrixAugment::s_functionHelper.aliasesList().mainAlias(), context);
}

int MatrixAugmentNode::serialize(char* buffer, int bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
    return SerializationHelper::Prefix(
        this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
        MatrixAugment::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Evaluation<T> MatrixAugmentNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
    Evaluation<T> input0 = childAtIndex(0)->approximate(T(), approximationContext);
    if (input0.type() != EvaluationNode<T>::Type::MatrixComplex) {
        return Complex<T>::Undefined();
    }
    Evaluation<T> input1 = childAtIndex(1)->approximate(T(), approximationContext);
    if (input1.type() != EvaluationNode<T>::Type::MatrixComplex) {
        return Complex<T>::Undefined();
    }
    return static_cast<MatrixComplex<T>&>(input0).augment(
      static_cast<MatrixComplex<T>*>(&input1));
}

Expression MatrixAugment::shallowReduce(ReductionContext reductionContext) {
    {
        Expression e = SimplificationHelper::defaultShallowReduce(
            *this, &reductionContext,
            SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
            SimplificationHelper::UnitReduction::BanUnits);
        if (!e.isUninitialized()) {
            return e;
        }
    }
    Expression c0 = childAtIndex(0);
    Expression c1 = childAtIndex(1);
    if (c0.type() == ExpressionNode::Type::Matrix &&
        c1.type() == ExpressionNode::Type::Matrix) {
        Matrix matrixChild0 = static_cast<Matrix&>(c0);
        Matrix matrixChild1 = static_cast<Matrix&>(c1);
        Expression a = matrixChild0.augment(&matrixChild1, reductionContext);
        replaceWithInPlace(a);
        return a.shallowReduce(reductionContext);
    }
    return replaceWithUndefinedInPlace();
}

}
#include <poincare/expression.h>

namespace Poincare {

class MatrixAugmentNode : public ExpressionNode {
public:
    constexpr static AliasesList k_functionName = "augment";

    // TreeNode
    size_t size() const override { return sizeof(MatrixAugmentNode); }
    int numberOfChildren() const override;
#if POINCARE_TREE_LOG
    void logNodeName(std::ostream& stream) const override {
        stream << "MatrixAugment";
    }
#endif

    // Properties
    Type type() const override { return Type::MatrixAugment; }

private:
    // Layout
    Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
    int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;
    // Simplification
    Expression shallowReduce(const ReductionContext& reductionContext) override;
    LayoutShape leftLayoutShape() const override {
        return LayoutShape::MoreLetters;
    };
    LayoutShape rightLayoutShape() const override {
        return LayoutShape::BoundaryPunctuation;
    }
    // Evaluation
    Evaluation<float> approximate(
            SinglePrecision p,
            const ApproximationContext& approximationContext) const override {
        return templatedApproximate<float>(approximationContext);
    }
    Evaluation<double> approximate(
            DoublePrecision p,
            const ApproximationContext& approximationContext) const override {
        return templatedApproximate<double>(approximationContext);
    }
    template <typename T>
    Evaluation<T> templatedApproximate(
        const ApproximationContext& approximationContext) const;
};

class MatrixAugment final
    : public ExpressionTwoChildren<MatrixAugment, MatrixAugmentNode> {
public:
    using ExpressionBuilder::ExpressionBuilder;
    Expression shallowReduce(ReductionContext reductionContext);
};

}
#ifndef POINCARE_EXPRESSION_REFERENCE_H
#define POINCARE_EXPRESSION_REFERENCE_H

#include <ion/storage/file_system.h>
#include <poincare/aliases_list.h>
#include <poincare/complex.h>
#include <poincare/coordinate_2D.h>
#include <poincare/expression_node.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/tree_handle.h>

#include <utility>

namespace Poincare {

class Context;

class Expression : public TreeHandle {
  friend class AbsoluteValue;
  friend class Addition;
  friend class AdditionNode;
  friend class ArcCosecant;
  friend class ArcCosine;
  friend class ArcCotangent;
  friend class ArcSecant;
  friend class ArcSine;
  friend class ArcTangent;
  friend class Arithmetic;
  friend class BasedInteger;
  friend class BinomialCoefficient;
  friend class Ceiling;
  friend class Comparison;
  friend class ComplexArgument;
  friend class ComplexCartesian;
  friend class ComplexHelper;
  template <typename T>
  friend class ComplexNode;
  friend class Conjugate;
  friend class Constant;
  friend class Cosecant;
  friend class Cosine;
  friend class Cotangent;
  friend class Decimal;
  friend class Dependency;
  friend class DependencyNode;
  friend class Derivative;
  friend class DerivativeNode;
  friend class Determinant;
  friend class Dimension;
  friend class DistributionDispatcher;
  friend class Division;
  friend class DivisionQuotient;
  friend class DivisionRemainder;
  friend class EqualNode;
  template <typename T>
  friend class ExceptionExpressionNode;
  friend class ExpressionNode;
  friend class Factor;
  friend class Factorial;
  template <typename T>
  friend class FloatList;
  friend class Floor;
  friend class FracPart;
  friend class Function;
  friend class FunctionNode;
  friend class GlobalContext;
  friend class GreatCommonDivisor;
  friend class HyperbolicTrigonometricFunction;
  friend class ImaginaryPart;
  friend class Integer;
  friend class Integral;
  friend class IntegralNode;
  friend class InverseMethod;
  friend class LeastCommonMultiple;
  friend class List;
  friend class ListFunctionWithOneOrTwoParametersNode;
  friend class ListMean;
  friend class ListMedian;
  friend class ListSampleStandardDeviation;
  friend class ListSequence;
  friend class ListStandardDeviation;
  friend class ListVariance;
  friend class Logarithm;
  friend class LogarithmNode;
  friend class LogicalOperatorNode;
  friend class Matrix;
  friend class MatrixAugment;
  friend class MatrixEchelonForm;
  friend class MatrixIdentity;
  friend class MatrixInverse;
  friend class MatrixNode;
  friend class MatrixReducedRowEchelonForm;
  friend class MatrixRowEchelonForm;
  friend class MatrixTrace;
  friend class MatrixTranspose;
  friend class MixedFraction;
  friend class Multiplication;
  friend class MultiplicationNode;
  friend class NAryExpressionNode;
  friend class NAryExpression;
  friend class NAryInfixExpressionNode;
  friend class NaperianLogarithm;
  friend class NaperianLogarithmNode;
  friend class NthRoot;
  friend class Number;
  friend class Opposite;
  friend class ParameteredExpression;
  friend class ParameteredExpressionNode;
  friend class Parenthesis;
  friend class PercentAddition;
  friend class PercentSimple;
  friend class PermuteCoefficient;
  friend class PiecewiseOperator;
  friend class PolarConic;
  friend class Power;
  friend class PowerNode;
  friend class Product;
  friend class Randint;
  friend class RandintNode;
  friend class RealPart;
  friend class Round;
  friend class Secant;
  friend class Sequence;
  friend class SequenceNode;
  friend class SignFunction;
  friend class SimplificationHelper;
  friend class Sine;
  friend class SquareRoot;
  friend class SquareRootNode;
  friend class Store;
  friend class StoreNode;
  friend class Subtraction;
  friend class SubtractionNode;
  friend class Sum;
  friend class SumAndProduct;
  friend class SumAndProductNode;
  friend class Symbol;
  friend class SymbolNode;
  friend class SymbolAbstract;
  friend class SymbolAbstractNode;
  friend class Tangent;
  friend class Trigonometry;
  friend class TrigonometryCheatTable;
  friend class Unit;
  friend class UnitConvert;
  friend class UnitNode;
  friend class VectorCross;
  friend class VectorDot;
  friend class VectorNorm;

 public:
  static bool IsExpression() { return true; }

  /* Constructor & Destructor */
  Expression() : TreeHandle() {}
  Expression clone() const;
  static Expression Parse(char const* string, Context* context,
                          bool addMissingParenthesis = true,
                          bool parseForAssignment = false);
  static Expression ExpressionFromAddress(const void* address, size_t size);

  /* Hierarchy */
  Expression childAtIndex(int i) const;
  void setChildrenInPlace(Expression other) {
    node()->setChildrenInPlace(other);
  }

  /* Properties */
  ExpressionNode::Type type() const { return node()->type(); }
  bool isOfType(std::initializer_list<ExpressionNode::Type> types) const {
    return node()->isOfType(types);
  }
  TrinaryBoolean isPositive(Context* context) const {
    return node()->isPositive(context);
  }
  TrinaryBoolean isNull(Context* context) const {
    return node()->isNull(context);
  }
  bool isUndefined() const { return node()->isUndefined(); }
  bool allChildrenAreUndefined();
  bool isNumber() const { return node()->isNumber(); }
  bool isZero() const;
  bool isRationalOne() const;
  bool isOne() const;
  bool isMinusOne() const;
  bool isInteger() const;
  bool isRandomNumber() const { return node()->isRandomNumber(); }
  bool isRandomList() const { return node()->isRandomList(); }
  bool isRandom() const { return node()->isRandom(); }
  bool isParameteredExpression() const {
    return node()->isParameteredExpression();
  }
  bool isBasedIntegerCappedBy(const char* integerString) const;
  bool isDivisionOfIntegers() const;
  bool isAlternativeFormOfRationalNumber() const;
  template <typename T>
  bool hasDefinedComplexApproximation(Context* context,
                                      Preferences::ComplexFormat complexFormat,
                                      Preferences::AngleUnit angleUnit,
                                      T* returnRealPart = nullptr,
                                      T* returnImagPart = nullptr) const;
  bool isCombinationOfUnits() const { return node()->isCombinationOfUnits(); }
  /* This two functions only return true if the discontinuity is not asymptotic
   * (i.e. for the functions random, randint, round, floor and ceil).
   * Functions like 1/x are not handled here since it "obvious" that they are
   * discontinuous. */
  bool involvesDiscontinuousFunction(Context* context) const;
  bool isDiscontinuousBetweenValuesForSymbol(
      const char* symbol, float x1, float x2, Context* context,
      Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit) const;
  bool hasBooleanValue() const;
  bool hasMatrixOrListChild(Context* context, bool isReduced = true) const {
    return node()->hasMatrixOrListChild(context, isReduced);
  }

  /* recursivelyMatches will test each node recursively with the function
   * provided as argument. If the result is True, it will stop searching and
   * return true. If the result is Unknown, it will continue searching for a
   * True in the children. If an expression is tested as False, its children
   * will not be tested and the result will be false. */
  typedef TrinaryBoolean (*ExpressionTrinaryTest)(const Expression e,
                                                  Context* context,
                                                  void* auxiliary);
  bool recursivelyMatches(
      ExpressionTrinaryTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      void* auxiliary = nullptr) const;
  typedef bool (*ExpressionTest)(const Expression e, Context* context);
  bool recursivelyMatches(
      ExpressionTest test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;
  typedef bool (*ExpressionTestAuxiliary)(const Expression e, Context* context,
                                          void* auxiliary);
  bool recursivelyMatches(
      ExpressionTestAuxiliary test, Context* context = nullptr,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      void* auxiliary = nullptr) const;

  bool deepIsMatrix(Context* context, bool canContainMatrices = true,
                    bool isReduced = true) const;
  bool deepIsList(Context* context) const;
  // Set of ExpressionTest that can be used with recursivelyMatches
  static bool IsUninitialized(const Expression e, Context* context) {
    return e.isUninitialized();
  }
  static bool IsUndefined(const Expression e, Context* context) {
    return e.isUndefined();
  }
  static bool IsNAry(const Expression e, Context* context);
  static bool IsApproximate(const Expression e, Context* context);
  static bool IsRandom(const Expression e, Context* context);
  static bool IsMatrix(const Expression e, Context* context);
  static bool IsInfinity(const Expression e, Context* context);
  static bool IsPercent(const Expression e, Context* context);
  static bool IsDiscontinuous(const Expression e, Context* context);
  static bool IsSymbolic(const Expression e, Context* context);
  static bool IsPoint(const Expression e, Context*) {
    return e.isUndefined() || e.type() == ExpressionNode::Type::Point;
  }
  static bool IsSequence(const Expression e, Context* context) {
    return e.type() == ExpressionNode::Type::Sequence;
  }

  typedef bool (*PatternTest)(const Expression& e, Context* context,
                              const char* symbol);
  static bool IsRationalFraction(const Expression& e, Context* context,
                                 const char* symbol);
  bool isLinearCombinationOfFunction(Context* context, PatternTest testFunction,
                                     const char* symbol) const;
  bool deepIsSymbolic(Context* context,
                      SymbolicComputation replaceSymbols) const;

  /* polynomialDegree returns:
   * - (-1) if the expression is not a polynomial
   * - the degree of the polynomial otherwise */
  int polynomialDegree(Context* context, const char* symbolName) const {
    return this->node()->polynomialDegree(context, symbolName);
  }
  /* getVariables fills the matrix variables with the symbols in the expression
   * that pass the test isVariable. It returns the number of entries filled in
   * variables. For instance, getVariables of 'x+y+2*w/cos(4)' would result in
   * variables = {"x", "y", "w"} and would return 3. If the final number of
   * variables would overflow the maxNumberOfVariables, getVariables return -1.
   * If one of the variable lengths overflows maxVariableSize, getVariables
   * returns -2. */
  constexpr static int k_maxNumberOfVariables = 6;
  int getVariables(Context* context, ExpressionNode::isVariableTest isVariable,
                   char* variables, int maxVariableSize,
                   int nextVariableIndex = 0) const {
    return node()->getVariables(context, isVariable, variables, maxVariableSize,
                                nextVariableIndex);
  }
  /* getLinearCoefficients return false if the expression is not linear with
   * the variables hold in 'variables'. Otherwise, it fills 'coefficients' with
   * the coefficients of the variables hold in 'variables' (following the same
   * order) and 'constant' with the constant of the expression. */
  bool getLinearCoefficients(char* variables, int maxVariableSize,
                             Expression coefficients[], Expression* constant,
                             Context* context,
                             Preferences::ComplexFormat complexFormat,
                             Preferences::AngleUnit angleUnit,
                             Preferences::UnitFormat unitFormat,
                             SymbolicComputation symbolicComputation) const;
  constexpr static int k_maxPolynomialDegree = 3;
  constexpr static int k_maxNumberOfPolynomialCoefficients =
      k_maxPolynomialDegree + 1;
  int getPolynomialReducedCoefficients(const char* symbolName,
                                       Expression coefficients[],
                                       Context* context,
                                       Preferences::ComplexFormat complexFormat,
                                       Preferences::AngleUnit angleUnit,
                                       Preferences::UnitFormat unitFormat,
                                       SymbolicComputation symbolicComputation,
                                       bool keepDependencies = false) const;
  Expression replaceSymbolWithExpression(const SymbolAbstract& symbol,
                                         const Expression& expression) {
    return node()->replaceSymbolWithExpression(symbol, expression);
  }

  // These three functions are helpers for the function additional results
  int numberOfNumericalValues() const {
    return node()->numberOfNumericalValues();
  }
  /* Generalizes an expression to a function, by replacing numerical values with
   * a symbol. Beware that 2^3 is generalized as x^3, not x^x. */
  void replaceNumericalValuesWithSymbol(Symbol x);
  /* Return any numerical value in the expression. */
  float getNumericalValue();

  /* Units */
  // Call this method on properly reduced expressions only
  Expression removeUnit(Expression* unit) { return node()->removeUnit(unit); }
  bool hasUnit(bool ignoreAngleUnits = false, bool replaceSymbols = false,
               Context* ctx = nullptr) const;
  bool isPureAngleUnit() const;
  bool isInRadians(Context* context) const;

  /* Complex */
  static bool EncounteredComplex();
  static void SetEncounteredComplex(bool encounterComplex);
  bool hasComplexI(
      Context* context,
      SymbolicComputation replaceSymbols =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition) const;
  // WARNING: this methods must be called on reduced expressions
  bool isReal(Context* context, bool canContainMatrices = true) const;

  static void SetReductionEncounteredUndistributedList(bool encounter);

  /* Comparison */
  /* isIdenticalTo is the "easy" equality, it returns true if both trees have
   * same structures and all their nodes have same types and values (ie,
   * sqrt(pi^2) is NOT identical to pi). */
  bool isIdenticalTo(const Expression e) const;
  /* isIdenticalToWithoutParentheses behaves as isIdenticalTo, but without
   * taking into account parentheses: e^(0) is identical to e^0. */
  bool isIdenticalToWithoutParentheses(const Expression e) const;
  bool containsSameDependency(const Expression e,
                              const ReductionContext& reductionContext) const;

  static bool ExactAndApproximateExpressionsAreEqual(
      Expression exactExpression, Expression approximateExpression);

  /* Layout Helper */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits, Context* context,
                      bool forceStripMargin = false, bool nested = false) const;
  ExpressionNode::LayoutShape leftLayoutShape() const {
    return node()->leftLayoutShape();
  }
  ExpressionNode::LayoutShape rightLayoutShape() const {
    return node()->rightLayoutShape();
  }

  /* TODO:
   * - change signature to
   *   size_t serialize(char * buffer, size_t bufferSize...)
   * - Use same convention as strlcpy: return size of the source even if the
   * bufferSize was too small.*/
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode =
                    Preferences::PrintFloatMode::Decimal,
                int numberOfSignificantDigits =
                    PrintFloat::k_numberOfStoredSignificantDigits) const;

  /* Simplification */
  /* Simplification routines are divided in 2 groups:
   * - ParseAndSimplify & simplify methods are used before approximating the
   *   expression. We simplify beforehand to avoid precision error but the
   *   simplified expression is never displayed. The ReductionTarget is
   *   therefore the System for these methods.
   * - ParseAndSimplifyAndApproximate & simplifyAndApproximate methods are used
   *   to simplify and approximate the expression for the User. They take into
   *   account the complex format required in the expression they return.
   *   (For instance, in Polar mode, they return an expression of the form
   *   r*e^(i*th) reduced and approximated.) */
  static Expression ParseAndSimplify(
      const char* text, Context* context,
      Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      UnitConversion unitConversion = UnitConversion::Default,
      bool* reductionFailure = nullptr);
  Expression cloneAndSimplify(ReductionContext reductionContext,
                              bool* reductionFailure = nullptr);

  static void ParseAndSimplifyAndApproximate(
      const char* text, Expression* parsedExpression,
      Expression* simplifiedExpression, Expression* approximateExpression,
      Context* context, Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      UnitConversion unitConversion = UnitConversion::Default);
  void cloneAndSimplifyAndApproximate(
      Expression* simplifiedExpression, Expression* approximateExpression,
      Context* context, Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      UnitConversion unitConversion = UnitConversion::Default,
      bool approximateKeepingSymbols = false) const;
  Expression cloneAndReduce(ReductionContext reductionContext) const;
  /* TODO: deepReduceWithSystemCheckpoint should be private but we need to make
   * poincare/test/helper.h a class to be able to friend it */
  Expression cloneAndDeepReduceWithSystemCheckpoint(
      ReductionContext* reductionContext, bool* reduceFailure,
      bool approximateDuringReduction = false) const;
  Expression cloneAndReduceAndRemoveUnit(ReductionContext reductionContext,
                                         Expression* unit) const;
  Expression cloneAndReduceOrSimplify(ReductionContext reductionContext,
                                      bool beautify) {
    return beautify ? cloneAndSimplify(reductionContext)
                    : cloneAndReduce(reductionContext);
  }
  /* WARNING: this must be called only on expressions that:
   *  - are reduced.
   *  - have a known sign. (isPositive() != Unknown) */
  Expression setSign(bool positive, const ReductionContext& reductionContext);

  Expression deepRemoveUselessDependencies(
      const ReductionContext& reductionContext);
  Expression deepReplaceSymbols(const ReductionContext& reductionContext);

  /* 'ExpressionWithoutSymbols' replaces symbols in place and returns an
   * uninitialized expression if it is circularly defined.
   * SymbolicComputation defines how to handle functions
   * and undefined symbols. */
  static Expression ExpressionWithoutSymbols(
      Expression expressionWithSymbols, Context* context,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);

  Expression radianToAngleUnit(Preferences::AngleUnit angleUnit);
  Expression angleUnitToRadian(Preferences::AngleUnit angleUnit);

  /* Returns:
   * - a number >= 0 if all the lists have the same size,
   * - -1 if there are no lists in the children
   * - -2 if there are lists of differents lengths. */
  constexpr static int k_noList = -1;
  constexpr static int k_mismatchedLists = -2;
  /* LengthOfListChildren is to be called during reduction, when all children
   * are already reduced. If you expression is NAry, it must be sorted before
   * calling this function. */
  int lengthOfListChildren() const;

  /* Approximation Helper
   * These methods reset the sApproximationEncounteredComplex flag. They should
   * not be use to implement node approximation */
  template <typename U>
  static U ParseAndSimplifyAndApproximateToScalar(
      const char* text, Context* context,
      Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat,
      SymbolicComputation symbolicComputation =
          SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition);
  template <typename U>
  Expression approximate(Context* context,
                         Preferences::ComplexFormat complexFormat,
                         Preferences::AngleUnit angleUnit,
                         bool withinReduce = false) const;
  // WARNING: this will reduce the expression before removing units
  template <typename U>
  Expression approximateKeepingUnits(
      const ReductionContext& reductionContext) const;
  template <typename U>
  U approximateToScalar(Context* context,
                        Preferences::ComplexFormat complexFormat,
                        Preferences::AngleUnit angleUnit,
                        bool withinReduce = false) const;
  template <typename U>
  U approximateWithValueForSymbol(const char* symbol, U x, Context* context,
                                  Preferences::ComplexFormat complexFormat,
                                  Preferences::AngleUnit angleUnit) const;
  // This also reduces the expression. Approximation is in double.
  Expression cloneAndApproximateKeepingSymbols(
      ReductionContext reductionContext) const;

  /* This class is meant to contain data about named functions (e.g. sin,
   * tan...) in one place: their name, their number of children and a pointer to
   * a builder. It is used in particular by the parser. */
  class FunctionHelper {
   public:
    constexpr FunctionHelper(AliasesList aliasesList,
                             const int minNumberOfChildren,
                             const int maxNumberOfChildren,
                             Expression (*const builder)(Expression))
        : FunctionHelper(aliasesList, minNumberOfChildren, maxNumberOfChildren,
                         builder, nullptr, 0) {}
    constexpr FunctionHelper(AliasesList aliasesList,
                             const int numberOfChildren,
                             Expression (*const builder)(Expression))
        : FunctionHelper(aliasesList, numberOfChildren, numberOfChildren,
                         builder) {}
    constexpr FunctionHelper(AliasesList aliasesList,
                             const int minNumberOfChildren,
                             const int maxNumberOfChildren,
                             TreeNode::Initializer initializer, size_t size)
        : FunctionHelper(aliasesList, minNumberOfChildren, maxNumberOfChildren,
                         nullptr, initializer, size) {}
    constexpr AliasesList aliasesList() const { return m_aliasesList; }
    int minNumberOfChildren() const { return m_minNumberOfChildren; }
    int maxNumberOfChildren() const { return m_maxNumberOfChildren; }
    int numberOfChildren() const {
      assert(m_minNumberOfChildren == m_maxNumberOfChildren);
      return m_minNumberOfChildren;
    }
    Expression build(Expression children) const;

   private:
    constexpr FunctionHelper(AliasesList aliasesList,
                             const int minNumberOfChildren,
                             const int maxNumberOfChildren,
                             Expression (*const builder)(Expression),
                             TreeNode::Initializer initializer, size_t size)
        : m_aliasesList(aliasesList),
          m_minNumberOfChildren(minNumberOfChildren),
          m_maxNumberOfChildren(maxNumberOfChildren),
          m_untypedBuilder(builder),
          m_initializer(initializer),
          m_size(size) {
      assert(minNumberOfChildren >= 0 &&
             minNumberOfChildren <= maxNumberOfChildren);
    }
    AliasesList m_aliasesList;
    const int m_minNumberOfChildren;
    const int m_maxNumberOfChildren;
    Expression (*const m_untypedBuilder)(Expression children);
    TreeNode::Initializer m_initializer;
    const size_t m_size;
  };

  /* Tuple */
  typedef std::initializer_list<Expression> Tuple;

  /* Iterator */
  friend Direct<Expression, ExpressionNode>;
  Direct<Expression, ExpressionNode> directChildren() const {
    return Direct<Expression, ExpressionNode>(*this);
  }

 protected:
  Expression(const ExpressionNode* n) : TreeHandle(n) {}

  template <typename U>
  static Expression UntypedBuilderOneChild(Expression children) {
    assert(children.type() == ExpressionNode::Type::List);
    return U::Builder(children.childAtIndex(0));
  }
  template <typename U>
  static Expression UntypedBuilderTwoChildren(Expression children) {
    assert(children.type() == ExpressionNode::Type::List);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1));
  }
  template <typename U>
  static Expression UntypedBuilderThreeChildren(Expression children) {
    assert(children.type() == ExpressionNode::Type::List);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1),
                      children.childAtIndex(2));
  }
  template <typename U>
  static Expression UntypedBuilderFourChildren(Expression children) {
    assert(children.type() == ExpressionNode::Type::List);
    return U::Builder(children.childAtIndex(0), children.childAtIndex(1),
                      children.childAtIndex(2), children.childAtIndex(3));
  }
  template <typename U>
  static Expression UntypedBuilderMultipleChildren(Expression children) {
    // Only with Expression classes implementing addChildAtIndexInPlace
    assert(children.type() == ExpressionNode::Type::List);
    int childrenNumber = children.numberOfChildren();
    assert(childrenNumber > 0);
    U expression = U::Builder({children.childAtIndex(0)});
    for (int i = 1; i < childrenNumber; ++i) {
      expression.addChildAtIndexInPlace(children.childAtIndex(i), i, i);
    }
    return std::move(expression);
  }

  template <class T>
  T convert() const {
    /* This function allows to convert Expression to derived Expressions.
     * The asserts ensure that the Expression can only be casted to another
     * Expression, but this does not prevent Expression types mismatches (cast
     * Float to Symbol for instance).
     *
     * We could have overridden the operator T(). However, even with the
     * 'explicit' keyword (which prevents implicit casts), direct initialization
     * are enable which can lead to weird code:
     * ie, you can write: 'Rational a(2); AbsoluteValue b(a);'
     * */

    assert(T::IsExpression());
    static_assert(sizeof(T) == sizeof(Expression), "Size mismatch");
    return *reinterpret_cast<T*>(const_cast<Expression*>(this));
  }

  static_assert(sizeof(TreeHandle::Tuple) == sizeof(Tuple), "Size mismatch");
  static const TreeHandle::Tuple& convert(const Tuple& l) {
    assert(sizeof(TreeHandle) == sizeof(Expression));
    return reinterpret_cast<const TreeHandle::Tuple&>(l);
  }

  /* Reference */
  ExpressionNode* node() const {
    assert(identifier() != TreeNode::NoNodeIdentifier &&
           !TreeHandle::node()->isGhost());
    return static_cast<ExpressionNode*>(TreeHandle::node());
  }

  /* Hierarchy */
  Expression parent() const;  // TODO try to inline
  Expression replaceWithUndefinedInPlace();
  void defaultSetChildrenInPlace(Expression other);
  void addChildAtIndexInPlace(TreeHandle t, int index,
                              int currentNumberOfChildren) = delete;
  void removeChildAtIndexInPlace(int i) = delete;
  void removeChildInPlace(TreeHandle t, int childNumberOfChildren) = delete;
  void removeChildrenInPlace(int currentNumberOfChildren) = delete;

  /* Properties */
  /* getPolynomialCoefficients fills the table coefficients with the expressions
   * of the first 3 polynomial coefficients and returns the  polynomial degree.
   * It is supposed to be called on a reduced expression.
   * coefficients has up to 3 entries.  */
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                Expression coefficients[]) const {
    return node()->getPolynomialCoefficients(context, symbolName, coefficients);
  }
  Expression deepReplaceSymbolWithExpression(const SymbolAbstract& symbol,
                                             const Expression expression);
  /* 'deepReplaceReplaceableSymbols' replaces symbols in place. isCircular
   * indicates if it is circularly defined and has been interrupted.
   * SymbolicComputation defines how to handle functions and undefined symbols.
   */
  bool involvesCircularity(Context* context, int maxDepth,
                           const char** visitedFunctions,
                           int numberOfVisitedFunctions) {
    return node()->involvesCircularity(context, maxDepth, visitedFunctions,
                                       numberOfVisitedFunctions);
  }
  Expression deepReplaceReplaceableSymbols(
      Context* context, TrinaryBoolean* isCircular,
      int parameteredAncestorsCount, SymbolicComputation symbolicComputation) {
    return node()->deepReplaceReplaceableSymbols(
        context, isCircular, parameteredAncestorsCount, symbolicComputation);
  }
  Expression defaultReplaceReplaceableSymbols(
      Context* context, TrinaryBoolean* isCircular,
      int parameteredAncestorsCount, SymbolicComputation symbolicComputation);

  /* Simplification */
  static void SimplifyAndApproximateChildren(
      Expression input, Expression* simplifiedOutput,
      Expression* approximateOutput, const ReductionContext& reductionContext);
  void beautifyAndApproximateScalar(Expression* simplifiedExpression,
                                    Expression* approximateExpression,
                                    ReductionContext userReductionContext);
  /* makePositiveAnyNegativeNumeralFactor looks for:
   * - a negative numeral
   * - a multiplication who has one numeral child whose is negative
   * and turns the negative factor into a positive one.
   * The given Expression should already be reduced and the return Expression
   * is reduced (only a numeral factor was potentially made positive, and if it
   *  was -1, it was removed from the multiplication).
   * Warning: this must be called on reduced expressions
   */
  Expression shallowBeautify(const ReductionContext& reductionContext) {
    return node()->shallowBeautify(reductionContext);
  }
  Expression makePositiveAnyNegativeNumeralFactor(
      const ReductionContext& reductionContext);
  Expression denominator(const ReductionContext& reductionContext) const {
    return node()->denominator(reductionContext);
  }
  /* shallowReduce takes a copy of reductionContext and not a reference
   * because it might need to modify it during reduction, namely in
   * SimplificationHelper::undefinedOnMatrix */
  Expression shallowReduce(ReductionContext reductionContext) {
    return node()->shallowReduce(reductionContext);
  }
  Expression deepBeautify(const ReductionContext& reductionContext) {
    return node()->deepBeautify(reductionContext);
  }

  /* Derivation */
  /* This method is used for the reduction of Derivative expressions.
   * It returns whether the instance is differentiable, and differentiates it if
   * able. */
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue);
  void derivateChildAtIndexInPlace(int index,
                                   const ReductionContext& reductionContext,
                                   Symbol symbol, Expression symbolValue);
  Expression unaryFunctionDifferential(
      const ReductionContext& reductionContext) {
    return node()->unaryFunctionDifferential(reductionContext);
  }

 private:
  constexpr static int k_maxSymbolReplacementsCount = 10;

  /* Add missing parenthesis will add parentheses that easen the reading of the
   * expression or that are required by math rules. For example:
   * 2+-1 --> 2+(-1)
   * *(+(2,1),3) --> (2+1)*3
   */
  Expression addMissingParentheses();
  void shallowAddMissingParenthesis();

  /* Simplification */
  /* The largest integer such that all smaller integers can be stored without
   * any precision loss in IEEE754 double representation is 2E53 as the
   * mantissa is stored on 53 bits (2E308 can be stored exactly in IEEE754
   * representation but some smaller integers can't - like 2E308-1). */
  constexpr static double k_largestExactIEEE754Integer = 9007199254740992.0;
  Expression deepReduce(ReductionContext reductionContext);
  void deepReduceChildren(const ReductionContext& reductionContext) {
    node()->deepReduceChildren(reductionContext);
  }

  Expression shallowReduceUsingApproximation(
      const ReductionContext& reductionContext);
  Expression defaultShallowBeautify() { return *this; }

  Expression defaultUnaryFunctionDifferential() { return *this; }

  /* Approximation */
  template <typename U>
  Evaluation<U> approximateToEvaluation(
      Context* context, Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit, bool withinReduce = false) const;

  Expression deepApproximateKeepingSymbols(ReductionContext reductionContext,
                                           bool* parentCanApproximate,
                                           bool* parentShouldReduce);
  Expression deepApproximateKeepingSymbols(ReductionContext reductionContext) {
    bool dummy = false;
    return deepApproximateKeepingSymbols(reductionContext, &dummy, &dummy);
  }
  void deepApproximateChildrenKeepingSymbols(
      const ReductionContext& reductionContextbool, bool* canApproximate,
      bool* shouldReduce);

  /* Properties */
  int defaultGetPolynomialCoefficients(int degree, Context* context,
                                       const char* symbol,
                                       Expression expression[]) const;

  /* Builder */
  static Expression CreateComplexExpression(
      Expression ra, Expression tb, Preferences::ComplexFormat complexFormat,
      bool isNegativeRa, bool isNegativeTb);
};

// Helper to create the expression associated to a node
template <typename T, typename U, int Nmin, int Nmax,
          typename Parent = Expression>
class ExpressionBuilder : public Parent {
 public:
#ifndef PLATFORM_DEVICE
  static_assert(std::is_base_of<Expression, Parent>::value);
#endif
  ExpressionBuilder(const U* n) : Parent(n) {}
  static T Builder() {
    static_assert(Nmin <= 0 && Nmax >= 0);
    TreeHandle h =
        TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(Expression child) {
    static_assert(Nmin <= 1 && Nmax >= 1);
    TreeHandle h =
        TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {child});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(Expression child1, Expression child2) {
    static_assert(Nmin <= 2 && Nmax >= 2);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U),
                                                   {child1, child2});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(Expression child1, Expression child2, Expression child3) {
    static_assert(Nmin <= 3 && Nmax >= 3);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U),
                                                   {child1, child2, child3});
    T expression = static_cast<T&>(h);
    return expression;
  }
  static T Builder(Expression child1, Expression child2, Expression child3,
                   Expression child4) {
    static_assert(Nmin <= 4 && Nmax >= 4);
    TreeHandle h = TreeHandle::BuilderWithChildren(
        Initializer<U>, sizeof(U), {child1, child2, child3, child4});
    T expression = static_cast<T&>(h);
    return expression;
  }
  constexpr static Expression::FunctionHelper s_functionHelper =
      Expression::FunctionHelper(U::k_functionName, Nmin, Nmax, Initializer<U>,
                                 sizeof(U));
};

template <typename T, typename U, typename P = Expression>
using ExpressionNoChildren = ExpressionBuilder<T, U, 0, 0, P>;
template <typename T, typename U, typename P = Expression>
using ExpressionOneChild = ExpressionBuilder<T, U, 1, 1, P>;
template <typename T, typename U, typename P = Expression>
using ExpressionTwoChildren = ExpressionBuilder<T, U, 2, 2, P>;
template <typename T, typename U, typename P = Expression>
using ExpressionUpToTwoChildren = ExpressionBuilder<T, U, 1, 2, P>;
template <typename T, typename U, typename P = Expression>
using ExpressionThreeChildren = ExpressionBuilder<T, U, 3, 3, P>;
template <typename T, typename U, typename P = Expression>
using ExpressionFourChildren = ExpressionBuilder<T, U, 4, 4, P>;
}  // namespace Poincare

#endif

#include <iostream>
#include <assert.h>
#include <float.h>
#include <ion/circuit_breaker.h>
#include <poincare/absolute_value.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/float.h>
#include <poincare/horizontal_layout.h>
#include <poincare/matrix.h>
#include <poincare/matrix_layout.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <cmath>
#include <utility>

namespace Poincare {

int MatrixNode::polynomialDegree(Context *context,
                                 const char *symbolName) const {
  return -1;
}

Expression MatrixNode::shallowReduce(const ReductionContext &reductionContext) {
  return Matrix(this).shallowReduce(reductionContext);
}

Layout MatrixNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits,
                                Context *context) const {
  assert(numberOfChildren() > 0);
  MatrixLayout layout = MatrixLayout::Builder();
  for (ExpressionNode *c : children()) {
    layout.addChildAtIndexInPlace(
        c->createLayout(floatDisplayMode, numberOfSignificantDigits, context),
        layout.numberOfChildren(), layout.numberOfChildren());
  }
  layout.setDimensions(m_numberOfRows, m_numberOfColumns);
  return std::move(layout);
}

int MatrixNode::serialize(char *buffer, int bufferSize,
                          Preferences::PrintFloatMode floatDisplayMode,
                          int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
  }
  buffer[bufferSize - 1] = 0;
  if (bufferSize == 1) {
    return bufferSize - 1;
  }
  int currentChar = SerializationHelper::CodePoint(buffer, bufferSize, '[');
  if (currentChar >= bufferSize - 1) {
    return currentChar;
  }
  for (int i = 0; i < m_numberOfRows; i++) {
    currentChar += SerializationHelper::CodePoint(
        buffer + currentChar, bufferSize - currentChar, '[');
    if (currentChar >= bufferSize - 1) {
      return currentChar;
    }
    currentChar +=
        childAtIndex(i * m_numberOfColumns)
            ->serialize(buffer + currentChar, bufferSize - currentChar,
                        floatDisplayMode, numberOfSignificantDigits);
    if (currentChar >= bufferSize - 1) {
      return currentChar;
    }
    for (int j = 1; j < m_numberOfColumns; j++) {
      currentChar += SerializationHelper::CodePoint(
          buffer + currentChar, bufferSize - currentChar, ',');
      if (currentChar >= bufferSize - 1) {
        return currentChar;
      }
      currentChar +=
          childAtIndex(i * m_numberOfColumns + j)
              ->serialize(buffer + currentChar, bufferSize - currentChar,
                          floatDisplayMode, numberOfSignificantDigits);
      if (currentChar >= bufferSize - 1) {
        return currentChar;
      }
    }
    currentChar = strlen(buffer);
    if (currentChar >= bufferSize - 1) {
      return currentChar;
    }
    currentChar += SerializationHelper::CodePoint(
        buffer + currentChar, bufferSize - currentChar, ']');
    if (currentChar >= bufferSize - 1) {
      return currentChar;
    }
  }
  currentChar += SerializationHelper::CodePoint(buffer + currentChar,
                                                bufferSize - currentChar, ']');
  return std::min(currentChar, bufferSize - 1);
}

template <typename T>
Evaluation<T> MatrixNode::templatedApproximate(
    const ApproximationContext &approximationContext) const {
  MatrixComplex<T> matrix = MatrixComplex<T>::Builder();
  for (ExpressionNode *c : children()) {
    matrix.addChildAtIndexInPlace(c->approximate(T(), approximationContext),
                                  matrix.numberOfChildren(),
                                  matrix.numberOfChildren());
  }
  matrix.setDimensions(numberOfRows(), numberOfColumns());
  return std::move(matrix);
}

// MATRIX

void Matrix::setDimensions(int rows, int columns) {
  assert(rows * columns == numberOfChildren());
  setNumberOfRows(rows);
  setNumberOfColumns(columns);
}

void Matrix::addChildrenAsRowInPlace(TreeHandle t, int i) {
  int previousNumberOfColumns = numberOfColumns();
  if (previousNumberOfColumns > 0) {
    assert(t.numberOfChildren() == numberOfColumns());
  }
  int previousNumberOfRows = numberOfRows();
  int childrenNumber = t.numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    addChildAtIndexInPlace(t.childAtIndex(i), numberOfChildren(),
                           numberOfChildren());
  }
  setDimensions(previousNumberOfRows + 1, previousNumberOfColumns == 0
                                              ? t.numberOfChildren()
                                              : previousNumberOfColumns);
}

int Matrix::rank(Context *context, bool forceCanonization) {
  assert(!recursivelyMatches(Expression::IsUninitialized, context));
  if (recursivelyMatches(Expression::IsUndefined, context)) {
    return -1;
  }
  Expression m;
  ReductionContext systemReductionContext =
      ReductionContext::DefaultReductionContextForAnalysis(context);

  bool canonizationSuccess = true;
  {
    ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      Matrix cannonizedM = clone().convert<Matrix>();
      m = cannonizedM.rowCanonize(systemReductionContext, &canonizationSuccess,
                                  nullptr, true, forceCanonization);
    } else {
      /* rowCanonize can create expression that are too big for the pool.
       * If it's the case, compute the rank with approximated values. */
      Expression mApproximation =
          approximate<double>(context, systemReductionContext.complexFormat(),
                              systemReductionContext.angleUnit());
      if (mApproximation.type() != ExpressionNode::Type::Matrix) {
        /* The approximation was able to conclude that a coefficient is undef
         * while the reduction could not. */
        return -1;
      }
      m = static_cast<Matrix &>(mApproximation)
              .rowCanonize(systemReductionContext, &canonizationSuccess,
                           nullptr, true, forceCanonization);
    }
  }

  if (!canonizationSuccess) {
    return -1;
  }

  assert(m.type() == ExpressionNode::Type::Matrix);
  Matrix cannonizedMatrix = static_cast<Matrix &>(m);
  *this = cannonizedMatrix;

  int rank = cannonizedMatrix.numberOfRows();
  int i = rank - 1;
  while (i >= 0) {
    int j = cannonizedMatrix.numberOfColumns() - 1;
    // TODO: Handle TrinaryBoolean::Unknown. See rowCanonize comment
    while (j >= i && cannonizedMatrix.matrixChild(i, j).isNull(context) ==
                         TrinaryBoolean::True) {
      j--;
    }
    if (j <= i - 1) {
      rank--;
    } else {
      break;
    }
    i--;
  }
  return rank;
}

Expression Matrix::createTrace() {
  assert(numberOfRows() == numberOfColumns());
  int n = numberOfRows();
  Addition a = Addition::Builder();
  for (int i = 0; i < n; i++) {
    a.addChildAtIndexInPlace(matrixChild(i, i).clone(), i, i);
  }
  return std::move(a);
}

template <typename T>
int Matrix::ArrayInverse(T *array, int numberOfRows, int numberOfColumns) {
  if (numberOfRows != numberOfColumns) {
    return -1;
  }
  assert(numberOfRows * numberOfColumns <= k_maxNumberOfChildren);
  int dim = numberOfRows;
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim
   * identity matrix (A is squared with less than k_maxNumberOfChildren so (A|I)
   * has less than 2*k_maxNumberOfChildren */
  T operands[2 * k_maxNumberOfChildren];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      T cell = array[i * numberOfColumns + j];
      // Using abs function to be compatible with both double and std::complex
      if (!std::isfinite(std::abs(cell))) {
        return -2;
      }
      operands[i * 2 * dim + j] = cell;
    }
    for (int j = dim; j < 2 * dim; j++) {
      operands[i * 2 * dim + j] = j - dim == i ? 1.0 : 0.0;
    }
  }
  ArrayRowCanonize(operands, dim, 2 * dim);
  // Check inversibility
  for (int i = 0; i < dim; i++) {
    T cell = operands[i * 2 * dim + i];
    if (!std::isfinite(std::abs(cell)) ||
        std::abs(cell - static_cast<T>(1.0)) > Float<float>::EpsilonLax()) {
      return -2;
    }
  }
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      array[i * numberOfColumns + j] = operands[i * 2 * dim + j + dim];
    }
  }
  return 0;
}

bool Matrix::isCanonizable(const ReductionContext &reductionContext) {
  int m = numberOfRows();
  int n = numberOfColumns();
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      if (std::isnan(
              AbsoluteValue::Builder(matrixChild(i, j).clone())
                  .approximateToScalar<float>(reductionContext.context(),
                                              reductionContext.complexFormat(),
                                              reductionContext.angleUnit()))) {
        return false;
      }
    }
  }
  return true;
}

Matrix Matrix::rowCanonize(const ReductionContext &reductionContext,
                           bool *canonizationSuccess, Expression *determinant,
                           bool reduced, bool forceCanonization) {
  assert(canonizationSuccess);
  *canonizationSuccess = true;

  // The matrix children have to be reduced to be able to spot 0
  deepReduceChildren(reductionContext);

  if (!forceCanonization && !isCanonizable(reductionContext)) {
    *canonizationSuccess = false;
    return *this;
  }

  Multiplication det = Multiplication::Builder();

  int m = numberOfRows();
  int n = numberOfColumns();

  int h = 0;  // row pivot
  int k = 0;  // column pivot

  while (h < m && k < n) {
    /* In non-reduced form, the pivot selection method will affect the output.
     * Here we prioritize the biggest pivot (in value) to get an output that
     * does not depends on the order of the rows of the matrix.
     * We could also take lowest non null pivots, or just first non null as we
     * already do with reduced forms. Output would be different, but correct. */
    int iPivot_temp = h;
    int iPivot = h;
    float bestPivot = 0.0;
    while (iPivot_temp < m) {
      // Using float to find the biggest pivot is sufficient.
      float pivot =
          AbsoluteValue::Builder(matrixChild(iPivot_temp, k).clone())
              .approximateToScalar<float>(reductionContext.context(),
                                          reductionContext.complexFormat(),
                                          reductionContext.angleUnit());
      // Handle very low pivots
      if (pivot == 0.0f &&
          matrixChild(iPivot_temp, k).isNull(reductionContext.context()) !=
              TrinaryBoolean::True) {
        pivot = FLT_MIN;
      }

      if (pivot > bestPivot) {
        // Update best pivot
        bestPivot = pivot;
        iPivot = iPivot_temp;
        if (reduced) {
          /* In reduced form, taking the first non null pivot is enough, and
           * more efficient. */
          break;
        }
      }
      iPivot_temp++;
    }
    /* TODO: Handle isNull == TrinaryBoolean::Unknown : rowCanonize will
     * output a mathematically wrong result (and divide expressions by a null
     * expression) if expression is actually null. For examples,
     * 1-cos(x)^2-sin(x)^2 would be mishandled. */
    if (matrixChild(iPivot, k).isNull(reductionContext.context()) ==
        TrinaryBoolean::True) {
      // No non-null coefficient in this column, skip
      k++;
      if (determinant) {
        // Update determinant: det *= 0
        det.addChildAtIndexInPlace(Rational::Builder(0), det.numberOfChildren(),
                                   det.numberOfChildren());
      }
    } else {
      // Swap row h and iPivot
      if (iPivot != h) {
        for (int col = h; col < n; col++) {
          swapChildrenInPlace(iPivot * n + col, h * n + col);
        }
        if (determinant) {
          // Update determinant: det *= -1
          det.addChildAtIndexInPlace(Rational::Builder(-1),
                                     det.numberOfChildren(),
                                     det.numberOfChildren());
        }
      }
      // Set to 1 M[h][k] by linear combination
      Expression divisor = matrixChild(h, k);
      // Update determinant: det *= divisor
      if (determinant) {
        det.addChildAtIndexInPlace(divisor.clone(), det.numberOfChildren(),
                                   det.numberOfChildren());
      }
      for (int j = k + 1; j < n; j++) {
        Expression opHJ = matrixChild(h, j);
        Expression newOpHJ = Division::Builder(opHJ, divisor.clone());
        replaceChildAtIndexInPlace(h * n + j, newOpHJ);
        newOpHJ = newOpHJ.shallowReduce(reductionContext);
        if (reductionContext.target() == ReductionTarget::SystemForAnalysis &&
            newOpHJ.type() == ExpressionNode::Type::Dependency) {
          Expression e = newOpHJ.childAtIndex(0);
          newOpHJ.replaceChildAtIndexWithGhostInPlace(0);
          newOpHJ.replaceWithInPlace(e);
        }
      }
      replaceChildInPlace(divisor, Rational::Builder(1));

      int l = reduced ? 0 : h + 1;
      /* Set to 0 all M[i][j] i != h, j > k by linear combination. If a
       * non-reduced form is computed (ref), only rows below the pivot are
       * reduced, i > h as well */
      for (int i = l; i < m; i++) {
        if (i == h) {
          continue;
        }
        Expression factor = matrixChild(i, k);
        for (int j = k + 1; j < n; j++) {
          Expression opIJ = matrixChild(i, j);
          Expression newOpIJ = Subtraction::Builder(
              opIJ, Multiplication::Builder(matrixChild(h, j).clone(),
                                            factor.clone()));
          replaceChildAtIndexInPlace(i * n + j, newOpIJ);
          newOpIJ.childAtIndex(1).shallowReduce(reductionContext);
          newOpIJ = newOpIJ.shallowReduce(reductionContext);
          if (reductionContext.target() == ReductionTarget::SystemForAnalysis &&
              newOpIJ.type() == ExpressionNode::Type::Dependency) {
            Expression e = newOpIJ.childAtIndex(0);
            newOpIJ.replaceChildAtIndexWithGhostInPlace(0);
            newOpIJ.replaceWithInPlace(e);
          }
        }
        replaceChildAtIndexInPlace(i * n + k, Rational::Builder(0));
      }
      h++;
      k++;
    }
  }
  if (determinant) {
    *determinant = det;
  }
  return *this;
}

template <typename T>
void Matrix::ArrayRowCanonize(T *array, int numberOfRows, int numberOfColumns,
                              T *determinant, bool reduced) {
  int h = 0;  // row pivot
  int k = 0;  // column pivot

  while (h < numberOfRows && k < numberOfColumns) {
    // Find the biggest pivot (in absolute value). See comment on rowCanonize.
    int iPivot_temp = h;
    int iPivot = h;
    // Using double to stay accurate with any type T
    double bestPivot = 0.0;
    while (iPivot_temp < numberOfRows) {
      double pivot = std::abs(array[iPivot_temp * numberOfColumns + k]);
      if (pivot > bestPivot) {
        // Update best pivot
        bestPivot = pivot;
        iPivot = iPivot_temp;
        if (reduced) {
          /* In reduced form, taking the first non null pivot is enough, and
           * more efficient. */
          break;
        }
      }
      iPivot_temp++;
    }
    if (bestPivot < DBL_MIN) {
      // No non-null coefficient in this column, skip
      k++;
      // Update determinant: det *= 0
      if (determinant) {
        *determinant *= static_cast<T>(0.0);
      }
    } else {
      // Swap row h and iPivot
      if (iPivot != h) {
        for (int col = h; col < numberOfColumns; col++) {
          // Swap array[iPivot, col] and array[h, col]
          T temp = array[iPivot * numberOfColumns + col];
          array[iPivot * numberOfColumns + col] =
              array[h * numberOfColumns + col];
          array[h * numberOfColumns + col] = temp;
        }
        // Update determinant: det *= -1
        if (determinant) {
          *determinant *= static_cast<T>(-1.0);
        }
      }
      // Set to 1 array[h][k] by linear combination
      T divisor = array[h * numberOfColumns + k];
      // Update determinant: det *= divisor
      if (determinant) {
        *determinant *= divisor;
      }
      for (int j = k + 1; j < numberOfColumns; j++) {
        array[h * numberOfColumns + j] /= divisor;
      }
      array[h * numberOfColumns + k] = 1;

      int l = reduced ? 0 : h + 1;
      /* Set to 0 all M[i][j] i != h, j > k by linear combination. If a
       * non-reduced form is computed (ref), only rows below the pivot are
       * reduced, i > h as well */
      for (int i = l; i < numberOfRows; i++) {
        if (i == h) {
          continue;
        }
        T factor = array[i * numberOfColumns + k];
        for (int j = k + 1; j < numberOfColumns; j++) {
          array[i * numberOfColumns + j] -=
              array[h * numberOfColumns + j] * factor;
        }
        array[i * numberOfColumns + k] = 0;
      }
      h++;
      k++;
    }
  }
}

Matrix Matrix::CreateIdentity(int dim) {
  Matrix matrix = Matrix::Builder();
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      matrix.addChildAtIndexInPlace(
          i == j ? Rational::Builder(1) : Rational::Builder(0), i * dim + j,
          i * dim + j);
    }
  }
  matrix.setDimensions(dim, dim);
  return matrix;
}

Matrix Matrix::createTranspose() const {
  Matrix matrix = Matrix::Builder();
  for (int j = 0; j < numberOfColumns(); j++) {
    for (int i = 0; i < numberOfRows(); i++) {
      matrix.addChildAtIndexInPlace(
          const_cast<Matrix *>(this)->matrixChild(i, j).clone(),
          matrix.numberOfChildren(), matrix.numberOfChildren());
    }
  }
  // Intentionally swapping dimensions for transpose
  matrix.setDimensions(numberOfColumns(), numberOfRows());
  return matrix;
}

Expression Matrix::createRef(const ReductionContext &reductionContext,
                             bool *couldComputeRef, bool reduced) const {
  // Compute Matrix Row Echelon Form
  /* If the matrix is too big, the rowCanonization might not be computed exactly
   * because of a pool allocation error, but we might still be able to compute
   * it approximately. We thus encapsulate the ref creation in an exception
   * checkpoint.
   * We can safely use an exception checkpoint here because we are sure of not
   * modifying any pre-existing node in the pool. We are sure there is no Store
   * in the matrix. */
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    /* We clone the current matrix to extract its children later. We can't clone
     * its children directly. Indeed, the current matrix node (this->node()) is
     * located before the exception checkpoint. In order to clone its chidlren,
     * we would temporary increase the reference counter of each child (also
     * located before the checkpoint). If an exception is raised before
     * destroying the child handle, its reference counter would be off by one
     * after the long jump. */
    Matrix result = clone().convert<Matrix>();
    /* Reduced row echelon form is also called row canonical form. To compute
     * the row echelon form (non reduced one), fewer steps are required. */
    result =
        result.rowCanonize(reductionContext, couldComputeRef, nullptr, reduced);
    return std::move(result);
  } else {
    *couldComputeRef = false;
    return Expression();
  }
}

Expression Matrix::createInverse(const ReductionContext &reductionContext,
                                 bool *couldComputeInverse) const {
  int dim = numberOfRows();
  if (dim != numberOfColumns()) {
    *couldComputeInverse = true;
    return Undefined::Builder();
  }
  Expression result =
      computeInverseOrDeterminant(false, reductionContext, couldComputeInverse);
  assert(!(*couldComputeInverse) || !result.isUninitialized());
  return result;
}

Expression Matrix::augment(Matrix *b, const ReductionContext &reductionContext) const {
  Matrix result = Matrix::Builder();
  for(int i = 0; i < numberOfRows(); i++) {
    for(int j = 0; j < numberOfColumns(); j++) {
      result.addChildAtIndexInPlace(childAtIndex(i * numberOfColumns() + j), result.numberOfChildren(), result.numberOfChildren());
    }
    for(int j = 0; j < b->numberOfColumns(); j++) {
      result.addChildAtIndexInPlace(b->childAtIndex(i * b->numberOfColumns() + j), result.numberOfChildren(), result.numberOfChildren());
    }
  }
  result.setDimensions(numberOfRows(), numberOfColumns() + b->numberOfColumns());
  return result;
}

Expression Matrix::determinant(const ReductionContext &reductionContext,
                               bool *couldComputeDeterminant, bool inPlace) {
  // Determinant must be called on a reduced matrix only.
  *couldComputeDeterminant = true;
  Matrix m = inPlace ? *this : clone().convert<Matrix>();
  int dim = m.numberOfRows();
  if (dim != m.numberOfColumns()) {
    // Determinant is for square matrices
    return Undefined::Builder();
  }
  // Explicit formulas for dimensions from 1 to 3
  if (dim == 1) {
    // Determinant of [[a]] is a
    return m.childAtIndex(0);
  }
  if (dim == 2) {
    /*                |a b|
     * Determinant of |c d| is ad-bc   */
    Multiplication ad =
        Multiplication::Builder(m.matrixChild(0, 0), m.matrixChild(1, 1));
    Multiplication bc =
        Multiplication::Builder(m.matrixChild(0, 1), m.matrixChild(1, 0));
    Expression result = Subtraction::Builder(ad, bc);
    ad.shallowReduce(reductionContext);
    bc.shallowReduce(reductionContext);
    return result;
  }
  if (dim == 3) {
    /*                |a b c|
     * Determinant of |d e f| is aei+bfg+cdh-ceg-bdi-afh
     *                |g h i|                             */
    Expression a = m.matrixChild(0, 0);
    Expression b = m.matrixChild(0, 1);
    Expression c = m.matrixChild(0, 2);
    Expression d = m.matrixChild(1, 0);
    Expression e = m.matrixChild(1, 1);
    Expression f = m.matrixChild(1, 2);
    Expression g = m.matrixChild(2, 0);
    Expression h = m.matrixChild(2, 1);
    Expression i = m.matrixChild(2, 2);
    Tuple children = {Multiplication::Builder(a.clone(), e.clone(), i.clone()),
                      Multiplication::Builder(b.clone(), f.clone(), g.clone()),
                      Multiplication::Builder(c.clone(), d.clone(), h.clone()),
                      Multiplication::Builder(Rational::Builder(-1), c, e, g),
                      Multiplication::Builder(Rational::Builder(-1), b, d, i),
                      Multiplication::Builder(Rational::Builder(-1), a, f, h)};
    Expression result = Addition::Builder(children);
    for (Expression child : children) {
      child.shallowReduce(reductionContext);
    }
    return result;
  }

  // Dimension >= 4
  Expression result = computeInverseOrDeterminant(true, reductionContext,
                                                  couldComputeDeterminant);
  assert(!(*couldComputeDeterminant) || !result.isUninitialized());
  return result;
}

Expression Matrix::norm(const ReductionContext &reductionContext) const {
  // Norm is defined on vectors only
  assert(vectorType() != Array::VectorType::None);
  Addition sum = Addition::Builder();
  int childrenNumber = numberOfChildren();
  for (int j = 0; j < childrenNumber; j++) {
    Expression absValue = AbsoluteValue::Builder(
        const_cast<Matrix *>(this)->childAtIndex(j).clone());
    Expression squaredAbsValue = Power::Builder(absValue, Rational::Builder(2));
    absValue.shallowReduce(reductionContext);
    sum.addChildAtIndexInPlace(squaredAbsValue, sum.numberOfChildren(),
                               sum.numberOfChildren());
    squaredAbsValue.shallowReduce(reductionContext);
  }
  Expression result = SquareRoot::Builder(sum);
  sum.shallowReduce(reductionContext);
  return result;
}

Expression Matrix::dot(Matrix *b,
                       const ReductionContext &reductionContext) const {
  // Dot product is defined between two vectors of same size and type
  assert(vectorType() != Array::VectorType::None &&
         vectorType() == b->vectorType() &&
         numberOfChildren() == b->numberOfChildren());
  Addition sum = Addition::Builder();
  int childrenNumber = numberOfChildren();
  for (int j = 0; j < childrenNumber; j++) {
    Expression product = Multiplication::Builder(
        const_cast<Matrix *>(this)->childAtIndex(j).clone(),
        const_cast<Matrix *>(b)->childAtIndex(j).clone());
    sum.addChildAtIndexInPlace(product, sum.numberOfChildren(),
                               sum.numberOfChildren());
    product.shallowReduce(reductionContext);
  }
  return std::move(sum);
}

Matrix Matrix::cross(Matrix *b,
                     const ReductionContext &reductionContext) const {
  // Cross product is defined between two vectors of size 3 and of same type.
  assert(vectorType() != Array::VectorType::None &&
         vectorType() == b->vectorType() && numberOfChildren() == 3 &&
         b->numberOfChildren() == 3);
  Matrix matrix = Matrix::Builder();
  for (int j = 0; j < 3; j++) {
    int j1 = (j + 1) % 3;
    int j2 = (j + 2) % 3;
    Expression a1b2 = Multiplication::Builder(
        const_cast<Matrix *>(this)->childAtIndex(j1).clone(),
        const_cast<Matrix *>(b)->childAtIndex(j2).clone());
    Expression a2b1 = Multiplication::Builder(
        const_cast<Matrix *>(this)->childAtIndex(j2).clone(),
        const_cast<Matrix *>(b)->childAtIndex(j1).clone());
    Expression difference = Subtraction::Builder(a1b2, a2b1);
    a1b2.shallowReduce(reductionContext);
    a2b1.shallowReduce(reductionContext);
    matrix.addChildAtIndexInPlace(difference, matrix.numberOfChildren(),
                                  matrix.numberOfChildren());
    difference.shallowReduce(reductionContext);
  }
  matrix.setDimensions(numberOfRows(), numberOfColumns());
  return matrix;
}

Expression Matrix::shallowReduce(ReductionContext reductionContext) {
  if (node()->hasMatrixOrListChild(reductionContext.context())) {
    return replaceWithUndefinedInPlace();
  }
  Expression e =
      SimplificationHelper::bubbleUpDependencies(*this, reductionContext);
  if (!e.isUninitialized()) {
    return e;
  }
  return *this;
}

Expression Matrix::computeInverseOrDeterminant(
    bool computeDeterminant, const ReductionContext &reductionContext,
    bool *couldCompute) const {
  assert(numberOfRows() == numberOfColumns());
  int dim = numberOfRows();
  /* If the matrix is too big, the rowCanonization might not be computed exactly
   * because of a pool allocation error, but we might still be able to compute
   * it approximately. We thus encapsulate the inverse/determinant creation in
   * an exception checkpoint.
   * We can safely use an exception checkpoint here because we are sure of not
   * modifying any pre-existing node in the pool. We are sure there is no Store
   * in the matrix. */
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    /* We clone the current matrix to extract its children later. We can't clone
     * its children directly. Indeed, the current matrix node (this->node()) is
     * located before the exception checkpoint. In order to clone its chidlren,
     * we would temporary increase the reference counter of each child (also
     * located before the checkpoint). If an exception is raised before
     * destroying the child handle, its reference counter would be off by one
     * after the long jump. */
    Matrix cl = clone().convert<Matrix>();
    *couldCompute = true;
    /* Create the matrix (A|I) with A is the input matrix and I the dim
     * identity matrix */
    Matrix matrixAI = Matrix::Builder();
    for (int i = 0; i < dim; i++) {
      for (int j = 0; j < dim; j++) {
        Expression mChildIJ = cl.matrixChild(i, j);
        matrixAI.addChildAtIndexInPlace(mChildIJ, i * 2 * dim + j,
                                        i * 2 * dim + j);
      }
      for (int j = dim; j < 2 * dim; j++) {
        matrixAI.addChildAtIndexInPlace(
            j - dim == i ? Rational::Builder(1) : Rational::Builder(0),
            i * 2 * dim + j, i * 2 * dim + j);
      }
    }
    matrixAI.setDimensions(dim, 2 * dim);
    Expression det;
    bool canonizationSuccess = true;
    matrixAI =
        matrixAI.rowCanonize(reductionContext, &canonizationSuccess, &det);
    if (!canonizationSuccess) {
      *couldCompute = false;
      return Undefined::Builder();
    }
    if (computeDeterminant) {
      return det;
    }
    // Check inversibility
    for (int i = 0; i < dim; i++) {
      if (!matrixAI.matrixChild(i, i).isRationalOne()) {
        return Undefined::Builder();
      }
    }
    Matrix inverse = Matrix::Builder();
    for (int i = 0; i < dim; i++) {
      for (int j = 0; j < dim; j++) {
        // We can steal matrixAI's children
        inverse.addChildAtIndexInPlace(matrixAI.matrixChild(i, j + dim),
                                       i * dim + j, i * dim + j);
      }
    }
    inverse.setDimensions(dim, dim);
    return std::move(inverse);
  } else {
    *couldCompute = false;
    return Expression();
  }
}

template int Matrix::ArrayInverse<double>(double *, int, int);
template int Matrix::ArrayInverse<std::complex<float>>(std::complex<float> *,
                                                       int, int);
template int Matrix::ArrayInverse<std::complex<double>>(std::complex<double> *,
                                                        int, int);
template void Matrix::ArrayRowCanonize<std::complex<float>>(
    std::complex<float> *, int, int, std::complex<float> *, bool);
template void Matrix::ArrayRowCanonize<std::complex<double>>(
    std::complex<double> *, int, int, std::complex<double> *, bool);

}  // namespace Poincare

#include "ir.h"

Program::Program(ExprVector&& exprs, std::vector<std::uint32_t>&& outputIndices)
  : m_exprs(std::move(exprs)), m_outputExprs(std::move(outputIndices))
{

}

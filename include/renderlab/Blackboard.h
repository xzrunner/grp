#pragma once

#include <cu/cu_macro.h>

#include <memory>

namespace renderlab
{

class Evaluator;

class Blackboard
{
public:
    void SetEval(const std::shared_ptr<Evaluator>& eval) {
        m_eval = eval;
    }
    auto GetEval() const { return m_eval; }

private:
    std::shared_ptr<Evaluator> m_eval = nullptr;

    CU_SINGLETON_DECLARATION(Blackboard);

}; // Blackboard

}
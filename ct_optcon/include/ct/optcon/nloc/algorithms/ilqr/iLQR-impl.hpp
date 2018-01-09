/**********************************************************************************************************************
This file is part of the Control Toolbox (https://adrlab.bitbucket.io/ct), copyright by ETH Zurich, Google Inc.
Authors:  Michael Neunert, Markus Giftthaler, Markus Stäuble, Diego Pardo, Farbod Farshidian
Licensed under Apache2 license (see LICENSE file in main directory)
 **********************************************************************************************************************/

#pragma once

#include <ct/optcon/solver/NLOptConSettings.hpp>
#include <ct/optcon/nloc/NLOCAlgorithm.hpp>

namespace ct {
namespace optcon {

template <size_t STATE_DIM, size_t CONTROL_DIM, size_t P_DIM, size_t V_DIM, typename SCALAR, typename OPTCONPROBLEM>
iLQR<STATE_DIM, CONTROL_DIM, P_DIM, V_DIM, SCALAR, OPTCONPROBLEM>::iLQR(std::shared_ptr<Backend_t>& backend_,
    const Settings_t& settings)
    : Base(backend_)
{
}

template <size_t STATE_DIM, size_t CONTROL_DIM, size_t P_DIM, size_t V_DIM, typename SCALAR, typename OPTCONPROBLEM>
iLQR<STATE_DIM, CONTROL_DIM, P_DIM, V_DIM, SCALAR, OPTCONPROBLEM>::~iLQR()
{
}

template <size_t STATE_DIM, size_t CONTROL_DIM, size_t P_DIM, size_t V_DIM, typename SCALAR, typename OPTCONPROBLEM>
void iLQR<STATE_DIM, CONTROL_DIM, P_DIM, V_DIM, SCALAR, OPTCONPROBLEM>::configure(const Settings_t& settings)
{
    this->backend_->configure(settings);
}

template <size_t STATE_DIM, size_t CONTROL_DIM, size_t P_DIM, size_t V_DIM, typename SCALAR, typename OPTCONPROBLEM>
void iLQR<STATE_DIM, CONTROL_DIM, P_DIM, V_DIM, SCALAR, OPTCONPROBLEM>::setInitialGuess(const Policy_t& initialGuess)
{
    this->backend_->setInitialGuess(initialGuess);
}

template <size_t STATE_DIM, size_t CONTROL_DIM, size_t P_DIM, size_t V_DIM, typename SCALAR, typename OPTCONPROBLEM>
bool iLQR<STATE_DIM, CONTROL_DIM, P_DIM, V_DIM, SCALAR, OPTCONPROBLEM>::runIteration()
{
    prepareIteration();

    return finishIteration();
}

template <size_t STATE_DIM, size_t CONTROL_DIM, size_t P_DIM, size_t V_DIM, typename SCALAR, typename OPTCONPROBLEM>
void iLQR<STATE_DIM, CONTROL_DIM, P_DIM, V_DIM, SCALAR, OPTCONPROBLEM>::prepareIteration()
{
    if (!this->backend_->isInitialized())
        throw std::runtime_error("iLQR is not initialized!");

    if (!this->backend_->isConfigured())
        throw std::runtime_error("iLQR is not configured!");

    this->backend_->checkProblem();
}

template <size_t STATE_DIM, size_t CONTROL_DIM, size_t P_DIM, size_t V_DIM, typename SCALAR, typename OPTCONPROBLEM>
bool iLQR<STATE_DIM, CONTROL_DIM, P_DIM, V_DIM, SCALAR, OPTCONPROBLEM>::finishIteration()
{
    int K = this->backend_->getNumSteps();

    bool debugPrint = this->backend_->getSettings().debugPrint;

    // if first iteration, compute shots and rollout and cost!
    if (this->backend_->iteration() == 0)
    {
        if (!this->backend_->nominalRollout())
            throw std::runtime_error("Rollout failed. System became unstable");

        this->backend_->updateCosts();
    }

#ifdef MATLAB_FULL_LOG
    if (this->backend_->iteration() == 0)
        this->backend_->logInitToMatlab();
#endif

    auto start = std::chrono::steady_clock::now();
    auto startEntire = start;

    // set box constraints and do LQ approximation
    this->backend_->setBoxConstraintsForLQOCProblem();
    this->backend_->computeLQApproximation(0, K - 1);
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    if (debugPrint)
        std::cout << "[iLQR]: Computing LQ approximation took "
                  << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;

    end = std::chrono::steady_clock::now();
    diff = end - startEntire;
    if (debugPrint)
        std::cout << "[iLQR]: Forward pass took " << std::chrono::duration<double, std::milli>(diff).count() << " ms"
                  << std::endl;

    if (debugPrint)
        std::cout << "[iLQR]: #2 Solve LQOC Problem" << std::endl;

    start = std::chrono::steady_clock::now();
    this->backend_->solveFullLQProblem();
    end = std::chrono::steady_clock::now();
    diff = end - start;
    if (debugPrint)
        std::cout << "[iLQR]: Solving LQOC problem took " << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

    // update solutions
    this->backend_->getFeedback();
    this->backend_->getControlUpdates();
    this->backend_->getStateUpdates();

    // line-search
    if (debugPrint)
        std::cout << "[iLQR]: #3 LineSearch" << std::endl;

    start = std::chrono::steady_clock::now();
    bool foundBetter = this->backend_->lineSearchSingleShooting();
    end = std::chrono::steady_clock::now();
    diff = end - start;
    if (debugPrint)
        std::cout << "[iLQR]: Line search took " << std::chrono::duration<double, std::milli>(diff).count() << " ms"
                  << std::endl;

    diff = end - startEntire;
    if (debugPrint)
        std::cout << "[iLQR]: finishIteration took " << std::chrono::duration<double, std::milli>(diff).count() << " ms"
                  << std::endl;

    this->backend_->printSummary();

#ifdef MATLAB_FULL_LOG
    this->backend_->logToMatlab(this->backend_->iteration());
#endif

    this->backend_->iteration()++;

    return foundBetter;
}

template <size_t STATE_DIM, size_t CONTROL_DIM, size_t P_DIM, size_t V_DIM, typename SCALAR, typename OPTCONPROBLEM>
void iLQR<STATE_DIM, CONTROL_DIM, P_DIM, V_DIM, SCALAR, OPTCONPROBLEM>::prepareMPCIteration()
{
    prepareIteration();
}

template <size_t STATE_DIM, size_t CONTROL_DIM, size_t P_DIM, size_t V_DIM, typename SCALAR, typename OPTCONPROBLEM>
bool iLQR<STATE_DIM, CONTROL_DIM, P_DIM, V_DIM, SCALAR, OPTCONPROBLEM>::finishMPCIteration()
{
    finishIteration();
    return true;  //! \todo : in MPC always returning true. Unclear how user wants to deal with varying costs, etc.
}

}  // namespace optcon
}  // namespace ct

/*
 * steppers.h
 *
 *  Created on: May 25, 2017
 *      Author: neunertm
 */

#ifndef INCLUDE_CT_CORE_INTEGRATION_INTERNAL_STEPPERS_H_
#define INCLUDE_CT_CORE_INTEGRATION_INTERNAL_STEPPERS_H_

namespace ct {
namespace core {
namespace internal {
/*****************************************************************************
 * Defining the (explicit) steppers
 *****************************************************************************/
//! Simple Euler stepper
template <size_t STATE_DIM>
using euler_t = boost::numeric::odeint::euler<
		Eigen::Matrix<double, STATE_DIM, 1>,
		double,
		Eigen::Matrix<double, STATE_DIM, 1>,
		double,
		boost::numeric::odeint::vector_space_algebra >;

//! Modified Midpoint stepper
template <size_t STATE_DIM>
using modified_midpoint_t = boost::numeric::odeint::modified_midpoint<
		Eigen::Matrix<double, STATE_DIM, 1>,
		double,
		Eigen::Matrix<double, STATE_DIM, 1>,
		double,
		boost::numeric::odeint::vector_space_algebra >;

//! Runge-Kutta4 stepper
template <size_t STATE_DIM>
using runge_kutta_4_t = boost::numeric::odeint::runge_kutta4<
		Eigen::Matrix<double, STATE_DIM, 1>,
		double,
		Eigen::Matrix<double, STATE_DIM, 1>,
		double,
		boost::numeric::odeint::vector_space_algebra >;

//! Runge-Kutta Dormand Price 5 stepper
template <size_t STATE_DIM>
using runge_kutta_dopri5_t = boost::numeric::odeint::runge_kutta_dopri5 <
		Eigen::Matrix<double, STATE_DIM, 1>,
		double,
		Eigen::Matrix<double, STATE_DIM, 1>,
		double,
		boost::numeric::odeint::vector_space_algebra>;

//! Dense Output Runge Kutta 4 stepper
template <size_t STATE_DIM>
using dense_runge_kutta5_t = boost::numeric::odeint::dense_output_runge_kutta <
		boost::numeric::odeint::controlled_runge_kutta <runge_kutta_dopri5_t<STATE_DIM>> >;

//! Runge Kutta Fehlberg 78 stepper
template <size_t STATE_DIM>
using runge_kutta_fehlberg78_t = boost::numeric::odeint::runge_kutta_fehlberg78<
		Eigen::Matrix<double, STATE_DIM, 1>,
		double,
		Eigen::Matrix<double, STATE_DIM, 1>,
		Time,
		boost::numeric::odeint::vector_space_algebra >;

//! Bulirsch Stoer stepper
template <size_t STATE_DIM>
using bulirsch_stoer_t = boost::numeric::odeint::bulirsch_stoer <
		Eigen::Matrix<double, STATE_DIM, 1>,
		double,
		Eigen::Matrix<double, STATE_DIM, 1>,
		double,
		boost::numeric::odeint::vector_space_algebra>;

//! Adams Bashforth stepper
template <size_t STATE_DIM, size_t STEPS>
using adams_bashforth_uncontrolled_t =
		boost::numeric::odeint::adams_bashforth<
		STEPS,
		Eigen::Matrix<double, STATE_DIM, 1>,	// state
		double,									// typename value
		Eigen::Matrix<double, STATE_DIM, 1>,	// derivative
		double, 								// typename time
		boost::numeric::odeint::vector_space_algebra> ;


/*****************************************************************************
 * Defining the (implicit) steppers
 *****************************************************************************/
// // works only for boost 1.56 or higher
//template <size_t STATE_DIM, size_t STEPS>
//using adams_bashforth_moulton_uncontrolled_t =
//		boost::numeric::odeint::adams_bashforth_moulton<
//		STEPS,
//		Eigen::Matrix<double, STATE_DIM, 1>,	// state
//		double,									// typename value
//		Eigen::Matrix<double, STATE_DIM, 1>,	// derivative
//		double, 								// typename time
//		boost::numeric::odeint::vector_space_algebra> ;

} // namespace internal
} // namespace core
} // namespace ct


#endif /* INCLUDE_CT_CORE_INTEGRATION_INTERNAL_STEPPERS_H_ */

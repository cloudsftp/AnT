/* 
 * Copyright (C) 1999-2007 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * This file is part of AnT,
 * a simulation and analysis tool for dynamical systems.
 * 
 * AnT is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AnT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *
 * $Id: SemanticCheck.cpp,v 1.102 2010/11/11 14:53:16 schanzml Exp $
 *
 */

#define VERIFY__SEMANTIC_CHECK_CPP 1
#define DEBUG__SEMANTIC_CHECK_CPP 0

#ifndef OPTION__DYNAMIC_PARAMETERS
#define OPTION__DYNAMIC_PARAMETERS 1
#else
#error OPTION__DYNAMIC_PARAMETERS already defined!
#endif

#ifndef OPTION__DISABLE_SOME_DIMENSION_FILES
#define OPTION__DISABLE_SOME_DIMENSION_FILES 0
#endif

#ifndef OPTION__DISABLE_SOME_SYMBOLIC_FILES
#define OPTION__DISABLE_SOME_SYMBOLIC_FILES 1
#endif

#include "../utils/CompilerDetector.hpp" /* TEMPLATE__ */

#include "SemanticCheck.hpp"
#include "MainWindow.hpp"
#include "Callbacks.hpp"
#include "../engine/AnT-init.hpp"
#include "CustomConfigurators.hpp"
#include "methods/output/IOStreamFactory.hpp" /* printTimeOfDay (ostream* f) */

const string AnTGuiKey ("AnT root window");

/* ********************************************************************** */

INITIALIZE_KEY (dynamical_system::type);
INITIALIZE_KEY (dynamical_system::state_space_dimension);
INITIALIZE_KEY (dynamical_system::discrete_state_space_dimension);
INITIALIZE_KEY (dynamical_system::delay);
INITIALIZE_KEY (dynamical_system::maximal_delay);
INITIALIZE_KEY (dynamical_system::recurrence_level);
INITIALIZE_KEY (dynamical_system::domain_boundary);
INITIALIZE_KEY (dynamical_system::number_of_grid_points);

const string
dynamical_system::neumann_boundary_conditions_
("dynamical_system::neumann_boundary_conditions");

const string
dynamical_system::neumann_boundary_conditions::operator__
( "dynamical_system::neumann_boundary_conditions::operator[]" );

const string
dynamical_system::neumann_boundary_conditions
::operator_::name
( "dynamical_system::neumann_boundary_conditions::operator[]::name" );

const string
dynamical_system::neumann_boundary_conditions
::operator_::policy
( "dynamical_system::neumann_boundary_conditions::operator[]::policy" );

const string
dynamical_system::neumann_boundary_conditions
::operator_::lower_boundary_value
( "dynamical_system::neumann_boundary_conditions::operator[]::lower_boundary_value" );

const string
dynamical_system::neumann_boundary_conditions::operator_::upper_boundary_value
( "dynamical_system::neumann_boundary_conditions::operator[]::upper_boundary_value" );

INITIALIZE_KEY (dynamical_system::dirichlet_boundary_conditions);
INITIALIZE_KEY (dynamical_system::external_data_filename);
INITIALIZE_KEY (dynamical_system::number_of_columns);
INITIALIZE_KEY (dynamical_system::action_on_wrong_lines);
INITIALIZE_KEY (dynamical_system::accept_empty_lines);
INITIALIZE_KEY (dynamical_system::data_input);
INITIALIZE_KEY (dynamical_system::number_of_cells);
INITIALIZE_KEY (dynamical_system::parameter_space_dimension);

const string
dynamical_system::parameters_
( "dynamical_system::parameters" );

const string
dynamical_system::parameters::parameter_
( "dynamical_system::parameters::parameter[]" );
INITIALIZE_KEY (dynamical_system::parameters::array);
INITIALIZE_KEY (dynamical_system::initial_state);
INITIALIZE_KEY (dynamical_system::continuous_initial_state);
INITIALIZE_KEY (dynamical_system::discrete_initial_state);
INITIALIZE_KEY (dynamical_system::reset_initial_states_from_orbit);

/* ********************************************************************** */

const string
dynamical_system::temporal_initial_function_
( "dynamical_system::temporal_initial_function[]" );


const string
dynamical_system::temporal_initial_function::type
( "dynamical_system::temporal_initial_function[]::type" );

const string
dynamical_system::temporal_initial_function::amplitude
( "dynamical_system::temporal_initial_function[]::amplitude" );

const string
dynamical_system::temporal_initial_function::offset
( "dynamical_system::temporal_initial_function[]::offset" );

const string
dynamical_system::temporal_initial_function::period
( "dynamical_system::temporal_initial_function[]::period" );

const string
dynamical_system::temporal_initial_function::sigma
( "dynamical_system::temporal_initial_function[]::sigma" );

const string
dynamical_system::temporal_initial_function::width
( "dynamical_system::temporal_initial_function[]::width" );

const string
dynamical_system::temporal_initial_function::degree
( "dynamical_system::temporal_initial_function[]::degree" );

const string
dynamical_system::temporal_initial_function::coefficients
( "dynamical_system::temporal_initial_function[]::coefficients" );

const string
dynamical_system::temporal_initial_function::relative_peak_position
( "dynamical_system::temporal_initial_function[]::relative_peak_position" );

const string
dynamical_system::temporal_initial_function::argument_shift
( "dynamical_system::temporal_initial_function[]::argument_shift" );

const string
dynamical_system::temporal_initial_function::random_specification
( "dynamical_system::temporal_initial_function[]::random_specification" );

/* ********************************************************************** */

const string
dynamical_system::spatial_initial_function_
( "dynamical_system::spatial_initial_function[]" );

const string
dynamical_system::integration_
( "dynamical_system::integration" );

INITIALIZE_KEY (dynamical_system::inside);

const string
dynamical_system::poincare_section_
( "dynamical_system::poincare_section" );

INITIALIZE_KEY (dynamical_system::number_of_subsystems);
INITIALIZE_KEY (dynamical_system::number_of_noisy_variables);

const string
dynamical_system::noisy_variable_
( "dynamical_system::noisy_variable[]" );

const string
dynamical_system::noisy_variable::min_
( "dynamical_system::noisy_variable[]::min" );

const string
dynamical_system::noisy_variable::max_
( "dynamical_system::noisy_variable[]::max" );

const string
dynamical_system::noisy_variable::mean_value
( "dynamical_system::noisy_variable[]::mean_value" );

const string
dynamical_system::noisy_variable::standard_deviation
( "dynamical_system::noisy_variable[]::standard_deviation" );

/* ********************************************************************** */
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::policy);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::points_step);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::state_space_distance);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::velocity_space_distance);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::trajectory);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::cobweb);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::cobweb_file);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::velocity);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::phase_portrait);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::initial_states);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::save_only_specific_area);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::state_space_saving_area);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::saving::velocity_space_saving_area);

const string
investigation_methods::general_trajectory_evaluations::pgm_output::picture_
( "investigation_methods::general_trajectory_evaluations::pgm_output::picture[]" );

INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::statistics::mean_value);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::statistics::standard_deviation);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::statistics::average_velocity);
INITIALIZE_KEY (investigation_methods::general_trajectory_evaluations::statistics::correlation_coefficient);

INITIALIZE_KEY (investigation_methods::period_analysis::period);
INITIALIZE_KEY (investigation_methods::period_analysis::cyclic_asymptotic_set);
INITIALIZE_KEY (investigation_methods::period_analysis::acyclic_last_states);
INITIALIZE_KEY (investigation_methods::period_analysis::cyclic_graphical_iteration);
INITIALIZE_KEY (investigation_methods::period_analysis::acyclic_graphical_iteration);
INITIALIZE_KEY (investigation_methods::period_analysis::period_selections);
INITIALIZE_KEY (investigation_methods::period_analysis::periods_to_select);


#if OPTION__SYMBOLIC_IMAGE_ANALYSIS
const string
investigation_methods::symbolic_image_analysis::covering::uniform_grid_
( "investigation_methods::symbolic_image_analysis::covering::uniform_grid[]" );

const string
investigation_methods::symbolic_image_analysis::covering::interval_select_
( "investigation_methods::symbolic_image_analysis::covering::interval_select[]" );

INITIALIZE_KEY (investigation_methods::symbolic_image_analysis::covering::point_select::positions);

const string 
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::box_output
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::box_output" );

const string 
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::range_output
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::range_output" );


const string 
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::invariant_set
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::invariant_set" );

const string 
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::periodic_paths
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::periodic_paths" );

const string 
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::period_size
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::period_size" );

const string 
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::cell_edges
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::cell_edges" );

const string
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::only_periods
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::only_periods" );

const string
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::only_subdivisions
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::only_subdivisions" );

const string
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::number_of_cells
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::number_of_cells" );

const string
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::only_more_cells_than
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::only_more_cells_than" );


const string
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::all_larger_sets
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::all_larger_sets" );

const string
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::set_edges
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::set_edges" );

const string
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::max_values
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::max_values" );

const string
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::min_values
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::min_values" );

const string
investigation_methods::symbolic_image_analysis::image_cells::output_writer_::interpolated_values
( "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::interpolated_values" );

#endif /* OPTION__SYMBOLIC_IMAGE_ANALYSIS */

/* ********************************************************************** */


INITIALIZE_KEY (investigation_methods::band_counter::basis_points);
INITIALIZE_KEY (investigation_methods::band_counter::bandcount_compare_precision);   
INITIALIZE_KEY (investigation_methods::band_counter::fast_termination);
INITIALIZE_KEY (investigation_methods::band_counter::termination_after); 
INITIALIZE_KEY (investigation_methods::band_counter::automatic_domain_detection);
INITIALIZE_KEY (investigation_methods::band_counter::domain_boundaries);
INITIALIZE_KEY (investigation_methods::band_counter::number_of_boxes);
INITIALIZE_KEY (investigation_methods::band_counter::error_threshold);
INITIALIZE_KEY (investigation_methods::band_counter::use_hash);
INITIALIZE_KEY (investigation_methods::band_counter::hash_size);
INITIALIZE_KEY (investigation_methods::band_counter::period);
INITIALIZE_KEY (investigation_methods::band_counter::period_file);

/* ********************************************************************** */

const string
investigation_methods::symbolic_analysis
::critical_points_for_LR_symbolic_dynamics
( "investigation_methods::symbolic_analysis::critical_points_for_<L/R>_symbolic_dynamics" );

INITIALIZE_KEY (investigation_methods::symbolic_analysis::symbolic_entropy_approximate);
INITIALIZE_KEY (investigation_methods::symbolic_analysis::symbolic_probability_approximate);
INITIALIZE_KEY (investigation_methods::symbolic_analysis::symbolic_entropy_exact);
INITIALIZE_KEY (investigation_methods::symbolic_analysis::symbolic_probability_exact);
INITIALIZE_KEY (investigation_methods::symbolic_analysis::symbolic_sequence);
INITIALIZE_KEY (investigation_methods::symbolic_analysis::periodic_symbolic_sequence);

/* ********************************************************************** */

INITIALIZE_KEY (investigation_methods::lyapunov_exponents_analysis::use_linearized_system);
INITIALIZE_KEY (investigation_methods::lyapunov_exponents_analysis::p);
INITIALIZE_KEY (investigation_methods::lyapunov_exponents_analysis::exponents);
INITIALIZE_KEY (investigation_methods::lyapunov_exponents_analysis::dimension);
INITIALIZE_KEY (investigation_methods::lyapunov_exponents_analysis::status);

/* ********************************************************************** */

const string investigation_methods::regions_analysis_
= "investigation_methods::regions_analysis";

/* ********************************************************************** */

INITIALIZE_KEY (investigation_methods::dimensions_analysis::automatic_range_detection);
INITIALIZE_KEY (investigation_methods::dimensions_analysis::ranges);
INITIALIZE_KEY (investigation_methods::dimensions_analysis::min_layer);
INITIALIZE_KEY (investigation_methods::dimensions_analysis::invariant_measure);
INITIALIZE_KEY (investigation_methods::dimensions_analysis::measure_deviation);
INITIALIZE_KEY (investigation_methods::dimensions_analysis::metric_entropy);
INITIALIZE_KEY (investigation_methods::dimensions_analysis::capacity_dimension);
INITIALIZE_KEY (investigation_methods::dimensions_analysis::information_dimension);
INITIALIZE_KEY (investigation_methods::dimensions_analysis::correlation_dimension);
INITIALIZE_KEY (investigation_methods::dimensions_analysis::q_dimension);
INITIALIZE_KEY (investigation_methods::dimensions_analysis::save_outside_points);

/* ********************************************************************** */

#if OPTION__FREQUENCY_ANALYSIS
INITIALIZE_KEY (investigation_methods::frequency_analysis::using_variables);
INITIALIZE_KEY (investigation_methods::frequency_analysis::frequency_output_range);
INITIALIZE_KEY (investigation_methods::frequency_analysis::neighborhood_width);
INITIALIZE_KEY (investigation_methods::frequency_analysis::frequency_weighting);
INITIALIZE_KEY (investigation_methods::frequency_analysis::frequency_step_is_given);
INITIALIZE_KEY (investigation_methods::frequency_analysis::real_part);
INITIALIZE_KEY (investigation_methods::frequency_analysis::imaginary_part);
INITIALIZE_KEY (investigation_methods::frequency_analysis::real_and_imaginary_parts);
INITIALIZE_KEY (investigation_methods::frequency_analysis::power_spectrum);
INITIALIZE_KEY (investigation_methods::frequency_analysis::autocorrelation);
INITIALIZE_KEY (investigation_methods::frequency_analysis::total_power);
INITIALIZE_KEY (investigation_methods::frequency_analysis::period);
INITIALIZE_KEY (investigation_methods::frequency_analysis::spectrum_oscillation);
INITIALIZE_KEY (investigation_methods::frequency_analysis::spectrum_max_points);
INITIALIZE_KEY (investigation_methods::frequency_analysis::spectrum_waving);
INITIALIZE_KEY (investigation_methods::frequency_analysis::fourier_coefficients);
#endif /* OPTION__FREQUENCY_ANALYSIS */

/* ********************************************************************** */

#if OPTION__SINGULAR_VALUE_ANALYSIS
INITIALIZE_KEY (investigation_methods::singular_value_analysis::eigenvalues);
INITIALIZE_KEY (investigation_methods::singular_value_analysis::eigenvectors);
INITIALIZE_KEY (investigation_methods::singular_value_analysis::covariance_matrix);
#endif /* OPTION__SINGULAR_VALUE_ANALYSIS */

/* ********************************************************************** */

const string
investigation_methods::check_for_conditions::condition_
( "investigation_methods::check_for_conditions::condition[]" );

const string
investigation_methods::check_for_conditions::condition::time
( "investigation_methods::check_for_conditions::condition[]::time" );

const string
investigation_methods::check_for_conditions::condition::time_until_fixed_point_file
( "investigation_methods::check_for_conditions::condition[]::time_until_fixed_point_file" );

const string
investigation_methods::check_for_conditions::condition::time_until_given_point_file
( "investigation_methods::check_for_conditions::condition[]::time_until_given_point_file" );

const string
investigation_methods::check_for_conditions::condition::time_until_rectangle_is_leaved_file
( "investigation_methods::check_for_conditions::condition[]::time_until_rectangle_is_leaved_file" );

const string
investigation_methods::check_for_conditions::condition::time_until_circle_is_leaved_file
( "investigation_methods::check_for_conditions::condition[]::time_until_circle_is_leaved_file" );

const string
investigation_methods::check_for_conditions::condition::time_until_user_defined_condition_file
( "investigation_methods::check_for_conditions::condition[]::time_until_user_defined_condition_file" );


const string
investigation_methods::check_for_conditions::condition::way
( "investigation_methods::check_for_conditions::condition[]::way" );

const string
investigation_methods::check_for_conditions::condition::way_until_fixed_point_file
( "investigation_methods::check_for_conditions::condition[]::way_until_fixed_point_file" );

const string
investigation_methods::check_for_conditions::condition::way_until_given_point_file
( "investigation_methods::check_for_conditions::condition[]::way_until_given_point_file" );

const string
investigation_methods::check_for_conditions::condition::way_until_rectangle_is_leaved_file
( "investigation_methods::check_for_conditions::condition[]::way_until_rectangle_is_leaved_file" );

const string
investigation_methods::check_for_conditions::condition::way_until_circle_is_leaved_file
( "investigation_methods::check_for_conditions::condition[]::way_until_circle_is_leaved_file" );

const string
investigation_methods::check_for_conditions::condition::way_within_rectangle_file
( "investigation_methods::check_for_conditions::condition[]::way_within_rectangle_file" );

const string
investigation_methods::check_for_conditions::condition::way_within_circle_file
( "investigation_methods::check_for_conditions::condition[]::way_within_circle_file" );

const string
investigation_methods::check_for_conditions::condition::way_until_user_defined_condition_file
( "investigation_methods::check_for_conditions::condition[]::way_until_user_defined_condition_file" );


const string
investigation_methods::check_for_conditions::condition::setting
( "investigation_methods::check_for_conditions::condition[]::setting" );

const string
investigation_methods::check_for_conditions::condition::settings_leading_to_fixed_point_file
( "investigation_methods::check_for_conditions::condition[]::settings_leading_to_fixed_point_file" );

const string
investigation_methods::check_for_conditions::condition::settings_leading_to_given_point_file
( "investigation_methods::check_for_conditions::condition[]::settings_leading_to_given_point_file" );

const string
investigation_methods::check_for_conditions::condition::settings_leading_from_rectangle_file
( "investigation_methods::check_for_conditions::condition[]::settings_leading_from_rectangle_file" );

const string
investigation_methods::check_for_conditions::condition::settings_leading_from_circle_file
( "investigation_methods::check_for_conditions::condition[]::settings_leading_from_circle_file" );

const string
investigation_methods::check_for_conditions::condition::settings_staying_in_rectangle_file
( "investigation_methods::check_for_conditions::condition[]::settings_staying_in_rectangle_file" );

const string
investigation_methods::check_for_conditions::condition::settings_staying_in_circle_file
( "investigation_methods::check_for_conditions::condition[]::settings_staying_in_circle_file" );

const string
investigation_methods::check_for_conditions::condition::settings_for_user_defined_condition_file
( "investigation_methods::check_for_conditions::condition[]::settings_for_user_defined_condition_file" );

const string
investigation_methods::check_for_conditions::condition::ranges
( "investigation_methods::check_for_conditions::condition[]::ranges" );

const string
investigation_methods::check_for_conditions::condition::center
( "investigation_methods::check_for_conditions::condition[]::center" );

const string
investigation_methods::check_for_conditions::condition::radius
( "investigation_methods::check_for_conditions::condition[]::radius" );

const string
investigation_methods::check_for_conditions::condition::points
( "investigation_methods::check_for_conditions::condition[]::points" );

const string
investigation_methods::check_for_conditions::condition::name
( "investigation_methods::check_for_conditions::condition[]::name" );

/* ********************************************************************** */

INITIALIZE_KEY (investigation_methods::spatial_evaluations::mean_value);
INITIALIZE_KEY (investigation_methods::spatial_evaluations::standard_deviation);
INITIALIZE_KEY (investigation_methods::spatial_evaluations::average_correlation);
INITIALIZE_KEY (investigation_methods::spatial_evaluations::wave_numbers);

const string
investigation_methods::spatial_evaluations::specific_counter::ranges
( "investigation_methods::spatial_evaluations::specific_counter[]::ranges" );

/* ********************************************************************** */

INITIALIZE_KEY (dynamical_system::integration::step_size);
INITIALIZE_KEY (dynamical_system::integration::array_name);
INITIALIZE_KEY (dynamical_system::integration::butcher_order_is_known);
INITIALIZE_KEY (dynamical_system::integration::butcher_order);
INITIALIZE_KEY (dynamical_system::integration::nominators_array);
INITIALIZE_KEY (dynamical_system::integration::denominators_array);
INITIALIZE_KEY (dynamical_system::integration::backward_threshold);
INITIALIZE_KEY (dynamical_system::integration::bdf_order);
INITIALIZE_KEY (dynamical_system::integration::adams_moulton_order);
INITIALIZE_KEY (dynamical_system::integration::adams_bashforth_order);
INITIALIZE_KEY (dynamical_system::integration::manhattan_threshold);
INITIALIZE_KEY (dynamical_system::integration::integration_accuracy);
INITIALIZE_KEY (dynamical_system::integration::variables_weighting);

/* ********************************************************************** */

INITIALIZE_KEY (dynamical_system::poincare_section::type);
INITIALIZE_KEY (dynamical_system::poincare_section::variable_index);
INITIALIZE_KEY (dynamical_system::poincare_section::plane_coefficients);
INITIALIZE_KEY (dynamical_system::poincare_section::additional_parameters);

/* ********************************************************************** */

INITIALIZE_KEY (scan::mode);

const string
scan::cas
( "scan::cas" );

const string
scan::cas_::iss
( "scan::cas::iss" );

const string
scan::cas_::iss_::mode
( "scan::cas::iss::mode" );

const string
scan::cas_::iss_::item__
( "scan::cas::iss::item[]" );

const string
scan::cas_::iss_::item_::points
( "scan::cas::iss::item[]::points" );

const string
scan::cas_::iss_::item_::min
( "scan::cas::iss::item[]::min" );

const string
scan::cas_::iss_::item_::max
( "scan::cas::iss::item[]::max" );

const string
scan::cas_::iss_::item_::first_min
( "scan::cas::iss::item[]::first_min" );

const string
scan::cas_::iss_::item_::first_max
( "scan::cas::iss::item[]::first_max" );

const string
scan::cas_::iss_::item_::second_min
( "scan::cas::iss::item[]::second_min" );

const string
scan::cas_::iss_::item_::second_max
( "scan::cas::iss::item[]::second_max" );

const string
scan::cas_::iss_::item_::min_angle
( "scan::cas::iss::item[]::min_angle" );

const string
scan::cas_::iss_::item_::max_angle
( "scan::cas::iss::item[]::max_angle" );

const string
scan::cas_::iss_::item_::first_axis
( "scan::cas::iss::item[]::first_axis" );

const string
scan::cas_::iss_::item_::second_axis
( "scan::cas::iss::item[]::second_axis" );

const string
scan::cas_::iss_::item_::first_center
( "scan::cas::iss::item[]::first_center" );

const string
scan::cas_::iss_::item_::second_center
( "scan::cas::iss::item[]::second_center" );

const string
scan::cas_::iss_::item_::object
( "scan::cas::iss::item[]::object" );

const string
scan::cas_::iss_::item_::first_object
( "scan::cas::iss::item[]::first_object" );

const string
scan::cas_::iss_::item_::second_object
( "scan::cas::iss::item[]::second_object" );

const string
scan::cas_::iss_::item_::external_data_filename
( "scan::cas::iss::item[]::external_data_filename" );

const string
scan::cas_::iss_::item_::number_of_columns
( "scan::cas::iss::item[]::number_of_columns" );

const string
scan::cas_::iss_::item_::subitem
( "scan::cas::iss::item[]::subitem[]" );

const string
scan::cas_::iss_::item_::subitem_::object
( "scan::cas::iss::item[]::subitem[]::object" );

const string
scan::cas_::iss_::item_::subitem_::column
( "scan::cas::iss::item[]::subitem[]::column" );

const string
scan::cas_::steps_between_iss
( "scan::cas::steps_between_iss" );

const string
scan::cas_::max_backtrack_depth
( "scan::cas::max_backtrack_depth" );

const string
scan::cas_::backtrack_depth
( "scan::cas::backtrack_depth" );

const string
scan::cas_::iss_iterations
( "scan::cas::iss_iterations" );

const string
scan::cas_::transient_steps
( "scan::cas::transient_steps" );

const string
scan::cas_::reset_from_iss_orbit
( "scan::cas::reset_from_iss_orbit" );

const string
scan::cas_::infinity_threshold
( "scan::cas::infinity_threshold" );

const string
scan::cas_::regular_compare_precision
( "scan::cas::regular_compare_precision" );

const string
scan::cas_::reduced_compare_precision
( "scan::cas::reduced_compare_precision" );

const string
scan::cas_::min_max
( "scan::cas::min_max" );

const string
scan::cas_::max_period
( "scan::cas::max_period" );

const string
scan::item_::points
( "scan::item[]::points" );

const string
scan::item_::min
( "scan::item[]::min" );

const string
scan::item_::max
( "scan::item[]::max" );

const string
scan::item_::first_min
( "scan::item[]::first_min" );

const string
scan::item_::first_max
( "scan::item[]::first_max" );

const string
scan::item_::second_min
( "scan::item[]::second_min" );

const string
scan::item_::second_max
( "scan::item[]::second_max" );

const string
scan::item_::min_angle
( "scan::item[]::min_angle" );

const string
scan::item_::max_angle
( "scan::item[]::max_angle" );

const string
scan::item_::first_axis
( "scan::item[]::first_axis" );

const string
scan::item_::second_axis
( "scan::item[]::second_axis" );

const string
scan::item_::first_center
( "scan::item[]::first_center" );

const string
scan::item_::second_center
( "scan::item[]::second_center" );

const string
scan::item_::object
( "scan::item[]::object" );

const string
scan::item_::first_object
( "scan::item[]::first_object" );

const string
scan::item_::second_object
( "scan::item[]::second_object" );

// VA:
const string
scan::item_::external_data_filename
( "scan::item[]::external_data_filename" );

const string
scan::item_::number_of_columns
( "scan::item[]::number_of_columns" );

const string
scan::item_::subitem
( "scan::item[]::subitem[]" );

const string
scan::item_::subitem_::object
( "scan::item[]::subitem[]::object" );

const string
scan::item_::subitem_::column
( "scan::item[]::subitem[]::column" );

/* ********************************************************************** */

const string
scan::item__
( "scan::item[]" );

/* ********************************************************************** */

#if OPTION__VISUALIZATION
const string
visualization::window_
( "visualization::window[]" );

const string
visualization::window::screen_anchor
( "visualization::window[]::screen_anchor" );

const string
visualization::window::object_
( "visualization::window[]::object[]" );

const string
visualization::window::object::inner_radius
( "visualization::window[]::object[]::inner_radius" );

const string
visualization::window::object::vector_
( "visualization::window[]::object[]::vector" );

const string
visualization::window::object::vector2
( "visualization::window[]::object[]::vector2" );

const string
visualization::window::object::facing
( "visualization::window[]::object[]::facing" );

const string
visualization::window::object::alpha
( "visualization::window[]::object[]::alpha" );

const string
visualization::window::object::lighting
( "visualization::window[]::object[]::lighting" );

const string
visualization::window::object::radius
( "visualization::window[]::object[]::radius" );

const string
visualization::window::object::number_of_objects
( "visualization::window[]::object[]::number_of_objects" );

const string
visualization::window::object::object_
( "visualization::window[]::object[]::object[]" );

const string
visualization::window::object::text
( "visualization::window[]::object[]::text" );

const string
visualization::window::object::align
( "visualization::window[]::object[]::align" );

const string
visualization::window::object::variable
( "visualization::window[]::object[]::variable" );

const string
visualization::window::object::variables
( "visualization::window[]::object[]::variables" );

const string
visualization::window::object::color
( "visualization::window[]::object[]::color" );

const string
visualization::window::object::scale
( "visualization::window[]::object[]::scale" );

const string
visualization::window::object::coloring_method
( "visualization::window[]::object[]::coloring_method" );

const string
visualization::window::object::plotting_method
( "visualization::window[]::object[]::plotting_method" );

const string
visualization::window::object::font
( "visualization::window[]::object[]::font" );

const string
visualization::window::object::position
( "visualization::window[]::object[]::position" );

const string
visualization::window::axes::stepping
( "visualization::window[]::axes::stepping" );

const string
visualization::window::axes::z_label
( "visualization::window[]::axes::z_label" );
#endif /* OPTION__VISUALIZATION */

/* ********************************************************************** */

/* helper functions with local scope: */

#if 0 /* commented out */
string
getAbsoluteDynSysType (AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  string dynSysType;

  const AbstractConfigurator* const rootConfigurator
    = aConfigurator->getRootConfigurator ();

  const Node* const leafNode
    = (rootConfigurator->getConfig ())
    ->followKeyPath (dynamical_system::type);

  leafNodeTo (leafNode, dynSysType);
#if DEBUG__SEMANTIC_CHECK_CPP
  cout << "dynSysType: " << dynSysType << endl;
#endif

#if VERIFY__SEMANTIC_CHECK_CPP
  /* check for valid enum entry: */
  if (! dynSysType.empty ()) {
    try {
      const string dynSysTypePath
	= string ("dynamical_system::")
	+ SPECIFICATION_KEYWORDS::RECORD
	+ "::type::"
	+ SPECIFICATION_KEYWORDS::ENUM;

      const Node* dynSysTypeEnum
	=  (rootConfigurator->specNode)
	->followKeyPath (dynSysTypePath);
      assert (dynSysTypeEnum != NULL);

      PreSemanticChecker::EnumEntry
	enumEntry (dynSysType, dynSysTypeEnum);
      assert (dynSysType == string (enumEntry));
    } catch (...) {
      assert (false);
      throw;
    }
  }
#endif

  return dynSysType;
}
#endif /* commented out */


enum dynamical_system__search_t {FROM_ROOT, NEAREST_PARENT};

template <const dynamical_system__search_t searchOpt>
const Node*
findDynSysNode (const AbstractConfigurator* aConfigurator);

/* template specialization */
template <>
const Node*
findDynSysNode<FROM_ROOT> (const AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  return
    ((aConfigurator->getRootConfigurator ())->getConfig ())
    ->findNode ("dynamical_system");
}

/* template specialization */
template <>
const Node*
findDynSysNode<NEAREST_PARENT> (const AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  const AbstractConfigurator* nearestValidConfigurator
    = aConfigurator;
  while (! nearestValidConfigurator->hasConfig ()) {
    assert (! nearestValidConfigurator->isRootConfigurator ());
    assert (dynamic_cast<const IndexedContainerConfigurator*> (aConfigurator));
    nearestValidConfigurator = nearestValidConfigurator
      ->getParentConfigurator ();
  }

  const Node* result
    = nearestValidConfigurator->getConfig ();
  
  if (result->getKey () != "dynamical_system") {
    string relativeKeyPath;
    result = result
      ->findMatchingParentNode (relativeKeyPath, "dynamical_system");

#if DEBUG__SEMANTIC_CHECK_CPP
    cout << "'findDynSysNode<NEAREST_PARENT>', relative path:\n"
         << relativeKeyPath
         << endl;
#endif
  }

  assert (result->getKey () == "dynamical_system");
  return result;
}

template <const dynamical_system__search_t searchOpt>
const Node*
getDynSysNode (const AbstractConfigurator* aConfigurator)
{
  const Node* const dynSysNode
    = findDynSysNode<searchOpt> (aConfigurator);
  assert (dynSysNode != NULL);
  return dynSysNode;
}

template <const dynamical_system__search_t searchOpt>
unsigned int
getStateSpaceDim (const AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  const Node* const stateSpaceDimNode
    = getDynSysNode<searchOpt> (aConfigurator)
    ->findNode ("state_space_dimension");
  if (stateSpaceDimNode == NULL) {
#if DEBUG__SEMANTIC_CHECK_CPP || 1
    cerr << "WARNING: getStateSpaceDim<FROM_ROOT> "
	 << "misses 'state_space_dimension'-node!"
	 << endl;
#endif

    return 0;
  } else {
    unsigned int result = 0;
    leafNodeTo (stateSpaceDimNode, result);
    return result;
  }
}


template <const dynamical_system__search_t searchOpt>
const vector<unsigned int>
toDimFunc_getStateSpaceDim (const AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;

  result.push_back (getStateSpaceDim<searchOpt> (aConfigurator));

  return result;
}

template <const dynamical_system__search_t searchOpt>
const vector<unsigned int>
toRanges_getStateSpaceDim (const AbstractConfigurator* aConfigurator)
{
  vector<unsigned int> result;

  result.push_back (getStateSpaceDim<searchOpt> (aConfigurator) );
  result.push_back (2);
  return result;
}

/* ********************************************************************** */

/** template specialization */
template<>
const vector<unsigned int>
getDimensions<dynamical_system::temporal_initial_function_>
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;

  const AbstractConfigurator* const
    stateSpaceDimConfigurator
    = findSibling ( aConfigurator,
		    "state_space_dimension" );
  assert (stateSpaceDimConfigurator != NULL);

  unsigned int stateSpaceDim = 0;
  leafNodeTo ( stateSpaceDimConfigurator->getConfig (),
	       stateSpaceDim );
  result.push_back (stateSpaceDim);

  return result;
}

/** template specialization */
template<>
const vector<unsigned int>
getDimensions<dynamical_system::domain_boundary>
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;

  unsigned int spatialComponents = 1; /* to do... */
  result.push_back (spatialComponents);
  result.push_back (2); /* left_boundary, right_boundary */

  return result;
}

#if OPTION__DYNAMIC_PARAMETERS
/* 'parameters::array' and 'parameters::parameter_' are dynamic, so we
   do not need to determine their dimensions anymore (but see the
   corresponding save conditions): */
#else
/** template specialization */
template<>
const vector<unsigned int>
getDimensions<dynamical_system::parameters::array>
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;

  const Node* const dimNode
    = getDynSysNode<NEAREST_PARENT> (aConfigurator)
    ->findNode ("parameter_space_dimension");
  assert (dimNode != NULL);

  unsigned int dim = 0;
  leafNodeTo (dimNode, dim);

  result.push_back (dim);
  return result;
}

/** template specialization */
template<>
const vector<unsigned int>
getDimensions<dynamical_system::parameters::parameter_>
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;

  assert (! aConfigurator->hasConfig ());
  /*: because the Configurator is an indexed one... */

  const Node* const dimNode
    = getDynSysNode<NEAREST_PARENT> (aConfigurator->getParentConfigurator ())
    ->findNode ("parameter_space_dimension");
  assert (dimNode != NULL);

  unsigned int dim = 0;
  leafNodeTo (dimNode, dim);

  result.push_back (dim);
  return result;
}
#endif /* OPTION__DYNAMIC_PARAMETERS */

/** template specialization */
template<>
const vector<unsigned int>
getDimensions<dynamical_system::initial_state>
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;

  const Node* const dynSysNode
    = getDynSysNode<NEAREST_PARENT> (aConfigurator);

  const Node* recurrenceLevelNode
    = dynSysNode->findNode ("recurrence_level");
  if (recurrenceLevelNode != NULL) {
    unsigned int recurrenceLevel = 0;
    leafNodeTo (recurrenceLevelNode, recurrenceLevel);

    if (recurrenceLevel > 1) {
      result.push_back (recurrenceLevel);
    }
  }

  const Node* const dimNode
    = dynSysNode->findNode ("state_space_dimension");
  assert (dimNode != NULL);

  unsigned int dim = 0;
  leafNodeTo (dimNode, dim);

  result.push_back (dim);
  return result;
}

/** template specialization */
template<>
const vector<unsigned int>
getDimensions<dynamical_system::discrete_initial_state>
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;

  const Node* const dynSysNode
    = getDynSysNode<NEAREST_PARENT> (aConfigurator);

  const Node* const recurrenceLevelNode
    = dynSysNode->findNode ("recurrence_level");
  if (recurrenceLevelNode != NULL) {
    unsigned int recurrenceLevel = 0;
    leafNodeTo (recurrenceLevelNode, recurrenceLevel);

    if (recurrenceLevel > 1) {
      result.push_back (recurrenceLevel);
    }
  }

  const Node* const dimNode
    = dynSysNode->findNode ("discrete_state_space_dimension");
  assert (dimNode != NULL);

  unsigned int dim = 0;
  leafNodeTo (dimNode, dim);

  result.push_back (dim);
  return result;
}


/** template specialization */
template<>
const vector<unsigned int>
getDimensions<dynamical_system::poincare_section::plane_coefficients>
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;

  string outerKeyPath;
  const Node* const outerDynSysNode
    = getDynSysNode<NEAREST_PARENT> (aConfigurator);

  unsigned int stateSpaceDim;
  const Node* const stateSpaceDimNode
    = outerDynSysNode->followKeyPath
    ( "inside::dynamical_system::state_space_dimension" );
  if (stateSpaceDimNode == NULL) {
    result.push_back (0); /*: or 1? */

    return result;
  }

  leafNodeTo (stateSpaceDimNode, stateSpaceDim);

#if DEBUG__SEMANTIC_CHECK_CPP
  cout << "getDimensions<dynamical_system::poincare_section"
       << "::plane_coefficients>, stateSpaceDim: "
       << stateSpaceDim << endl;
#endif

  result.push_back (stateSpaceDim + 1);
  return result;
}


#if OPTION__FREQUENCY_ANALYSIS
/** template specialization */
template<>
const vector<unsigned int>
getDimensions
< investigation_methods::frequency_analysis::frequency_output_range >
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;
  result.push_back (2);
  return result;
}

/** template specialization */
template<>
const vector<unsigned int>
getDimensions<investigation_methods::frequency_analysis::frequency_weighting>
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;

  string relativeKeyPath;
  const Node* freqAnalysisNode
    = (aConfigurator->getConfig ())
    ->findMatchingParentNode ( relativeKeyPath,
			       "frequency_analysis" );
  assert (freqAnalysisNode != NULL);

  const Node* neighborhoodWidthNode
    = freqAnalysisNode->findNode ("neighborhood_width");
  assert (neighborhoodWidthNode != NULL);

  unsigned int neighborhoodWidth = 0;
  leafNodeTo (neighborhoodWidthNode, neighborhoodWidth);

  result.push_back (neighborhoodWidth);
  return result;
}
#endif /* OPTION__FREQUENCY_ANALYSIS */

/** template specialization */
template<>
const vector<unsigned int>
getDimensions<investigation_methods::check_for_conditions::condition
::points>
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;
  result.push_back (0);
  /*: should be 0, indicating the special case of a dynamic first
    component */
  result.push_back (getStateSpaceDim<FROM_ROOT> (aConfigurator));
  return result;
}


unsigned int
getScanMode (const AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  const AbstractConfigurator* const scanConfigurator  
    = findSubconfigurator ( aConfigurator->getRootConfigurator (),
			    "scan" );
  assert (scanConfigurator != NULL);

  const AbstractConfigurator* const scanModeConfigurator
    = findSubconfigurator ( scanConfigurator, "mode" );
  if (scanModeConfigurator == NULL) {
#if DEBUG__SEMANTIC_CHECK_CPP
    cout << "getScanMode: 'mode'-configurator not available, returning 0..."
	 << endl;
#endif
    return 0; /*: the default scan mode */
  }

  unsigned int scanMode = 0;
  leafNodeTo ( scanModeConfigurator->getConfig (),
	       scanMode );

#if DEBUG__SEMANTIC_CHECK_CPP
  cout << "scan::mode = " << scanMode << endl;
#endif

  return scanMode;
}

unsigned int
getISScanMode (const AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  const AbstractConfigurator* const scanConfigurator  
    = findSubconfigurator ( aConfigurator->getRootConfigurator (),
			    "scan" );
  assert (scanConfigurator != NULL);

  const AbstractConfigurator* const casConfigurator
    = findSubconfigurator ( scanConfigurator, "cas" );
  assert (casConfigurator != NULL);

  const AbstractConfigurator* const issConfigurator
    = findSubconfigurator ( casConfigurator, "iss" );
  assert (issConfigurator != NULL);

  const AbstractConfigurator* const scanModeConfigurator
    = findSubconfigurator ( issConfigurator, "mode" );
  if (scanModeConfigurator == NULL) {
#if DEBUG__SEMANTIC_CHECK_CPP
    cout << "getScanMode: 'mode'-configurator not available, returning 0..."
	 << endl;
#endif
    return 0; /*: the default scan mode */
  }

  unsigned int scanMode = 0;
  leafNodeTo ( scanModeConfigurator->getConfig (),
	       scanMode );

#if DEBUG__SEMANTIC_CHECK_CPP
  cout << "scan::cas::iss::mode = " << scanMode << endl;
#endif

  return scanMode;
}

/** template specialization */
template<>
const vector<unsigned int>
getDimensions<scan::item__> (const AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;
  result.push_back (getScanMode (aConfigurator));
  return result;
}

template<>
const vector<unsigned int>
getDimensions<scan::cas_::iss_::item__> (const AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);
  
  vector<unsigned int> result;
  //result.push_back (getStateSpaceDim<FROM_ROOT> (aConfigurator));
  result.push_back (getISScanMode (aConfigurator));
  return result;
}

#if OPTION__VISUALIZATION
inline
string
getVisWindowType (const AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  const AbstractConfigurator*
    windowConfigurator = aConfigurator;
  while (windowConfigurator->getSpecKey () != "window[]") {
    windowConfigurator
      = windowConfigurator->getParentConfigurator ();
  }

  const Node* const
    windowTypeNode = (windowConfigurator->getConfig ())
    ->getNode ("type");

  string result;
  leafNodeTo (windowTypeNode, result);

  return result;
}

const vector<unsigned int>
getDimensions_WindowDependent (const AbstractConfigurator* aConfigurator)
{
  vector<unsigned int> result;

  string windowType = getVisWindowType (aConfigurator);

  if (windowType == "win3d") {
    result.push_back (3);
    return result;
  }

  if (windowType == "win2d") {
    result.push_back (2);
    return result;
  }

  if (windowType == "win1d") {
    result.push_back (2);
    return result;
  }

  assert (false);
  return result;
}

/** template specialization */
template<>
const vector<unsigned int>
getDimensions<visualization::window::object::variables>
( const AbstractConfigurator* aConfigurator )
{
  vector<unsigned int> result;

  string windowType = getVisWindowType (aConfigurator);

  if (windowType == "win3d") {
    result.push_back (3);
    return result;
  }

  if (windowType == "win2d") {
    result.push_back (2);
    return result;
  }

  if (windowType == "win1d") {
    result.push_back (1);
    return result;
  }

  assert (false);
  return result;
}
#endif /* OPTION__VISUALIZATION */

#if OPTION__SYMBOLIC_IMAGE_ANALYSIS
/** template specialization */
template<>
const vector<unsigned int>
getDimensions<investigation_methods::symbolic_image_analysis::covering::point_select::positions>
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;

  const AbstractConfigurator* const
    siblingDimConfigurator
    = findSibling ( aConfigurator,
		    "number_of_positions" );
  assert (siblingDimConfigurator != NULL);

  unsigned int dim = 0;
  leafNodeTo ( siblingDimConfigurator->getConfig (),
	       dim );
  result.push_back (dim);

  result.push_back (getStateSpaceDim<FROM_ROOT> (aConfigurator));

  return result;
}
#endif /* OPTION__SYMBOLIC_IMAGE_ANALYSIS */


/** member template specialization */
// static
template<>
void
GuiController<AnTGuiKey>::
setupDimensionsFuncMap (map<const string, dim_func_t*>& funcMap)
{
#ifdef ADD_DIM_FUNC
#error "ADD_DIM_FUNC already defined!"
#endif
#define ADD_DIM_FUNC(s) funcMap[s]=&(getDimensions<s>)

  ADD_DIM_FUNC (scan::cas_::iss_::item__);

  ADD_DIM_FUNC (dynamical_system::domain_boundary);

#if OPTION__DYNAMIC_PARAMETERS
  funcMap [ dynamical_system::parameters::array ]
    = &(getDimensions_dynamic_1d);
#else
  /* Are dynamic now; see definitions above... */
  ADD_DIM_FUNC (dynamical_system::parameters::parameter_);
  ADD_DIM_FUNC (dynamical_system::parameters::array);
#endif /* OPTION__DYNAMIC_PARAMETERS */

// VA:
  funcMap [ "dynamical_system::s[]" ]
#if 0 // why?! I think, it should be from nearest parent!
  = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);
#else
  = &(toDimFunc_getStateSpaceDim<NEAREST_PARENT>);
#endif

  ADD_DIM_FUNC (dynamical_system::initial_state);
  funcMap [ dynamical_system::continuous_initial_state ]
    = &(getDimensions<dynamical_system::initial_state>);
  ADD_DIM_FUNC (dynamical_system::discrete_initial_state);
  ADD_DIM_FUNC (dynamical_system::temporal_initial_function_);

  funcMap [ "dynamical_system::data_input::variable[]" ]
#if 1
    = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);
#else
  = &(toDimFunc_getStateSpaceDim<NEAREST_PARENT>);
#endif

#if 0 /* commented out since @default given... */
  funcMap [ dynamical_system::integration::denominators_array ]
    = &(toDimensions_1d<2>);
#endif

  funcMap [ dynamical_system::integration::variables_weighting ]
    = &(toDimFunc_getStateSpaceDim<NEAREST_PARENT>);

  ADD_DIM_FUNC (dynamical_system::poincare_section::plane_coefficients);

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::saving::state_space_saving_area ]
    = &(toRanges_getStateSpaceDim<FROM_ROOT>);

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::saving::velocity_space_saving_area ]
    = &(toRanges_getStateSpaceDim<FROM_ROOT>);

  funcMap [ investigation_methods::band_counter::domain_boundaries ]
    = &(toRanges_getStateSpaceDim<FROM_ROOT>);

  funcMap [ investigation_methods::band_counter::number_of_boxes ]
    = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);

  funcMap [ investigation_methods::symbolic_analysis
	    ::critical_points_for_LR_symbolic_dynamics ]
    = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);

  funcMap [ investigation_methods::dimensions_analysis
	    ::ranges ]
    = &(toRanges_getStateSpaceDim<FROM_ROOT>);

#if OPTION__FREQUENCY_ANALYSIS
  funcMap [ investigation_methods::frequency_analysis
	    ::using_variables ]
    = &(getDimensions_dynamic_1d);

  ADD_DIM_FUNC ( investigation_methods::frequency_analysis
		 ::frequency_output_range );
  ADD_DIM_FUNC ( investigation_methods::frequency_analysis
		 ::frequency_weighting );
#endif /* OPTION__FREQUENCY_ANALYSIS */

  funcMap [ investigation_methods::check_for_conditions
	    ::condition::ranges ]
    = &(toRanges_getStateSpaceDim<FROM_ROOT>);

  funcMap [ investigation_methods::check_for_conditions
	    ::condition::center ]
    = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);

  ADD_DIM_FUNC ( investigation_methods::check_for_conditions
		 ::condition::points );

  funcMap [ investigation_methods::spatial_evaluations
	    ::specific_counter::ranges ]
    = &(toRanges_getStateSpaceDim<FROM_ROOT>);

  // for dynamic arrays this has to be done!
  funcMap [ "investigation_methods::spatial_evaluations::specific_counter[]::parameters" ]
    = &(getDimensions_dynamic_1d);

  funcMap [ dynamical_system::temporal_initial_function
	    ::coefficients ]
    = &(getDimensions_dynamic_1d);

  funcMap [ dynamical_system::poincare_section
	    ::additional_parameters ]
    = &(getDimensions_dynamic_1d);

  funcMap [ dynamical_system::integration
	    ::nominators_array ]
    = &(getDimensions_dynamic_1d);

  funcMap [ investigation_methods::period_analysis
	    ::periods_to_select ]
    = &(getDimensions_dynamic_1d);

  /* ********************************************************************** */



#if OPTION__SYMBOLIC_IMAGE_ANALYSIS
  funcMap [ investigation_methods::symbolic_image_analysis::covering::uniform_grid_ ]
    = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);

//  funcMap [ investigation_methods::symbolic_image_analysis::covering::interval_select_ ]
//    = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);

  funcMap ["investigation_methods::symbolic_image_analysis::covering::interval_select[]::interval_range[]"]
    = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);

  ADD_DIM_FUNC (investigation_methods::symbolic_image_analysis::covering::point_select::positions);

  funcMap [ "investigation_methods::symbolic_image_analysis::covering::output_format[]::value_position" ]
    = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);

  funcMap [ "investigation_methods::symbolic_image_analysis::image_cells::recurrent_set_selection[]::select_recurrent_sets[]::range" ]
    = &(toRanges_getStateSpaceDim<FROM_ROOT>);

  funcMap [ "investigation_methods::symbolic_image_analysis::image_cells::recurrent_set_selection[]::select_recurrent_sets[]::set_numbers" ]
    = &(getDimensions_dynamic_1d);

  funcMap [ "investigation_methods::symbolic_image_analysis::image_cells::recurrent_set_selection[]::select_recurrent_sets[]::connect_with" ]
    = &(getDimensions_dynamic_1d);

  //funcMap [ investigation_methods::symbolic_image_analysis::image_cells
  //	    ::output_writer_::only_subdivisions ]
  //  = &(getDimensions_dynamic_1d);

  funcMap [ investigation_methods::symbolic_image_analysis::image_cells
  	    ::output_writer_::only_subdivisions ]
    = &(getDimensions_dynamic_1d);

  funcMap [ "investigation_methods::symbolic_image_analysis::image_cells::output_writer[]::only_periods" ]
    = &(getDimensions_dynamic_1d);
#endif /* OPTION__SYMBOLIC_IMAGE_ANALYSIS */

#if OPTION__SIPA_ANALYSIS

  funcMap [ "investigation_methods::rim_analysis::uniform_grid[]"]
    = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);

  funcMap [ "investigation_methods::rim_analysis::interval_select[]"]
    = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);


  funcMap [ "investigation_methods::rim_analysis::solution_point" ]
    = &(toDimFunc_getStateSpaceDim<FROM_ROOT>);

  funcMap [ "investigation_methods::rim_analysis::max_value_range" ]
    = &(toRanges_getStateSpaceDim<FROM_ROOT>);

  funcMap [ "investigation_methods::rim_analysis::min_zero_distance" ]
    = &(toRanges_getStateSpaceDim<FROM_ROOT>);

#endif /* OPTION__SIPA_ANALYSIS */

  /* ********************************************************************** */

  ADD_DIM_FUNC (scan::item__);

  /* ********************************************************************** */

#if OPTION__VISUALIZATION
  funcMap [ visualization::window::object::vector_ ]
    = &(getDimensions_WindowDependent);

  funcMap [ visualization::window::object::vector2 ]
    = &(getDimensions_WindowDependent);

  ADD_DIM_FUNC (visualization::window::object::variables);

  funcMap [ "visualization::window[]::object[]::position::offset" ]
    = &(getDimensions<visualization::window::object::variables>);

#if 0 /* commented out since @default given... */
  funcMap [ visualization::window::object::scale ]
    = &(toDimensions_1d<3>);
#endif

  funcMap [ visualization::window::axes::stepping ]
    = &(getDimensions_WindowDependent);
#endif /* OPTION__VISUALIZATION */

#undef ADD_DIM_FUNC
}

/* ********************************************************************** */

namespace DynSysType {
  extern const string typeSiblingKey ("type");

  extern INITIALIZE_KEY (map);
  extern INITIALIZE_KEY (ode);
  extern INITIALIZE_KEY (dde);
  extern INITIALIZE_KEY (fde);
  extern INITIALIZE_KEY (pde_1d);
  extern INITIALIZE_KEY (codel);
  extern INITIALIZE_KEY (recurrent_map);
  extern INITIALIZE_KEY (cml);
  extern INITIALIZE_KEY (external_data);
  extern INITIALIZE_KEY (hybrid_map);
  extern INITIALIZE_KEY (hybrid_ode);
  extern INITIALIZE_KEY (hybrid_dde);
  extern INITIALIZE_KEY (stochastical_map);
  extern INITIALIZE_KEY (averaged_map);
  extern INITIALIZE_KEY (stochastical_ode);
  extern INITIALIZE_KEY (stochastical_dde);
  extern INITIALIZE_KEY (poincare_map);
}

#define isDynSysType(t) createIfSiblingHasEnumValue<DynSysType::typeSiblingKey, DynSysType::t>

bool
isHybridDynSysType ( AbstractConfigurator* aParentConfigurator,
		     const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isDynSysType(hybrid_map) ( aParentConfigurator,
			       aSpecNode )
    ||
    isDynSysType(hybrid_ode) ( aParentConfigurator,
			       aSpecNode )
    ||
    isDynSysType(hybrid_dde) ( aParentConfigurator,
			       aSpecNode );
}

bool
isStochasticalType ( AbstractConfigurator* aParentConfigurator,
		     const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isDynSysType(stochastical_map) ( aParentConfigurator,
				     aSpecNode )
    ||
    isDynSysType(stochastical_ode) ( aParentConfigurator,
				     aSpecNode )
    ||
    isDynSysType(stochastical_dde) ( aParentConfigurator,
				     aSpecNode );
}

bool
isCellularType ( AbstractConfigurator* aParentConfigurator,
		 const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isDynSysType(cml) ( aParentConfigurator,
			aSpecNode )
    ||
    isDynSysType(codel) ( aParentConfigurator,
			  aSpecNode );
}

/* ********************************************************************** */

template <const dynamical_system__search_t searchOpt>
bool
isSpatialType
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const dynSysNode
    = findDynSysNode<searchOpt> (aParentConfigurator);
  if (dynSysNode == NULL) {
    cerr << "WARNING: you should define your dynamical system"
	 << " before initializing the investigation methods!"
	 << endl;
    return false;
  }

  const Node* const typeNode
    = dynSysNode->findNode ("type");
  if (typeNode == NULL) {
#if DEBUG__SEMANTIC_CHECK_CPP && 0
    cerr << "WARNING: seems that you entered the investigation methods"
	 << " even though the dynamical system is not fully defined yet!"
	 << endl;
#endif
    return false;
  }

  string type;
  leafNodeTo (typeNode, type);
  return ( (type == DynSysType::cml)
	   || (type == DynSysType::codel)
	   || (type == DynSysType::pde_1d) );
}

/* ********************************************************************** */

/** template specialization */
template <>
bool
creationCondition<dynamical_system::noisy_variable::min_>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    ( getEnumValueFromParent ("type", aParentConfigurator)
      == "uniform" );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::noisy_variable::mean_value>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    ( getEnumValueFromParent ("type", aParentConfigurator)
      == "gauss" );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::state_space_dimension>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    ! isDynSysType(poincare_map) ( aParentConfigurator,
				   aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::parameter_space_dimension>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    ! ( isDynSysType(external_data) ( aParentConfigurator,
				      aSpecNode )
	||
	isDynSysType(poincare_map) ( aParentConfigurator,
				     aSpecNode )
	);
}

enum ParametersModeEnum {DEDUCE, ARRAY, INDEXED};
static ParametersModeEnum parametersMode = DEDUCE;

/** template specialization */
template <>
bool
creationCondition<dynamical_system::parameters::parameter_>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    (parametersMode == INDEXED)
    ||
    ( (parametersMode == DEDUCE)
      &&
      ( ( (aParentConfigurator->getConfig ())->findNode ("array")
	  == NULL )
	||
	( (aParentConfigurator->getConfig ())->findNode ("parameter[0]")
	  != NULL ) ) );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::parameters::array>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    (parametersMode == ARRAY)
    ||
    ( (parametersMode == DEDUCE)
      &&
      ( ( (aParentConfigurator->getConfig ())->findNode ("parameter[0]")
	  == NULL )
	||
	( (aParentConfigurator->getConfig ())->findNode ("array")
	  != NULL ) ) );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::initial_state>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    ! ( isDynSysType(cml) ( aParentConfigurator,
			    aSpecNode )
	||
	isDynSysType(codel) ( aParentConfigurator,
			      aSpecNode )
	||
	isDynSysType(dde) ( aParentConfigurator,
			    aSpecNode )
	||
	isDynSysType(external_data) ( aParentConfigurator,
				      aSpecNode )
	||
	isDynSysType(fde) ( aParentConfigurator,
			    aSpecNode )
	||
	isHybridDynSysType ( aParentConfigurator,
			     aSpecNode )
	//  	||
	//  	isDynSysType(hybrid_dde) ( aParentConfigurator,
	//  				   aSpecNode )
	||
	isDynSysType(pde_1d) ( aParentConfigurator,
			       aSpecNode )
	||
	isDynSysType(poincare_map) ( aParentConfigurator,
				     aSpecNode )
	||
	isDynSysType(stochastical_dde) ( aParentConfigurator,
					 aSpecNode )
	);
}


/** template specialization */
template <>
bool
creationCondition<dynamical_system::continuous_initial_state>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isDynSysType(hybrid_map) ( aParentConfigurator,
			       aSpecNode )
    ||
    isDynSysType(hybrid_ode) ( aParentConfigurator,
			       aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::reset_initial_states_from_orbit>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    ! isDynSysType(external_data) ( aParentConfigurator,
				    aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::temporal_initial_function_>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isDynSysType(dde) ( aParentConfigurator,
			aSpecNode )
    ||
    isDynSysType(fde) ( aParentConfigurator,
			aSpecNode )
    ||
    isDynSysType(hybrid_dde) ( aParentConfigurator,
			       aSpecNode )
    ||
    isDynSysType(stochastical_dde) ( aParentConfigurator,
				     aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::spatial_initial_function_>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isDynSysType(cml) ( aParentConfigurator,
			aSpecNode )
    ||
    isDynSysType(codel) ( aParentConfigurator,
			  aSpecNode )
    ||
    isDynSysType(pde_1d) ( aParentConfigurator,
			   aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::integration_>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isDynSysType(codel) ( aParentConfigurator,
			  aSpecNode )
    ||
    isDynSysType(dde) ( aParentConfigurator,
			aSpecNode )
    ||
    isDynSysType(fde) ( aParentConfigurator,
			aSpecNode )
    ||
    isDynSysType(hybrid_dde) ( aParentConfigurator,
			       aSpecNode )
    ||
    isDynSysType(hybrid_ode) ( aParentConfigurator,
			       aSpecNode )
    ||
    isDynSysType(ode) ( aParentConfigurator,
			aSpecNode )
    ||
    isDynSysType(pde_1d) ( aParentConfigurator,
			   aSpecNode )
    ||
    isDynSysType(stochastical_dde) ( aParentConfigurator,
				     aSpecNode )
    ||
    isDynSysType(stochastical_ode) ( aParentConfigurator,
				     aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::delay>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isDynSysType(dde) ( aParentConfigurator,
			aSpecNode )
    ||
    isDynSysType(hybrid_dde) ( aParentConfigurator,
			       aSpecNode )
    ||
    isDynSysType(stochastical_dde) ( aParentConfigurator,
				     aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::neumann_boundary_conditions
::operator_::lower_boundary_value>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  string policy = getEnumValueFromParent
    ( "policy", aParentConfigurator );

  return (policy == "constant");
}

/* ********************************************************************** */

namespace InitialFunctionType {
  extern const string typeSiblingKey ("type");

  extern INITIALIZE_KEY (constant);
  extern INITIALIZE_KEY (random);
  extern INITIALIZE_KEY (step);
  extern INITIALIZE_KEY (periodic_step);
  extern INITIALIZE_KEY (singular);
  extern INITIALIZE_KEY (sawtooth);
  extern INITIALIZE_KEY (polynomial);
  extern INITIALIZE_KEY (sin);
  extern INITIALIZE_KEY (sinc);
  extern INITIALIZE_KEY (gauss);
  extern INITIALIZE_KEY (fermi);
}

#define isInitialFunctionType(t) createIfSiblingHasEnumValue<InitialFunctionType::typeSiblingKey, InitialFunctionType::t>

/** template specialization */
template <>
bool
creationCondition<dynamical_system::temporal_initial_function
::amplitude>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    ! ( isInitialFunctionType(constant) (aParentConfigurator, aSpecNode)
	||
	isInitialFunctionType(polynomial) (aParentConfigurator, aSpecNode)
	||
	isInitialFunctionType(random) (aParentConfigurator, aSpecNode)
	);
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::temporal_initial_function
::offset>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return 
    ! ( isInitialFunctionType(polynomial) (aParentConfigurator, aSpecNode)
	||
	isInitialFunctionType(random) (aParentConfigurator, aSpecNode)
	);
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::temporal_initial_function
::period>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isInitialFunctionType(periodic_step) (aParentConfigurator, aSpecNode)
    ||
    isInitialFunctionType(sawtooth) (aParentConfigurator, aSpecNode)
    ||
    isInitialFunctionType(sin) (aParentConfigurator, aSpecNode)
    ||
    isInitialFunctionType(sinc) (aParentConfigurator, aSpecNode);
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::temporal_initial_function
::sigma>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isInitialFunctionType(gauss) (aParentConfigurator, aSpecNode)
    ||
    isInitialFunctionType(fermi) (aParentConfigurator, aSpecNode);
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::temporal_initial_function
::width>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isInitialFunctionType(step) (aParentConfigurator, aSpecNode)
    ||
    isInitialFunctionType(periodic_step) (aParentConfigurator, aSpecNode)
    ||
    isInitialFunctionType(sawtooth) (aParentConfigurator, aSpecNode);
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::temporal_initial_function
::argument_shift>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    ! ( isInitialFunctionType(constant) (aParentConfigurator, aSpecNode)
	||
	isInitialFunctionType(polynomial) (aParentConfigurator, aSpecNode)
	||
	isInitialFunctionType(random) (aParentConfigurator, aSpecNode)
	);
}

/* ********************************************************************** */

namespace IntegrationMethod {
  extern const string methodSiblingKey ("method");

  extern INITIALIZE_KEY (euler_forward);
  extern INITIALIZE_KEY (heun);
  extern INITIALIZE_KEY (midpoint);
  extern INITIALIZE_KEY (ralston);
  extern INITIALIZE_KEY (radau);
  extern INITIALIZE_KEY (rk44);
  extern INITIALIZE_KEY (rkm45);
  extern INITIALIZE_KEY (rkf456);

  extern INITIALIZE_KEY (using_butcher_array);

  extern INITIALIZE_KEY (euler_backward);
  extern INITIALIZE_KEY (heun_backward);

  extern INITIALIZE_KEY (bdf);
  extern INITIALIZE_KEY (adams_moulton);
  extern INITIALIZE_KEY (adams_bashforth);
  extern INITIALIZE_KEY (pece_ab_am);
  extern INITIALIZE_KEY (pece_ab_bdf);

  extern INITIALIZE_KEY (euler_forward_manhattan);
  extern INITIALIZE_KEY (heun_manhattan);
  extern INITIALIZE_KEY (midpoint_manhattan);
  extern INITIALIZE_KEY (ralston_manhattan);
  extern INITIALIZE_KEY (radau_manhattan);
  extern INITIALIZE_KEY (rk44_manhattan);
  extern INITIALIZE_KEY (rkm45_manhattan);
  extern INITIALIZE_KEY (rkf456_manhattan);
  extern INITIALIZE_KEY (using_butcher_array_with_manhattan);
  extern INITIALIZE_KEY (adams_bashforth_manhattan);
  extern INITIALIZE_KEY (pece_ab_am_manhattan);
  extern INITIALIZE_KEY (pece_ab_bdf_manhattan);

  extern INITIALIZE_KEY (euler_forward_halfstep);
  extern INITIALIZE_KEY (heun_halfstep);
  extern INITIALIZE_KEY (midpoint_halfstep);
  extern INITIALIZE_KEY (ralston_halfstep);
  extern INITIALIZE_KEY (radau_halfstep);
  extern INITIALIZE_KEY (rk44_halfstep);
  extern INITIALIZE_KEY (rkm45_halfstep);
  extern INITIALIZE_KEY (rkf456_halfstep);
  extern INITIALIZE_KEY (using_butcher_array_with_halfstep);
  extern INITIALIZE_KEY (adams_bashforth_halfstep);
  extern INITIALIZE_KEY (pece_ab_am_halfstep);
  extern INITIALIZE_KEY (pece_ab_bdf_halfstep);

  extern INITIALIZE_KEY (euler_forward_heun);
  extern INITIALIZE_KEY (heun_midpoint);
  extern INITIALIZE_KEY (midpoint_ralston);
  extern INITIALIZE_KEY (ralston_radau);
  extern INITIALIZE_KEY (radau_rk44);
  extern INITIALIZE_KEY (rk44_rkm45);
  extern INITIALIZE_KEY (rk44_rkf456);
  extern INITIALIZE_KEY (rkm45_rkf456);
  extern INITIALIZE_KEY (adams_bashforth_pece_ab_am);
  extern INITIALIZE_KEY (adams_bashforth_pece_ab_bdf);
  extern INITIALIZE_KEY (pece_ab_am_pece_ab_bdf);
}

#define isIntegrationMethod(t) createIfSiblingHasEnumValue<IntegrationMethod::methodSiblingKey, IntegrationMethod::t>

bool
isButcherIntegrator ( AbstractConfigurator* aParentConfigurator,
		      const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isIntegrationMethod(using_butcher_array)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(using_butcher_array_with_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(using_butcher_array_with_halfstep)
    ( aParentConfigurator, aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::integration
::nominators_array>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isButcherIntegrator (aParentConfigurator, aSpecNode)
    &&
    ( getEnumValueFromParent ("array_name", aParentConfigurator)
      == "user_defined" );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::integration
::backward_threshold>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isIntegrationMethod(euler_backward)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(heun_backward)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(bdf)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(adams_moulton)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_bdf)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_bdf_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_bdf_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(adams_bashforth_pece_ab_am)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(adams_bashforth_pece_ab_bdf)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_pece_ab_bdf)
    ( aParentConfigurator, aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::integration
::bdf_order>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isIntegrationMethod(bdf)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_bdf)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_bdf_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_bdf_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(adams_bashforth_pece_ab_bdf)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_pece_ab_bdf)
    ( aParentConfigurator, aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::integration
::adams_moulton_order>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isIntegrationMethod(adams_moulton)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(adams_bashforth_pece_ab_am)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_pece_ab_bdf)
    ( aParentConfigurator, aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::integration
::adams_bashforth_order>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isIntegrationMethod(adams_bashforth)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_bdf)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(adams_bashforth_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_bdf_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(adams_bashforth_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_bdf_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(adams_bashforth_pece_ab_am)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(adams_bashforth_pece_ab_bdf)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_pece_ab_bdf)
    ( aParentConfigurator, aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::integration
::manhattan_threshold>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isIntegrationMethod(euler_forward_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(midpoint_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(ralston_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(radau_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(rk44_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(rkm45_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(rkf456_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(adams_bashforth_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_bdf_manhattan)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(using_butcher_array_with_manhattan)
    ( aParentConfigurator, aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<dynamical_system::integration
::integration_accuracy>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isIntegrationMethod(euler_forward_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(midpoint_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(ralston_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(radau_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(rk44_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(rkm45_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(rkf456_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(adams_bashforth_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_am_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(pece_ab_bdf_halfstep)
    ( aParentConfigurator, aSpecNode )
    ||
    isIntegrationMethod(using_butcher_array_with_halfstep)
    ( aParentConfigurator, aSpecNode );
}

/* ********************************************************************** */

namespace PoincareSectionType {
  extern const string typeSiblingKey ("type");

  extern INITIALIZE_KEY (fixed_plane);
  extern INITIALIZE_KEY (parameter_dependent_plane);
  extern INITIALIZE_KEY (user_defined);
  extern INITIALIZE_KEY (min_max_points);
  extern INITIALIZE_KEY (max_points);
  extern INITIALIZE_KEY (min_points);
  extern INITIALIZE_KEY (change_of_hybrid_state);
}

#define isPoincareType(t) createIfSiblingHasEnumValue<PoincareSectionType::typeSiblingKey, PoincareSectionType::t>

/** template specialization */
template <>
bool
creationCondition<dynamical_system::poincare_section
::variable_index>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isPoincareType(min_points)
    ( aParentConfigurator, aSpecNode )
    ||
    isPoincareType(max_points)
    ( aParentConfigurator, aSpecNode )
    ||
    isPoincareType(min_max_points)
    ( aParentConfigurator, aSpecNode );
}

/* ********************************************************************** */

namespace ScanItem {
  extern const string typeSiblingKey ("type");

  extern INITIALIZE_KEY (real_linear);
  extern INITIALIZE_KEY (real_linear_cas);
  extern INITIALIZE_KEY (real_logarithmic);
  extern INITIALIZE_KEY (integer_linear);
  extern INITIALIZE_KEY (integer_logarithmic);
  extern INITIALIZE_KEY (real_linear_2d);
  extern INITIALIZE_KEY (real_elliptic_2d);
  extern INITIALIZE_KEY (from_file);
}

#define isScanItem(t) createIfSiblingHasEnumValue<ScanItem::typeSiblingKey, ScanItem::t>

/** template specialization */
template <>
bool
creationCondition<scan::item_
::object>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isScanItem(real_linear)
    ( aParentConfigurator, aSpecNode )
    ||
    isScanItem(real_linear_cas)
    ( aParentConfigurator, aSpecNode )
    ||
    isScanItem(real_logarithmic)
    ( aParentConfigurator, aSpecNode )
    ||
    isScanItem(integer_linear)
    ( aParentConfigurator, aSpecNode )
    ||
    isScanItem(integer_logarithmic)
    ( aParentConfigurator, aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<scan::item_
::first_object>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isScanItem(real_linear_2d)
    ( aParentConfigurator, aSpecNode )
    ||
    isScanItem(real_elliptic_2d)
    ( aParentConfigurator, aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<scan::item_
::points>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    ! isScanItem(from_file)
    ( aParentConfigurator, aSpecNode );
}

template <>
bool
creationCondition<scan::cas_::iss_::item_
::object>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  return creationCondition<scan::item_::object>
    ( aParentConfigurator, aSpecNode );
}

template <>
bool
creationCondition<scan::cas_::iss_::item_
::first_object>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  return creationCondition<scan::item_::first_object>
    ( aParentConfigurator, aSpecNode );
}

template <>
bool
creationCondition<scan::cas_::iss_::item_
::points>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  return creationCondition<scan::item_::points>
    ( aParentConfigurator, aSpecNode );
}

namespace ScanType {
  extern const string typeSiblingKey ("type");

  extern INITIALIZE_KEY (nested_items);
  extern INITIALIZE_KEY (nested_items_cas);
}

#define isScanType(t) createIfSiblingHasEnumValue<ScanType::typeSiblingKey, ScanType::t>

bool
isCASActive
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);
  
  return
    isScanType(nested_items_cas)
    ( aParentConfigurator, aSpecNode );
}

template <>
bool
creationCondition<scan::cas>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  return isCASActive (aParentConfigurator, aSpecNode);
}

/** template specialization */
template <>
bool
creationCondition<scan::item_::subitem_::
object>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return true;
  /* within a subitem the object should always be created. The
     creation condition is needed because the 'object' is defined as a
     '@like' to the 'scan::item::object' and its creation condition
     function can not be called here */
}

template <>
bool
creationCondition<scan::cas_::iss_::item_::subitem_
::object>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  return creationCondition<scan::item_::subitem_::object>
    ( aParentConfigurator, aSpecNode );
}

/* ********************************************************************** */

/** template specialization */
template <>
bool
creationCondition<investigation_methods::general_trajectory_evaluations
::saving::points_step>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  string type = getEnumValueFromParent
    ( "type", aParentConfigurator );

  return (type == "time_oriented");
}

/** template specialization */
template <>
bool
creationCondition<investigation_methods::general_trajectory_evaluations
::saving::state_space_distance>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  string type = getEnumValueFromParent
    ( "type", aParentConfigurator );

  return (type == "space_oriented");
}

/* ********************************************************************** */

/** template specialization */
template <>
bool
creationCondition<investigation_methods::symbolic_analysis
::critical_points_for_LR_symbolic_dynamics>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  string symbolicFunc = getEnumValueFromParent
    ( "symbolic_function", aParentConfigurator );

  return (symbolicFunc == "using_<L/R>_symbolic_dynamics");
}

/* ********************************************************************** */

/** template specialization */
template <>
bool
creationCondition<investigation_methods::lyapunov_exponents_analysis
::p>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  string norm = getEnumValueFromParent
    ( "using_norm", aParentConfigurator );

  return (norm == "Lp");
}

/* ********************************************************************** */

/** template specialization */
template <>
bool
creationCondition<investigation_methods::regions_analysis_>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return (getScanMode (aParentConfigurator) == 2);
}


/* ********************************************************************** */

namespace BandCounterType {
  extern const string typeSiblingKey ("method");

  extern INITIALIZE_KEY (gcd_based);
  extern INITIALIZE_KEY (box_counting_based);
}

template <const string& aType>
bool
isBandCounterType ( AbstractConfigurator* aParentConfigurator,
		    const Node* aSpecNode )
{
  return createIfSiblingHasEnumValue
    < BandCounterType::typeSiblingKey, aType >
    ( aParentConfigurator, aSpecNode );
}

/** template specialization */
template <>
bool
  creationCondition<investigation_methods::band_counter::basis_points>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isBandCounterType<BandCounterType::gcd_based> (aParentConfigurator, aSpecNode);
}

template <>
bool
creationCondition<investigation_methods::band_counter::number_of_boxes>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isBandCounterType<BandCounterType::box_counting_based> (aParentConfigurator, aSpecNode);
}

/* ********************************************************************** */
// Dimensions Analysis


namespace DimensionsAnalysisType {
  extern const string typeSiblingKey ("median_calculation");

  extern INITIALIZE_KEY (plateau_algorithm);
  extern INITIALIZE_KEY (user_selected_layers);
}

template <const string& aType>
    bool
    isDimensionsAnalysisType( AbstractConfigurator* aParentConfigurator,
			     const Node* aSpecNode )
{
  return createIfSiblingHasEnumValue
      < DimensionsAnalysisType::typeSiblingKey, aType >
      ( aParentConfigurator, aSpecNode );
}

/** template specialization median_calculation user_selected_layers */
template <>
    bool
    creationCondition<investigation_methods::dimensions_analysis::min_layer>
    ( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
      isDimensionsAnalysisType<DimensionsAnalysisType::user_selected_layers> (aParentConfigurator, aSpecNode) ||
      isDimensionsAnalysisType<DimensionsAnalysisType::plateau_algorithm> (aParentConfigurator, aSpecNode);
}

template <>
void
updateEvent<scan::cas_::max_backtrack_depth>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ((*cIter)->getSpecKey () == "backtrack_depth") {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
        (*cIter, !(isActive && isSensitive), activationSiblings);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

template <>
void
updateEvent<scan::cas_::min_max>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ((*cIter)->getSpecKey () == "infinity_threshold") {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
        (*cIter, (isActive && isSensitive), activationSiblings);
      continue;
    }
    if ((*cIter)->getSpecKey () == "regular_compare_precision") {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
        (*cIter, (isActive && isSensitive), activationSiblings);
      continue;
    }
    if ((*cIter)->getSpecKey () == "reduced_compare_precision") {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
        (*cIter, (isActive && isSensitive), activationSiblings);
      continue;
    }
    if ((*cIter)->getSpecKey () == "max_period") {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
        (*cIter, (isActive && isSensitive), activationSiblings);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization q-dimension*/
template<>
void
updateEvent<investigation_methods::dimensions_analysis::q_dimension>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {

    if ( (*cIter)->getSpecKey () == "q_start" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "q_end" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }
 
    if ( (*cIter)->getSpecKey () == "q_dimension_file_suffix" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }


  } // for

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}


/* ********************************************************************** */


namespace ConditionType {
  extern const string typeSiblingKey ("type");

  extern INITIALIZE_KEY (any_fixed_point);
  extern INITIALIZE_KEY (given_points);
  extern INITIALIZE_KEY (leaves_rectangle_area);
  extern INITIALIZE_KEY (leaves_circle_area);
  extern INITIALIZE_KEY (stays_in_rectangle_area);
  extern INITIALIZE_KEY (stays_in_circle_area);
  extern INITIALIZE_KEY (user_defined);
}

template <const string& aType>
bool
isConditionType ( AbstractConfigurator* aParentConfigurator,
		  const Node* aSpecNode )
{
  return createIfSiblingHasEnumValue
    < ConditionType::typeSiblingKey, aType >
    ( aParentConfigurator, aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<investigation_methods::check_for_conditions::condition
::ranges>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isConditionType<ConditionType::leaves_rectangle_area> (aParentConfigurator, aSpecNode)
    || isConditionType<ConditionType::stays_in_rectangle_area> (aParentConfigurator, aSpecNode);
}

/** template specialization */
template <>
bool
creationCondition<investigation_methods::check_for_conditions::condition
::center>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isConditionType<ConditionType::leaves_circle_area> (aParentConfigurator, aSpecNode)
    || isConditionType<ConditionType::stays_in_circle_area> (aParentConfigurator, aSpecNode);
}

/* ********************************************************************** */

#if OPTION__VISUALIZATION
namespace VisObjectType {
  extern const string typeSiblingKey ("type");

  extern INITIALIZE_KEY (curve);
  extern INITIALIZE_KEY (sphere);
  extern INITIALIZE_KEY (marker);
  extern INITIALIZE_KEY (plane);
  extern INITIALIZE_KEY (line);
  extern INITIALIZE_KEY (arrow);
  extern INITIALIZE_KEY (text);
  extern INITIALIZE_KEY (surface);
  extern INITIALIZE_KEY (torus);
  extern INITIALIZE_KEY (cube);
  extern INITIALIZE_KEY (scene);
}

#define isVisObjectType(t) createIfSiblingHasEnumValue<VisObjectType::typeSiblingKey, VisObjectType::t>

#if 0 /* commented out */
/** template specialization */
template <>
bool
creationCondition<visualization::window::object::>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isVisObjectType() ( aParentConfigurator,
			aSpecNode );
}
#endif

/** template specialization */
template <>
bool
creationCondition<visualization::window::object::vector_>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isVisObjectType(plane) ( aParentConfigurator,
			     aSpecNode )
    ||
    isVisObjectType(text) ( aParentConfigurator,
			    aSpecNode )
    ||
    isVisObjectType(arrow) ( aParentConfigurator,
			     aSpecNode ) 
    ||
    isVisObjectType(line) ( aParentConfigurator,
			    aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<visualization::window::object::vector2>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isVisObjectType(plane) ( aParentConfigurator,
			     aSpecNode )
    ||
    isVisObjectType(text) ( aParentConfigurator,
			    aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<visualization::window::object::lighting>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isVisObjectType(cube) ( aParentConfigurator,
			    aSpecNode )
    ||
    isVisObjectType(marker) ( aParentConfigurator,
			      aSpecNode )
    ||
    isVisObjectType(sphere) ( aParentConfigurator,
			      aSpecNode )
    ||
    isVisObjectType(torus) ( aParentConfigurator,
			     aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<visualization::window::object::radius>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isVisObjectType(marker) ( aParentConfigurator,
			      aSpecNode )
    ||
    isVisObjectType(sphere) ( aParentConfigurator,
			      aSpecNode )
    ||
    isVisObjectType(torus) ( aParentConfigurator,
			     aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<visualization::window::object::scale>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    isVisObjectType(cube) ( aParentConfigurator,
			    aSpecNode )
    ||
    isVisObjectType(marker) ( aParentConfigurator,
			      aSpecNode )
    ||
    isVisObjectType(sphere) ( aParentConfigurator,
			      aSpecNode )
    ||
    isVisObjectType(curve) ( aParentConfigurator,
			      aSpecNode )
    ||
    isVisObjectType(scene) ( aParentConfigurator,
			      aSpecNode )
    ||
    isVisObjectType(torus) ( aParentConfigurator,
			     aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<visualization::window::object::position>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    ! isVisObjectType(curve) ( aParentConfigurator,
			       aSpecNode );
}

/** template specialization */
template <>
bool
creationCondition<visualization::window::object::color>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  if ( isVisObjectType(curve) ( aParentConfigurator,
				aSpecNode ) ) {
    string coloringMethod
      = getEnumValueFromParent ( "coloring_method",
				 aParentConfigurator );

    return (coloringMethod == "fixed");
  }

  return
    ! ( isVisObjectType(scene) ( aParentConfigurator,
				 aSpecNode )
	||
	isVisObjectType(surface) ( aParentConfigurator,
				   aSpecNode )
	||
	isVisObjectType(text) ( aParentConfigurator,
				aSpecNode )
	);
}

/** template specialization */
template <>
bool
creationCondition<visualization::window::axes::z_label>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return
    ( (getDimensions_WindowDependent (aParentConfigurator))[0] > 2 );
}
#endif /* OPTION__VISUALIZATION */

/* ********************************************************************** */

namespace symimage_point_select {
  extern const string type ("type");
  extern const string position ("position");
}

namespace output_writer_select {
   extern const string type ("type");
   extern const string periodic_cell_writer ("periodic_cell_writer");
   extern const string all_cell_writer ("all_cell_writer");
   extern const string selected_sets_cell_writer ("selected_sets_cell_writer");
   extern const string attraction_domain_cell_writer ("attraction_domain_cell_writer");
   extern const string inverse_attraction_domain_cell_writer ("inverse_attraction_domain_cell_writer");
   extern const string cells_on_trajectory_writer ("cells_on_trajectory_writer");
   extern const string period_writer ("period_writer");
   extern const string set_order_writer ("set_order_writer");
   extern const string morse_spectrum_writer ("morse_spectrum_writer");
}

namespace symimages_output_format {
  extern const string type ("type");
  extern const string box_output ("box_output");
  extern const string range_output ("range_output");
}

namespace symimages_image_cells {
  extern const string period_finder_type ("period_finder");
  extern const string data_file ("data_file");
}

namespace rim_type {
  extern const string type ("type");
  extern const string solution_point ("solution_points");
}


/** template specialization */
template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::box_output>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer[]::boxoutput -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "all_cell_writer")
    || (typeStr == "periodic_cell_writer")
    || (typeStr == "selected_sets_cell_writer")
	|| (typeStr == "attraction_domain_cell_writer")
	|| (typeStr == "inverse_attraction_domain_cell_writer")
	|| (typeStr == "cells_on_trajectory_writer");
}

template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::invariant_set>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::invariant_set -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "all_cell_writer")
    || (typeStr == "periodic_cell_writer")
    || (typeStr == "selected_sets_cell_writer")
	|| (typeStr == "attraction_domain_cell_writer")
	|| (typeStr == "inverse_attraction_domain_cell_writer")
	|| (typeStr == "cells_on_trajectory_writer");
}

template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::periodic_paths>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::periodic_paths -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "all_cell_writer")
    || (typeStr == "periodic_cell_writer")
    || (typeStr == "selected_sets_cell_writer")
	|| (typeStr == "attraction_domain_cell_writer")
	|| (typeStr == "inverse_attraction_domain_cell_writer")
	|| (typeStr == "cells_on_trajectory_writer");
}

template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::period_size>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::period_size -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "all_cell_writer")
    || (typeStr == "periodic_cell_writer")
    || (typeStr == "selected_sets_cell_writer")
	|| (typeStr == "attraction_domain_cell_writer")
	|| (typeStr == "inverse_attraction_domain_cell_writer")
	|| (typeStr == "cells_on_trajectory_writer");
}

template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::cell_edges>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::cell_edges -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "all_cell_writer")
    || (typeStr == "periodic_cell_writer")
    || (typeStr == "selected_sets_cell_writer")
	|| (typeStr == "attraction_domain_cell_writer")
	|| (typeStr == "inverse_attraction_domain_cell_writer")
	|| (typeStr == "cells_on_trajectory_writer");
}


template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::range_output>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::range_output -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "set_order_writer");
}


template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::all_larger_sets>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::all_larger_sets -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "set_order_writer");
}

template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::set_edges>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::set_edges -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "set_order_writer");
}

template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::number_of_cells>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::number_of_cells -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "set_order_writer");
}

template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::only_more_cells_than>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::only_more_cells_than -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "set_order_writer");
}

template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::max_values>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::max_values -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "morse_spectrum_writer");
}

template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::min_values>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::min_values -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "morse_spectrum_writer");
}

template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::interpolated_values>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "output_writer_::interpolated_values -> type is: "
       << typeStr
       << endl;
#endif
  return (typeStr == "morse_spectrum_writer");
}

/** template specialization */
template <>
bool
creationCondition<investigation_methods::symbolic_image_analysis::image_cells::output_writer_::only_periods>
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const Node* const typeNode
    = (aParentConfigurator->getConfig ())
    ->getNode ("type");
  assert (typeNode != NULL);

  string typeStr;
  leafNodeTo (typeNode, typeStr);

  if((typeStr == "all_cell_writer")
    || (typeStr == "periodic_cell_writer")
    || (typeStr == "selected_sets_cell_writer")
	|| (typeStr == "attraction_domain_cell_writer")
	|| (typeStr == "inverse_attraction_domain_cell_writer")
	|| (typeStr == "cells_on_trajectory_writer")){

  }
  else{
	  return false;
  }

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "entering 'creationCondition<...::only_periods>' ..."
       << endl;
#endif

  const AbstractConfigurator* const periodSizeConfigurator  
    = findSubconfigurator ( aParentConfigurator,
			    "period_size" );
  if (periodSizeConfigurator == NULL) {
    return false;
  } else {
    string isActiveStr;
    leafNodeTo ( periodSizeConfigurator->getConfig (),
		 isActiveStr );
#if DEBUG__SEMANTIC_CHECK_CPP && 0
    cout << "'creationCondition<...::only_periods>' -> "
	 << isActiveStr
	 << endl;
#endif
    return PreSemanticChecker
      ::getBoolFromIniString (isActiveStr);
  }
}


/** member template specialization */
// static
template<>
void
GuiController<AnTGuiKey>::
setupCreationCondFuncMap (map<const string, creation_cond_func_t*>& funcMap)
{
#ifdef ADD_COND_FUNC
#error "ADD_COND_FUNC already defined!"
#endif
#define ADD_COND_FUNC(s) funcMap[s]=&(creationCondition<s>)

  /* ! (DynSysType::external_data || DynSysType::poincare_map) :*/
  ADD_COND_FUNC (dynamical_system::state_space_dimension);

  funcMap [ dynamical_system::parameters::parameter_ ]
    = &(creationCondition<dynamical_system::parameters::parameter_>);

  ADD_COND_FUNC (dynamical_system::parameters::array);
  
  ADD_COND_FUNC (scan::cas);

  /* DynSysType::external_data :*/
  funcMap [ dynamical_system::external_data_filename ]
    = &(isDynSysType(external_data));

  funcMap [ dynamical_system::number_of_columns ]
    = &(isDynSysType(external_data));

  funcMap [ dynamical_system::action_on_wrong_lines ]
    = &(isDynSysType(external_data));

  funcMap [ dynamical_system::accept_empty_lines ]
    = &(isDynSysType(external_data));

  funcMap [ dynamical_system::data_input ]
    = &(isDynSysType(external_data));

  /* DynSysType::hybrid_* :*/
  funcMap [ dynamical_system::discrete_state_space_dimension ]
    = &(isHybridDynSysType);

  funcMap [ dynamical_system::discrete_initial_state ]
    = &(isHybridDynSysType);

  /* dynamical systems with delay :*/
  ADD_COND_FUNC (dynamical_system::delay);

  /* DynSysType::fde :*/
  funcMap [ dynamical_system::maximal_delay ]
    = &(isDynSysType(fde));

  /* DynSysType::recurrent_map :*/
  funcMap [ dynamical_system::recurrence_level ]
    = &(isDynSysType(recurrent_map));

  /* DynSysType::pde_1d :*/
  funcMap [ dynamical_system::domain_boundary ]
    = &(isDynSysType(pde_1d));

  funcMap [ dynamical_system::number_of_grid_points ]
    = &(isDynSysType(pde_1d));

  funcMap [ dynamical_system::neumann_boundary_conditions_ ]
    = &(isDynSysType(pde_1d));

  funcMap [ dynamical_system::dirichlet_boundary_conditions ]
    = &(isDynSysType(pde_1d));

  /* DynSysType::cml || DynSysType::codel :*/
  funcMap [ dynamical_system::number_of_cells ]
    = &(isCellularType);

  /* ! (DynSysType::external_data || DynSysType::poincare_map) :*/
  ADD_COND_FUNC (dynamical_system::parameter_space_dimension);

  funcMap [ dynamical_system::parameters_ ]
    = &(creationCondition<dynamical_system::parameter_space_dimension>);

  /* complex condition :*/
  ADD_COND_FUNC (dynamical_system::initial_state);

  /* ! DynSysType::external_data :*/
  ADD_COND_FUNC (dynamical_system::reset_initial_states_from_orbit);

  /* hybrid systems without memory :*/
  ADD_COND_FUNC (dynamical_system::continuous_initial_state);

  /* dynamical systems with delay :*/
  ADD_COND_FUNC (dynamical_system::temporal_initial_function_);

  /* dynamical systems with one spatial component :*/
  ADD_COND_FUNC (dynamical_system::spatial_initial_function_);

  /* complex condition :*/
  ADD_COND_FUNC (dynamical_system::integration_);

  /* DynSysType::poincare_map :*/
  funcMap [ dynamical_system::inside ]
    = &(isDynSysType(poincare_map));

  funcMap [ dynamical_system::poincare_section_ ]
    = &(isDynSysType(poincare_map));

  /* DynSysType::averaged_map :*/
  funcMap [ dynamical_system::number_of_subsystems ]
    = &(isDynSysType(averaged_map));

  /* DynSysType::stochastical_* :*/
  funcMap [ dynamical_system::number_of_noisy_variables ]
    = &(isStochasticalType);

  funcMap [ dynamical_system::noisy_variable_ ]
    = &(isStochasticalType);

  /* ************************************************************ */

  ADD_COND_FUNC (dynamical_system::noisy_variable::min_);

  funcMap [ dynamical_system::noisy_variable::max_ ]
    = &(creationCondition<dynamical_system::noisy_variable::min_>);

  ADD_COND_FUNC (dynamical_system::noisy_variable::mean_value);

  funcMap [ dynamical_system::noisy_variable::standard_deviation ]
    = &(creationCondition<dynamical_system::noisy_variable::mean_value>);

  /* ************************************************************ */

  ADD_COND_FUNC ( dynamical_system::neumann_boundary_conditions
		  ::operator_::lower_boundary_value );

  funcMap [ dynamical_system::neumann_boundary_conditions
	    ::operator_::upper_boundary_value ]
    = &(creationCondition<dynamical_system
	::neumann_boundary_conditions
	::operator_::lower_boundary_value>);

  ADD_COND_FUNC ( dynamical_system::temporal_initial_function
		  ::amplitude );

  ADD_COND_FUNC ( dynamical_system::temporal_initial_function
		  ::offset );

  ADD_COND_FUNC ( dynamical_system::temporal_initial_function
		  ::period );

  ADD_COND_FUNC ( dynamical_system::temporal_initial_function
		  ::sigma );

  ADD_COND_FUNC ( dynamical_system::temporal_initial_function
		  ::width );

  funcMap [ dynamical_system::temporal_initial_function
	    ::degree ]
    = &(isInitialFunctionType(polynomial));


  funcMap [ dynamical_system::temporal_initial_function
	    ::coefficients ]
    = &(isInitialFunctionType(polynomial));

  funcMap [ dynamical_system::temporal_initial_function
	    ::relative_peak_position ]
    = &(isInitialFunctionType(sawtooth));

  ADD_COND_FUNC ( dynamical_system::temporal_initial_function
		  ::argument_shift );
  
  funcMap [ dynamical_system::temporal_initial_function
	    ::random_specification ]
    = &(isInitialFunctionType(random));

  /* ************************************************************ */

  funcMap [ dynamical_system::integration
	    ::array_name ]
    = &(isButcherIntegrator);

  funcMap [ dynamical_system::integration
	    ::butcher_order_is_known ]
    = &(isButcherIntegrator);

  funcMap [ dynamical_system::integration
	    ::butcher_order ]
    = &(isButcherIntegrator);

  ADD_COND_FUNC (dynamical_system::integration::nominators_array);

  funcMap [ dynamical_system::integration
	    ::denominators_array ]
    = &( creationCondition<dynamical_system::integration
	 ::nominators_array> );

  ADD_COND_FUNC (dynamical_system::integration::backward_threshold);

  ADD_COND_FUNC (dynamical_system::integration::bdf_order);

  ADD_COND_FUNC (dynamical_system::integration::adams_moulton_order);

  ADD_COND_FUNC (dynamical_system::integration::adams_bashforth_order);

  ADD_COND_FUNC (dynamical_system::integration::manhattan_threshold);

  ADD_COND_FUNC (dynamical_system::integration::integration_accuracy);

  funcMap [ dynamical_system::integration
	    ::variables_weighting ]
    = &( creationCondition<dynamical_system::integration
	 ::integration_accuracy> );

  /* ************************************************************ */

  ADD_COND_FUNC (dynamical_system::poincare_section::variable_index);

  funcMap [ dynamical_system::poincare_section
	    ::plane_coefficients ]
    = &(isPoincareType(fixed_plane));

  funcMap [ dynamical_system::poincare_section
	    ::additional_parameters ]
    = &(isPoincareType(user_defined));

  /* ************************************************************ */

  // VA:
  ADD_COND_FUNC (scan::item_
		 ::points );

  funcMap [ scan::item_
	    ::min ]
    = &( creationCondition<scan::item_
	 ::object> );

  funcMap [ scan::item_
	    ::max ]
    = &( creationCondition<scan::item_
	 ::object> );

  funcMap [ scan::item_
	    ::first_min ]
    = &(isScanItem(real_linear_2d));

  funcMap [ scan::item_
	    ::first_max ]
    = &(isScanItem(real_linear_2d));

  funcMap [ scan::item_
	    ::second_min ]
    = &(isScanItem(real_linear_2d));

  funcMap [ scan::item_
	    ::second_max ]
    = &(isScanItem(real_linear_2d));

  funcMap [ scan::item_
	    ::min_angle ]
    = &(isScanItem(real_elliptic_2d));

  funcMap [ scan::item_
	    ::max_angle ]
    = &(isScanItem(real_elliptic_2d));

  funcMap [ scan::item_
	    ::first_axis ]
    = &(isScanItem(real_elliptic_2d));

  funcMap [ scan::item_
	    ::second_axis ]
    = &(isScanItem(real_elliptic_2d));

  funcMap [ scan::item_
	    ::first_center ]
    = &(isScanItem(real_elliptic_2d));

  funcMap [ scan::item_
	    ::second_center ]
    = &(isScanItem(real_elliptic_2d));

  // VA:
  funcMap [ scan::item_
	    ::external_data_filename ]
    = &(isScanItem(from_file));

  funcMap [ scan::item_
	    ::number_of_columns ]
    = &(isScanItem(from_file));

  funcMap [ scan::item_
	    ::subitem ]
    = &(isScanItem(from_file));

  ADD_COND_FUNC (scan::item_::subitem_
		 ::object );

  ADD_COND_FUNC (scan::item_::object);

  ADD_COND_FUNC (scan::item_::first_object);

  funcMap [ scan::item_
	    ::second_object ]
    = &( creationCondition<scan::item_
	 ::first_object> );

  ADD_COND_FUNC (scan::cas_::iss_::item_
		 ::points );

  funcMap [ scan::cas_::iss_::item_
	    ::min ]
    = &( creationCondition<scan::cas_::iss_::item_
	 ::object> );

  funcMap [ scan::cas_::iss_::item_
	    ::max ]
    = &( creationCondition<scan::cas_::iss_::item_
	 ::object> );

  funcMap [ scan::cas_::iss_::item_
	    ::first_min ]
    = &(isScanItem(real_linear_2d));

  funcMap [ scan::cas_::iss_::item_
	    ::first_max ]
    = &(isScanItem(real_linear_2d));

  funcMap [ scan::cas_::iss_::item_
	    ::second_min ]
    = &(isScanItem(real_linear_2d));

  funcMap [ scan::cas_::iss_::item_
	    ::second_max ]
    = &(isScanItem(real_linear_2d));

  funcMap [ scan::cas_::iss_::item_
	    ::min_angle ]
    = &(isScanItem(real_elliptic_2d));

  funcMap [ scan::cas_::iss_::item_
	    ::max_angle ]
    = &(isScanItem(real_elliptic_2d));

  funcMap [ scan::cas_::iss_::item_
	    ::first_axis ]
    = &(isScanItem(real_elliptic_2d));

  funcMap [ scan::cas_::iss_::item_
	    ::second_axis ]
    = &(isScanItem(real_elliptic_2d));

  funcMap [ scan::cas_::iss_::item_
	    ::first_center ]
    = &(isScanItem(real_elliptic_2d));

  funcMap [ scan::cas_::iss_::item_
	    ::second_center ]
    = &(isScanItem(real_elliptic_2d));

  funcMap [ scan::cas_::iss_::item_
	    ::external_data_filename ]
    = &(isScanItem(from_file));

  funcMap [ scan::cas_::iss_::item_
	    ::number_of_columns ]
    = &(isScanItem(from_file));

  funcMap [ scan::cas_::iss_::item_
	    ::subitem ]
    = &(isScanItem(from_file));
    
  ADD_COND_FUNC (scan::cas_::iss_::item_::subitem_
		 ::object );

  ADD_COND_FUNC (scan::cas_::iss_::item_::object);

  ADD_COND_FUNC (scan::cas_::iss_::item_::first_object);

  funcMap [ scan::cas_::iss_::item_
	    ::second_object ]
    = &( creationCondition<scan::cas_::iss_::item_
	 ::first_object> );


  /* ************************************************************ */
  // band counter

  // Method 1
  funcMap ["investigation_methods::band_counter::basis_points"]
    = & (creationCondition<investigation_methods::band_counter::basis_points>);

  // the following fields will be created using the creation condition of the
  // field 'basis_points', because they have the same condition and we do not
  // intend to reimplement it for each of these fields...

  funcMap ["investigation_methods::band_counter::bandcount_compare_precision"]
    = & (creationCondition<investigation_methods::band_counter::basis_points>);

  funcMap ["investigation_methods::band_counter::fast_termination"]
    = & (creationCondition<investigation_methods::band_counter::basis_points>);

  funcMap ["investigation_methods::band_counter::termination_after"]
    = & (creationCondition<investigation_methods::band_counter::basis_points>);

  
  // Method 2
  funcMap ["investigation_methods::band_counter::automatic_domain_detection"]
    = & (creationCondition<investigation_methods::band_counter::number_of_boxes>);
    
  funcMap ["investigation_methods::band_counter::domain_boundaries"]
    = & (creationCondition<investigation_methods::band_counter::number_of_boxes>);
    
  funcMap ["investigation_methods::band_counter::number_of_boxes"]
    = & (creationCondition<investigation_methods::band_counter::number_of_boxes>);

  funcMap ["investigation_methods::band_counter::error_threshold"]
    = & (creationCondition<investigation_methods::band_counter::number_of_boxes>);

  funcMap ["investigation_methods::band_counter::use_hash"]
    = & (creationCondition<investigation_methods::band_counter::number_of_boxes>);
  
  funcMap ["investigation_methods::band_counter::hash_size"]
    = & (creationCondition<investigation_methods::band_counter::number_of_boxes>);


  //

#if ! (OPTION__SIPA_ANALYSIS)
  funcMap [ "investigation_methods::rim_analysis" ]
    = &doNotCreate;
#else

  funcMap ["investigation_methods::rim_analysis::solution_point"]
    = & (createIfSiblingHasEnumValue<rim_type::type,
	 rim_type::solution_point>);
  
#endif /* OPTION__SIPA_ANALYSIS */

#if ! (OPTION__SYMBOLIC_IMAGE_ANALYSIS)
  funcMap [ "investigation_methods::symbolic_image_analysis" ]
    = &doNotCreate;
#else

  funcMap ["investigation_methods::symbolic_image_analysis::covering::point_select::positions"]
    = & (createIfSiblingHasEnumValue<symimage_point_select::type, symimage_point_select::position>);

  funcMap [ "investigation_methods::symbolic_image_analysis::covering::point_select::error_tolerance" ]
    = &doNotCreate;

  funcMap [ "investigation_methods::symbolic_image_analysis::covering::linear_extension" ]
    = &doNotCreate;
    
  funcMap ["investigation_methods::symbolic_image_analysis::covering::output_format[]::box_indices"]
    = & (createIfSiblingHasEnumValue<symimages_output_format::type,
	 symimages_output_format::box_output>);

  funcMap ["investigation_methods::symbolic_image_analysis::covering::output_format[]::box_values"]
    = & (createIfSiblingHasEnumValue<symimages_output_format::type,
	 symimages_output_format::box_output>);

  funcMap ["investigation_methods::symbolic_image_analysis::covering::output_format[]::value_position"]
    = & (createIfSiblingHasEnumValue<symimages_output_format::type,
	 symimages_output_format::box_output>);

  funcMap ["investigation_methods::symbolic_image_analysis::covering::output_format[]::range_indices"]
    = & (createIfSiblingHasEnumValue<symimages_output_format::type,
	 symimages_output_format::range_output>);

  funcMap ["investigation_methods::symbolic_image_analysis::covering::output_format[]::range_values"]
    = & (createIfSiblingHasEnumValue<symimages_output_format::type,
	 symimages_output_format::range_output>);


  funcMap ["investigation_methods::symbolic_image_analysis::image_cells::load_recurrent_set_description[]"]
    = & (createIfSiblingHasEnumValue<symimages_image_cells::period_finder_type,
	 symimages_image_cells::data_file>);

  // Type periodic_cell_writers, all_cell_writers, selected_set_writer, 
  // attraction_domain_writer,inverse_attraction_domain_cell_writer, 
  // cells_on_trajectory_writer
  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::box_output );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::invariant_set );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::periodic_paths );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::period_size );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::cell_edges );

  
  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::only_periods );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::range_output );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::all_larger_sets );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::set_edges );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::number_of_cells );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::only_more_cells_than );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::max_values );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::min_values );

  ADD_COND_FUNC ( investigation_methods::symbolic_image_analysis
		  ::image_cells::output_writer_::interpolated_values );


#endif /* OPTION__SYMBOLIC_IMAGE_ANALYSIS */

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::saving::policy ]
    = &( isSpatialType<FROM_ROOT> );

  ADD_COND_FUNC ( investigation_methods::general_trajectory_evaluations
		  ::saving::points_step );

  ADD_COND_FUNC ( investigation_methods::general_trajectory_evaluations
		  ::saving::state_space_distance );

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::saving::velocity_space_distance ]
    = &( creationCondition<investigation_methods::general_trajectory_evaluations
	 ::saving::state_space_distance> );

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::saving::cobweb ]
    = &( creationCondition<investigation_methods::general_trajectory_evaluations
	 ::saving::points_step> );

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::saving::cobweb_file ]
    = &( creationCondition<investigation_methods::general_trajectory_evaluations
	 ::saving::points_step> );

  /* ************************************************************ */

  ADD_COND_FUNC ( investigation_methods::symbolic_analysis
		  ::critical_points_for_LR_symbolic_dynamics );

  ADD_COND_FUNC ( investigation_methods::lyapunov_exponents_analysis
		  ::p );

#if OPTION__DISABLE_SOME_SYMBOLIC_FILES

  funcMap [ investigation_methods::symbolic_analysis
	    ::symbolic_probability_approximate ]
    = &doNotCreate;
  
  funcMap [ investigation_methods::symbolic_analysis
	    ::symbolic_probability_exact ]
    = &doNotCreate;

  funcMap [ "investigation_methods::symbolic_analysis::symb_prob_a_file" ]
    = &doNotCreate;
  
  funcMap [ "investigation_methods::symbolic_analysis::symb_prob_e_file" ]
    = &doNotCreate;

#endif

  /* ********************************************************************** */

#if 1
  funcMap [ "investigation_methods::lyapunov_exponents_analysis::integration" ]
    = &doNotCreate;
#endif

  /* ********************************************************************** */

  ADD_COND_FUNC (investigation_methods::regions_analysis_);

  /* ********************************************************************** */

  // Dimensions Analysis

    // median_calculation: user_selected_layers
  funcMap [ investigation_methods::dimensions_analysis::min_layer ]
      = & (creationCondition<investigation_methods::dimensions_analysis::min_layer>);

  
#if OPTION__DISABLE_SOME_DIMENSION_FILES
  funcMap [ investigation_methods::dimensions_analysis
	    ::metric_entropy ]
    = &doNotCreate;

  funcMap [ investigation_methods::dimensions_analysis
	    ::capacity_dimension ]
    = &doNotCreate;

  funcMap [ investigation_methods::dimensions_analysis
	    ::information_dimension ]
    = &doNotCreate;

  funcMap [ investigation_methods::dimensions_analysis
	    ::correlation_dimension ]
    = &doNotCreate;


  funcMap [ "investigation_methods::dimensions_analysis::entropy_file" ]
    = &doNotCreate;

  funcMap [ "investigation_methods::dimensions_analysis::cap_dimension_file" ]
    = &doNotCreate;

  funcMap [ "investigation_methods::dimensions_analysis::info_dimension_file" ]
    = &doNotCreate;

  funcMap [ "investigation_methods::dimensions_analysis::corr_dimension_file" ]
    = &doNotCreate;

#endif /* OPTION__DISABLE_SOME_DIMENSION_FILES */

  /* ********************************************************************** */

#if ! (OPTION__FREQUENCY_ANALYSIS)
  funcMap [ "investigation_methods::frequency_analysis" ]
    = &doNotCreate;
#endif /* OPTION__FREQUENCY_ANALYSIS */

#if ! (OPTION__SINGULAR_VALUE_ANALYSIS)
  funcMap [ "investigation_methods::singular_value_analysis" ]
    = &doNotCreate;
#endif /* OPTION__SINGULAR_VALUE_ANALYSIS */

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::time_until_fixed_point_file ]
    = &(isConditionType<ConditionType::any_fixed_point>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::time_until_given_point_file ]
    = &(isConditionType<ConditionType::given_points>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::time_until_rectangle_is_leaved_file ]
    = &(isConditionType<ConditionType::leaves_rectangle_area>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::time_until_circle_is_leaved_file ]
    = &(isConditionType<ConditionType::leaves_circle_area>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::time_until_user_defined_condition_file ]
    = &(isConditionType<ConditionType::user_defined>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::way_until_fixed_point_file ]
    = &(isConditionType<ConditionType::any_fixed_point>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::way_until_given_point_file ]
    = &(isConditionType<ConditionType::given_points>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::way_until_rectangle_is_leaved_file ]
    = &(isConditionType<ConditionType::leaves_rectangle_area>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::way_until_circle_is_leaved_file ]
    = &(isConditionType<ConditionType::leaves_circle_area>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::way_within_rectangle_file ]
    = &(isConditionType<ConditionType::stays_in_rectangle_area>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::way_within_circle_file ]
    = &(isConditionType<ConditionType::stays_in_circle_area>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::way_until_user_defined_condition_file ]
    = &(isConditionType<ConditionType::user_defined>);


  funcMap [ investigation_methods::check_for_conditions::condition
	    ::settings_leading_to_fixed_point_file ]
    = &(isConditionType<ConditionType::any_fixed_point>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::settings_leading_to_given_point_file ]
    = &(isConditionType<ConditionType::given_points>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::settings_leading_from_rectangle_file ]
    = &(isConditionType<ConditionType::leaves_rectangle_area>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::settings_leading_from_circle_file ]
    = &(isConditionType<ConditionType::leaves_circle_area>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::settings_staying_in_rectangle_file ]
    = &(isConditionType<ConditionType::stays_in_rectangle_area>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::settings_staying_in_circle_file ]
    = &(isConditionType<ConditionType::stays_in_circle_area>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::settings_for_user_defined_condition_file ]
    = &(isConditionType<ConditionType::user_defined>);

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::points ]
    = &(isConditionType<ConditionType::given_points>);

  ADD_COND_FUNC ( investigation_methods::check_for_conditions::condition
		  ::ranges );

  ADD_COND_FUNC ( investigation_methods::check_for_conditions::condition
		  ::center );

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::radius ]
    = &( creationCondition< investigation_methods
	 ::check_for_conditions::condition::center > );

  funcMap [ investigation_methods::check_for_conditions::condition
	    ::name ]
    = &(isConditionType<ConditionType::user_defined>);
  /* ************************************************************ */

  funcMap [ "investigation_methods::spatial_evaluations" ]
    = &( isSpatialType<FROM_ROOT> );

  /* ************************************************************ */

#if OPTION__VISUALIZATION
  funcMap [ visualization::window::object::inner_radius ]
    = &( isVisObjectType (torus) );

  funcMap [ visualization::window::object::facing ]
    = &( isVisObjectType (text) );

  funcMap [ visualization::window::object::text ]
    = &( isVisObjectType (text) );

  funcMap [ visualization::window::object::align ]
    = &( isVisObjectType (text) );

  funcMap [ visualization::window::object::font ]
    = &( isVisObjectType (text) );

  funcMap [ visualization::window::object::alpha ]
    = &( isVisObjectType (plane) );

  funcMap [ visualization::window::object::variable ]
    = &( isVisObjectType (text) );

  funcMap [ visualization::window::object::variables ]
    = &( isVisObjectType (curve) );

  funcMap [ visualization::window::object::coloring_method ]
    = &( isVisObjectType (curve) );

  funcMap [ visualization::window::object::plotting_method ]
    = &( isVisObjectType (curve) );

#if 0 /* commented out */
  funcMap [ visualization::window::object:: ]
    = &( isVisObjectType () );
#endif

  ADD_COND_FUNC (visualization::window::object::vector_);

  ADD_COND_FUNC (visualization::window::object::vector2);

  ADD_COND_FUNC (visualization::window::object::lighting);

  ADD_COND_FUNC (visualization::window::object::radius);

  ADD_COND_FUNC (visualization::window::object::scale);

  ADD_COND_FUNC (visualization::window::object::position);

  ADD_COND_FUNC (visualization::window::object::color);

  funcMap [ visualization::window::object::number_of_objects ]
    = &( isVisObjectType (scene) );

  funcMap [ visualization::window::object::object_ ]
    = &( isVisObjectType (scene) );

  ADD_COND_FUNC (visualization::window::axes::z_label);

#else
  funcMap [ "visualization" ]
    = &doNotCreate;
#endif /* OPTION__VISUALIZATION */

#undef ADD_COND_FUNC
} /* setupCreationCondFuncMap */

/* ********************************************************************** */

#if 0 /* commented out */
inline bool
isInvestigationMethodActivator (const AbstractConfigurator* aConfigurator)
{
  assert (isBooleanConfigurator (aConfigurator));

  if (aConfigurator->getSpecKey () != "is_active") {
    return false;
  }

  string keyPath;
  const Node* investigationMethodNode
    = (aConfigurator->getConfig ())
    ->findMatchingParentNode (keyPath, "investigation_methods");

  return (investigationMethodNode != NULL);
}
#endif

/* ********************************************************************** */

void
activate__saving__state_space_distance
( AbstractConfigurator* aConfigurator,
  AbstractContainerConfigurator* parentConfigurator )
{
  /* 'saving' is active, otherwise we were not here... */
  assert ( aConfigurator->getParentConfigurator ()
	   == parentConfigurator );

  bool isSensitive = false;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "is_active" ) {
      if (! getCurrentBoolean (*cIter)) {
	isSensitive = false;
	break;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "trajectory" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "phase_portrait" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }
  }

  aConfigurator->makeSensitive (isSensitive);
}

void
activate__saving__save_only_specific_area
( AbstractConfigurator* aConfigurator,
  AbstractContainerConfigurator* parentConfigurator )
{
  /* 'saving' is active, otherwise we were not here... */
  assert ( aConfigurator->getParentConfigurator ()
	   == parentConfigurator );

  bool isSensitive = false;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "is_active" ) {
      if (! getCurrentBoolean (*cIter)) {
	isSensitive = false;
	break;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "trajectory" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "velocity" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "phase_portrait" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }
  }

  aConfigurator->makeSensitive (isSensitive);
}

void
activate__saving__state_space_saving_area
( AbstractConfigurator* aConfigurator,
  AbstractContainerConfigurator* parentConfigurator )
{
  /* 'saving' is active, otherwise we were not here... */
  assert ( aConfigurator->getParentConfigurator ()
	   == parentConfigurator );

  bool isSensitive = false;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "is_active" ) {
      if (! getCurrentBoolean (*cIter)) {
	isSensitive = false;
	break;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "save_only_specific_area" ) {
      if ( (! getCurrentBoolean (*cIter))
	   || (! GTK_WIDGET_IS_SENSITIVE ((*cIter)->getLhsWidget ())) ) {
	isSensitive = false;
	break;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "trajectory" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "phase_portrait" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }
  }

  aConfigurator->makeSensitive (isSensitive);
}

void
activate__saving__velocity_space_saving_area
( AbstractConfigurator* aConfigurator,
  AbstractContainerConfigurator* parentConfigurator )
{
  /* 'saving' is active, otherwise we were not here... */
  assert ( aConfigurator->getParentConfigurator ()
	   == parentConfigurator );

  bool isSensitive = false;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "is_active" ) {
      if (! getCurrentBoolean (*cIter)) {
	isSensitive = false;
	break;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "save_only_specific_area" ) {
      if ( (! getCurrentBoolean (*cIter))
	   || (! GTK_WIDGET_IS_SENSITIVE ((*cIter)->getLhsWidget ())) ) {
	isSensitive = false;
	break;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "velocity" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "phase_portrait" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }
  }

  aConfigurator->makeSensitive (isSensitive);
}

void
activate__saving__velocity_space_distance
( AbstractConfigurator* aConfigurator,
  AbstractContainerConfigurator* parentConfigurator )
{
  /* 'saving' is active, otherwise we were not here... */
  assert ( aConfigurator->getParentConfigurator ()
	   == parentConfigurator );

  bool isSensitive = false;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "is_active" ) {
      if (! getCurrentBoolean (*cIter)) {
	isSensitive = false;
	break;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "velocity" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "phase_portrait" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }
  }

  aConfigurator->makeSensitive (isSensitive);
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::general_trajectory_evaluations::saving
::trajectory>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "save_only_specific_area" ) {
      activate__saving__save_only_specific_area (*cIter, parentConfigurator);

      update_func_t* updateFunction
	= GuiController<AnTGuiKey>::getUpdateFunc (*cIter);
      assert (updateFunction != NULL);
      updateFunction (*cIter);

      continue;
    }

    if ( (*cIter)->getSpecKey () == "trajectory_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "state_space_distance" ) {
      activate__saving__state_space_distance (*cIter, parentConfigurator);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::general_trajectory_evaluations::saving
::velocity>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "save_only_specific_area" ) {
      activate__saving__save_only_specific_area (*cIter, parentConfigurator);

      update_func_t* updateFunction
	= GuiController<AnTGuiKey>::getUpdateFunc (*cIter);
      assert (updateFunction != NULL);
      updateFunction (*cIter);

      continue;
    }

    if ( (*cIter)->getSpecKey () == "current_velocity_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "velocity_space_distance" ) {
      activate__saving__velocity_space_distance (*cIter, parentConfigurator);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::general_trajectory_evaluations::saving
::phase_portrait>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "save_only_specific_area" ) {
      activate__saving__save_only_specific_area (*cIter, parentConfigurator);

      update_func_t* updateFunction
	= GuiController<AnTGuiKey>::getUpdateFunc (*cIter);
      assert (updateFunction != NULL);
      updateFunction (*cIter);

      continue;
    }

    if ( (*cIter)->getSpecKey () == "phase_portrait_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "state_space_distance" ) {
      activate__saving__state_space_distance (*cIter, parentConfigurator);
      continue;
    }

    if ( (*cIter)->getSpecKey () == "velocity_space_distance" ) {
      activate__saving__velocity_space_distance (*cIter, parentConfigurator);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::general_trajectory_evaluations::saving
::save_only_specific_area>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "state_space_saving_area" ) {
      activate__saving__state_space_saving_area (*cIter, parentConfigurator);
      continue;
    }

    if ( (*cIter)->getSpecKey () == "velocity_space_saving_area" ) {
      activate__saving__velocity_space_saving_area (*cIter, parentConfigurator);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

void
activate__period_analysis__max_period ( AbstractConfigurator* aConfigurator,
					AbstractContainerConfigurator* parentConfigurator )
{
  /* 'period_analysis' is active, otherwise we were not here... */
  assert ( aConfigurator->getParentConfigurator ()
	   == parentConfigurator );

  bool isSensitive = false;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "is_active" ) {
      if (! getCurrentBoolean (*cIter)) {
	isSensitive = false;
	break;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "period" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "cyclic_asymptotic_set" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "cyclic_graphical_iteration" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "period_selections" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }
  }

  gtk_widget_set_sensitive ( aConfigurator->getLhsWidget (),
			     isSensitive );
  gtk_widget_set_sensitive ( aConfigurator->getRhsWidget (),
			     isSensitive );
}

#define activate__period_analysis__compare_precision activate__period_analysis__max_period

/** template specialization */
template<>
void
updateEvent<investigation_methods::period_analysis
::period>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "period_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "max_period" ) {
      activate__period_analysis__max_period (*cIter, parentConfigurator);
      continue;
    }

    if ( (*cIter)->getSpecKey () == "compare_precision" ) {
      activate__period_analysis__compare_precision (*cIter, parentConfigurator);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::period_analysis
::cyclic_asymptotic_set>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "cyclic_bif_dia_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "max_period" ) {
      activate__period_analysis__max_period (*cIter, parentConfigurator);
      continue;
    }

    if ( (*cIter)->getSpecKey () == "compare_precision" ) {
      activate__period_analysis__compare_precision (*cIter, parentConfigurator);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::period_analysis
::cyclic_graphical_iteration>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "cyclic_graph_iter_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "max_period" ) {
      activate__period_analysis__max_period (*cIter, parentConfigurator);
      continue;
    }

    if ( (*cIter)->getSpecKey () == "compare_precision" ) {
      activate__period_analysis__compare_precision (*cIter, parentConfigurator);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::period_analysis
::period_selections>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "periods_to_select" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "period_selection_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "period_selection_file_extension" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "max_period" ) {
      activate__period_analysis__max_period (*cIter, parentConfigurator);
      continue;
    }

    if ( (*cIter)->getSpecKey () == "compare_precision" ) {
      activate__period_analysis__compare_precision (*cIter, parentConfigurator);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

void
activate__period_analysis__using_last_points
( AbstractConfigurator* aConfigurator,
  AbstractContainerConfigurator* parentConfigurator )
{
  /* 'period_analysis' is active, otherwise we were not here... */
  assert ( aConfigurator->getParentConfigurator ()
	   == parentConfigurator );

  bool isSensitive = false;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "is_active" ) {
      if (! getCurrentBoolean (*cIter)) {
	isSensitive = false;
	break;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "acyclic_graphical_iteration" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "acyclic_last_states" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }
  }

  gtk_widget_set_sensitive ( aConfigurator->getLhsWidget (),
			     isSensitive );
  gtk_widget_set_sensitive ( aConfigurator->getRhsWidget (),
			     isSensitive );
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::period_analysis
::acyclic_last_states>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "acyclic_bif_dia_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "using_last_points" ) {
      activate__period_analysis__using_last_points (*cIter, parentConfigurator);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::period_analysis
::acyclic_graphical_iteration>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "acyclic_graph_iter_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "using_last_points" ) {
      activate__period_analysis__using_last_points (*cIter, parentConfigurator);
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/* ********************************************************************** */

#if OPTION__FREQUENCY_ANALYSIS
/** template specialization */
template<>
void
updateEvent<investigation_methods::frequency_analysis
::frequency_step_is_given>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "frequency_step" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "number_of_points" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, ((! isActive) && isSensitive), activationSiblings );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

void
activate__frequency_analysis__frequency_output_range
( AbstractConfigurator* aConfigurator,
  AbstractContainerConfigurator* parentConfigurator )
{
  /* 'frequency_analysis' is active, otherwise we were not here... */
  assert ( aConfigurator->getParentConfigurator ()
	   == parentConfigurator );

  bool isSensitive = false;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "is_active" ) {
      if (! getCurrentBoolean (*cIter)) {
	isSensitive = false;
	break;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "real_part" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "imaginary_part" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "real_and_imaginary_parts" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "power_spectrum" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }
  }

  gtk_widget_set_sensitive ( aConfigurator->getLhsWidget (),
			     isSensitive );
  gtk_widget_set_sensitive ( aConfigurator->getRhsWidget (),
			     isSensitive );
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::frequency_analysis
::real_part>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "re_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "frequency_output_range" ) {
      activate__frequency_analysis__frequency_output_range
	( *cIter, parentConfigurator );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::frequency_analysis
::imaginary_part>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "im_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "frequency_output_range" ) {
      activate__frequency_analysis__frequency_output_range
	( *cIter, parentConfigurator );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::frequency_analysis
::real_and_imaginary_parts>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "re_im_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "frequency_output_range" ) {
      activate__frequency_analysis__frequency_output_range
	( *cIter, parentConfigurator );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::frequency_analysis
::power_spectrum>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "power_sp_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "frequency_output_range" ) {
      activate__frequency_analysis__frequency_output_range
	( *cIter, parentConfigurator );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

void
activate__frequency_analysis__frequency_weighting
( AbstractConfigurator* aConfigurator,
  AbstractContainerConfigurator* parentConfigurator )
{
  /* 'frequency_analysis' is active, otherwise we were not here... */
  assert ( aConfigurator->getParentConfigurator ()
	   == parentConfigurator );

  bool isSensitive = false;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "is_active" ) {
      if (! getCurrentBoolean (*cIter)) {
	isSensitive = false;
	break;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "period" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "spectrum_max_points" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "spectrum_waving" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }

    if ( (*cIter)->getSpecKey () == "fourier_coefficients" ) {
      if (getCurrentBoolean (*cIter)) {
	isSensitive = true;
      }
      continue;
    }
  }

  gtk_widget_set_sensitive ( aConfigurator->getLhsWidget (),
			     isSensitive );
  gtk_widget_set_sensitive ( aConfigurator->getRhsWidget (),
			     isSensitive );
}

#define activate__frequency_analysis__neighborhood_width activate__frequency_analysis__frequency_weighting

/** template specialization */
template<>
void
updateEvent<investigation_methods::frequency_analysis
::period>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "period_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "frequency_weighting" ) {
      activate__frequency_analysis__frequency_weighting
	( *cIter, parentConfigurator );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "neighborhood_width" ) {
      activate__frequency_analysis__neighborhood_width
	( *cIter, parentConfigurator );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::frequency_analysis
::spectrum_max_points>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "spectrum_max_points_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "frequency_weighting" ) {
      activate__frequency_analysis__frequency_weighting
	( *cIter, parentConfigurator );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "neighborhood_width" ) {
      activate__frequency_analysis__neighborhood_width
	( *cIter, parentConfigurator );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::frequency_analysis
::spectrum_waving>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "spectrum_waving_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "frequency_weighting" ) {
      activate__frequency_analysis__frequency_weighting
	( *cIter, parentConfigurator );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "neighborhood_width" ) {
      activate__frequency_analysis__neighborhood_width
	( *cIter, parentConfigurator );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::frequency_analysis
::fourier_coefficients>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "improved_re_im_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "frequency_weighting" ) {
      activate__frequency_analysis__frequency_weighting
	( *cIter, parentConfigurator );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "neighborhood_width" ) {
      activate__frequency_analysis__neighborhood_width
	( *cIter, parentConfigurator );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}
#endif /* OPTION__FREQUENCY_ANALYSIS */

/* ********************************************************************** */

/** template specialization */
template<>
void
updateEvent<investigation_methods::check_for_conditions::condition
::time>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "time_until_fixed_point_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "time_until_given_point_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "time_until_rectangle_is_leaved_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "time_until_circle_is_leaved_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "time_until_user_defined_condition_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::check_for_conditions::condition
::way>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "way_until_fixed_point_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "way_until_given_point_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "way_until_rectangle_is_leaved_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "way_until_circle_is_leaved_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "way_within_rectangle_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "way_within_circle_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "way_until_user_defined_condition_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}

/** template specialization */
template<>
void
updateEvent<investigation_methods::check_for_conditions::condition
::setting>
( AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  bool isSensitive = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ( (*cIter)->getSpecKey () == "settings_leading_to_fixed_point_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "settings_leading_to_given_point_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "settings_leading_from_rectangle_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "settings_leading_from_circle_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "settings_staying_in_rectangle_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "settings_staying_in_circle_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }

    if ( (*cIter)->getSpecKey () == "settings_for_user_defined_condition_file" ) {
      GuiController<AnTGuiKey>::activateBoolDependentSibling
	( *cIter, (isActive && isSensitive), activationSiblings );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<AnTGuiKey>::updateConfigurators (activationSiblings);
  }
}


extern INITIALIZE_KEY (butcher_order);

extern INITIALIZE_KEY (cobweb_file);
extern INITIALIZE_KEY (initial_states_file);

extern INITIALIZE_KEY (mean_value_file);
extern INITIALIZE_KEY (standard_deviation_file);
extern INITIALIZE_KEY (average_velocity_file);
extern INITIALIZE_KEY (correlation_coefficient_file);

// for band counter
extern INITIALIZE_KEY (period_file);
extern INITIALIZE_KEY (termination_after);
extern INITIALIZE_KEY (hash_size);
extern INITIALIZE_KEY (domain_boundaries);

extern INITIALIZE_KEY (symb_entr_a_file);
extern INITIALIZE_KEY (symb_prob_a_file);
extern INITIALIZE_KEY (symb_entr_e_file);
extern INITIALIZE_KEY (symb_prob_e_file);
extern INITIALIZE_KEY (symbolic_sequence_file);
extern INITIALIZE_KEY (periodic_symbolic_sequence_file);

extern INITIALIZE_KEY (epsilon);
extern INITIALIZE_KEY (exponents_file);
extern INITIALIZE_KEY (dimension_file);
extern INITIALIZE_KEY (errors_file);


extern INITIALIZE_KEY (ranges);
extern INITIALIZE_KEY (cap_dimension_file);
extern INITIALIZE_KEY (info_dimension_file);
extern INITIALIZE_KEY (corr_dimension_file);
extern INITIALIZE_KEY (q_dimension_file);
extern INITIALIZE_KEY (q_start);
extern INITIALIZE_KEY (q_end);
extern INITIALIZE_KEY (inv_measure_file);
extern INITIALIZE_KEY (measure_deviation_file);
extern INITIALIZE_KEY (entropy_file);
extern INITIALIZE_KEY (outside_points_file);

extern INITIALIZE_KEY (eval_file);
extern INITIALIZE_KEY (evec_file);
extern INITIALIZE_KEY (cov_matrix_file);

extern INITIALIZE_KEY (auto_corr_file);
extern INITIALIZE_KEY (total_power_file);
extern INITIALIZE_KEY (spectrum_oscillation_file);

extern INITIALIZE_KEY (wave_numbers_file);
extern INITIALIZE_KEY (compare_precision);
extern INITIALIZE_KEY (average_correlation_file);

/** member template specialization */
// static
template<>
void
GuiController<AnTGuiKey>::
setupUpdateFuncMap (map<const string, update_func_t*>& funcMap)
{
#ifdef ADD_UPDATE_FUNC
#error "ADD_UPDATE_FUNC already defined!"
#endif
#define ADD_UPDATE_FUNC(s) funcMap[s]=&(updateEvent<s>)

  ADD_UPDATE_FUNC (scan::cas_::max_backtrack_depth);
  ADD_UPDATE_FUNC (scan::cas_::min_max);

  ADD_UPDATE_FUNC (investigation_methods::general_trajectory_evaluations
		   ::saving::save_only_specific_area);

  /* ************************************************************ */

  funcMap [ dynamical_system::integration
	    ::butcher_order_is_known ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < dynamical_system::integration
	 ::butcher_order_is_known,
	 butcher_order > );

  /* ************************************************************ */

  ADD_UPDATE_FUNC ( investigation_methods::general_trajectory_evaluations
		    ::saving::trajectory );

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::saving::cobweb ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::general_trajectory_evaluations
	 ::saving::cobweb,
	 cobweb_file > );


  ADD_UPDATE_FUNC ( investigation_methods::general_trajectory_evaluations
		    ::saving::velocity );

  ADD_UPDATE_FUNC ( investigation_methods::general_trajectory_evaluations
		    ::saving::phase_portrait );

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::saving::initial_states ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::general_trajectory_evaluations
	 ::saving::initial_states,
	 initial_states_file > );

  /* ************************************************************ */

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::statistics::mean_value ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::general_trajectory_evaluations
	 ::statistics::mean_value,
	 mean_value_file > );

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::statistics::standard_deviation ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::general_trajectory_evaluations
	 ::statistics::standard_deviation,
	 standard_deviation_file > );

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::statistics::average_velocity ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::general_trajectory_evaluations
	 ::statistics::average_velocity,
	 average_velocity_file > );

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::statistics::correlation_coefficient ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::general_trajectory_evaluations
	 ::statistics::correlation_coefficient,
	 correlation_coefficient_file > );

  /* ************************************************************ */

  ADD_UPDATE_FUNC ( investigation_methods::period_analysis
		    ::period );

  ADD_UPDATE_FUNC ( investigation_methods::period_analysis
		    ::cyclic_asymptotic_set );

  ADD_UPDATE_FUNC ( investigation_methods::period_analysis
		    ::acyclic_last_states );

  ADD_UPDATE_FUNC ( investigation_methods::period_analysis
		    ::cyclic_graphical_iteration );

  ADD_UPDATE_FUNC ( investigation_methods::period_analysis
		    ::acyclic_graphical_iteration );

  ADD_UPDATE_FUNC ( investigation_methods::period_analysis
		    ::period_selections );


  /* ************************************************************ */
  
  funcMap [ investigation_methods::band_counter::period ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::band_counter::period, period_file > );

  funcMap [ investigation_methods::band_counter::fast_termination ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
      < investigation_methods::band_counter::fast_termination, termination_after > );

  funcMap [ investigation_methods::band_counter::use_hash ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::band_counter::use_hash, hash_size > );

  funcMap [ investigation_methods::band_counter::automatic_domain_detection ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ deactivateOneDependentSibling
      < investigation_methods::band_counter::automatic_domain_detection, domain_boundaries > );

  /* ************************************************************ */

  funcMap [ investigation_methods::symbolic_analysis
	    ::symbolic_entropy_approximate ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::symbolic_analysis
	 ::symbolic_entropy_approximate,
	 symb_entr_a_file > );

#if OPTION__DISABLE_SOME_SYMBOLIC_FILES

  funcMap [ investigation_methods::symbolic_analysis
	    ::symbolic_probability_approximate ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::symbolic_analysis
	 ::symbolic_probability_approximate,
	 symb_prob_a_file > );
#endif

  funcMap [ investigation_methods::symbolic_analysis
	    ::symbolic_entropy_exact ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::symbolic_analysis
	 ::symbolic_entropy_exact,
	 symb_entr_e_file > );

#if OPTION__DISABLE_SOME_SYMBOLIC_FILES

  funcMap [ investigation_methods::symbolic_analysis
	    ::symbolic_probability_exact ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::symbolic_analysis
	 ::symbolic_probability_exact,
	 symb_prob_e_file > );
#endif

  funcMap [ investigation_methods::symbolic_analysis
	    ::symbolic_sequence ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::symbolic_analysis
	 ::symbolic_sequence,
	 symbolic_sequence_file > );

  funcMap [ investigation_methods::symbolic_analysis
	    ::periodic_symbolic_sequence ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::symbolic_analysis
	 ::periodic_symbolic_sequence,
	 periodic_symbolic_sequence_file > );

  /* ************************************************************ */

#if OPTION__SYMBOLIC_IMAGE_ANALYSIS
  funcMap [ investigation_methods::symbolic_image_analysis
	    ::image_cells::output_writer_::period_size ]
    = &(updateEvent_restructureParentOnToggle);
#endif

  funcMap [ investigation_methods::lyapunov_exponents_analysis
	    ::use_linearized_system ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ deactivateOneDependentSibling
	 < investigation_methods::lyapunov_exponents_analysis
	 ::use_linearized_system,
	 epsilon > );

  funcMap [ investigation_methods::lyapunov_exponents_analysis
	    ::exponents ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::lyapunov_exponents_analysis
	 ::exponents,
	 exponents_file > );

  funcMap [ investigation_methods::lyapunov_exponents_analysis
	    ::dimension ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::lyapunov_exponents_analysis
	 ::dimension,
	 dimension_file > );

  funcMap [ investigation_methods::lyapunov_exponents_analysis
	    ::status ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::lyapunov_exponents_analysis
	 ::status,
	 errors_file > );

  /* ************************************************************ */
  
  // Dimensions Analysis

  
#define ACTIVATE_ON_BOOL(s,t) funcMap [s]=&(GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling<s,t>)
#define DEACTIVATE_ON_BOOL(s,t) funcMap [s]=&(GuiController<AnTGuiKey>::TEMPLATE__ deactivateOneDependentSibling<s,t>)


  DEACTIVATE_ON_BOOL(investigation_methods::dimensions_analysis
	    ::automatic_range_detection,
	      ranges);

  ACTIVATE_ON_BOOL(investigation_methods::dimensions_analysis
	    ::capacity_dimension,
	      cap_dimension_file);

  ACTIVATE_ON_BOOL(investigation_methods::dimensions_analysis
	    ::information_dimension,
	      info_dimension_file);

  ACTIVATE_ON_BOOL(investigation_methods::dimensions_analysis
	    ::information_dimension,
	      info_dimension_file);

  ACTIVATE_ON_BOOL(investigation_methods::dimensions_analysis
	    ::correlation_dimension,
	      corr_dimension_file);



  ACTIVATE_ON_BOOL(investigation_methods::dimensions_analysis
	    ::invariant_measure,
	      inv_measure_file);

  ACTIVATE_ON_BOOL(investigation_methods::dimensions_analysis
	    ::measure_deviation,
	      measure_deviation_file);

  ACTIVATE_ON_BOOL(investigation_methods::dimensions_analysis
                    ::metric_entropy,
                      entropy_file);


  // q-dimension
  funcMap [ investigation_methods::dimensions_analysis
            ::q_dimension ]
      = &( updateEvent<investigation_methods::dimensions_analysis::q_dimension>);  

#if OPTION__DISABLE_SOME_DIMENSION_FILES
  funcMap [ investigation_methods::dimensions_analysis
	    ::metric_entropy ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::dimensions_analysis
	 ::metric_entropy,
	 entropy_file > );

  funcMap [ investigation_methods::dimensions_analysis
	    ::capacity_dimension ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::dimensions_analysis
	 ::capacity_dimension,
	 cap_dimension_file > );

  funcMap [ investigation_methods::dimensions_analysis
	    ::information_dimension ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::dimensions_analysis
	 ::information_dimension,
	 info_dimension_file > );

  funcMap [ investigation_methods::dimensions_analysis
	    ::correlation_dimension ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::dimensions_analysis
	 ::correlation_dimension,
	 corr_dimension_file > );
#endif /* OPTION__DISABLE_SOME_DIMENSION_FILES */

  funcMap [ investigation_methods::dimensions_analysis
	    ::save_outside_points ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::dimensions_analysis
	 ::save_outside_points,
	 outside_points_file > );

  /* ************************************************************ */

#if OPTION__FREQUENCY_ANALYSIS
  ADD_UPDATE_FUNC (investigation_methods::frequency_analysis::frequency_step_is_given);

  ADD_UPDATE_FUNC (investigation_methods::frequency_analysis::real_part );

  ADD_UPDATE_FUNC (investigation_methods::frequency_analysis::imaginary_part );

  ADD_UPDATE_FUNC (investigation_methods::frequency_analysis::real_and_imaginary_parts );

  ADD_UPDATE_FUNC (investigation_methods::frequency_analysis::power_spectrum );

  funcMap [ investigation_methods::frequency_analysis
	    ::autocorrelation ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::frequency_analysis
	 ::autocorrelation,
	 auto_corr_file > );

  funcMap [ investigation_methods::frequency_analysis
	    ::total_power ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::frequency_analysis
	 ::total_power,
	 total_power_file > );

  ADD_UPDATE_FUNC (investigation_methods::frequency_analysis::period );

  funcMap [ investigation_methods::frequency_analysis
	    ::spectrum_oscillation ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::frequency_analysis
	 ::spectrum_oscillation,
	 spectrum_oscillation_file > );

  ADD_UPDATE_FUNC (investigation_methods::frequency_analysis::spectrum_max_points );

  ADD_UPDATE_FUNC (investigation_methods::frequency_analysis::spectrum_waving );

  ADD_UPDATE_FUNC (investigation_methods::frequency_analysis::fourier_coefficients );
#endif /* OPTION__FREQUENCY_ANALYSIS */

  /* ************************************************************ */

#if OPTION__SINGULAR_VALUE_ANALYSIS
  funcMap [ investigation_methods::singular_value_analysis
	    ::eigenvalues ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::singular_value_analysis
	 ::eigenvalues,
	 eval_file > );

  funcMap [ investigation_methods::singular_value_analysis
	    ::eigenvectors ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::singular_value_analysis
	 ::eigenvectors,
	 evec_file > );

  funcMap [ investigation_methods::singular_value_analysis
	    ::covariance_matrix ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::singular_value_analysis
	 ::covariance_matrix,
	 cov_matrix_file > );
#endif /* OPTION__SINGULAR_VALUE_ANALYSIS */

  /* ************************************************************ */

  ADD_UPDATE_FUNC (investigation_methods::check_for_conditions::condition::time );

  ADD_UPDATE_FUNC (investigation_methods::check_for_conditions::condition::way );

  ADD_UPDATE_FUNC (investigation_methods::check_for_conditions::condition::setting );

  /* ************************************************************ */

  funcMap [ investigation_methods::spatial_evaluations::mean_value ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::spatial_evaluations::mean_value,
	 mean_value_file > );

  funcMap [ investigation_methods::spatial_evaluations::standard_deviation ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::spatial_evaluations::standard_deviation,
	 standard_deviation_file > );

  funcMap [ investigation_methods::spatial_evaluations::average_correlation ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::spatial_evaluations::average_correlation,
	 average_correlation_file > );

  funcMap [ investigation_methods::spatial_evaluations::wave_numbers ]
    = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
	 < investigation_methods::spatial_evaluations::wave_numbers,
	 wave_numbers_file > );

  //   funcMap [ investigation_methods::spatial_evaluations::wave_numbers ]
  //     = &( GuiController<AnTGuiKey>::TEMPLATE__ activateOneDependentSibling
  // 	 < investigation_methods::spatial_evaluations::wave_numbers,
  // 	 compare_precision > );

  /* ************************************************************ */

#undef ADD_UPDATE_FUNC
}

/* ********************************************************************** */

SavingStatus
onSaving__RestructureInvestigationMethods
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  SavingStatus result
    = onSaving__CheckDefaults ( aConfigurator,
				errList );

  RecordContainerConfigurator* const investigationMethods
    = dynamic_cast<RecordContainerConfigurator*>
    ( findSubconfigurator ( (aConfigurator->getRootConfigurator ()),
			    "investigation_methods") );
  assert (investigationMethods != NULL);

  if (investigationMethods->hasOpeningWindow ()) {
    investigationMethods->restructureAfterEnumChanging ();
  }

  return result;
}

/** template specialization */
template<>
SavingStatus
onSaving<dynamical_system::number_of_columns>
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  AbstractConfigurator* const stateSpaceDimConfigurator
    = findSibling (aConfigurator, "state_space_dimension");
  assert (stateSpaceDimConfigurator != NULL);

  unsigned int numberOfColumns = 0;
  leafNodeTo ( aConfigurator->getConfig (),
	       numberOfColumns );

  unsigned int stateSpaceDim = 0;
  leafNodeTo ( stateSpaceDimConfigurator->getConfig (),
	       stateSpaceDim );

  if (numberOfColumns < stateSpaceDim) {
    string errorMsg
      = string ("Seems that your setting for\n'")
      + aConfigurator->getLhsText ()
      + "' ("
      + toString (numberOfColumns)
      + ")\nis less than that of\n'"
      + stateSpaceDimConfigurator->getLhsText ()
      + "' ("
      + toString (stateSpaceDim)
      + ").\nUsually, this is a real fault,\n"
      + "so we suggest to fix it...";

    errList.push_back (errorMsg);

    return WARNING;
  }

  return OK;
}

/** template specialization */
template<>
SavingStatus
onSaving<dynamical_system::data_input>
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  /* This provides for an extra level default checking on 'data_input'
     while closing the 'dynamical_system' input widget. This is needed
     in order to perform dimesion checking on
     'data_input::variable[]', which would not be done otherwise: */
  return
    onSaving__CheckDefaults ( aConfigurator,
			      errList );
}

/** template specialization */
template<>
SavingStatus
onSaving<dynamical_system::parameters::parameter_>
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  RecordContainerConfigurator* parentConfigurator
    = dynamic_cast<RecordContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  /* this is almost the same code as in
     'onSaving__SyncDimensionDependentSibling': */
  /* the following configurator is a @readonly leaf configuirator: */
  AbstractConfigurator* const paramSpaceDimConfigurator
    = findSibling ( parentConfigurator,
		    "parameter_space_dimension" );
  assert (paramSpaceDimConfigurator != NULL);

  const AbstractContainerConfigurator* const thisContainer
    = dynamic_cast<AbstractContainerConfigurator*> (aConfigurator);
  assert (thisContainer != NULL);
  setLeafConfigurator ( paramSpaceDimConfigurator,
			toString ((thisContainer->configurators).size ()) );

  const AbstractConfigurator* const
    arraySiblingConfigurator
    = findSibling (aConfigurator, "array");
  if (arraySiblingConfigurator != NULL) {
    assert (parametersMode == DEDUCE);
    parametersMode = INDEXED;

    /* we have to hide before restructuring, because restructuring makes
       'aConfigurator' invalid. This is allowed, as we finally return
       'DONE' (see the comments on SavingStatus): */
    AbstractContainerConfigurator::hide (NULL, aConfigurator);
    parentConfigurator->restructureAfterEnumChanging ();

    parametersMode = DEDUCE; /*: see 'assert' above */

    return DONE;
  }

  return OK;
}

/** template specialization */
template<>
SavingStatus
onSaving<dynamical_system::parameters::array>
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  RecordContainerConfigurator* parentConfigurator
    = dynamic_cast<RecordContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  /* this is almost the same code as in
     'onSaving__SyncDimensionDependentSibling': */
  /* the following configurator is a @readonly leaf configuirator: */
  AbstractConfigurator* const paramSpaceDimConfigurator
    = findSibling ( parentConfigurator,
		    "parameter_space_dimension" );
  assert (paramSpaceDimConfigurator != NULL);

  const EntityList* const
    arrayOfParameters
    = getEntity<EntityList> ( (parentConfigurator->getConfig ())
			      ->getNode ("array") );
  setLeafConfigurator ( paramSpaceDimConfigurator,
			toString ((arrayOfParameters->entities).size ()) );

  const AbstractConfigurator* const
    parameterSiblingConfigurator
    = findSibling (aConfigurator, "parameter[]");
  if (parameterSiblingConfigurator != NULL) {
    assert (parametersMode == DEDUCE);
    parametersMode = ARRAY;

    /* we have to hide before restructuring, because restructuring
       makes 'aConfigurator' invalid. This is allowed, as we finally
       return 'DONE' (see the comments on SavingStatus): */
    AbstractContainerConfigurator::hide
      ( NULL, static_cast<AbstractContainerConfigurator*> (aConfigurator));
    parentConfigurator->restructureAfterEnumChanging ();

    parametersMode = DEDUCE; /*: see 'assert' above */

    return DONE;
  }

  return OK;
}

/** template specialization */
template<>
SavingStatus
onSaving<dynamical_system::parameters_>
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  if ( (findSubconfigurator (aConfigurator, "parameter[]") != NULL)
       && (findSubconfigurator (aConfigurator, "array") != NULL) ) {
    string errorMsg
      = string ("Seems that you have both parameter definitions,\n")
      + "i.e. array and indexed as well.\n"
      + "Please select one of them\n"
      + "and set your parameters properly!";

    errList.push_back (errorMsg);

    return WARNING;
  }

  return
    onSaving__CheckDefaults (aConfigurator, errList);
}

/** template specialization */
template<>
SavingStatus
onSaving<dynamical_system::temporal_initial_function::coefficients>
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  /* this is almost the same code as in
     'onSaving__SyncDimensionDependentSibling': */
  /* the following configurator is a @readonly leaf configuirator: */
  AbstractConfigurator* const degreeConfigurator
    = findSibling (aConfigurator, "degree");
  assert (degreeConfigurator != NULL);

#if 1
  int degree = ( getEntity<EntityList> (aConfigurator->getConfig ())
		 ->entities )
    .size () - 1;
#else
  /* this causes an error if 'coefficients' is rebuild as another
     temporal initial function changes its type: */
  const AbstractContainerConfigurator* const thisContainer
    = dynamic_cast<AbstractContainerConfigurator*> (aConfigurator);
  assert (thisContainer != NULL);
  int degree = (thisContainer->configurators).size () - 1;
#endif /* 1 */

  degree = std::max (degree, 0);

  setLeafConfigurator ( degreeConfigurator,
			toString (degree) );

  return OK;
}

/** template specialization */
template<>
SavingStatus
onSaving<scan::item__>
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  const AbstractContainerConfigurator* const scanItemConfigurator
    = dynamic_cast<AbstractContainerConfigurator*> (aConfigurator);
  assert (scanItemConfigurator != NULL);

  if (scanItemConfigurator->isEmpty ()) {
    const AbstractConfigurator* const scanModeConfigurator
      = findSibling (aConfigurator, "mode");
    assert (scanModeConfigurator != NULL);

    int scanMode = 0;
    leafNodeTo ( scanModeConfigurator->getConfig (),
		 scanMode );

    if (scanMode == 0) {
      return OK;
    }
  }

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "onSaving<scan::item__>, onSaving__CheckDefaults"
       << endl;
#endif

  return
    onSaving__CheckDefaults (aConfigurator, errList);
}

/** template specialization */
template<>
SavingStatus
onSaving<scan::item_::object>
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);
  const Node* const thisConfig
    = aConfigurator->getConfig ();

  string scanObject;
  leafNodeTo (thisConfig, scanObject);

  if (scanObject.empty ()) {
    string errorMsg
      = string ("Seems that the scan object\n'")
      + (thisConfig->getParentNode ())->getKey ()
      + "->" + thisConfig->getKey ()
      + "'\nis not defined! This may cause serious problems,\n"
      + "so we suggest to fix it...";

    errList.push_back (errorMsg);

    return WARNING;
  } else {
    return OK;
  }
}


void
onSaving__printAnTHeader ( const ConfiguratorSaver* aConfiguratorSaver,
			   std::ofstream& outStream,
			   const char* saveFileName )
{
  (MainWindow::AnT_GUI ()).iniFile = saveFileName;
  /// @see runAnTOnSaving

  outStream
    << OUTPUT_COMMENT_CHAR 
    << " Generated by AnT " << ANT_MAJOR << '.' << ANT_MINOR
    << ", Release " << ANT_SUB << ANT_REV
    << " (v " << ANT_SUB << "." << ANT_REV_NUMBER << "." << ANT_PATCH << ")"
    << ", (c) " << ANT_YEARS << ","
    << endl
    << OUTPUT_COMMENT_CHAR 
    << " at ";

  printTimeOfDay (&outStream);

  outStream << "\n\n";
}


template<>
SavingStatus
onSaving<GUI_ROOT_KEY>
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  SavingStatus result
    = onSaving__CheckDefaults (aConfigurator, errList);

#ifdef IN_PLACE_CONFIGURATOR
  gtk_widget_set_sensitive ((MainWindow::AnT_GUI ()).newButton, TRUE);
  gtk_widget_set_sensitive ((MainWindow::AnT_GUI ()).loadButton, TRUE);
#else
  /* Activate the 'buttonsBar': */
  gtk_widget_set_sensitive ((MainWindow::AnT_GUI ()).buttonsBar, TRUE);
#endif

  /* Activate the 'edit' button: */
  gtk_widget_set_sensitive ((MainWindow::AnT_GUI ()).editButton, TRUE);

  if (result == OK) {
    /* Activate the 'run' button: */
    gtk_widget_set_sensitive ((MainWindow::AnT_GUI ()).runButton, TRUE);
  
    static GtkWidget* theSaveFileSelector = NULL;
    if (theSaveFileSelector == NULL) {
      theSaveFileSelector = newFileDialog (CONFIGURATOR_SAVER_TITLE);

      string configFilePattern
	= string ("*.") + ANT_INI_FILE_EXT;
      gtk_file_selection_complete
	( GTK_FILE_SELECTION (theSaveFileSelector),
	  configFilePattern.c_str () );
    }

    static ConfiguratorSaver theConfiguratorSaver
      ( aConfigurator,
	&onSaving__printAnTHeader,
	theSaveFileSelector );

    gtk_widget_show_all (theConfiguratorSaver.saveFileSelector);
  }

  return result;
}


namespace {
  bool savingCovering = false;

  bool& intervalSelectWasVisited ()
  {
    static bool result = false;

    return result;
  }
}

SavingStatus
onSaving__checkCoveringIntervalSelect
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  if (! savingCovering) {
    intervalSelectWasVisited () = true;
    return onSaving__CheckDefaults (aConfigurator, errList);
  } else {
    return OK;
  }
}

SavingStatus
onSaving__checkCoveringSelection
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  savingCovering = true; /* needed by
			    'onSaving__checkCoveringIntervalSelect' */
  SavingStatus result
    = onSaving__CheckDefaults (aConfigurator, errList);
  savingCovering = false; /* reset to normal value */


  const AbstractContainerConfigurator* pointSelection
    = dynamic_cast<AbstractContainerConfigurator*>
    ( findSubconfigurator (aConfigurator, "point_select") );
  if (pointSelection == NULL) {
    /* the investigation method is either turned off or we have an
       appropriate warning due to 'onSaving__CheckDefaults' being
       called before, so: */
    return result;
  }
  assert (pointSelection != NULL);

  IndexedContainerConfigurator* intervalSelection
    = dynamic_cast<IndexedContainerConfigurator*>
    ( findSubconfigurator (aConfigurator, "interval_select[]") );
  assert (intervalSelection != NULL); /* since 'pointSelection' not
					 NULL, see above... */

  if (! intervalSelectWasVisited ()) {
    SavingErrListType tmpErrList;
    SavingStatus intervalSelectionStatus
      = onSaving__CheckDefaults (intervalSelection, tmpErrList);

    if ( (! pointSelection->hasConfig () )
	 || pointSelection->isEmpty () ) {
      if (intervalSelectionStatus == WARNING) {
	string errorMsg
	  = string ("No scan points defined!\nSet either '")
	  + pointSelection->getLhsText ()
	  + "' or '"
	  + intervalSelection->getLhsText ()
	  + "'!";

	errList.push_back (errorMsg);

	result = WARNING;
      }
    } else { /* the user has defined 'point_select' but
		'interval_select[]' is unvisited! */
      if (result == OK) { /* the 'covering' widget will be closed... */
	if (intervalSelectionStatus == WARNING) {
	  /* otherwise there may be a loaded and unvisited
	     configuration, which we are not allowed to delete! */
	  intervalSelection->clearNotebook ();
	  intervalSelection->saveConfig ();
	  SavingStatus removeIntervalSelectStatus = OK;
	  SavingErrListType tmpErrList;
	  removeIntervalSelectStatus
	    = onSaving__RemoveOptionalIfEmpty (intervalSelection, tmpErrList);
	  assert (removeIntervalSelectStatus = OK);
	  assert ( (aConfigurator->getConfig ())
		   ->findNode ("interval_select[0]") == NULL );
#if DEBUG__SEMANTIC_CHECK_CPP
	  cout << "Uninitialized 'interval_select[*]' removed..." << endl;
#endif
	}
      }
    }
  }

  return result;
}


extern INITIALIZE_KEY (state_space_dimension);
extern INITIALIZE_KEY (number_of_differential_operators);
extern INITIALIZE_KEY (degree);
extern INITIALIZE_KEY (number_of_noisy_variables);
extern INITIALIZE_KEY (number_of_pictures);
extern INITIALIZE_KEY (number_of_conditions);

#if OPTION__VISUALIZATION
extern INITIALIZE_KEY (number_of_windows);
extern INITIALIZE_KEY (number_of_objects);
#endif /* OPTION__VISUALIZATION */


/** member template specialization */
// static
template<>
void
GuiController<AnTGuiKey>::
setupSavingFuncMap (map<const string, saving_func_t*>& funcMap)
{
#ifdef ADD_SAVING_FUNC
#error "ADD_SAVING_FUNC already defined!"
#endif
#define ADD_SAVING_FUNC(s) funcMap[s]=&(onSaving<s>)

  ADD_SAVING_FUNC (GUI_ROOT_KEY);

  funcMap [ "dynamical_system" ]
    = &(onSaving__RestructureInvestigationMethods);

  ADD_SAVING_FUNC (dynamical_system::number_of_columns);

  funcMap [ dynamical_system::external_data_filename ]
    = &(onSaving__CheckForEmptyLeaf);

  ADD_SAVING_FUNC (dynamical_system::data_input);

  ADD_SAVING_FUNC (dynamical_system::parameters::parameter_);

  ADD_SAVING_FUNC (dynamical_system::parameters::array);

  ADD_SAVING_FUNC (dynamical_system::parameters_);

  funcMap [ dynamical_system::neumann_boundary_conditions::operator__ ]
    = &( onSaving__SyncDimensionDependentSibling
	 <number_of_differential_operators> );

  ADD_SAVING_FUNC (dynamical_system::temporal_initial_function::coefficients);

  funcMap [ dynamical_system::temporal_initial_function::random_specification ]
    = &(onSaving__CheckDefaults);

  funcMap [ dynamical_system::noisy_variable_ ]
    = &( onSaving__SyncDimensionDependentSibling
	 <number_of_noisy_variables> );

  funcMap [ "scan" ]
    = &(onSaving__RestructureInvestigationMethods);

  ADD_SAVING_FUNC (scan::item__);

  ADD_SAVING_FUNC (scan::item_::object);

  funcMap [ investigation_methods::general_trajectory_evaluations
	    ::pgm_output::picture_ ]
    = &( onSaving__SyncDimensionDependentSibling
	 <number_of_pictures> );

  funcMap [ investigation_methods::check_for_conditions::condition_ ]
    = &( onSaving__SyncDimensionDependentSibling
	 <number_of_conditions> );

#if OPTION__VISUALIZATION
  funcMap [ visualization::window_ ]
    = &( onSaving__SyncDimensionDependentSibling
	 <number_of_windows> );

  funcMap [ visualization::window::object_ ]
    = &( onSaving__SyncDimensionDependentSibling
	 <number_of_objects> );

  /* Without this definition, 'fixed_scene' will reset the
     'number_of_objects' because is defined as '@like = (...,
     object[])' */
  funcMap [ "visualization::window[]::fixed_scene" ]
    = &onSaving__CheckDefaults;
#endif /* OPTION__VISUALIZATION */

#if OPTION__SYMBOLIC_IMAGE_ANALYSIS
  funcMap [ "investigation_methods::symbolic_image_analysis::covering" ]
    = &(onSaving__checkCoveringSelection);

  funcMap [ "investigation_methods::symbolic_image_analysis::covering::interval_select[]" ]
    = &(onSaving__checkCoveringIntervalSelect);
#endif

#undef ADD_SAVING_FUNC
}

/* ********************************************************************** */

/** template specialization */
template<>
NEW_ALLOCATED(AbstractConfigurator*)
  newCustomConfigurator<dynamical_system::neumann_boundary_conditions
::operator_::name>
( AbstractConfigurator* aParentConfigurator,
  const Node* aSpecNode,
  const char* givenType )
{
  NEW_ALLOCATED(CustomSelectorConfigurator*)
    result
    = dynamic_cast<CustomSelectorConfigurator*>
    ( CustomSelectorConfigurator
      ::customCreator (aParentConfigurator, aSpecNode, givenType) );

  (result->selectionCombo).appendLabel ("D<0>");
  (result->selectionCombo).appendLabel ("D<0,0>");
  (result->selectionCombo).appendLabel ("D<0,0,0>");
  (result->selectionCombo).appendLabel ("D<0,0,0,0>");
  (result->selectionCombo).appendLabel ("D<0,0,0,0,0>");

  GtkWidget* indexedItem
    = (result->selectionCombo).appendEmptyIndexed ();
  ComboWidget::appendLabelToIndexedItem
    ( indexedItem, "D<0>(" );
  ComboWidget::appendIndexToIndexedItem
    ( indexedItem, 1, 9 );
  ComboWidget::appendLabelToIndexedItem
    ( indexedItem, ")" );

  return result;
}

/* ********************************************************************** */

Node* getDynSysNode_deepestInside (AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  Node* root
    = (aConfigurator->getRootConfigurator ())
    ->getConfig ();

  Node* result = NULL;
  Node* dynSysParent = root;
  while (dynSysParent != NULL) {
    result = dynSysParent->findNode ("dynamical_system");
    assert (result != NULL);

    dynSysParent = result->findNode ("inside");
  }

  assert (result != NULL);
  return result;
}

class ScanObjectConfigurator
  : public CustomSelectorConfigurator
{
#define OPTION__DELETE_SCAN_OBJECT_ON_CHANGE 1

#if OPTION__DELETE_SCAN_OBJECT_ON_CHANGE
private:
  static
  map<string, string> scanObjToTypeMap;
#endif /* OPTION__DELETE_SCAN_OBJECT_ON_CHANGE */

private:
  ScanObjectConfigurator ( RootConfigurator* aRootConfigurator,
			   const Node* aSpecNode )
    : CustomSelectorConfigurator (aRootConfigurator, aSpecNode)
  {}

public:
  static
  NEW_ALLOCATED(AbstractConfigurator*)
    customCreator ( AbstractConfigurator* aParentConfigurator,
		    const Node* aSpecNode,
		    const char* givenType )
  {
    return
      new ScanObjectConfigurator
      ( aParentConfigurator->getRootConfigurator (), aSpecNode );
  }

protected:
  /** redefine */
  virtual
  void syncWidgetsProtected ( const AbstractConfigurator* originator,
			      SyncMode syncMode )
  {
#if DEBUG__SEMANTIC_CHECK_CPP && 0
    cout << "'ScanObjectConfigurator::syncWidgetsProtected' called..."
	 << endl;
#endif

    selectionCombo.clearSelectionWindow ();

    const AbstractConfigurator* scanItemConfigurator
      = getParentConfigurator ();
    while ( scanItemConfigurator->getSpecKey ()
	    != "item[]" ) {
      scanItemConfigurator
	= scanItemConfigurator->getParentConfigurator ();
    }

    const Node* const scanItemTypeNode
      = (scanItemConfigurator->getConfig ())
      ->findNode ("type");

    string scanItemType;
    leafNodeTo (scanItemTypeNode, scanItemType);

    bool isDiscreteScanItemType
      = (scanItemType == "integer_linear")
      || (scanItemType == "integer_logarithmic");

    const Node* const dynSysNode
      = getDynSysNode_deepestInside (this);
    if (dynSysNode != NULL) {
      integer_t recLevel = 0;
      const Node* const recLevelNode
	= dynSysNode->findNode ("recurrence_level");
      if (recLevelNode != NULL) {
	leafNodeTo (recLevelNode, recLevel);
      }

      bool hasLyapunov = false;
      const Node* const lyapunovActiveNode
	= (getRootConfigurator ()->getConfig ())->followKeyPath
	( "investigation_methods::lyapunov_exponents_analysis::is_active" );
      if (lyapunovActiveNode != NULL) {
	string isActive;
	leafNodeTo (lyapunovActiveNode, isActive);
	hasLyapunov = PreSemanticChecker
	  ::getBoolFromIniString (isActive);
      }

      if (! isDiscreteScanItemType) {
	const Node* const paramSpaceDimNode
	  = dynSysNode->findNode ("parameter_space_dimension");
	if (paramSpaceDimNode != NULL) {


	  integer_t paramSpaceDim = 0;
	  leafNodeTo (paramSpaceDimNode, paramSpaceDim);
	  if (paramSpaceDim > 0) {
	    const Node* const parametersNode
	      = dynSysNode->findNode ("parameters");
	    assert (parametersNode != NULL);
	    if (parametersNode->findNode ("array") == NULL) {
	      for ( int paramIndex = 0;
		    paramIndex < paramSpaceDim;
		    ++paramIndex ) {
		string indexedParamKey
		  = string ("parameter[")
		  + toString (paramIndex)
		  + "]";
		const Node* const indexedParamNode
		  = parametersNode->findNode (indexedParamKey);
		assert (indexedParamNode != NULL);
		string paramName;
		leafNodeTo ( indexedParamNode->findNode ("name"),
			     paramName );
		if (! paramName.empty ()) {
		  selectionCombo.appendLabel (paramName);
		}
	      }
	    }

	    GtkWidget* paramsItem = selectionCombo.appendEmptyIndexed ();
	    ComboWidget
	      ::appendLabelToIndexedItem (paramsItem, "parameter[");
	    ComboWidget
	      ::appendIndexToIndexedItem (paramsItem, 0, (paramSpaceDim - 1));
	    ComboWidget
	      ::appendLabelToIndexedItem (paramsItem, "]");
	  }
	}

	const Node* const additionalParametersNode
	  = (getRootConfigurator ()->getConfig ())->followKeyPath
	  ( "dynamical_system::poincare_section::additional_parameters" );
	if (additionalParametersNode != NULL) {
	  int additionalParamsDim
	    = ( (getEntity<EntityList> (additionalParametersNode))
		->entities ).size ();
	  if (additionalParamsDim > 0) {
	    GtkWidget* additionalParamsItem
	      = selectionCombo.appendEmptyIndexed ();
	    ComboWidget
	      ::appendLabelToIndexedItem
	      ( additionalParamsItem, "additional_parameters[" );
	    ComboWidget
	      ::appendIndexToIndexedItem
	      ( additionalParamsItem, 0, (additionalParamsDim - 1) );
	    ComboWidget
	      ::appendLabelToIndexedItem
	      ( additionalParamsItem, "]" );
	  }
	}

	const Node* const stateSpaceDimNode
	  = dynSysNode->findNode ("state_space_dimension");
	integer_t stateSpaceDim = 0;
	if (stateSpaceDimNode != NULL) {
	  leafNodeTo (stateSpaceDimNode, stateSpaceDim);
	}

	const Node* const initialStateNode
	  = dynSysNode->findNode ("initial_state");
	if (initialStateNode != NULL) {
	  assert (stateSpaceDimNode != NULL);
	  if (stateSpaceDim > 0) {
	    GtkWidget* stateItem = selectionCombo.appendEmptyIndexed ();
	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "initial_state[");

	    if (recLevel > 1) {
	      ComboWidget
		::appendIndexToIndexedItem (stateItem, 0, (recLevel - 1));
	    } else {
	      ComboWidget
		::appendLabelToIndexedItem (stateItem, "0");
	    }

	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "][");	    
	    ComboWidget
	      ::appendIndexToIndexedItem (stateItem, 0, (stateSpaceDim - 1));
	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "]");
	  }
	}

	const Node* const continuousInitialStateNode
	  = dynSysNode->findNode ("continuous_initial_state");
	if (continuousInitialStateNode != NULL) {
	  assert (stateSpaceDimNode != NULL);
	  if (stateSpaceDim > 0) {
	    GtkWidget* stateItem = selectionCombo.appendEmptyIndexed ();
	    ComboWidget
	      ::appendLabelToIndexedItem
	      ( stateItem, "continuous_initial_state[" );

	    if (recLevel > 1) {
	      ComboWidget
		::appendIndexToIndexedItem (stateItem, 0, (recLevel - 1));
	    } else {
	      ComboWidget
		::appendLabelToIndexedItem (stateItem, "0");
	    }

	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "][");	    
	    ComboWidget
	      ::appendIndexToIndexedItem (stateItem, 0, (stateSpaceDim - 1));
	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "]");
	  }
	}

	if (hasLyapunov) {
	  selectionCombo.appendLabel
	    ( "lyapunov_exponents_analysis::epsilon" );
	}
      } else { /* discrete scan objects */
	const Node* const discreteInitialStateNode
	  = dynSysNode->findNode ("discrete_initial_state");

	if (discreteInitialStateNode != NULL) {
	  const Node* const discreteStateSpaceDimNode
	    = dynSysNode->findNode ("discrete_state_space_dimension");
	  assert (discreteStateSpaceDimNode != NULL);
	  integer_t discreteStateSpaceDim = 0;
	  leafNodeTo (discreteStateSpaceDimNode, discreteStateSpaceDim);

	  if (discreteStateSpaceDim > 0) {
	    GtkWidget* stateItem = selectionCombo.appendEmptyIndexed ();
	    ComboWidget
	      ::appendLabelToIndexedItem
	      ( stateItem, "discrete_initial_state[" );

	    if (recLevel > 1) {
	      ComboWidget
		::appendIndexToIndexedItem (stateItem, 0, (recLevel - 1));
	    } else {
	      ComboWidget
		::appendLabelToIndexedItem (stateItem, "0");
	    }

	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "][");	    
	    ComboWidget
	      ::appendIndexToIndexedItem
	      ( stateItem, 0, (discreteStateSpaceDim - 1) );
	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "]");
	  }
	}

	const Node* const dimensionsAnalysisActiveNode
	  = (getRootConfigurator ()->getConfig ())->followKeyPath
	  ( "investigation_methods::dimensions_analysis::is_active" );
	if (dimensionsAnalysisActiveNode != NULL) {
	  string isActive;
	  leafNodeTo (dimensionsAnalysisActiveNode, isActive);
	  if (PreSemanticChecker::getBoolFromIniString (isActive)) {
	    selectionCombo.appendLabel
	      ( "dimensions_analysis::partition_factor" );
	  }
	}

	if (hasLyapunov) {
	  selectionCombo.appendLabel
	    ( "lyapunov_exponents_analysis::steps_between_reorthonormalization" );
	}
      }

      int initFuncIndex = 0;
      const Node* initFuncNode = NULL;
      string initFuncPrefix = "temporal_initial_function";
      initFuncNode = dynSysNode
	->findNode ("temporal_initial_function[0]");
      if (initFuncNode == NULL) {
	initFuncPrefix = "spatial_initial_function";
	initFuncNode = dynSysNode
	  ->findNode ("spatial_initial_function[0]");
      }
      while (initFuncNode != NULL) {
	string initFuncType;
	leafNodeTo ( initFuncNode->findNode ("type"),
		     initFuncType );
	string entryPrefix
	  = initFuncPrefix + "[" + toString (initFuncIndex) + "]"
	  + "::" + initFuncType + "::";

	const Node* const randomSpecificationNode
	  = initFuncNode->findNode ("random_specification");
	string randomType;
	if (randomSpecificationNode != NULL) {
	  leafNodeTo ( randomSpecificationNode->getNode ("type"),
		       randomType );
	  assert (! randomType.empty ());
	}
	string itemKey;
	if (isDiscreteScanItemType) {
#if 0 /* commented out */
	  /*: commented out, because a scan on 'seed' is probably not
	    possible... */
	  if (randomSpecificationNode != NULL) {
	    itemKey = "seed";
	    if (randomSpecificationNode->findNode (itemKey) != NULL) {
	      selectionCombo.appendLabel
		( entryPrefix + randomType + "::" + itemKey );
	    }
	  }
#endif // 0 /* commented out */
	} else { /* real valued scan items: */
	  if (randomSpecificationNode != NULL) {
	    itemKey = "min";
	    if (randomSpecificationNode->findNode (itemKey) != NULL) {
	      selectionCombo.appendLabel
		( entryPrefix + randomType + "::" + itemKey );
	    }

	    itemKey = "max";
	    if (randomSpecificationNode->findNode (itemKey) != NULL) {
	      selectionCombo.appendLabel
		( entryPrefix + randomType + "::" + itemKey );
	    }

	    itemKey = "mean_value";
	    if (randomSpecificationNode->findNode (itemKey) != NULL) {
	      selectionCombo.appendLabel
		( entryPrefix + randomType + "::" + itemKey );
	    }

	    itemKey = "standard_deviation";
	    if (randomSpecificationNode->findNode (itemKey) != NULL) {
	      selectionCombo.appendLabel
		( entryPrefix + randomType + "::" + itemKey );
	    }
	  }

	  itemKey = "amplitude";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "offset";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "period";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "sigma";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "width";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "relative_peak_position";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "argument_shift";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  const Node* const coefficientsNode
	    = initFuncNode->findNode ("coefficients");
	  if (coefficientsNode != NULL) {
	    int degree;
	    leafNodeTo ( initFuncNode->findNode ("degree"),
			 degree );
	    if (degree >= 0) {
	      GtkWidget* coeffItem
		= selectionCombo.appendEmptyIndexed ();
	      ComboWidget
		::appendLabelToIndexedItem
		( coeffItem, entryPrefix + "coefficients[" );
	      ComboWidget
		::appendIndexToIndexedItem
		( coeffItem, 0, degree );
	      ComboWidget
		::appendLabelToIndexedItem
		( coeffItem, "]" );
	    }
	  }
	}

	++initFuncIndex;
	initFuncNode = dynSysNode
	  ->findNode (initFuncPrefix + "[" + toString (initFuncIndex) + "]");
      }
    }

    CustomSelectorConfigurator
      ::syncWidgetsProtected (originator, syncMode);

#if OPTION__DELETE_SCAN_OBJECT_ON_CHANGE
    string currObjName
      = string ( (scanItemConfigurator->getConfig ())->getKey () )
      + "::"
      + getConfig ()->getKey ();
    string oldScanItemType
      = scanObjToTypeMap[currObjName];

#if DEBUG__SEMANTIC_CHECK_CPP && 0
    cout << "'oldScanItemType': "
	 << oldScanItemType
	 << endl;
    cout << "'scanItemType': "
	 << scanItemType
	 << endl;
#endif // DEBUG__SEMANTIC_CHECK_CPP

    if (! oldScanItemType.empty ()) {
      if (oldScanItemType != scanItemType) {
	selectionCombo.setText ("");
      }
    }

    scanObjToTypeMap[currObjName] = scanItemType;
#endif /* OPTION__DELETE_SCAN_OBJECT_ON_CHANGE */
  }
};

#if OPTION__DELETE_SCAN_OBJECT_ON_CHANGE
// static
map<string, string>
ScanObjectConfigurator::scanObjToTypeMap;
#endif /* OPTION__DELETE_SCAN_OBJECT_ON_CHANGE */

class ISScanObjectConfigurator
  : public CustomSelectorConfigurator
{
#if OPTION__DELETE_SCAN_OBJECT_ON_CHANGE
private:
  static
  map<string, string> scanObjToTypeMap;
#endif /* OPTION__DELETE_SCAN_OBJECT_ON_CHANGE */

private:
  ISScanObjectConfigurator ( RootConfigurator* aRootConfigurator,
			   const Node* aSpecNode )
    : CustomSelectorConfigurator (aRootConfigurator, aSpecNode)
  {}

public:
  static
  NEW_ALLOCATED(AbstractConfigurator*)
    customCreator ( AbstractConfigurator* aParentConfigurator,
		    const Node* aSpecNode,
		    const char* givenType )
  {
    return
      new ISScanObjectConfigurator
      ( aParentConfigurator->getRootConfigurator (), aSpecNode );
  }

protected:
  /** redefine */
  virtual
  void syncWidgetsProtected ( const AbstractConfigurator* originator,
			      SyncMode syncMode )
  {
#if DEBUG__SEMANTIC_CHECK_CPP && 0
    cout << "'ISScanObjectConfigurator::syncWidgetsProtected' called..."
	 << endl;
#endif

    selectionCombo.clearSelectionWindow ();

    const AbstractConfigurator* scanItemConfigurator
      = getParentConfigurator ();
    while ( scanItemConfigurator->getSpecKey ()
	    != "item[]" ) {
      scanItemConfigurator
	= scanItemConfigurator->getParentConfigurator ();
    }

    const Node* const scanItemTypeNode
      = (scanItemConfigurator->getConfig ())
      ->findNode ("type");

    string scanItemType;
    leafNodeTo (scanItemTypeNode, scanItemType);

    bool isDiscreteScanItemType
      = (scanItemType == "integer_linear")
      || (scanItemType == "integer_logarithmic");

    const Node* const dynSysNode
      = getDynSysNode_deepestInside (this);
    if (dynSysNode != NULL) {
      integer_t recLevel = 0;
      const Node* const recLevelNode
	= dynSysNode->findNode ("recurrence_level");
      if (recLevelNode != NULL) {
	leafNodeTo (recLevelNode, recLevel);
      }

      if (! isDiscreteScanItemType) {
	const Node* const stateSpaceDimNode
	  = dynSysNode->findNode ("state_space_dimension");
	integer_t stateSpaceDim = 0;
	if (stateSpaceDimNode != NULL) {
	  leafNodeTo (stateSpaceDimNode, stateSpaceDim);
	}

	const Node* const initialStateNode
	  = dynSysNode->findNode ("initial_state");
	if (initialStateNode != NULL) {
	  assert (stateSpaceDimNode != NULL);
	  if (stateSpaceDim > 0) {
	    GtkWidget* stateItem = selectionCombo.appendEmptyIndexed ();
	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "initial_state[");

	    if (recLevel > 1) {
	      ComboWidget
		::appendIndexToIndexedItem (stateItem, 0, (recLevel - 1));
	    } else {
	      ComboWidget
		::appendLabelToIndexedItem (stateItem, "0");
	    }

	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "][");	    
	    ComboWidget
	      ::appendIndexToIndexedItem (stateItem, 0, (stateSpaceDim - 1));
	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "]");
	  }
	}

	const Node* const continuousInitialStateNode
	  = dynSysNode->findNode ("continuous_initial_state");
	if (continuousInitialStateNode != NULL) {
	  assert (stateSpaceDimNode != NULL);
	  if (stateSpaceDim > 0) {
	    GtkWidget* stateItem = selectionCombo.appendEmptyIndexed ();
	    ComboWidget
	      ::appendLabelToIndexedItem
	      ( stateItem, "continuous_initial_state[" );

	    if (recLevel > 1) {
	      ComboWidget
		::appendIndexToIndexedItem (stateItem, 0, (recLevel - 1));
	    } else {
	      ComboWidget
		::appendLabelToIndexedItem (stateItem, "0");
	    }

	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "][");	    
	    ComboWidget
	      ::appendIndexToIndexedItem (stateItem, 0, (stateSpaceDim - 1));
	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "]");
	  }
	}

      } else { /* discrete scan objects */
	const Node* const discreteInitialStateNode
	  = dynSysNode->findNode ("discrete_initial_state");

	if (discreteInitialStateNode != NULL) {
	  const Node* const discreteStateSpaceDimNode
	    = dynSysNode->findNode ("discrete_state_space_dimension");
	  assert (discreteStateSpaceDimNode != NULL);
	  integer_t discreteStateSpaceDim = 0;
	  leafNodeTo (discreteStateSpaceDimNode, discreteStateSpaceDim);

	  if (discreteStateSpaceDim > 0) {
	    GtkWidget* stateItem = selectionCombo.appendEmptyIndexed ();
	    ComboWidget
	      ::appendLabelToIndexedItem
	      ( stateItem, "discrete_initial_state[" );

	    if (recLevel > 1) {
	      ComboWidget
		::appendIndexToIndexedItem (stateItem, 0, (recLevel - 1));
	    } else {
	      ComboWidget
		::appendLabelToIndexedItem (stateItem, "0");
	    }

	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "][");	    
	    ComboWidget
	      ::appendIndexToIndexedItem
	      ( stateItem, 0, (discreteStateSpaceDim - 1) );
	    ComboWidget
	      ::appendLabelToIndexedItem (stateItem, "]");
	  }
	}
      }

      int initFuncIndex = 0;
      const Node* initFuncNode = NULL;
      string initFuncPrefix = "temporal_initial_function";
      initFuncNode = dynSysNode
	->findNode ("temporal_initial_function[0]");
      if (initFuncNode == NULL) {
	initFuncPrefix = "spatial_initial_function";
	initFuncNode = dynSysNode
	  ->findNode ("spatial_initial_function[0]");
      }
      while (initFuncNode != NULL) {
	string initFuncType;
	leafNodeTo ( initFuncNode->findNode ("type"),
		     initFuncType );
	string entryPrefix
	  = initFuncPrefix + "[" + toString (initFuncIndex) + "]"
	  + "::" + initFuncType + "::";

	const Node* const randomSpecificationNode
	  = initFuncNode->findNode ("random_specification");
	string randomType;
	if (randomSpecificationNode != NULL) {
	  leafNodeTo ( randomSpecificationNode->getNode ("type"),
		       randomType );
	  assert (! randomType.empty ());
	}
	string itemKey;
	if (isDiscreteScanItemType) {
#if 0 /* commented out */
	  /*: commented out, because a scan on 'seed' is probably not
	    possible... */
	  if (randomSpecificationNode != NULL) {
	    itemKey = "seed";
	    if (randomSpecificationNode->findNode (itemKey) != NULL) {
	      selectionCombo.appendLabel
		( entryPrefix + randomType + "::" + itemKey );
	    }
	  }
#endif // 0 /* commented out */
	} else { /* real valued scan items: */
	  if (randomSpecificationNode != NULL) {
	    itemKey = "min";
	    if (randomSpecificationNode->findNode (itemKey) != NULL) {
	      selectionCombo.appendLabel
		( entryPrefix + randomType + "::" + itemKey );
	    }

	    itemKey = "max";
	    if (randomSpecificationNode->findNode (itemKey) != NULL) {
	      selectionCombo.appendLabel
		( entryPrefix + randomType + "::" + itemKey );
	    }

	    itemKey = "mean_value";
	    if (randomSpecificationNode->findNode (itemKey) != NULL) {
	      selectionCombo.appendLabel
		( entryPrefix + randomType + "::" + itemKey );
	    }

	    itemKey = "standard_deviation";
	    if (randomSpecificationNode->findNode (itemKey) != NULL) {
	      selectionCombo.appendLabel
		( entryPrefix + randomType + "::" + itemKey );
	    }
	  }

	  itemKey = "amplitude";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "offset";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "period";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "sigma";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "width";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "relative_peak_position";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  itemKey = "argument_shift";
	  if (initFuncNode->findNode (itemKey) != NULL) {
	    selectionCombo.appendLabel
	      ( entryPrefix + itemKey );
	  }

	  const Node* const coefficientsNode
	    = initFuncNode->findNode ("coefficients");
	  if (coefficientsNode != NULL) {
	    int degree;
	    leafNodeTo ( initFuncNode->findNode ("degree"),
			 degree );
	    if (degree >= 0) {
	      GtkWidget* coeffItem
		= selectionCombo.appendEmptyIndexed ();
	      ComboWidget
		::appendLabelToIndexedItem
		( coeffItem, entryPrefix + "coefficients[" );
	      ComboWidget
		::appendIndexToIndexedItem
		( coeffItem, 0, degree );
	      ComboWidget
		::appendLabelToIndexedItem
		( coeffItem, "]" );
	    }
	  }
	}

	++initFuncIndex;
	initFuncNode = dynSysNode
	  ->findNode (initFuncPrefix + "[" + toString (initFuncIndex) + "]");
      }
    }

    CustomSelectorConfigurator
      ::syncWidgetsProtected (originator, syncMode);

#if OPTION__DELETE_SCAN_OBJECT_ON_CHANGE
    string currObjName
      = string ( (scanItemConfigurator->getConfig ())->getKey () )
      + "::"
      + getConfig ()->getKey ();
    string oldScanItemType
      = scanObjToTypeMap[currObjName];

#if DEBUG__SEMANTIC_CHECK_CPP && 0
    cout << "'oldScanItemType': "
	 << oldScanItemType
	 << endl;
    cout << "'scanItemType': "
	 << scanItemType
	 << endl;
#endif // DEBUG__SEMANTIC_CHECK_CPP

    if (! oldScanItemType.empty ()) {
      if (oldScanItemType != scanItemType) {
	selectionCombo.setText ("");
      }
    }

    scanObjToTypeMap[currObjName] = scanItemType;
#endif /* OPTION__DELETE_SCAN_OBJECT_ON_CHANGE */
  }
};

#if OPTION__DELETE_SCAN_OBJECT_ON_CHANGE
// static
map<string, string>
ISScanObjectConfigurator::scanObjToTypeMap;
#endif /* OPTION__DELETE_SCAN_OBJECT_ON_CHANGE */

/* ********************************************************************** */

/** member template specialization */
// static
template<>
void
GuiController<AnTGuiKey>::
setupCustomConfiguratorCreatorFuncMap
( map<const string, custom_creation_func_t*>& funcMap )
{
#ifdef ADD_CREATION_FUNC
#error "ADD_CREATION_FUNC already defined!"
#endif
#define ADD_CREATION_FUNC(s) funcMap[s]=&(newCustomConfigurator<s>)

  funcMap [ dynamical_system::external_data_filename ]
    = &(FileInputConfigurator::customCreator);

  ADD_CREATION_FUNC ( dynamical_system::neumann_boundary_conditions
		      ::operator_::name );

  funcMap [ scan::item_::object ]
    = &(ScanObjectConfigurator::customCreator);

  funcMap [ scan::item_::first_object ]
    = &(ScanObjectConfigurator::customCreator);

  funcMap [ scan::item_::second_object ]
    = &(ScanObjectConfigurator::customCreator);

  funcMap [ scan::cas_::iss_::item_::object ]
    = &(ISScanObjectConfigurator::customCreator);

  funcMap [ scan::cas_::iss_::item_::first_object ]
    = &(ISScanObjectConfigurator::customCreator);

  funcMap [ scan::cas_::iss_::item_::second_object ]
    = &(ISScanObjectConfigurator::customCreator);


  funcMap [ "investigation_methods::symbolic_image_analysis::covering::load_domain_description" ]
    = &(FileInputConfigurator::customCreator);

  funcMap [ "investigation_methods::symbolic_image_analysis::image_cells::load_recurrent_set_description[]::file" ]
    = &(FileInputConfigurator::customCreator);

  funcMap [ "investigation_methods::symbolic_image_analysis::image_cells::load_recurrent_set_description[]::file" ]
    = &(FileInputConfigurator::customCreator);
    
#undef ADD_CREATION_FUNC
}


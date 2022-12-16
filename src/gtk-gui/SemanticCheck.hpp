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
 * $Id: SemanticCheck.hpp,v 1.54 2010/11/11 14:53:16 schanzml Exp $
 *
 */

#ifndef SEMANTIC_CHECK_HPP
#define SEMANTIC_CHECK_HPP

#ifndef OPTION__SYMBOLIC_IMAGE_ANALYSIS
#define OPTION__SYMBOLIC_IMAGE_ANALYSIS 1
#endif

#ifndef OPTION__SIPA_ANALYSIS
#define OPTION__SIPA_ANALYSIS 1
#endif

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#ifndef OPTION__VISUALIZATION
#define OPTION__VISUALIZATION (ANT_HAS_VIS)
#else
#error OPTION__VISUALIZATION is private, use ANT_HAS_VIS instead!
#endif

#ifndef OPTION__FREQUENCY_ANALYSIS
#define OPTION__FREQUENCY_ANALYSIS (ANT_HAS_LIBFFTW)
#else
#error OPTION__FREQUENCY_ANALYSIS is private, use ANT_HAS_LIBFFTW instead!
#endif

#ifndef OPTION__SINGULAR_VALUE_ANALYSIS
#define OPTION__SINGULAR_VALUE_ANALYSIS (ANT_HAS_LIBLAPACK)
#else
#error OPTION__SINGULAR_VALUE_ANALYSIS is private, use ANT_HAS_LIBLAPACK instead!
#endif

#include "GuiController.hpp"

extern const string AnTGuiKey;

/** member template specialization */
// static
template<>
void
GuiController<AnTGuiKey>::
setupDimensionsFuncMap (map<const string, dim_func_t*>& funcMap);

/** member template specialization */
// static
template<>
void
GuiController<AnTGuiKey>::
setupCreationCondFuncMap (map<const string, creation_cond_func_t*>& funcMap);

/** member template specialization */
// static
template<>
void
GuiController<AnTGuiKey>::
setupUpdateFuncMap (map<const string, update_func_t*>& funcMap);

/** member template specialization */
// static
template<>
void
GuiController<AnTGuiKey>::
setupSavingFuncMap (map<const string, saving_func_t*>& funcMap);

/** member template specialization */
// static
template<>
void
GuiController<AnTGuiKey>::
setupCustomConfiguratorCreatorFuncMap
( map<const string, custom_creation_func_t*>& funcMap );

/* ********************************************************************** */

namespace dynamical_system {
  extern const string type;
  extern const string state_space_dimension;
  extern const string discrete_state_space_dimension;
  extern const string delay;
  extern const string maximal_delay;
  extern const string recurrence_level;
  extern const string domain_boundary;
  extern const string number_of_grid_points;
  extern const string neumann_boundary_conditions_;

  namespace neumann_boundary_conditions {
    extern const string operator__;
    namespace operator_ {
      /*: it should be 'operator', but unfortunately this is a
	keyword */
      extern const string name;
      extern const string policy;
      extern const string lower_boundary_value;
      extern const string upper_boundary_value;
    }
  }

  extern const string dirichlet_boundary_conditions;
  extern const string external_data_filename;
  extern const string number_of_columns;
  extern const string action_on_wrong_lines;
  extern const string accept_empty_lines;
  extern const string data_input;
  extern const string number_of_cells;
  extern const string parameter_space_dimension;
  extern const string parameters_;
  extern const string initial_state;
  extern const string continuous_initial_state;
  extern const string discrete_initial_state;
  extern const string reset_initial_states_from_orbit;

  extern const string temporal_initial_function_;
  namespace temporal_initial_function {
    extern const string type;
    extern const string amplitude;
    extern const string offset;
    extern const string period;
    extern const string sigma;
    extern const string width;
    extern const string degree;
    extern const string coefficients;
    extern const string relative_peak_position;
    extern const string argument_shift;
    extern const string random_specification;
  }

  extern const string spatial_initial_function_;
  extern const string integration_;
  namespace integration {
    extern const string step_size;
    extern const string array_name;
    extern const string butcher_order_is_known;
    extern const string butcher_order;
    extern const string nominators_array;
    extern const string denominators_array;
    extern const string backward_threshold;
    extern const string bdf_order;
    extern const string adams_moulton_order;
    extern const string adams_bashforth_order;
    extern const string manhattan_threshold;
    extern const string integration_accuracy;
    extern const string variables_weighting;
  }

  extern const string inside;
  extern const string poincare_section_;
  extern const string number_of_subsystems;
  extern const string number_of_noisy_variables;
  extern const string noisy_variable_;
  namespace noisy_variable {
    extern const string min_;
    extern const string max_;
    extern const string mean_value;
    extern const string standard_deviation;
  }

  namespace parameters {
    extern const string parameter_;
    extern const string array;
  }

  namespace poincare_section {
    extern const string type;
    extern const string variable_index;
    extern const string plane_coefficients;
    extern const string additional_parameters;
  }
}

/* ********************************************************************** */

namespace scan {
  extern const string mode;
  extern const string item__;
  namespace item_ {
    extern const string points;
    extern const string min;
    extern const string max;
    extern const string first_min;
    extern const string first_max;
    extern const string second_min;
    extern const string second_max;
    extern const string min_angle;
    extern const string max_angle;
    extern const string first_axis;
    extern const string second_axis;
    extern const string first_center;
    extern const string second_center;
    extern const string object;
    extern const string first_object;
    extern const string second_object;
    extern const string external_data_filename;
    extern const string number_of_columns;
    extern const string subitem;
    namespace subitem_ {
      extern const string object;
      extern const string column;
    }
  }
  extern const string cas;
  namespace cas_ {
    extern const string iss;
    namespace iss_ {
      extern const string mode;
      extern const string item__;
      namespace item_ {
	extern const string points;
	extern const string min;
	extern const string max;
	extern const string first_min;
	extern const string first_max;
	extern const string second_min;
	extern const string second_max;
	extern const string min_angle;
	extern const string max_angle;
	extern const string first_axis;
	extern const string second_axis;
	extern const string first_center;
	extern const string second_center;
	extern const string object;
	extern const string first_object;
	extern const string second_object;
	extern const string external_data_filename;
	extern const string number_of_columns;
	extern const string subitem;
	namespace subitem_ {
	  extern const string object;
	  extern const string column;
	}
      }
    }
    extern const string steps_between_iss;
    extern const string max_backtrack_depth;
    extern const string backtrack_depth;
    extern const string iss_iterations;
    extern const string transient_steps;

    extern const string reset_from_iss_orbit;
      
    extern const string infinity_threshold;
    extern const string regular_compare_precision;
    extern const string reduced_compare_precision;

    extern const string min_max;

    extern const string max_period;
  }
}

/* ********************************************************************** */

namespace investigation_methods {
  namespace general_trajectory_evaluations {
    namespace saving {
      extern const string policy;
      extern const string points_step;
      extern const string state_space_distance;
      extern const string velocity_space_distance;
      extern const string trajectory;
      extern const string cobweb;
      extern const string cobweb_file;
      extern const string velocity;
      extern const string phase_portrait;
      extern const string initial_states;
      extern const string save_only_specific_area;
      extern const string state_space_saving_area;
      extern const string velocity_space_saving_area;
    }

    namespace pgm_output {
      extern const string picture_;
    }

    namespace statistics {
      extern const string mean_value;
      extern const string standard_deviation;
      extern const string average_velocity;
      extern const string correlation_coefficient;
    }
  }

  namespace period_analysis {
    extern const string period;
    extern const string cyclic_asymptotic_set;
    extern const string acyclic_last_states;
    extern const string cyclic_graphical_iteration;
    extern const string acyclic_graphical_iteration;
    extern const string period_selections;
    extern const string periods_to_select;
  }

  namespace band_counter {
    extern const string basis_points;
    extern const string bandcount_compare_precision;    
    extern const string fast_termination;    
    extern const string termination_after;
    extern const string automatic_domain_detection;
    extern const string domain_boundaries;
    extern const string number_of_boxes;
    extern const string error_threshold;
    extern const string use_hash;
    extern const string hash_size;
    extern const string period;
    extern const string period_file;
  }
  
  namespace symbolic_analysis {
    extern const string critical_points_for_LR_symbolic_dynamics;
    extern const string symbolic_entropy_approximate;
    extern const string symbolic_probability_approximate;
    extern const string symbolic_entropy_exact;
    extern const string symbolic_probability_exact;
    extern const string symbolic_sequence;
    extern const string periodic_symbolic_sequence;
  }

  namespace lyapunov_exponents_analysis {
    extern const string use_linearized_system;
    extern const string p;
    extern const string exponents;
    extern const string dimension;
    extern const string status;
  }

  extern const string regions_analysis_;

  namespace dimensions_analysis {
    extern const string automatic_range_detection;
    extern const string min_layer;
    extern const string ranges;
    extern const string invariant_measure;
    extern const string measure_deviation;
    extern const string metric_entropy;
    extern const string capacity_dimension;
    extern const string information_dimension;
    extern const string correlation_dimension;
    extern const string q_dimension;
    extern const string q_start;
    extern const string q_end;
    extern const string save_outside_points;
  }

#if OPTION__FREQUENCY_ANALYSIS
  namespace frequency_analysis {
    extern const string using_variables;
    extern const string frequency_output_range;
    extern const string neighborhood_width;
    extern const string frequency_weighting;
    extern const string frequency_step_is_given;
    extern const string real_part;
    extern const string imaginary_part;
    extern const string real_and_imaginary_parts;
    extern const string power_spectrum;
    extern const string autocorrelation;
    extern const string total_power;
    extern const string period;
    extern const string spectrum_oscillation;
    extern const string spectrum_max_points;
    extern const string spectrum_waving;
    extern const string fourier_coefficients;
  }
#endif /* OPTION__FREQUENCY_ANALYSIS */

#if OPTION__SINGULAR_VALUE_ANALYSIS
  namespace singular_value_analysis {
    extern const string eigenvalues;
    extern const string eigenvectors;
    extern const string covariance_matrix;
  }
#endif /* OPTION__SINGULAR_VALUE_ANALYSIS */

  namespace check_for_conditions {
    extern const string condition_;
    namespace condition {
      extern const string time;
      extern const string time_until_fixed_point_file;
      extern const string time_until_given_point_file;
      extern const string time_until_rectangle_is_leaved_file;
      extern const string time_until_circle_is_leaved_file;
      extern const string time_until_user_defined_condition_file;
      extern const string way;
      extern const string way_until_fixed_point_file;
      extern const string way_until_given_point_file;
      extern const string way_until_rectangle_is_leaved_file;
      extern const string way_until_circle_is_leaved_file;
      extern const string way_within_rectangle_file;
      extern const string way_within_circle_file;
      extern const string way_until_user_defined_condition_file;
      extern const string setting;
      extern const string settings_leading_to_fixed_point_file;
      extern const string settings_leading_to_given_point_file;
      extern const string settings_leading_from_rectangle_file;
      extern const string settings_leading_from_circle_file;
      extern const string settings_staying_in_rectangle_file;
      extern const string settings_staying_in_circle_file;
      extern const string settings_for_user_defined_condition_file;
      extern const string ranges;
      extern const string center;
      extern const string radius;
      extern const string points;
      extern const string name;
    }
  }

#if OPTION__SYMBOLIC_IMAGE_ANALYSIS
  namespace symbolic_image_analysis {
    namespace covering {
      extern const string uniform_grid_;
      extern const string interval_select_;
      namespace point_select {
	extern const string positions;
      }
    }
    namespace image_cells {
      namespace output_writer_ {
	extern const string box_output;
	extern const string range_output;
	extern const string invariant_set;
	extern const string periodic_paths;
	extern const string period_size;
	extern const string cell_edges;
	extern const string only_periods;
	extern const string only_subdivisions;
	extern const string all_larger_sets;
	extern const string set_edges;
	extern const string number_of_cells;
	extern const string only_more_cells_than;
	extern const string max_values;
	extern const string min_values;
	extern const string interpolated_values;
      }
    }
  }
#endif /* OPTION__SYMBOLIC_IMAGE_ANALYSIS */

  namespace spatial_evaluations {
    extern const string mean_value;
    extern const string standard_deviation;
    extern const string average_correlation;
    extern const string wave_numbers;
    namespace specific_counter {
      extern const string ranges;
    }
  }
}

#if OPTION__VISUALIZATION
namespace visualization {
  extern const string window_;
  namespace window {
    extern const string screen_anchor;
    extern const string object_;
    namespace object {
      extern const string inner_radius;
      extern const string vector_;
      extern const string vector2;
      extern const string facing;
      extern const string alpha;
      extern const string lighting;
      extern const string radius;
      extern const string number_of_objects;
      extern const string object_;
      extern const string text;
      extern const string align;
      extern const string variable;
      extern const string variables;
      extern const string color;
      extern const string scale;
      extern const string coloring_method;
      extern const string plotting_method;
      extern const string font;
      extern const string position;
    }
    namespace axes {
      extern const string stepping;
      extern const string z_label;
    }
  } 
}
#endif /* OPTION__VISUALIZATION */

#endif

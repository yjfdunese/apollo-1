/******************************************************************************
 * Copyright 2019 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file
 **/

#pragma once

#include <memory>
#include <vector>

#include "Eigen/Eigen"
#include "modules/common/configs/proto/vehicle_config.pb.h"
#include "modules/common/math/vec2d.h"
#include "modules/common/vehicle_state/proto/vehicle_state.pb.h"
#include "modules/planning/common/trajectory/discretized_trajectory.h"
#include "modules/planning/open_space/coarse_trajectory_generator/hybrid_a_star.h"
#include "modules/planning/open_space/trajectory_smoother/distance_approach_problem.h"
#include "modules/planning/open_space/trajectory_smoother/dual_variable_warm_start_problem.h"
#include "modules/planning/proto/open_space_trajectory_provider_config.pb.h"

namespace apollo {
namespace planning {
class OpenSpaceTrajectoryOptimizer {
 public:
  OpenSpaceTrajectoryOptimizer(
      const OpenSpaceTrajectoryOptimizerConfig& config);

  virtual ~OpenSpaceTrajectoryOptimizer() = default;

  common::Status Plan(
      const std::vector<common::TrajectoryPoint>& stitching_trajectory,
      const std::vector<double>& end_pose, const std::vector<double>& XYbounds,
      double rotate_angle, const common::math::Vec2d& translate_origin,
      const Eigen::MatrixXi& obstacles_edges_num,
      const Eigen::MatrixXd& obstacles_A, const Eigen::MatrixXd& obstacles_b,
      const std::vector<std::vector<common::math::Vec2d>>&
          obstacles_vertices_vec);

  void GetStitchingTrajectory(
      std::vector<common::TrajectoryPoint>* stitching_trajectory) {
    stitching_trajectory->clear();
    *stitching_trajectory = stitching_trajectory_;
  }

  void GetOptimizedTrajectory(DiscretizedTrajectory* optimized_trajectory) {
    optimized_trajectory->clear();
    *optimized_trajectory = optimized_trajectory_;
  }

  void RecordDebugInfo(const Vec2d& translate_origin,
                       const double& rotate_angle, const Eigen::MatrixXd& xWS,
                       const Eigen::MatrixXd& uWs,
                       const Eigen::MatrixXd& l_warm_up,
                       const Eigen::MatrixXd& n_warm_up,
                       const Eigen::MatrixXd& dual_l_result_ds,
                       const Eigen::MatrixXd& dual_n_result_ds,
                       const Eigen::MatrixXd& state_result_ds,
                       const Eigen::MatrixXd& control_result_ds,
                       const Eigen::MatrixXd& time_result_ds,
                       const std::vector<double>& XYbounds,
                       const std::vector<std::vector<common::math::Vec2d>>&
                           obstacles_vertices_vec);

  void UpdateDebugInfo(
      ::apollo::planning_internal::OpenSpaceDebug* open_space_debug);

 private:
  bool IsInitPointNearDestination(
      const common::TrajectoryPoint& planning_init_point,
      const std::vector<double>& end_pose, double rotate_angle,
      const Vec2d& translate_origin);

  void PathPointNormalizing(double rotate_angle,
                            const common::math::Vec2d& translate_origin,
                            double* x, double* y, double* phi);

  void PathPointDeNormalizing(double rotate_angle,
                              const common::math::Vec2d& translate_origin,
                              double* x, double* y, double* phi);

  void LoadTrajectory(const Eigen::MatrixXd& state_result_ds,
                      const Eigen::MatrixXd& control_result_ds,
                      const Eigen::MatrixXd& time_result_ds);

 private:
  OpenSpaceTrajectoryOptimizerConfig config_;

  std::unique_ptr<HybridAStar> warm_start_;
  std::unique_ptr<DistanceApproachProblem> distance_approach_;
  std::unique_ptr<DualVariableWarmStartProblem> dual_variable_warm_start_;

  std::vector<common::TrajectoryPoint> stitching_trajectory_;
  DiscretizedTrajectory optimized_trajectory_;

  apollo::planning_internal::OpenSpaceDebug open_space_debug_;
};
}  // namespace planning
}  // namespace apollo

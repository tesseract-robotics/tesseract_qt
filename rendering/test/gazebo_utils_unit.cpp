/**
 * @author Joel Kang <joel@tekton-dynamics.com>
 *
 * @copyright Copyright (C) 2026 Tekton Dynamics
 *
 * @par License
 * GNU Lesser General Public License Version 3, 29 June 2007
 * @par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * @par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 */
#include <tesseract/common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <gtest/gtest.h>
#include <gz/common/SubMesh.hh>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_qt/rendering/gazebo_utils.h>
#include <tesseract/geometry/impl/signed_distance_field.h>
#include <tesseract/geometry/impl/signed_distance_field_utils.h>

#include <cmath>

using tesseract::geometry::SignedDistanceField;
using tesseract::gui::SignedDistanceFieldShellBox;

namespace
{
const tesseract::geometry::SignedDistanceFunction SPHERE = [](const Eigen::Vector3d& p) { return p.norm() - 0.5; };

/** @brief A dense sphere field (radius 0.5) on a lattice over [min, max] with @p n samples per axis. */
SignedDistanceField::Ptr denseSphere(const Eigen::Vector3d& min, const Eigen::Vector3d& max, const Eigen::Vector3i& n)
{
  return tesseract::geometry::createDiscreteSignedDistanceField(SPHERE, min, max, n);
}

/** @brief The same, kept as a sparse field. */
SignedDistanceField::Ptr sparseSphere(const Eigen::Vector3d& min, const Eigen::Vector3d& max, const Eigen::Vector3i& n)
{
  return std::make_shared<SignedDistanceField>(
      tesseract::geometry::writeSignedDistanceFieldNVDB(*denseSphere(min, max, n)), 0.0);
}

/** @brief Every box sits within one box size of the sphere's surface. */
void expectOnSurface(const std::vector<SignedDistanceFieldShellBox>& boxes)
{
  ASSERT_FALSE(boxes.empty());
  for (const SignedDistanceFieldShellBox& box : boxes)
    EXPECT_LE(std::abs(SPHERE(box.center)), box.size.maxCoeff() + 1e-9) << box.center.transpose();
}
}  // namespace

TEST(TesseractQtGazeboUtils, SignedDistanceFieldShellDense)  // NOLINT
{
  // 0.05 m lattice; the shell is the surface, not the solid interior.
  const auto sdf =
      denseSphere(Eigen::Vector3d::Constant(-1), Eigen::Vector3d::Constant(1), Eigen::Vector3i::Constant(41));
  const std::vector<SignedDistanceFieldShellBox> boxes = tesseract::gui::signedDistanceFieldShell(*sdf);
  expectOnSurface(boxes);
  for (const SignedDistanceFieldShellBox& box : boxes)
    EXPECT_TRUE(box.size.isApprox(Eigen::Vector3d::Constant(0.05)));
  // About 4 pi r^2 / h^2 per layer, a few layers thick; far fewer than the ~10,000 interior samples.
  EXPECT_LT(boxes.size(), 6000U);
}

TEST(TesseractQtGazeboUtils, SignedDistanceFieldShellSparse)  // NOLINT
{
  // A sparse field has no dense grid to read; it is sampled on its own lattice.
  const auto sdf =
      sparseSphere(Eigen::Vector3d::Constant(-0.7), Eigen::Vector3d::Constant(0.7), Eigen::Vector3i::Constant(29));
  ASSERT_TRUE(sdf->isSparse());
  expectOnSurface(tesseract::gui::signedDistanceFieldShell(*sdf));
}

TEST(TesseractQtGazeboUtils, SignedDistanceFieldShellLevels)  // NOLINT
{
  // A fine level (0.05 m) over x <= 0 only, and a coarse one (0.1 m) over the whole sphere: the coarse
  // level fills in x > 0, and draws nothing where the fine level holds data.
  const auto fine =
      sparseSphere(Eigen::Vector3d(-0.7, -0.7, -0.7), Eigen::Vector3d(0, 0.7, 0.7), Eigen::Vector3i(15, 29, 29));
  const auto coarse =
      sparseSphere(Eigen::Vector3d::Constant(-0.8), Eigen::Vector3d::Constant(0.8), Eigen::Vector3i::Constant(17));
  const SignedDistanceField levels({ fine, coarse });

  const std::vector<SignedDistanceFieldShellBox> boxes = tesseract::gui::signedDistanceFieldShell(levels);
  expectOnSurface(boxes);
  std::size_t fine_boxes{ 0 };
  std::size_t coarse_boxes{ 0 };
  for (const SignedDistanceFieldShellBox& box : boxes)
  {
    if (box.size.isApprox(Eigen::Vector3d::Constant(0.05)))
    {
      ++fine_boxes;
      EXPECT_LE(box.center.x(), 1e-9);
    }
    else
    {
      ++coarse_boxes;
      EXPECT_TRUE(box.size.isApprox(Eigen::Vector3d::Constant(0.1)));
      EXPECT_FALSE(std::isfinite(fine->getDistance(box.center))) << box.center.transpose();
    }
  }
  EXPECT_GT(fine_boxes, 0U);
  EXPECT_GT(coarse_boxes, 0U);
}

TEST(TesseractQtGazeboUtils, SignedDistanceFieldShellBudget)  // NOLINT
{
  // Past the budget the lattice is strided and the boxes grow, still covering the surface.
  const auto sdf =
      denseSphere(Eigen::Vector3d::Constant(-1), Eigen::Vector3d::Constant(1), Eigen::Vector3i::Constant(41));
  const std::size_t all = tesseract::gui::signedDistanceFieldShell(*sdf).size();
  const std::vector<SignedDistanceFieldShellBox> boxes = tesseract::gui::signedDistanceFieldShell(*sdf, all / 4);
  EXPECT_LE(boxes.size(), all / 4);
  expectOnSurface(boxes);
  EXPECT_GT(boxes.front().size.x(), 0.05 + 1e-9);
}

TEST(TesseractQtGazeboUtils, ConvertShellBoxes)  // NOLINT
{
  const std::vector<SignedDistanceFieldShellBox> boxes{ { Eigen::Vector3d(1, 2, 3), Eigen::Vector3d(0.2, 0.4, 0.6) },
                                                        { Eigen::Vector3d::Zero(), Eigen::Vector3d::Ones() } };
  const gz::common::SubMesh mesh = tesseract::gui::convert(boxes);
  ASSERT_EQ(mesh.VertexCount(), 2U * 24U);
  ASSERT_EQ(mesh.NormalCount(), 2U * 24U);
  ASSERT_EQ(mesh.IndexCount(), 2U * 36U);

  for (unsigned int i = 0; i < mesh.IndexCount(); i += 3)
  {
    // Flat faces, wound counter-clockwise seen from outside: each triangle's normal is its vertices'.
    const gz::math::Vector3d a = mesh.Vertex(static_cast<unsigned int>(mesh.Index(i)));
    const gz::math::Vector3d b = mesh.Vertex(static_cast<unsigned int>(mesh.Index(i + 1)));
    const gz::math::Vector3d c = mesh.Vertex(static_cast<unsigned int>(mesh.Index(i + 2)));
    const gz::math::Vector3d n = mesh.Normal(static_cast<unsigned int>(mesh.Index(i)));
    EXPECT_NEAR((b - a).Cross(c - a).Normalized().Dot(n), 1.0, 1e-9);
  }
  // The first box spans its size around its centre.
  gz::math::Vector3d min{ 1e9, 1e9, 1e9 };
  gz::math::Vector3d max{ -1e9, -1e9, -1e9 };
  for (unsigned int i = 0; i < 24; ++i)
  {
    min.Min(mesh.Vertex(i));
    max.Max(mesh.Vertex(i));
  }
  EXPECT_TRUE(min.Equal(gz::math::Vector3d(0.9, 1.8, 2.7), 1e-9));
  EXPECT_TRUE(max.Equal(gz::math::Vector3d(1.1, 2.2, 3.3), 1e-9));
}

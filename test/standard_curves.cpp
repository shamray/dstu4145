#include <gtest/gtest.h>

#include "standard_curves.h"

#include <stdexcept>
#include <tuple>

using namespace testing;

// domain_params rejects a base point that is off the curve or does not have
// order n, so constructing each curve catches transcription errors in the table.
struct standard_curves : TestWithParam<std::tuple<dstu4145::curve_id, int>>
{
};

TEST_P(standard_curves, base_point_has_order_n)
{
    const auto [id, m] = GetParam();

    auto params = dstu4145::standard_curve(id);

    EXPECT_EQ(params.curve.field().m(), m);
}

INSTANTIATE_TEST_SUITE_P(appendix_g, standard_curves, Values(
    std::make_tuple(dstu4145::curve_id::uacurve0, 163),
    std::make_tuple(dstu4145::curve_id::uacurve1, 167),
    std::make_tuple(dstu4145::curve_id::uacurve2, 173),
    std::make_tuple(dstu4145::curve_id::uacurve3, 179),
    std::make_tuple(dstu4145::curve_id::uacurve4, 191),
    std::make_tuple(dstu4145::curve_id::uacurve5, 233),
    std::make_tuple(dstu4145::curve_id::uacurve6, 257),
    std::make_tuple(dstu4145::curve_id::uacurve7, 307),
    std::make_tuple(dstu4145::curve_id::uacurve8, 367),
    std::make_tuple(dstu4145::curve_id::uacurve9, 431)));

TEST(standard_curve, rejects_unknown_id)
{
    EXPECT_THROW(dstu4145::standard_curve(static_cast<dstu4145::curve_id>(10)), std::runtime_error);
}

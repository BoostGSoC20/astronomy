/*=============================================================================
Copyright 2018 Pranam Lashkari <plashkari628@gmail.com>
Copyright 2019 Sarthak Singhal <singhalsarthak2007@gmail.com>
Copyright 2020 Rohit Ranjan    <rohitrjn629@gmail.com>

Distributed under the Boost Software License, Version 1.0. (See accompanying
file License.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#ifndef BOOST_ASTRONOMY_COORDINATE_SPHERICAL_EQUATORIAL_REPRESENTATION_HPP
#define BOOST_ASTRONOMY_COORDINATE_SPHERICAL_EQUATORIAL_REPRESENTATION_HPP

#include <tuple>
#include <cstddef>

#include <boost/geometry/strategies/strategy_transform.hpp>
#include <boost/static_assert.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/algorithms/transform.hpp>
#include <boost/geometry/algorithms/equals.hpp>
#include <boost/units/physical_dimensions/plane_angle.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/get_dimension.hpp>
#include <boost/units/systems/si/dimensionless.hpp>

#include <boost/astronomy/detail/is_base_template_of.hpp>
#include <boost/astronomy/coordinate/rep/base_representation.hpp>
#include <boost/astronomy/coordinate/rep/cartesian_representation.hpp>


namespace boost { namespace astronomy { namespace coordinate {

namespace bu = boost::units;
namespace bg = boost::geometry;


//!Represents the coordinate in spherical equatorial representation
//!Uses three components to represent a point/vector (latitude, longitude, distance)
/*!
\brief Spherical equatorial coordinate system, in degree or in radian
\details This one resembles the geographic coordinate system, and has latitude
    up from zero at the equator, to 90 at the pole
    (opposite to the spherical(polar) coordinate system).
    Used in astronomy and in GIS (but there is also the geographic)

\see http://en.wikipedia.org/wiki/Spherical_coordinates
\ingroup cs
*/
template
<
    typename CoordinateType = double,
    typename LatQuantity = bu::quantity<bu::si::plane_angle, CoordinateType>,
    typename LonQuantity = bu::quantity<bu::si::plane_angle, CoordinateType>,
    typename DistQuantity = bu::quantity<bu::si::dimensionless, CoordinateType>
>
struct spherical_equatorial_representation : public base_representation
    <3, bg::cs::spherical_equatorial<radian>, CoordinateType>
{
    ///@cond INTERNAL
    BOOST_STATIC_ASSERT_MSG(
        ((std::is_same<typename bu::get_dimension<LatQuantity>::type,
            bu::plane_angle_dimension>::value) &&
            (std::is_same<typename bu::get_dimension<LonQuantity>::type,
            bu::plane_angle_dimension>::value)),
        "Latitude and Longitude must be of plane angle type");
    BOOST_STATIC_ASSERT_MSG((std::is_floating_point<CoordinateType>::value),
        "CoordinateType must be a floating-point type");
    ///@endcond

public:
    typedef LatQuantity quantity1;
    typedef LonQuantity quantity2;
    typedef DistQuantity quantity3;

    //default constructor no initialization
    spherical_equatorial_representation(){}

    //!constructs object from provided value of coordinates (latitude, longitude, distance)
    spherical_equatorial_representation
    (
        LatQuantity const& lat,
        LonQuantity const& lon,
        DistQuantity const& distance
    )
    {
        this->set_lat_lon_dist(lat, lon, distance);
    }

    //!constructs object from boost::geometry::model::point object
    template
    <
        std::size_t OtherDimensionCount,
        typename OtherCoordinateSystem,
        typename OtherCoordinateType
    >
    spherical_equatorial_representation
    (
        bg::model::point
        <
            OtherCoordinateType,
            OtherDimensionCount,
            OtherCoordinateSystem
        > const& pointObject
    )
    {
        bg::model::point<OtherCoordinateType, 3, bg::cs::cartesian> temp;
        bg::transform(pointObject, temp);
        bg::transform(temp, this->point);
    }

    //copy constructor
    spherical_equatorial_representation
    (
        spherical_equatorial_representation
        <
            CoordinateType,
            LatQuantity,
            LonQuantity,
            DistQuantity
        > const& object
    )
    {
        this->point = object.get_point();
    }

    //!constructs object from any type of representation
    template <typename Representation>
    spherical_equatorial_representation(Representation const& other)
    {
        BOOST_STATIC_ASSERT_MSG((boost::astronomy::detail::is_base_template_of
            <boost::astronomy::coordinate::base_representation, Representation>::value),
            "No constructor found with given argument type");

        BOOST_STATIC_ASSERT_MSG(
        ((std::is_same<typename bu::get_dimension<DistQuantity>::type,
        typename bu::get_dimension<typename Representation::quantity3>::type>::value)),
        "Two representations must have same dimensions");

        auto tempRep = make_spherical_equatorial_representation(other);
        bg::model::point
        <
            typename std::conditional
            <
                sizeof(CoordinateType) >= sizeof(typename Representation::type),
                CoordinateType,
                typename Representation::type
            >::type,
            3,
            bg::cs::spherical_equatorial<radian>
        > tempPoint;

        bg::set<0>(tempPoint,
            static_cast<
            bu::quantity<bu::si::plane_angle, CoordinateType>
            >(tempRep.get_lat()).value());
        bg::set<1>(tempPoint,
            static_cast<
            bu::quantity<bu::si::plane_angle, CoordinateType>
            >(tempRep.get_lon()).value());
        bg::set<2>(tempPoint,
            static_cast<DistQuantity>(tempRep.get_dist()).value());

        this->point = tempPoint;

    }
    //! returns the (lat, lon, distance) in the form of tuple
    std::tuple<LatQuantity, LonQuantity, DistQuantity> get_lat_lon_dist() const
    {
        return std::make_tuple(this->get_lat(), this->get_lon(), this->get_dist());
    }

    //!returns the lat component of point
    LatQuantity get_lat() const
    {
        return static_cast<LatQuantity>
            (
                bu::quantity<bu::si::plane_angle, CoordinateType>::from_value
                    (bg::get<0>(this->point))
            );
    }

    //!returns the lon component of point
    LonQuantity get_lon() const
    {
        return static_cast<LonQuantity>
            (
                bu::quantity<bu::si::plane_angle, CoordinateType>::from_value
                    (bg::get<1>(this->point))
            );
    }

    //!returns the distance component of point
    DistQuantity get_dist() const
    {
        return DistQuantity::from_value(bg::get<2>(this->point));
    }

    //!set value of (lat, lon, distance) in current object
    void set_lat_lon_dist
    (
        LatQuantity const& lat,
        LonQuantity const& lon,
        DistQuantity const& distance
    )
    {
        this->set_lat(lat);
        this->set_lon(lon);
        this->set_dist(distance);
    }

    //!set value of lat component of point
    void set_lat(LatQuantity const& lat)
    {
        bg::set<0>
            (
            this->point,
            static_cast<bu::quantity<bu::si::plane_angle, CoordinateType>>(lat).value()
            );
    }

    //!set value of lon component of point
    void set_lon(LonQuantity const& lon)
    {
        bg::set<1>
            (
            this->point,
            static_cast<bu::quantity<bu::si::plane_angle, CoordinateType>>(lon).value()
            );
    }

    //!set value of distance component of point
    void set_dist(DistQuantity const& distance)
    {
        bg::set<2>(this->point, distance.value());
    }

    //!operator for addition of representation
    template<typename Addend>
    spherical_equatorial_representation
    <
        CoordinateType,
        LatQuantity,
        LonQuantity,
        DistQuantity
    >
    operator +(Addend const& addend) const
    {

        auto cartesian1 = make_cartesian_representation
            <CoordinateType, DistQuantity, DistQuantity, DistQuantity>(this->point);
        auto cartesian2 = make_cartesian_representation(addend);

        auto temp = cartesian1 + cartesian2;

        spherical_equatorial_representation
        <
            CoordinateType,
            LatQuantity,
            LonQuantity,
            DistQuantity
        > result = make_spherical_equatorial_representation(temp);

        return result;
    }

}; //spherical_equatorial_representation


//!Constructs object from provided quantities
//!Each quantity can have different units but with same datatypes
template
<
    typename CoordinateType,
    template <typename Unit1, typename CoordinateType_> class LatQuantity,
    template <typename Unit2, typename CoordinateType_> class LonQuantity,
    template <typename Unit3, typename CoordinateType_> class DistQuantity,
    typename Unit1,
    typename Unit2,
    typename Unit3
>
spherical_equatorial_representation
<
    CoordinateType,
    LatQuantity<Unit1, CoordinateType>,
    LonQuantity<Unit2, CoordinateType>,
    DistQuantity<Unit3, CoordinateType>
>
make_spherical_equatorial_representation
(
    LatQuantity<Unit1, CoordinateType> const& lat,
    LonQuantity<Unit2, CoordinateType> const& lon,
    DistQuantity<Unit3, CoordinateType> const& dist
)
{
    return spherical_equatorial_representation
        <
            CoordinateType,
            LatQuantity<Unit1, CoordinateType>,
            LonQuantity<Unit2, CoordinateType>,
            DistQuantity<Unit3, CoordinateType>
        >(lat, lon, dist);
}

//!constructs object from provided components of representation with different units
template
<
    typename ReturnCoordinateType,
    typename ReturnLatQuantity,
    typename ReturnLonQuantity,
    typename ReturnDistQuantity,
    typename CoordinateType,
    typename LatQuantity,
    typename LonQuantity,
    typename DistQuantity
>
spherical_equatorial_representation
<
    ReturnCoordinateType,
    ReturnLatQuantity,
    ReturnLonQuantity,
    ReturnDistQuantity
>
make_spherical_equatorial_representation
(
    spherical_equatorial_representation
    <
        CoordinateType,
        LatQuantity,
        LonQuantity,
        DistQuantity
    > const& other
)
{
    return make_spherical_equatorial_representation(
        static_cast<ReturnLatQuantity>(other.get_lat()),
        static_cast<ReturnLonQuantity>(other.get_lon()),
        static_cast<ReturnDistQuantity>(other.get_dist())
    );
}

//!constructs object from provided representation
template
<
    typename CoordinateType,
    typename LatQuantity,
    typename LonQuantity,
    typename DistQuantity
>
spherical_equatorial_representation
<
    CoordinateType,
    LatQuantity,
    LonQuantity,
    DistQuantity
>
make_spherical_equatorial_representation
(
    spherical_equatorial_representation
    <
        CoordinateType,
        LatQuantity,
        LonQuantity,
        DistQuantity
    > const& other
)
{
    return spherical_equatorial_representation
        <
            CoordinateType,
            LatQuantity,
            LonQuantity,
            DistQuantity
        >(other);
}

//!constructs object from boost::geometry::model::point object
template
<
    typename CoordinateType = double,
    typename LatQuantity = bu::quantity<bu::si::plane_angle, CoordinateType>,
    typename LonQuantity = bu::quantity<bu::si::plane_angle, CoordinateType>,
    typename DistQuantity = bu::quantity<bu::si::dimensionless, CoordinateType>,
    std::size_t OtherDimensionCount,
    typename OtherCoordinateSystem,
    typename OtherCoordinateType
>
spherical_equatorial_representation
<
    CoordinateType,
    LatQuantity,
    LonQuantity,
    DistQuantity
>
make_spherical_equatorial_representation
(
    bg::model::point
    <
        OtherCoordinateType,
        OtherDimensionCount,
        OtherCoordinateSystem
    > const& pointObject
)
{
    return spherical_equatorial_representation
        <
            CoordinateType,
            LatQuantity,
            LonQuantity,
            DistQuantity
        >(pointObject);
}

//!constructs object from any type of representation
template
<
    typename OtherRepresentation
>
auto make_spherical_equatorial_representation
(
    OtherRepresentation const& other
)
{
    auto temp = make_cartesian_representation(other);
    typedef decltype(temp) cartesian_type;

    bg::model::point
        <
            typename cartesian_type::type,
            3,
            bg::cs::cartesian
        > tempPoint;

    bg::set<0>(tempPoint, temp.get_x().value());
    bg::set<1>(tempPoint, static_cast<typename cartesian_type::quantity1>
        (temp.get_y()).value());
    bg::set<2>(tempPoint, static_cast<typename cartesian_type::quantity1>
        (temp.get_z()).value());

    bg::model::point<typename cartesian_type::type, 3, bg::cs::spherical_equatorial
        <radian>> result(0.0, 0.0, 0.0);
    bg::transform(tempPoint, result);

    return spherical_equatorial_representation
        <
            typename cartesian_type::type,
            bu::quantity<bu::si::plane_angle, typename cartesian_type::type>,
            bu::quantity<bu::si::plane_angle, typename cartesian_type::type>,
            typename cartesian_type::quantity1
        >(result);
}

}}} //namespace boost::astronomy::coordinate

#endif // !BOOST_ASTRONOMY_COORDINATE_SPHERICAL_EQUATORIAL__REPRESENTATION_HPP


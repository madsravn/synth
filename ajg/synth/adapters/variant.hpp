//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_VARIANT_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_VARIANT_HPP_INCLUDED

#include <boost/variant/variant_fwd.hpp>

#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for boost::variant
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, BOOST_VARIANT_ENUM_PARAMS(class T)>
struct adapter<Behavior, boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> >
    : public base_adapter<Behavior> {

    typedef boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> variant_type;
    AJG_SYNTH_ADAPTER(variant_type)
    adapted_type adapted_;

  public:

    // FIXME: These should be forwarded to the real value.
    // boolean_type equal(base_type const& that) const { return this->template is_equal_as<adapter>(that); }
    number_type  to_number()  const { return adapted_.which(); }
    boolean_type to_boolean() const { return true; }


    // TODO: These rely on T0 ... TN all having these operators available,
    //       which obviously isn't the general case. So it might be better
    //       to switch to using one of our own individual adapters for
    //       this purpose (probably through a static_visitor.)
    void input (istream_type& in)        { AJG_SYNTH_THROW(not_implemented("input")); } // in >> adapted_; }
    void output(ostream_type& out) const { out << adapted_; }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_OPTIONAL_HPP_INCLUDED


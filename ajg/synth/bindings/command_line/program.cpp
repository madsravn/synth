//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <cstdlib>
#include <iostream>

#include <ajg/synth/engines.hpp>
#include <ajg/synth/bindings/command_line/command.hpp>
#include <ajg/synth/bindings/command_line/binding.hpp>
#include <ajg/synth/templates/multi_template.hpp>
#include <ajg/synth/templates/stream_template.hpp>


namespace {

namespace synth = ajg::synth;

typedef synth::command_line::binding<synth::detail::multi_template
    < char
    , synth::stream_template_identity
    , synth::django::engine<>
    , synth::ssi::engine<>
    , synth::tmpl::engine<>
    >
> binding_type;

typedef synth::command_line::command<binding_type> command_type;

} // namespace

namespace boost {
namespace property_tree {

binding_type::stream_type& operator <<( binding_type::stream_type&        output
                                      , binding_type::context_type const& context
                                      ) {
    return output << "[context]";
}

}} // namespace boost::property_tree

int main(int const argc, char const *const argv[])
try {
    command_type::run(argc, argv);
    return EXIT_SUCCESS;
} catch (std::exception const& e) {
    std::cerr << "synth: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
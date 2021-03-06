//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

// TODO? #include <boost/python/detail/wrap_python.hpp>
#include <Python.h>              // This prevents a strange macro issue in pyport.h.
#include <ajg/synth/support.hpp> // Must come ahead of everything except Python.h.

#include <boost/python.hpp>

#include <ajg/synth/bindings/python/binding.hpp>

BOOST_PYTHON_MODULE(synth) {
    namespace s  = ajg::synth;
    namespace py = boost::python;

    typedef s::default_traits<AJG_SYNTH_CONFIG_DEFAULT_CHAR_TYPE>                          traits_type;
    typedef s::bindings::python::binding<traits_type>                               binding_type;

    py::def("version", s::bindings::python::version);

    py::class_<binding_type>("Template", binding_type::constructor_type())
        .def("render_to_file",   &binding_type::render_to_file)
        .def("render_to_path",   &binding_type::render_to_path)
        .def("render_to_string", &binding_type::render_to_string)
    ;
}

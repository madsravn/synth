//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_SSI_BUILTIN_TAGS_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_SSI_BUILTIN_TAGS_HPP_INCLUDED

#include <map>
#include <string>
#include <functional>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <ajg/synth/detail/pipe.hpp>
#include <ajg/synth/detail/transformer.hpp>

namespace ajg {
namespace synth {
namespace engines {
namespace ssi {

template <class Kernel>
struct builtin_tags {
  private:

    typedef Kernel                                                              kernel_type;
    typedef typename kernel_type::id_type                                       id_type;
    typedef typename kernel_type::regex_type                                    regex_type;
    typedef typename kernel_type::match_type                                    match_type;
    typedef typename kernel_type::string_regex_type                             string_regex_type;
    typedef typename kernel_type::string_match_type                             string_match_type;
    typedef typename kernel_type::args_type                                     args_type;
    typedef typename kernel_type::engine_type                                   engine_type;

    typedef typename engine_type::environment_type                              environment_type;
    typedef typename engine_type::context_type                                  context_type;
    typedef typename engine_type::options_type                                  options_type;
    typedef typename engine_type::value_type                                    value_type;
    typedef typename engine_type::traits_type                                   traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::number_type                                   number_type;
    typedef typename traits_type::datetime_type                                 datetime_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::ostream_type                                  ostream_type;

    typedef detail::transformer<string_type>                                    transform;

  public:

    typedef void (*tag_type)(args_type const&);

  private:

    typedef std::map<id_type, tag_type>                                         tags_type;

  public:

    inline void initialize(kernel_type& kernel) {
        kernel.tag
            = add(kernel, config_tag::syntax(kernel),    config_tag::render)
            | add(kernel, echo_tag::syntax(kernel),      echo_tag::render)
            | add(kernel, exec_tag::syntax(kernel),      exec_tag::render)
            | add(kernel, fsize_tag::syntax(kernel),     fsize_tag::render)
            | add(kernel, flastmod_tag::syntax(kernel),  flastmod_tag::render)
            | add(kernel, if_tag::syntax(kernel),        if_tag::render)
            | add(kernel, include_tag::syntax(kernel),   include_tag::render)
            | add(kernel, printenv_tag::syntax(kernel),  printenv_tag::render)
            | add(kernel, set_tag::syntax(kernel),       set_tag::render)
            ;
    }

  private:

    inline regex_type const& add(kernel_type& kernel, regex_type const& regex, tag_type const tag) {
        tags_[regex.regex_id()] = tag;
        return regex;
    }

    tags_type tags_;

  public:

//
// get
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline tag_type get(id_type const id) const {
        typename tags_type::const_iterator it = tags_.find(id);
        return it == tags_.end() ? 0 : it->second;
    }

//
// AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN, AJG_SYNTH_SSI_NO_ATTRIBUTES_IN:
//     Macros to facilitate iterating over and validating tag attributes.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum { interpolated = true, raw = false };

// TODO: Make `name` and `value` arguments.
#define AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN(x, how, if_statement) do { \
    BOOST_FOREACH(match_type const& attr, args.kernel.unnest(x).nested_results()) { \
        std::pair<string_type, string_type> const attribute = args.kernel.parse_attribute(attr, args, how); \
        string_type const name = attribute.first, value = attribute.second; \
        if_statement else AJG_SYNTH_THROW(invalid_attribute(traits_type::narrow(name))); \
    } \
} while (0)

#define AJG_SYNTH_SSI_NO_ATTRIBUTES_IN(x) AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN(x, raw, if (false) {})

//
// validate_attribute
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline static void validate_attribute( char const* const  name
                                         , string_type const& value
                                         , char const* const  a     = 0
                                         , char const* const  b     = 0
                                         , char const* const  c     = 0
                                         ) {
        if ((a == 0 || value != traits_type::literal(a)) &&
            (b == 0 || value != traits_type::literal(b)) &&
            (c == 0 || value != traits_type::literal(c))) {
            AJG_SYNTH_THROW(invalid_attribute(name));
        }
    }

//
// config_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct config_tag {
        static regex_type syntax(kernel_type const& kernel) {
            return kernel.make_tag(traits_type::literal("config"));
        }

        static void render(args_type const& args) {
            AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN(args.match, interpolated,
                if (name == traits_type::literal("sizefmt")) {
                    validate_attribute("sizefmt", value, "bytes", "abbrev");
                    args.options.size_format = value;
                }
                else if (name == traits_type::literal("timefmt")) args.options.time_format = value;
                else if (name == traits_type::literal("echomsg")) args.options.echo_message = value;
                else if (name == traits_type::literal("errmsg"))  args.options.error_message = value;
            );
        }
    };


//
// echo_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct echo_tag {
        static regex_type syntax(kernel_type const& kernel) {
            return kernel.make_tag(traits_type::literal("echo"));
        }

        static void render(args_type const& args) {
            string_type encoding = traits_type::literal("entity");
            AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN(args.match, interpolated,
                if (name == traits_type::literal("var")) {
                    string_type const result = args.kernel.lookup_variable(args.context, args.options, value);
                    if      (encoding == traits_type::literal("none"))   args.ostream << result;
                    else if (encoding == traits_type::literal("url"))    args.ostream << transform::uri_encode(result);
                    else if (encoding == traits_type::literal("entity")) args.ostream << transform::escape_entities(result);
                    else AJG_SYNTH_THROW(invalid_attribute("encoding"));
                }
                else if (name == traits_type::literal("encoding")) {
                    validate_attribute("encoding", value, "none", "url", "entity");
                    encoding = value;
                }
            );
        }
    };

//
// exec_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct exec_tag {
        static regex_type syntax(kernel_type const& kernel) {
            return kernel.make_tag(traits_type::literal("exec"));
        }

        static void render(args_type const& args) {
            AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN(args.match, interpolated,
                if (name == traits_type::literal("cgi")) {
                    // TODO:
                    // BOOST_ASSERT(detail::file_exists(value));
                    AJG_SYNTH_THROW(not_implemented("exec cgi"));
                }
                else if (name == traits_type::literal("cmd")) {
                    detail::pipe pipe(traits_type::narrow(value));
                    pipe.read_into(args.ostream);
                }
            );
        }
    };

//
// flastmod_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct flastmod_tag {
        static regex_type syntax(kernel_type const& kernel) {
            return kernel.make_tag(traits_type::literal("flastmod"));
        }

        static void render(args_type const& args) {
            AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN(args.match, interpolated,
                if (name == traits_type::literal("virtual")) {
                    // TODO: Parse REQUEST_URI and figure our path out.
                    AJG_SYNTH_THROW(not_implemented("fsize virtual"));
                }
                else if (name == traits_type::literal("file")) {
                    std::time_t const stamp = detail::stat_file(traits_type::narrow(value)).st_mtime;
                    args.ostream << traits_type::format_time(args.options.time_format, traits_type::to_time(stamp));
                }
            );
        }
    };

//
// fsize_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct fsize_tag {
        static regex_type syntax(kernel_type const& kernel) {
            return kernel.make_tag(traits_type::literal("fsize"));
        }

        static void render(args_type const& args) {
            boolean_type const abbreviate = args.options.size_format == traits_type::literal("abbrev");
            validate_attribute("size_format", args.options.size_format, "bytes", "abbrev");

            AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN(args.match, interpolated,
                if (name == traits_type::literal("virtual")) {
                    // TODO: Parse REQUEST_URI and figure our path out.
                    AJG_SYNTH_THROW(not_implemented("fsize virtual"));
                }
                else if (name == traits_type::literal("file")) {
                    size_type const size = detail::stat_file(traits_type::narrow(value)).st_size;
                    abbreviate ? args.ostream << traits_type::format_size(size) : args.ostream << size;
                }
            );
        }
    };

//
// if_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct if_tag {
        static regex_type syntax(kernel_type const& kernel) {
            return  (kernel.make_tag(traits_type::literal("if"))   >> kernel.block)
                >> *(kernel.make_tag(traits_type::literal("elif")) >> kernel.block)
                >> !(kernel.make_tag(traits_type::literal("else")) >> kernel.block)
                >>  (kernel.make_tag(traits_type::literal("endif")));
        }

        static void render(args_type const& args) {
            boolean_type condition = false;

            BOOST_FOREACH(match_type const& nested, args.match.nested_results()) {
                if (kernel_type::is(nested, args.kernel.block)) {
                    if (condition) {
                        args.kernel.render_block(args.ostream, nested, args.context, args.options);
                        break;
                    }
                }
                else {
                    condition = evaluate_tag(args, nested);
                }
            }
        }

        inline static boolean_type evaluate_tag(args_type const& args, match_type const& tag) {
            boolean_type has_expr = false, result = false;
            string_match_type match;
            string_type const name = tag[s1].str();

            if (name == traits_type::literal("if") || name == traits_type::literal("elif")) {
                AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN(tag, raw,
                    if (name == traits_type::literal("expr")) {
                        if (!has_expr) has_expr = true;
                        else AJG_SYNTH_THROW(duplicate_attribute("expr"));

                        if (x::regex_match(value, match, args.kernel.expression)) {
                            result = args.kernel.evaluate_expression(args, match);
                        }
                        else {
                            AJG_SYNTH_THROW(invalid_attribute("expr"));
                        }
                    }
                );

                if (has_expr) return result;
                else AJG_SYNTH_THROW(missing_attribute("expr"));
            }
            else {
                AJG_SYNTH_SSI_NO_ATTRIBUTES_IN(tag);
                     if (name == traits_type::literal("else"))  return true;
                else if (name == traits_type::literal("endif")) return false;
                else AJG_SYNTH_THROW(std::logic_error("invalid tag"));
            }
        }
    };

//
// include_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct include_tag {
        static regex_type syntax(kernel_type const& kernel) {
            return kernel.make_tag(traits_type::literal("include"));
        }

        static void render(args_type const& args) {
            AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN(args.match, interpolated,
                if (name == traits_type::literal("virtual")) {
                    // TODO: Parse REQUEST_URI and figure our path out.
                    AJG_SYNTH_THROW(not_implemented("include virtual"));
                }
                else if (name == traits_type::literal("file")) {
                    args.kernel.render_path(args.ostream, traits_type::to_path(value), args.context, args.options);
                }
            );
        }
    };

//
// printenv_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct printenv_tag {
        static regex_type syntax(kernel_type const& kernel) {
            return kernel.make_tag(traits_type::literal("printenv"));
        }

        static void render(args_type const& args) {
            AJG_SYNTH_SSI_NO_ATTRIBUTES_IN(args.match);
            BOOST_FOREACH(typename environment_type::value_type const& nv, args.kernel.environment) {
                args.ostream << traits_type::widen(nv.first) << '=' << traits_type::widen(nv.second) << std::endl;
            }
        }
    };

//
// set_tag
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct set_tag {
        static regex_type syntax(kernel_type const& kernel) {
            return kernel.make_tag(traits_type::literal("set"));
        }

        static void render(args_type const& args) {
            optional<string_type> name_;
            optional<value_type>  value_;

            AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN(args.match, interpolated,
                if (name == traits_type::literal("var")) {
                    if (name_) AJG_SYNTH_THROW(duplicate_attribute("name"));
                    else name_ = value;
                }
                else if (name == traits_type::literal("value")) {
                    if (value_) AJG_SYNTH_THROW(duplicate_attribute("value"));
                    else value_ = value;
                }
            );

            if (!name_)  AJG_SYNTH_THROW(missing_attribute("name"));
            if (!value_) AJG_SYNTH_THROW(missing_attribute("value"));

            args.context[*name_] = *value_;
        }
    };

#undef AJG_SYNTH_SSI_FOREACH_ATTRIBUTE_IN
#undef AJG_SYNTH_SSI_NO_ATTRIBUTES_IN

}; // builtin_tags

}}}} // namespace ajg::synth::engines::ssi

#endif // AJG_SYNTH_ENGINES_SSI_BUILTIN_TAGS_HPP_INCLUDED


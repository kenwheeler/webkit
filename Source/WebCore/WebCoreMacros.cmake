# Helper macro for using all-in-one builds
# This macro removes the sources included in the _all_in_one_file from the input _file_list.
# _file_list is a list of source files
# _all_in_one_file is an all-in-one cpp file includes other cpp files
# _result_file_list is the output file list
macro(PROCESS_ALLINONE_FILE _file_list _all_in_one_file _result_file_list _no_compile)
    file(STRINGS ${_all_in_one_file} _all_in_one_file_content)
    set(${_result_file_list} ${_file_list})
    set(_allins "")
    foreach (_line ${_all_in_one_file_content})
        string(REGEX MATCH "^#include [\"<](.*)[\">]" _found ${_line})
        if (_found)
            list(APPEND _allins ${CMAKE_MATCH_1})
        endif ()
    endforeach ()

    foreach (_allin ${_allins})
        if (${_no_compile})
            # For DerivedSources.cpp, we still need the derived sources to be generated, but we do not want them to be compiled
            # individually. We add the header to the result file list so that CMake knows to keep generating the files.
            string(REGEX REPLACE "(.*)\\.cpp" "\\1" _allin_no_ext ${_allin})
            string(REGEX REPLACE ";([^;]*/)${_allin_no_ext}\\.cpp;" ";\\1${_allin_no_ext}.h;" _new_result "${${_result_file_list}};")
        else ()
            string(REGEX REPLACE ";[^;]*/${_allin};" ";" _new_result "${${_result_file_list}};")
        endif ()
        set(${_result_file_list} ${_new_result})
    endforeach ()

endmacro()


macro(MAKE_HASH_TOOLS _source)
    get_filename_component(_name ${_source} NAME_WE)

    if (${_source} STREQUAL "DocTypeStrings")
        set(_hash_tools_h "${DERIVED_SOURCES_WEBCORE_DIR}/HashTools.h")
    else ()
        set(_hash_tools_h "")
    endif ()

    add_custom_command(
        OUTPUT ${DERIVED_SOURCES_WEBCORE_DIR}/${_name}.cpp ${_hash_tools_h}
        MAIN_DEPENDENCY ${_source}.gperf
        COMMAND ${PERL_EXECUTABLE} ${WEBCORE_DIR}/make-hash-tools.pl ${DERIVED_SOURCES_WEBCORE_DIR} ${_source}.gperf ${GPERF_EXECUTABLE}
        VERBATIM)

    unset(_name)
    unset(_hash_tools_h)
endmacro()


# Append the given dependencies to the source file
# This one consider the given dependencies are in ${DERIVED_SOURCES_WEBCORE_DIR}
# and prepends this to every member of dependencies list
macro(ADD_SOURCE_WEBCORE_DERIVED_DEPENDENCIES _source _deps)
    set(_tmp "")
    foreach (f ${_deps})
        list(APPEND _tmp "${DERIVED_SOURCES_WEBCORE_DIR}/${f}")
    endforeach ()

    WEBKIT_ADD_SOURCE_DEPENDENCIES(${_source} ${_tmp})
    unset(_tmp)
endmacro()


macro(MAKE_JS_FILE_ARRAYS _output_cpp _output_h _namespace _scripts _scripts_dependencies)
    add_custom_command(
        OUTPUT ${_output_h} ${_output_cpp}
        DEPENDS ${JavaScriptCore_SCRIPTS_DIR}/make-js-file-arrays.py ${${_scripts}}
        COMMAND ${PYTHON_EXECUTABLE} ${JavaScriptCore_SCRIPTS_DIR}/make-js-file-arrays.py -n ${_namespace} ${_output_h} ${_output_cpp} ${${_scripts}}
        VERBATIM)
    WEBKIT_ADD_SOURCE_DEPENDENCIES(${${_scripts_dependencies}} ${_output_h} ${_output_cpp})
endmacro()


option(SHOW_BINDINGS_GENERATION_PROGRESS "Show progress of generating bindings" OFF)

# Helper macro which wraps generate-bindings-all.pl script.
#   target is a new target name to be added
#   OUTPUT_SOURCE is a list name which will contain generated sources.(eg. WebCore_SOURCES)
#   INPUT_FILES are IDL files to generate.
#   BASE_DIR is base directory where script is called.
#   IDL_INCLUDES is value of --include argument. (eg. ${WEBCORE_DIR}/bindings/js)
#   FEATURES is a value of --defines argument.
#   DESTINATION is a value of --outputDir argument.
#   GENERATOR is a value of --generator argument.
#   SUPPLEMENTAL_DEPFILE is a value of --supplementalDependencyFile. (optional)
#   PP_EXTRA_OUTPUT is extra outputs of preprocess-idls.pl. (optional)
#   PP_EXTRA_ARGS is extra arguments for preprocess-idls.pl. (optional)
function(GENERATE_BINDINGS target)
    set(options)
    set(oneValueArgs OUTPUT_SOURCE BASE_DIR FEATURES DESTINATION GENERATOR SUPPLEMENTAL_DEPFILE)
    set(multiValueArgs INPUT_FILES IDL_INCLUDES PP_EXTRA_OUTPUT PP_EXTRA_ARGS)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(binding_generator ${WEBCORE_DIR}/bindings/scripts/generate-bindings-all.pl)
    set(idl_attributes_file ${WEBCORE_DIR}/bindings/scripts/IDLAttributes.json)
    set(idl_files_list ${CMAKE_CURRENT_BINARY_DIR}/idl_files_${target}.tmp)
    set(_supplemental_dependency)

    set(content)
    foreach (f ${arg_INPUT_FILES})
        if (NOT IS_ABSOLUTE ${f})
            set(f ${CMAKE_CURRENT_SOURCE_DIR}/${f})
        endif ()
        set(content "${content}${f}\n")
    endforeach ()
    file(WRITE ${idl_files_list} ${content})

    set(args
        --defines ${arg_FEATURES}
        --generator ${arg_GENERATOR}
        --outputDir ${arg_DESTINATION}
        --idlFilesList ${idl_files_list}
        --preprocessor "${CODE_GENERATOR_PREPROCESSOR}"
        --idlAttributesFile ${idl_attributes_file}
    )
    if (arg_SUPPLEMENTAL_DEPFILE)
        list(APPEND args --supplementalDependencyFile ${arg_SUPPLEMENTAL_DEPFILE})
    endif ()
    ProcessorCount(PROCESSOR_COUNT)
    if (PROCESSOR_COUNT)
        list(APPEND args --numOfJobs ${PROCESSOR_COUNT})
    endif ()
    foreach (i IN LISTS arg_IDL_INCLUDES)
        if (IS_ABSOLUTE ${i})
            list(APPEND args --include ${i})
        else ()
            list(APPEND args --include ${CMAKE_CURRENT_SOURCE_DIR}/${i})
        endif ()
    endforeach ()
    foreach (i IN LISTS arg_PP_EXTRA_OUTPUT)
        list(APPEND args --ppExtraOutput ${i})
    endforeach ()
    foreach (i IN LISTS arg_PP_EXTRA_ARGS)
        list(APPEND args --ppExtraArgs ${i})
    endforeach ()

    set(common_generator_dependencies
        ${WEBCORE_DIR}/bindings/scripts/generate-bindings.pl
        ${SCRIPTS_BINDINGS}
        # Changing enabled features should trigger recompiling all IDL files
        # because some of them use #if.
        ${CMAKE_BINARY_DIR}/cmakeconfig.h
    )
    if (EXISTS ${WEBCORE_DIR}/bindings/scripts/CodeGenerator${arg_GENERATOR}.pm)
        list(APPEND common_generator_dependencies ${WEBCORE_DIR}/bindings/scripts/CodeGenerator${arg_GENERATOR}.pm)
    endif ()
    if (EXISTS ${arg_BASE_DIR}/CodeGenerator${arg_GENERATOR}.pm)
        list(APPEND common_generator_dependencies ${arg_BASE_DIR}/CodeGenerator${arg_GENERATOR}.pm)
    endif ()
    foreach (i IN LISTS common_generator_dependencies)
        list(APPEND args --generatorDependency ${i})
    endforeach ()

    set(gen_sources)
    set(gen_headers)
    foreach (_file ${arg_INPUT_FILES})
        get_filename_component(_name ${_file} NAME_WE)
        list(APPEND gen_sources ${arg_DESTINATION}/JS${_name}.cpp)
        list(APPEND gen_headers ${arg_DESTINATION}/JS${_name}.h)
    endforeach ()
    set(${arg_OUTPUT_SOURCE} ${${arg_OUTPUT_SOURCE}} ${gen_sources} PARENT_SCOPE)
    set(act_args)
    if (SHOW_BINDINGS_GENERATION_PROGRESS)
        list(APPEND args --showProgress)
    endif ()
    list(APPEND act_args BYPRODUCTS ${gen_sources} ${gen_headers})
    if (SHOW_BINDINGS_GENERATION_PROGRESS)
        list(APPEND act_args USES_TERMINAL)
    endif ()
    add_custom_target(${target}
        COMMAND ${PERL_EXECUTABLE} ${binding_generator} ${args}
        WORKING_DIRECTORY ${arg_BASE_DIR}
        COMMENT "Generate bindings (${target})"
        VERBATIM ${act_args})
endfunction()


macro(GENERATE_FONT_NAMES _infile)
    set(NAMES_GENERATOR ${WEBCORE_DIR}/dom/make_names.pl)
    set(_arguments  --fonts ${_infile})
    set(_outputfiles ${DERIVED_SOURCES_WEBCORE_DIR}/WebKitFontFamilyNames.cpp ${DERIVED_SOURCES_WEBCORE_DIR}/WebKitFontFamilyNames.h)

    add_custom_command(
        OUTPUT  ${_outputfiles}
        MAIN_DEPENDENCY ${_infile}
        DEPENDS ${MAKE_NAMES_DEPENDENCIES} ${NAMES_GENERATOR} ${SCRIPTS_BINDINGS}
        COMMAND ${PERL_EXECUTABLE} ${NAMES_GENERATOR} --outputDir ${DERIVED_SOURCES_WEBCORE_DIR} ${_arguments}
        VERBATIM)
endmacro()


macro(GENERATE_EVENT_FACTORY _infile _outfile)
    set(NAMES_GENERATOR ${WEBCORE_DIR}/dom/make_event_factory.pl)

    add_custom_command(
        OUTPUT  ${DERIVED_SOURCES_WEBCORE_DIR}/${_outfile}
        MAIN_DEPENDENCY ${_infile}
        DEPENDS ${NAMES_GENERATOR} ${SCRIPTS_BINDINGS}
        COMMAND ${PERL_EXECUTABLE} ${NAMES_GENERATOR} --input ${_infile} --outputDir ${DERIVED_SOURCES_WEBCORE_DIR}
        VERBATIM)
endmacro()


macro(GENERATE_EXCEPTION_CODE_DESCRIPTION _infile _outfile)
    set(NAMES_GENERATOR ${WEBCORE_DIR}/dom/make_dom_exceptions.pl)

    add_custom_command(
        OUTPUT  ${DERIVED_SOURCES_WEBCORE_DIR}/${_outfile}
        MAIN_DEPENDENCY ${_infile}
        DEPENDS ${NAMES_GENERATOR} ${SCRIPTS_BINDINGS}
        COMMAND ${PERL_EXECUTABLE} ${NAMES_GENERATOR} --input ${_infile} --outputDir ${DERIVED_SOURCES_WEBCORE_DIR}
        VERBATIM)
endmacro()


macro(GENERATE_SETTINGS_MACROS _infile _outfile)
    set(NAMES_GENERATOR ${WEBCORE_DIR}/page/make_settings.pl)

    # Do not list the output in more than one independent target that may
    # build in parallel or the two instances of the rule may conflict.
    # <https://cmake.org/cmake/help/v3.0/command/add_custom_command.html>
    set(_extra_output
        ${DERIVED_SOURCES_WEBCORE_DIR}/InternalSettingsGenerated.h
        ${DERIVED_SOURCES_WEBCORE_DIR}/InternalSettingsGenerated.cpp
        ${DERIVED_SOURCES_WEBCORE_DIR}/InternalSettingsGenerated.idl
    )
    set(_args BYPRODUCTS ${_extra_output})
    add_custom_command(
        OUTPUT ${DERIVED_SOURCES_WEBCORE_DIR}/${_outfile}
        MAIN_DEPENDENCY ${_infile}
        DEPENDS ${NAMES_GENERATOR} ${SCRIPTS_BINDINGS}
        COMMAND ${PERL_EXECUTABLE} ${NAMES_GENERATOR} --input ${_infile} --outputDir ${DERIVED_SOURCES_WEBCORE_DIR}
        VERBATIM ${_args})
endmacro()


macro(GENERATE_DOM_NAMES _namespace _attrs)
    set(NAMES_GENERATOR ${WEBCORE_DIR}/dom/make_names.pl)
    set(_arguments  --attrs ${_attrs})
    set(_outputfiles ${DERIVED_SOURCES_WEBCORE_DIR}/${_namespace}Names.cpp ${DERIVED_SOURCES_WEBCORE_DIR}/${_namespace}Names.h)
    set(_extradef)
    set(_tags)

    foreach (f ${ARGN})
        if (_tags)
            set(_extradef "${_extradef} ${f}")
        else ()
            set(_tags ${f})
        endif ()
    endforeach ()

    if (_tags)
        set(_arguments "${_arguments}" --tags ${_tags} --factory --wrapperFactory)
        set(_outputfiles "${_outputfiles}" ${DERIVED_SOURCES_WEBCORE_DIR}/${_namespace}ElementFactory.cpp ${DERIVED_SOURCES_WEBCORE_DIR}/${_namespace}ElementFactory.h ${DERIVED_SOURCES_WEBCORE_DIR}/JS${_namespace}ElementWrapperFactory.cpp ${DERIVED_SOURCES_WEBCORE_DIR}/JS${_namespace}ElementWrapperFactory.h)
    endif ()

    if (_extradef)
        set(_additionArguments "${_additionArguments}" --extraDefines=${_extradef})
    endif ()

    add_custom_command(
        OUTPUT  ${_outputfiles}
        DEPENDS ${MAKE_NAMES_DEPENDENCIES} ${NAMES_GENERATOR} ${SCRIPTS_BINDINGS} ${_attrs} ${_tags}
        COMMAND ${PERL_EXECUTABLE} ${NAMES_GENERATOR} --preprocessor "${CODE_GENERATOR_PREPROCESSOR_WITH_LINEMARKERS}" --outputDir ${DERIVED_SOURCES_WEBCORE_DIR} ${_arguments} ${_additionArguments}
        VERBATIM)
endmacro()

dofile("luapkg/formiga.lua")
local postprocess = dofile("luapkg/postprocess.lua")
formiga.build_dir = "build_debug"

local packages = dofile "profile_build_scripts/package_list.lua"
-- table.insert(packages, "rlcompleter") -- AUTOCOMPLETION => needs READLINE

luapkg{
  program_name = "nnlmutils.debug",
  verbosity_level = 0,  -- 0 => NONE, 1 => ONLY TARGETS, 2 => ALL
  packages = packages,
  global_flags = {
    debug="yes",
    use_lstrip = "no",
    optimization = "no",
    platform = "unix",
    extra_flags={
      "-march=native",
      "-msse",
      "-fPIC",
      "-fopenmp",
      "-std=c++11",
      assert(io.popen("pkg-config --cflags april-ann"):read("*l")),
    },
    extra_libs={
      "-fopenmp",
      assert(io.popen("pkg-config --libs april-ann"):read("*l")),
    },
    shared_extra_libs={
      "-fPIC",
      "-rdynamic",
      "-shared",
    },
  },
  
  main_package = package{
    name = "main_package",
    default_target = "build",
    target{
      name = "init",
      mkdir{ dir = "bin" },
      mkdir{ dir = "build" },
      mkdir{ dir = "include" },
    },
    target{
      name = "provide",
      depends = "init",
    },
    target{ name = "clean_all",
      exec{ command = [[find . -name "*~" -exec rm {} ';']] },
      delete{ dir = "bin" },
      delete{ dir = "build" },
      delete{ dir = "build_doc" },
      delete{ dir = "doxygen_doc" },
    },
    target{ name = "clean",
      delete{ dir = "bin" },
      delete{ dir = "build" },
      delete{ dir = "build_doc" },
    },
    target{ name = "document_clean",
      delete{ dir = "build_doc" },
      delete{ dir = "doxygen_doc" },
    },
    target{
      name = "build",
      depends = "provide",
      create_shared_library{},
      copy_header_files{},
      use_timestamp = true,
    },
  },
}

postprocess(arg, formiga)

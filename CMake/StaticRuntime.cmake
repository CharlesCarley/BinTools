# Reduce .dll dependencies in windows by linking 
# the runtime libraries statically 
macro(enable_static_runtime)
    IF(MSVC)
	    FOREACH(flag_var    CMAKE_CXX_FLAGS 
	                        CMAKE_CXX_FLAGS_DEBUG 
	                        CMAKE_CXX_FLAGS_RELEASE 
	                        CMAKE_CXX_FLAGS_MINSIZEREL 
	                        CMAKE_CXX_FLAGS_RELWITHDEBINFO
	                        CMAKE_C_FLAGS_DEBUG
	                        CMAKE_C_FLAGS_RELEASE
	                        CMAKE_C_FLAGS_MINSIZEREL 
	                        CMAKE_C_FLAGS_RELWITHDEBINFO
	                        )
		    IF(${flag_var} MATCHES "/MD")
			    STRING(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		    ENDIF(${flag_var} MATCHES "/MD")
		    IF(${flag_var} MATCHES "/MDd")
			    STRING(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
		    ENDIF(${flag_var} MATCHES "/MDd")
	    ENDFOREACH(flag_var)
    ENDIF(MSVC)
endmacro(enable_static_runtime)

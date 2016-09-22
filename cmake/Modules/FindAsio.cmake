set(ASIO_ROOT "asio/include")
 
set(ASIO_VERSION_ROOT "asio-${ASIO_VERSION}/include")
 
find_path(ASIO_INCLUDE_DIR
        NAMES
        asio.hpp
        PATHS
        /usr/include
        /usr/include/${ASIO_ROOT}
        /usr/include/${ASIO_VERSION_ROOT}
         
        /usr/local/include
        /usr/local/include/${ASIO_ROOT}
        /usr/local/include/${ASIO_VERSION_ROOT}
 
        C:/
        C:/${ASIO_ROOT}
        C:/${ASIO_VERSION_ROOT}
)
 
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    ASIO
    DEFAULT_MSG
    ASIO_INCLUDE_DIR
)

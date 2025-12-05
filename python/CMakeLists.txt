find_package(pybind11 CONFIG REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)

# ----------------------------
# Python extension module
# ----------------------------
pybind11_add_module(lancom_py python/lancom_pybind.cpp)
target_link_libraries(lancom_py PRIVATE lancom)
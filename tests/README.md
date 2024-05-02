This folder contains unit and integration tests for the OPTIGA&trade; Trust M host library.

The tests can be run in any linux environment without a need for the OPTIGA&trade; Trust M chip. This is achieved by using dummy components from the test_pal, which is located in extras/pal/test_pal.

To run the tests, in a console, run the following commands :


1- Navigate to your OPTIGA&trade; Trust M repository folder.

2- Execute the CMakefile.txt present in this folder :

    cmake -S tests/ -B ./build

3- Execute the Makefile :

    make --directory build

4- Once make is finished, execute the tests using ctest :

    ctest --output-on-failure --test-dir build


A coverage report can be generated after tests using the following command :

    gcovr --root .. --exclude external --print-summary --html --html-details -o build/coverage.html

The coverage.html file can be read in a browser.

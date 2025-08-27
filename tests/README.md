This folder contains unit and integration tests for the OPTIGA&trade; Trust M host library.

The tests can be run in any linux environment without a need for the OPTIGA&trade; Trust M chip. This is achieved by using dummy components from the test_pal, which is located in extras/pal/test_pal.

To run the tests, in a console, run the following commands :


1- Navigate to your OPTIGA&trade; Trust M repository folder.

2- The Host library support a range of platforms for which the library can be used. To execute the CMakefile.txt present in this folder, use the following commands :

    a) I2C Interface over Linux : cmake -S tests -B build -DBUILD_RPI_I2C=ON

    b) Test Interface over Linux : cmake -S tests -B build

    c) UART Interface over Linux : cmake -S tests -B build -DBUILD_RPI_UART=ON

    d) LIBUSB on Linux : cmake -S tests -B build -DBUILD_LIBUSB=ON

For MbedTLS 3.x, same commands plus -DBUILD_MBEDTLS_3=ON :

    a) I2C Interface over Linux : cmake -S tests -B build -DBUILD_RPI_I2C=ON -DBUILD_MBEDTLS_3=ON

    b) Test Interface over Linux : cmake -S tests -B build -DBUILD_MBEDTLS_3=ON

    c) UART Interface over Linux : cmake -S tests -B build -DBUILD_RPI_UART=ON -DBUILD_MBEDTLS_3=ON

    d) LIBUSB on Linux : cmake -S tests -B build -DBUILD_LIBUSB=ON -DBUILD_MBEDTLS_3=ON

Other options can be added to this cmake, which is the version of MbedTLS to be used. by default it is MbedTLS2, if MbedTLS3 is desired, please add the following define to the previous command : -DBUILD_MBEDTLS_3=ON

3- Execute the Makefile :

    make --directory build

4- Once make is finished, execute the tests using ctest :

    ctest --output-on-failure --test-dir build


A coverage report can be generated after tests using the following command :

    gcovr --root .. --exclude external --print-summary --html --html-details -o build/coverage.html

The coverage.html file can be read in a browser.

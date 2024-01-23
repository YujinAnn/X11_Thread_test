# X11_Thread_test
Test codes for X11 to display texts and some drawings.

## How to run
Make sure you have the necessary development libraries installed for X11 and Cairo. You can install them using the following command:

    sudo apt-get install libx11-dev libcairo2-dev

This command will install the required development libraries for X11 and Cairo.

Compile the code using g++:

    g++ -o x11_test x11_test.cpp -lX11 -lcairo -lpthread

This command compiles your code and generates an executable file named "x11_test". The "-lX11" and "-lcairo" flags link the X11 and Cairo libraries, and the "-lpthread" flag links the pthread library for thread support.

After successful compilation, you can run the executable:

    ./x11_test

This will execute your program.

#include <X11/Xlib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <cstring> // Include the header for strtok

#define REST 1
#define TASK_B 2
#define TASK_L 3
#define TASK_R 4
#define RESTING_TIME 10

std::atomic<bool> mouseClicked(false);

// thread signals
std::mutex mtx;
std::condition_variable cv;
int signalReceived = 0;
//

void drawText(cairo_t* cr, char* text, double x, double y) {
    // Set the font size and style
    cairo_set_font_size(cr, 36);
    cairo_select_font_face(cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    // Set the text color
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

    // Split the input text by newline characters
    const char* delimiter = "\n";
    char* token = strtok(text, delimiter);

    // Draw each sentence on a new line
    while (token != NULL) {
        cairo_move_to(cr, x, y);
        cairo_show_text(cr, token);

        // Move to the next line
        y += 50; // Adjust the spacing as needed

        token = strtok(NULL, delimiter);
    }
}


void x11Thread() {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        // Handle error, couldn't open display
        return;
    }

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);


    // Get screen dimensions
    int screenWidth = DisplayWidth(display, screen);
    int screenHeight = DisplayHeight(display, screen);
    int centerX = screenWidth / 4;
    int centerY = screenHeight / 4;

    Window window = XCreateSimpleWindow(display, root, 0, 0, screenWidth, screenHeight, 1,
                                        BlackPixel(display, screen), WhitePixel(display, screen));

    // Set the background color to black
    XSetWindowBackground(display, window, BlackPixel(display, screen));

    XSelectInput(display, window, ExposureMask | ButtonPressMask);

    XMapWindow(display, window);

    // Create a graphics context
    GC gc = XCreateGC(display, window, 0, NULL);XFlush(display);
    std::this_thread::sleep_for(std::chrono::seconds(1));



    int state = 0;
    std::unique_lock<std::mutex> lock(mtx);
    // Create a Cairo context
    XEvent event;
    
    int thread_state = 0;
    int radius = 100;

    while(true){
        printf("Thread_while \n");
        switch (thread_state)   {
            case REST:
                printf("Thread_REST \n");
                // code to be executed if 
                // expression is equal to constant1;
                // Write the text
                for(int i=0; i<RESTING_TIME; i++){

                    XClearWindow(display, window);
                    XFlush(display);


                    // Create a Cairo surface for the X11 window
                    cairo_surface_t *surface = cairo_xlib_surface_create(display, window,
                                                            DefaultVisual(display, screen),
                                                            screenWidth, screenHeight);
                    cairo_t *cr = cairo_create(surface);
                    // Set the font size and style
                    cairo_set_font_size(cr, 200);
                    cairo_select_font_face(cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

                    // Set the text color
                    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

                    // Text to display
                    const char* text = std::to_string(i+1).c_str();

                    // Calculate text dimensions
                    cairo_text_extents_t text_extents;
                    cairo_text_extents(cr, text, &text_extents);
                    double textWidth = text_extents.width;
                    double textHeight = text_extents.height;


                    cairo_move_to(cr, centerX- textWidth/2, centerY+ textHeight/2);
                    cairo_show_text(cr, text);

                    // Clean up
                    cairo_destroy(cr);
                    cairo_surface_destroy(surface);

                    XFlush(display);

                    std::this_thread::sleep_for(std::chrono::seconds(1));


                }

                //Thread: Waiting for signal from the main loop...
                
                cv.wait(lock, [] { return signalReceived; });
                printf("signal: %d\n", signalReceived);
                thread_state = signalReceived;
                signalReceived = 0;
                

                break;

            case TASK_B:
                printf("Thread_TASK_B\n");
                XClearWindow(display, window);
                XFlush(display);

                // Draw a cross inside the circle
                XSetForeground(display, DefaultGC(display, screen), 0xFFFF00); 
                XDrawLine(display, window, DefaultGC(display, screen),
                        centerX - radius, centerY, centerX + radius, centerY);
                XDrawLine(display, window, DefaultGC(display, screen),
                        centerX, centerY - radius, centerX, centerY + radius);

    
                XFlush(display);

                
                cv.wait(lock, [] { return signalReceived; });
                printf("signal: %d\n", signalReceived);
                thread_state = signalReceived;
                signalReceived = 0;

                break;

            case TASK_L:
                printf("Thread_TASK_L\n");
                
                
                XSetForeground(display, DefaultGC(display, screen), 0xFFFF00); // Yellow color
                XFillArc(display, window, DefaultGC(display, screen),
                        centerX*0.5 - radius, centerY - radius, 2 * radius, 2 * radius, 0, 360 * 64);

                // Draw a cross inside the circle
                XSetForeground(display, DefaultGC(display, screen), 0xFFFF00); 
                XDrawLine(display, window, DefaultGC(display, screen),
                        centerX - radius, centerY, centerX + radius, centerY);
                XDrawLine(display, window, DefaultGC(display, screen),
                        centerX, centerY - radius, centerX, centerY + radius);

                XFlush(display);
                cv.wait(lock, [] { return signalReceived; });
                printf("signal: %d\n", signalReceived);
                thread_state = signalReceived;
                signalReceived = 0;

                break;

            case TASK_R:
                printf("Thread_TASK_R\n");
                
                
                XSetForeground(display, DefaultGC(display, screen), 0xFFFF00); // Yellow color
                XFillArc(display, window, DefaultGC(display, screen),
                        centerX*1.5 - radius, centerY - radius, 2 * radius, 2 * radius, 0, 360 * 64);

                // Draw a cross inside the circle
                XSetForeground(display, DefaultGC(display, screen), 0xFFFF00); 
                XDrawLine(display, window, DefaultGC(display, screen),
                        centerX - radius, centerY, centerX + radius, centerY);
                XDrawLine(display, window, DefaultGC(display, screen),
                        centerX, centerY - radius, centerX, centerY + radius);

                XFlush(display);
                cv.wait(lock, [] { return signalReceived; });
                printf("signal: %d\n", signalReceived);
                thread_state = signalReceived;
                signalReceived = 0;

                break;

            default:
                printf("Thread_default \n");
                XClearWindow(display, window);
                XFlush(display);

                // Create a Cairo surface for the X11 window
                cairo_surface_t *surface = cairo_xlib_surface_create(display, window,
                                                        DefaultVisual(display, screen),
                                                        screenWidth, screenHeight);
                cairo_t *cr = cairo_create(surface);

                // Text to display with multiple sentences separated by newline characters
                char text[] = "Instruction: \n    - This experiment is going to take 7 minutes. \n    - When you see the cross, keep your eyes on the center of the cross.\n    - When you see the circle, imagine movement of your hand for the correspond direction.\n    - When you prepare to do this sesson, click the window.";

                // Draw the text at the specified position
                drawText(cr, text, 500, 500);


                // Clean up
                cairo_destroy(cr);
                cairo_surface_destroy(surface);

                XFlush(display);

                XNextEvent(display, &event);
                if(event.type == ButtonPress){
                    thread_state = REST;

                    // Set the flag to indicate a mouse click
                    mouseClicked.store(true, std::memory_order_relaxed);
                }
                
        }
    }
}

int main() {
    std::thread x11WindowThread(x11Thread);
    auto startTime = std::chrono::high_resolution_clock::now();
    int second = 0;

    // --- 1. defualt: show instruction.
    printf("1.--- Main_Instruction \n");
    for(int count = 0; count>-1; count++){
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

        if (elapsedTime.count() > second) {
            second = elapsedTime.count();
            std::cout << "count = " << second << std::endl;
        }

        // Check if a signal has been received
        if (mouseClicked.load(std::memory_order_relaxed)) {
            std::cout << "Signal comes." << std::endl;
            // Reset the flag
            mouseClicked.store(false, std::memory_order_relaxed);
            break;
        }


    }

    // --- 2. Running rest, take 15 secs.
    printf("2.--- Main_Rest \n");
    for(int rest_time = 0; rest_time < RESTING_TIME; rest_time++){
        std::cout << rest_time+1 << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }


    // --- 3. Running task Baseline
    printf("here! 1\n");
    {
        std::lock_guard<std::mutex> lock(mtx);
        signalReceived = TASK_B;
    }
    printf("here! 2\n");
    cv.notify_one(); // Notify the waiting thread
    printf("3.--- Main_Task_B \n");
    std::this_thread::sleep_for(std::chrono::seconds(4));


    // --- 4. Running task Lefthand
    {
        std::lock_guard<std::mutex> lock(mtx);
        signalReceived = TASK_L;
    }
    cv.notify_one(); // Notify the waiting thread
    printf("4.--- Main_Task_L \n");
    std::this_thread::sleep_for(std::chrono::seconds(4));

    // --- 5. Running task Baseline
    {
        std::lock_guard<std::mutex> lock(mtx);
        signalReceived = TASK_B;
    }
    cv.notify_one(); // Notify the waiting thread
    printf("5.--- Main_Task_B \n");
    std::this_thread::sleep_for(std::chrono::seconds(4));

    // --- 6. Running task Lefthand
    {
        std::lock_guard<std::mutex> lock(mtx);
        signalReceived = TASK_R;
    }
    cv.notify_one(); // Notify the waiting thread
    printf("6.--- Main_Task_R \n");
    std::this_thread::sleep_for(std::chrono::seconds(4));

    // --- 7. Running task Baseline
    {
        std::lock_guard<std::mutex> lock(mtx);
        signalReceived = TASK_B;
    }
    cv.notify_one(); // Notify the waiting thread
    printf("7.--- Main_Task_B \n");
    std::this_thread::sleep_for(std::chrono::seconds(4));
    

    // --- 8. Running rest, take 15 secs.
    { // Send a signal to the thread
        std::lock_guard<std::mutex> lock(mtx);
        signalReceived = REST;
    }
    cv.notify_one(); // Notify the waiting thread
    printf("8.--- Main_Rest \n");
    std::this_thread::sleep_for(std::chrono::seconds(RESTING_TIME));


    x11WindowThread.join();

    return 0;
}

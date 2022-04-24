#ifdef _WIN32
#include <Windows.h>
#endif
#include "OpenGLWindow.h"

#ifdef _WIN32
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    try
    {
        OpenGLWindow window;
        window.init();
    }
    catch (std::runtime_error& e)
    {
        std::string message = "An error has occurred during initialization:\n\n";
        message += e.what();
        std::cerr << message << std::endl;

        std::wstring message_w(message.begin(), message.end());
        MessageBox(NULL, message_w.c_str(), L"Error", MB_OK | MB_ICONERROR);
    }

    return 0;
}
#else
int main(int argc, char* argv[])
{
    try
    {
        OpenGLWindow window;
        window.init(&argc, argv);
    }
    catch (std::runtime_error& e)
    {
        std::string message = "An error has occurred during initialization:\n\n";
        message += e.what();
        std::cerr << message << std::endl;
    }

    return 0;
}
#endif
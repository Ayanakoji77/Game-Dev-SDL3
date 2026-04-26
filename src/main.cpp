#include "core/application.h"

int main()
{
    Application app;

    if (app.Initialize())
    {
        app.Run();
    }

    app.Destroy();
    return 0;
}

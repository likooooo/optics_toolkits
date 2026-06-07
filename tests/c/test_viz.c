#include <optics_toolkits/optics_viz.h>

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    if (!optics_has_python_core_plugins()) {
        printf("test_viz skipped (python core plugins not enabled)\n");
        return EXIT_SUCCESS;
    }

    const char* dirs[] = {"core_plugins"};
    if (optics_py_init(dirs, 1) != OPTICS_OK) {
        fprintf(stderr, "optics_py_init failed: %s\n", optics_last_error());
        return EXIT_FAILURE;
    }

    const double x[] = {0.0, 1.0, 2.0};
    const double y[] = {0.0, 1.0, 0.0};
    if (optics_viz_plot_xy(x, y, 3, "test", "x", "y", "b-") != OPTICS_OK) {
        fprintf(stderr, "optics_viz_plot_xy failed: %s\n", optics_last_error());
        optics_py_shutdown();
        return EXIT_FAILURE;
    }

    optics_py_shutdown();
    printf("test_viz passed\n");
    return EXIT_SUCCESS;
}

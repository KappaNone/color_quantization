#define NOB_STRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv)
{
    GO_REBUILD_URSELF(argc, argv);
    Cmd cmd = {0};
    cmd_append(&cmd, "gcc", "-Wall", "-Wextra", "-o", "main", "main.c");
    cmd_append(&cmd, "-IC:\\Users\\Anton Reunov\\Programming\\Thirdparty\\raylib-5.5_win32_mingw-w64\\include");
    cmd_append(&cmd, "-LC:\\Users\\Anton Reunov\\Programming\\Thirdparty\\raylib-5.5_win32_mingw-w64\\lib");
    cmd_append(&cmd, "-lraylib");
    cmd_append(&cmd, "-lopengl32");
    cmd_append(&cmd, "-lgdi32");
    cmd_append(&cmd, "-lwinmm");
    cmd_append(&cmd, "-lmingw32");
    String_Builder sb = { 0 };
    
    nob_cmd_render(cmd, &sb);
    if (!cmd_run(&cmd)) return 1;
    cmd_append(&cmd, "./main.exe");
    if (!cmd_run(&cmd)) return 1;
    return 0;
}

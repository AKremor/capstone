#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

extern void mainThread(void *arg0);

extern "C" {
void NDK_hookInit(){};
}

    int main(void) {
    Task_Params task_params;
    Task_Params_init(&task_params);
    task_params.stackSize = 5000;
    task_params.instance->name = "Main thread";
    task_params.priority = 1;
    Task_create((Task_FuncPtr)mainThread, &task_params, NULL);

    BIOS_start();

    return (0);
}

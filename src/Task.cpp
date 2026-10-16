#include "Task.h"

Task::Task(const uint32_t stackSize, const UBaseType_t priority, const char *taskName)
            : taskHandle(nullptr), _stackSize(stackSize), _priority(priority), _taskName(taskName) {}

void Task::start() {
    xTaskCreatePinnedToCore(wrapper, _taskName, _stackSize,
    this, _priority, &taskHandle, 1);
}

void Task::wrapper(void *pvParameters) {
    // when passing the pointer pointing to the Task object from the xTaskCreatePinnedToCore function
    // it passes it as a void* pointer, so we need to cast it to access the main function
    auto *task = static_cast<Task*>(pvParameters);
    task->main();
    vTaskDelete(nullptr);
}
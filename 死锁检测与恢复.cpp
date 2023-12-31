#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define PNUM 5  // 进程数量
#define RNUM 3  // 资源数量

typedef struct {
	int processId;
	int allocatedResources[RNUM];
} DeprivedProcess;

int allocation[PNUM][RNUM] = {
	{0, 1, 0},
	{2, 0, 0},
	{3, 0, 3},
	{2, 1, 1},
	{0, 0, 2}
};

int request[PNUM][RNUM] = {
	{0, 0, 0},
	{2, 0, 2},
	{0, 0, 1},
	{1, 0, 0},
	{0, 0, 2}
};

int available[RNUM] = { 0, 0, 0 };
int finish[PNUM];

void display() {
	printf("The resource state now:\n");
	printf("%-10s\t%-10s\t%-10s\n", "process", "allocation", "request");
	for (int i = 0; i < PNUM; i++) {
		int j = 0;
		printf("P%-9d\t", i);
		for (j = 0; j < RNUM; j++) {
			printf("%2d ", allocation[i][j]);
		}
		printf("\t");
		for (j = 0; j < RNUM; j++) {
			printf("%2d ", request[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

bool isSafeState() {
	int work[RNUM];  // 系统可用资源的工作副本
	for (int i = 0; i < RNUM; i++) {
		work[i] = available[i];  // 初始工作资源等于系统可用资源
	}

	bool finish[PNUM] = { false };  // 跟踪完成状态
	int safeSequence[PNUM];  // 安全序列
	int finishCount = 0;  // 完成的进程数量

	// 这里我们循环直到所有进程都完成或无法找到可以分配的进程
	while (finishCount < PNUM) {
		bool found = false;  // 在本轮循环中找到了进程可以执行

		for (int i = 0; i < PNUM; i++) {
			// 如果该进程没有完成且它的最大请求可以被满足
			if (!finish[i]) {
				bool canAllocate = true;  // 假定可以分配需要的资源
				for (int j = 0; j < RNUM; j++) {
					// 判断工作中的资源是否足够给这个process
					if (request[i][j] > work[j]) {
						canAllocate = false; // 不能分配资源，跳出循环
						break;
					}
				}
				// 如果资源足够，此进程可以执行
				if (canAllocate) {
					for (int j = 0; j < RNUM; j++) {
						work[j] += allocation[i][j];  // 更新工作资源
					}
					// 将进程标记为完成，并存入安全序列
					finish[i] = true;
					safeSequence[finishCount++] = i;

					found = true;  // 在此轮找到可以执行的进程
				}
			}
		}

		if (!found) {  // 一轮循环结束，如果没有找到可以执行的进程则终止
			break;
		}
	}

	// 检查是否所有进程都被放入了安全序列
	if (finishCount == PNUM) {
		// 打印安全序列
		for (int i = 0; i < PNUM; i++) {
			printf("P%d ", safeSequence[i]);
		}
		return true;  // 返回安全状态存在
	}
	else {
		return false;  // 返回系统不处于安全状态
	}
}

void detectAndRecoverDeadlock() {
	bool deadlockDetected = false;
	int deprivedCount = 0;
	DeprivedProcess deprivedProcesses[PNUM];

	while (!deadlockDetected) {
		// 检测死锁
		int maxAllocated = -1;
		int maxAllocatedProcess = -1;
		for (int i = 0; i < PNUM; i++) {
			if (finish[i] == -1) {
				bool canAllocate = true;
				for (int j = 0; j < RNUM; j++) {
					if (request[i][j] > available[j]) {
						canAllocate = false;
						break;
					}
				}
				if (canAllocate) {
					int totalAllocated = 0;
					for (int j = 0; j < RNUM; j++) {
						totalAllocated += allocation[i][j];
					}
					if (totalAllocated > maxAllocated) {
						maxAllocated = totalAllocated;
						maxAllocatedProcess = i;
					}
				}
			}
		}

		if (maxAllocatedProcess >= 0) {
			printf("Deadlock detected. Process P%d will be terminated.\n", maxAllocatedProcess);

			for (int j = 0; j < RNUM; j++) {
				available[j] += allocation[maxAllocatedProcess][j];
			}

			finish[maxAllocatedProcess] = maxAllocatedProcess + 1;

			deprivedProcesses[deprivedCount].processId = maxAllocatedProcess;
			for (int j = 0; j < RNUM; j++) {
				deprivedProcesses[deprivedCount].allocatedResources[j] = allocation[maxAllocatedProcess][j];
				allocation[maxAllocatedProcess][j] = 0;
				request[maxAllocatedProcess][j] = 0;
			}
			deprivedCount++;

			printf("\nResources after depriving process P%d:\n", maxAllocatedProcess);
			printf("%-10s\t%-10s\n", "resource", "available");
			for (int i = 0; i < RNUM; i++) {
				printf("R%-9d\t%2d\n", i, available[i]);
			}
			printf("\n");
		}
		else {
			deadlockDetected = true;
		}
	}

	if (isSafeState()) {
		printf("System is in safe state. Safe sequence: ");
		for (int i = 0; i < PNUM; i++) {
			if (finish[i] != -1) {
				printf("P%d ", finish[i]);
			}
		}
		printf("\n");
	}
	else {
		printf("System is still in deadlock state.\n");
	}

	printf("\nDeprived processes and their allocated resources:\n");
	printf("%-10s\t%-20s\n", "process", "allocated resources");
	for (int i = 0; i < deprivedCount; i++) {
		printf("P%-9d\t", deprivedProcesses[i].processId);
		for (int j = 0; j < RNUM; j++) {
			printf("%2d ", deprivedProcesses[i].allocatedResources[j]);
		}
		printf("\n");
	}
}

int main() {
	display();

	for (int i = 0; i < PNUM; i++) {
		finish[i] = -1;
	}

	detectAndRecoverDeadlock();

	return EXIT_SUCCESS;
}

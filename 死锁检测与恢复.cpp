#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define PNUM 5  // ��������
#define RNUM 3  // ��Դ����

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
	int work[RNUM];  // ϵͳ������Դ�Ĺ�������
	for (int i = 0; i < RNUM; i++) {
		work[i] = available[i];  // ��ʼ������Դ����ϵͳ������Դ
	}

	bool finish[PNUM] = { false };  // �������״̬
	int safeSequence[PNUM];  // ��ȫ����
	int finishCount = 0;  // ��ɵĽ�������

	// ��������ѭ��ֱ�����н��̶���ɻ��޷��ҵ����Է���Ľ���
	while (finishCount < PNUM) {
		bool found = false;  // �ڱ���ѭ�����ҵ��˽��̿���ִ��

		for (int i = 0; i < PNUM; i++) {
			// ����ý���û��������������������Ա�����
			if (!finish[i]) {
				bool canAllocate = true;  // �ٶ����Է�����Ҫ����Դ
				for (int j = 0; j < RNUM; j++) {
					// �жϹ����е���Դ�Ƿ��㹻�����process
					if (request[i][j] > work[j]) {
						canAllocate = false; // ���ܷ�����Դ������ѭ��
						break;
					}
				}
				// �����Դ�㹻���˽��̿���ִ��
				if (canAllocate) {
					for (int j = 0; j < RNUM; j++) {
						work[j] += allocation[i][j];  // ���¹�����Դ
					}
					// �����̱��Ϊ��ɣ������밲ȫ����
					finish[i] = true;
					safeSequence[finishCount++] = i;

					found = true;  // �ڴ����ҵ�����ִ�еĽ���
				}
			}
		}

		if (!found) {  // һ��ѭ�����������û���ҵ�����ִ�еĽ�������ֹ
			break;
		}
	}

	// ����Ƿ����н��̶��������˰�ȫ����
	if (finishCount == PNUM) {
		// ��ӡ��ȫ����
		for (int i = 0; i < PNUM; i++) {
			printf("P%d ", safeSequence[i]);
		}
		return true;  // ���ذ�ȫ״̬����
	}
	else {
		return false;  // ����ϵͳ�����ڰ�ȫ״̬
	}
}

void detectAndRecoverDeadlock() {
	bool deadlockDetected = false;
	int deprivedCount = 0;
	DeprivedProcess deprivedProcesses[PNUM];

	while (!deadlockDetected) {
		// �������
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

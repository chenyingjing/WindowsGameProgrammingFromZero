#include <stdio.h>

int data[] = {11, 1, 0, 5, 5, 8, 3};

void outputData()
{
	int length = sizeof(data) / sizeof(data[0]);

	for (size_t i = 0; i < length; i++)
	{
		printf("%d ", data[i]);
	}
	printf("\n");
}

void BubSort(int n)
{
	int i, j;
	bool f;
	int tmp;

	for (i = 0; i<n - 1; i++)
	{
		f = false;
		for (j = 0; j<n - i - 1; j++)
		{
			if (data[j + 1] < data[j])
			{
				tmp = data[j + 1];
				data[j + 1] = data[j];
				data[j] = tmp;
				f = true;
				outputData();
			}
		}
		if (!f)
			break;
	}
}

void main()
{
	outputData();
	int length = sizeof(data) / sizeof(data[0]);
	BubSort(length);
}
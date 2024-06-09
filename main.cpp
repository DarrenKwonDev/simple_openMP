#include <iostream>
#include <chrono>
#include <omp.h>
#include <random>


void hello_openmp()
{
	// �ƹ� ������ ���� ������ cpu�� thread ��ŭ �����Ŵ
	omp_set_num_threads(8);

	#pragma omp parallel 
	{
	#pragma omp critical
		std::cout << "hello openmp " << omp_get_thread_num() << " / " << omp_get_num_threads() << std::endl;
	}
}

void parallel_for()
{
	const int length = 1024 * 64;

	float* a = new float[length];
	float* b = new float[length];
	float* c = new float[length];
	float* res = new float[length];

	std::mt19937_64 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(0, 1);
	for (size_t i = 0; i < length; i++)
	{
		a[i] = dist(rng); b[i] = dist(rng); c[i] = dist(rng);
	}

	// for ���� ����ó���϶�.
	#pragma omp parallel for
	for (int i = 0; i < length; i++)
	{
		res[i] = a[i] + b[i] * c[i];
	}

	delete[] a; delete[] b; delete[] c; delete[] res;
}

void parallel_sections()
{
	omp_set_num_threads(6); // �ش� �Լ��� ���� �۾��� 6���� �����带 ����� ����

#pragma omp parallel sections // �� ������ ���ķ� ����
	// �� ������ ���������� ����ǹǷ�, ������ �������� ����� �մϴ�. 
	// �� ������ ����� �ٸ� ���ǿ� ������ ���� �ʾƾ� �մϴ�.
	// ���� ������ thread �� �˾Ƽ� omp�� �й��Ѵ�.
	{
	#pragma omp section	// section 1. 
		{
			for (int i = 0; i < 1000; i++)
			{
				std::cout << i;
			}
		} // end of section 1

	#pragma omp section	// section 2. 
		{
			for (int i = 0; i < 1000; i++)
			{
				std::cout << static_cast<char>('a' + (i % 26));
			}
		} // end of section 2

	} // end of sections
}

void single_master()
{
#pragma omp parallel 
{
	#pragma omp single  // ���� ���� �� �� �� ���� ����.
	{
		printf("gathering some input \n");
	}
	
	// ��� thread ���� ȣ���
	printf("parallel on %d \n", omp_get_thread_num());

	//  ��� �����尡 Ư�� ������ ������ ������ ��ٸ��� �մϴ�
	#pragma omp barrier

	#pragma omp master	// ���� ���� �� master thread ������ ����
	{
		printf("output on %d \n", omp_get_thread_num());
	}

} // end of omp parallel 
}


void atomic_sync()
{
	printf("\nATOMIC\n");

	int sum = 0;

	//-----------------------------------------------
	// �ִ� 128���� �����尡 �� ������ ���ķ� ����
	#pragma omp parallel for num_threads(128)
	for (int i = 0; i < 100; i++)
	{
		// �б�-����-���� ������ �ٸ� ������κ��� �����ϰ� ����ǵ��� �մϴ�
		// ���� �����尡 ���ÿ� ������ �����ϴ��� ������ ����(race condition)�� �߻����� �ʽ��ϴ�.
		// ���� atomic Ű���带 �����Ѵٸ� sum�� 100 �� �ƴ϶� �ٸ� ���� �����?
		#pragma omp atomic
		++sum;
	}
	//-----------------------------------------------
	std::cout << sum;
}

void ordered_for()
{
	printf("\nORDERED\n");

	std::vector<int> squares;

	// �� thread�� i�� ������ squares�� push ��.
	#pragma omp parallel for ordered
	for (int i = 0; i < 20; i++)
	{
		printf("%d : %d \t", omp_get_thread_num(), i);
		int j = i * i;

	#pragma omp ordered
		squares.push_back(j);
	}

	printf("\n");

	for (auto v : squares)
		printf("%d\t", v);
}

void data_sharing()
{
	int j = 10;

	// �� ������� �ʱ� ������ j�� ���� �����մϴ�. ��, �� ������� �ڽ��� ���� j ������ ������ �˴ϴ�.
	#pragma omp parallel for firstprivate(j)
	for (int i = 0; i < 10; i++)
	{
		printf("thread %d i = %d \n", omp_get_thread_num(), i);
		j = 1000 + omp_get_thread_num();
	}

// ���� �������� j ���� ���� ���� �ۿ��� ������� �ʽ��ϴ�. ���� ���� �������� j ���� ������ �ʱ� ���� 10�Դϴ�.
printf("%d \n", j);
}

int main()
{
	// hello_openmp();
	// parallel_for(); 
	// parallel_sections();
	// single_master();
	// atomic_sync();
	// ordered_for();
	// data_sharing();

	return 0;
}
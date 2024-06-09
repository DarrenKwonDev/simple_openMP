#include <iostream>
#include <chrono>
#include <omp.h>
#include <random>


void hello_openmp()
{
	// 아무 설정도 하지 않으면 cpu의 thread 만큼 실행시킴
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

	// for 문을 병렬처리하라.
	#pragma omp parallel for
	for (int i = 0; i < length; i++)
	{
		res[i] = a[i] + b[i] * c[i];
	}

	delete[] a; delete[] b; delete[] c; delete[] res;
}

void parallel_sections()
{
	omp_set_num_threads(6); // 해당 함수의 병렬 작업에 6개의 스레드를 사용할 예정

#pragma omp parallel sections // 각 섹션을 병렬로 실행
	// 각 섹션은 독립적으로 실행되므로, 데이터 의존성이 없어야 합니다. 
	// 한 섹션의 결과가 다른 섹션에 영향을 주지 않아야 합니다.
	// 가용 가능한 thread 중 알아서 omp가 분배한다.
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
	#pragma omp single  // 병렬 영역 내 딱 한 번만 실행.
	{
		printf("gathering some input \n");
	}
	
	// 모든 thread 에서 호출됨
	printf("parallel on %d \n", omp_get_thread_num());

	//  모든 스레드가 특정 지점에 도달할 때까지 기다리게 합니다
	#pragma omp barrier

	#pragma omp master	// 병렬 영역 내 master thread 에서만 실행
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
	// 최대 128개의 스레드가 이 루프를 병렬로 실행
	#pragma omp parallel for num_threads(128)
	for (int i = 0; i < 100; i++)
	{
		// 읽기-수정-쓰기 연산이 다른 스레드로부터 안전하게 수행되도록 합니다
		// 여러 스레드가 동시에 변수에 접근하더라도 데이터 경합(race condition)이 발생하지 않습니다.
		// 만약 atomic 키워드를 제거한다면 sum은 100 이 아니라 다른 값을 뱉겠죠?
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

	// 각 thread가 i를 가지고 squares에 push 함.
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

	// 각 스레드는 초기 값으로 j의 값을 복사합니다. 즉, 각 스레드는 자신의 로컬 j 변수를 가지게 됩니다.
	#pragma omp parallel for firstprivate(j)
	for (int i = 0; i < 10; i++)
	{
		printf("thread %d i = %d \n", omp_get_thread_num(), i);
		j = 1000 + omp_get_thread_num();
	}

// 메인 스레드의 j 값은 병렬 영역 밖에서 변경되지 않습니다. 따라서 메인 스레드의 j 값은 여전히 초기 값인 10입니다.
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
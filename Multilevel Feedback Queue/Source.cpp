#include <iostream>
#include <fstream>
#include <queue>
#include <deque>

void Input(int h, int w, const char* FileName, int** DataFIFO, int** DataMLFQ)
{
	std::ifstream fin(FileName);
	int num;
	for (int i = 0; i < h; i++)
		for (int j = 1; j < w; j++)
		{
			if ((fin.peek() == ',') || (fin.peek() == ';'))
				fin.ignore();
			fin >> num;
			//num = rand() % 100 + 1;
			DataFIFO[i][j] = num;
			DataMLFQ[i][j] = num;
		}
}

void FIFO(int** Data, int h, int w)
{
	bool Flag = false;
	int Time = 1; // текущий такт
	int CountStreamDone = 0; // количество выполненных процессов
	int CountStreamTotal = h; // количество процессов
	int* TimeWait = new int[h]; //время ожидания для процесса
	int* TimeToDo = new int[h]; //время выполнения для процесса
	for (int i = 0; i < h; i++)
	{
		TimeWait[i] = 0;
		TimeToDo[i] = 0;
	}
	float TimeWaitAvg = 0; //среднее время ожидания
	float TimeToDoAvg = 0; //среднее время выполнения
	std::queue <int> Queue;
	int Running = -1;

	while (CountStreamDone < CountStreamTotal)
	{
		Time++;

		if ((Flag == true) && (!Queue.empty()))
		{
			Running = Queue.front();
			Queue.pop();
			Flag = false;
			std::cout << "Такт " << Time << ": процесс с номером " << Data[Running][1] <<
				" перешел в состояние выполнения" << std::endl;
		}

		for (int i = 0; i < CountStreamTotal; i++)
		{
			if (Data[i][3] == 0)
			{
				continue;
			}
			else if (Data[i][2] > 0)
			{
				Data[i][2]--;
				if (Data[i][2] == 0)
				{
					Queue.push(Data[i][0]);
					std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
						" перешел из состояния блокировки в состояние готовности" << std::endl;
					if ((Queue.size() == 1) && (Running == -1))
					{
						Queue.pop();
						Running = Data[i][0];
						std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
							" перешел в состояние выполнения" << std::endl;
					}
				}
			}
			else if ((Data[i][3] > 0) && (Running == Data[i][0]))
			{
				Data[i][3]--;
				TimeToDo[i]++;
				if (Data[i][3] == 0)
				{
					CountStreamDone++;
					TimeToDo[i] += TimeWait[i];
					Flag = true;
					Running = -1;
					std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
						" успешно закончил выполнение" << std::endl;
				}
			}
			else if ((Data[i][2] == 0) && (Running != Data[i][0]))
			{
				TimeWait[i]++;
			}
		}
	}

	for (int i = 0; i < CountStreamTotal; i++)
	{
		TimeWaitAvg += TimeWait[i];
		TimeToDoAvg += TimeToDo[i];
	}
	TimeWaitAvg /= CountStreamTotal;
	TimeToDoAvg /= CountStreamTotal;

	std::cout << "Общее количество тактов на выполнение процессов: " << Time << std::endl;
	std::cout << "Среднее время ожидания: " << TimeWaitAvg << " Среднее время выполнения: " << TimeToDoAvg << std::endl;
}

void MLFQ(int** Data, int h, int w)
{
	bool Flag = false;
	int Time = 1; // текущий такт

	int CountStreamDone = 0; // количество выполненных процессов
	int CountStreamTotal = h; // количество процессов

	int* TimeWait = new int[h]; //время ожидания для процесса
	int* TimeToDo = new int[h]; //время выполнения для процесса
	for (int i = 0; i < h; i++)
	{
		TimeWait[i] = 0;
		TimeToDo[i] = 0;
	}

	float TimeWaitAvg = 0; //среднее время ожидания
	float TimeToDoAvg = 0; //среднее время выполнения

	std::queue <int> FIFO1; //Первая очередь
	std::queue <int> FIFO2; //Вторая очередь
	std::deque <int> SJR;   //Третья очередь

	int Running = -1;
	int RunningQueue = -1;
	int TimeSlice = 0;

	while (CountStreamDone < CountStreamTotal)
	{
		Time++;

		if (Flag == true)
		{
			if (!FIFO1.empty())
			{
				Running = FIFO1.front();
				RunningQueue = 1;
				FIFO1.pop();
				std::cout << "Такт " << Time << ": процесс с номером " << Data[Running][1] <<
					" перешел в состояние выполнения из очереди 1 (FIFO)" << std::endl;
			}
			else if (!FIFO2.empty())
			{
				Running = FIFO2.front();
				RunningQueue = 2;
				FIFO2.pop();
				std::cout << "Такт " << Time << ": процесс с номером " << Data[Running][1] <<
					" перешел в состояние выполнения из очереди 2 (FIFO)" << std::endl;
			}
			else
			{
				int MIN, index;
				for (int i = 0; i < SJR.size(); i++)
				{
					if (i == 0)
					{
						MIN = Data[SJR[i]][3];
						index = i;
						continue;
					}

					if (MIN > Data[SJR[i]][3])
					{
						MIN = Data[SJR[i]][3];
						index = i;
					}
				}
				Running = SJR[index];
				RunningQueue = 3;
				SJR.erase(SJR.begin() + index);
				std::cout << "Такт " << Time << ": процесс с номером " << Data[Running][1] <<
					" перешел в состояние выполнения из очереди 3 (SJR)" << std::endl;
			}
			Flag = false;
		}

		for (int i = 0; i < CountStreamTotal; i++)
		{
			if (Data[i][3] == 0)
			{
				continue;
			}
			else if (Data[i][2] > 0)
			{
				Data[i][2]--;
				if (Data[i][2] == 0)
				{
					std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
						" перешел из состояния блокировки в состояние готовности" << std::endl;
					if (Data[i][4] <= 10)
					{
						FIFO1.push(Data[i][0]);
						std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
							" попал в очередь 1 (FIFO) по приоритету" << std::endl;
						if (RunningQueue == 3)
						{
							SJR.push_back(Running);
							FIFO1.pop();
							std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
								" из очереди 1 (FIFO) вытеснил процесс c номером " << Data[Running][1] <<" из очереди 3 (SJR)" << std::endl;
							Running = Data[i][0];
							RunningQueue = 1;
						}
						else if (RunningQueue == 2)
						{
							FIFO2.push(Running);
							FIFO1.pop();
							std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
								" из очереди 1 (FIFO) вытеснил процесс c номером " << Data[Running][1] << " из очереди 2 (FIFO)" << std::endl;
							Running = Data[i][0];
							RunningQueue = 1;
						}
					}
					else if (Data[i][4] >= 50)
					{
						SJR.push_back(Data[i][0]);
						std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
							" попал в очередь 3 (SJR) по приоритету" << std::endl;
					}
					else
					{
						FIFO2.push(Data[i][0]);
						std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
							" попал в очередь 2 (FIFO) по приоритету" << std::endl;
						if (RunningQueue == 3)
						{
							SJR.push_back(Running);
							FIFO2.pop();
							std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
								" из очереди 2 (FIFO) вытеснил процесс c номером " << Data[Running][1] << " из очереди 3 (SJR) " << std::endl;
							Running = Data[i][0];
							RunningQueue = 2;
						}
					}

					if ((FIFO1.size() == 1) && (Running == -1))
					{
						FIFO1.pop();
						Running = Data[i][0];
						RunningQueue = 1;
						std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
							" перешел в состояние выполнения из очереди 1 (FIFO)" << std::endl;
					}
					else if ((FIFO2.size() == 1) && (FIFO1.size() == 0) && (Running == -1))
					{
						FIFO2.pop();
						Running = Data[i][0];
						RunningQueue = 2;
						std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
							" перешел в состояние выполнения из очереди 2 (FIFO)" << std::endl;
					}
					else if ((SJR.size() == 1) && (FIFO1.size() == 0) && (FIFO2.size() == 0) && (Running == -1))
					{
						SJR.pop_front();
						Running = Data[i][0];
						RunningQueue = 3;
						std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
							" перешел в состояние выполнения  из очереди 3 (SJR)" << std::endl;
					}
				}
			}
			else if ((Data[i][3] > 0) && (Running == Data[i][0]))
			{
				Data[i][3]--;
				TimeToDo[i]++;
				TimeSlice++;
				if (Data[i][3] == 0)
				{
					CountStreamDone++;
					TimeToDo[i] += TimeWait[i];
					Flag = true;
					Running = -1;
					RunningQueue = -1;
					TimeSlice = 0;
					std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
						" успешно закончил выполнение" << std::endl;
				}
				else if ((RunningQueue == 1) && (TimeSlice == 20))
				{
					Flag = true;
					Running = -1;
					RunningQueue = -1;
					TimeSlice = 0;
					FIFO2.push(Data[i][0]);
					std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
						" не выполнился в выделенный квант времени (10 тактов) и перенесен в очередь 2 (FIFO)" << std::endl;
				}
				else if ((RunningQueue == 2) && (TimeSlice == 30))
				{
					Flag = true;
					Running = -1;
					RunningQueue = -1;
					TimeSlice = 0;
					SJR.push_back(Data[i][0]);
					std::cout << "Такт " << Time << ": процесс с номером " << Data[i][1] <<
						" не выполнился в выделенный квант времени (15 тактов) и перенесен в очередь 3 (SJR)" << std::endl;
				}
			}
			else if ((Data[i][2] == 0) && (Running != Data[i][0]))
			{
				TimeWait[i]++;
			}
		}
	}

	for (int i = 0; i < CountStreamTotal; i++)
	{
		TimeWaitAvg += TimeWait[i];
		TimeToDoAvg += TimeToDo[i];
	}
	TimeWaitAvg /= CountStreamTotal;
	TimeToDoAvg /= CountStreamTotal;

	std::cout << "Общее количество тактов на выполнение процессов: " << Time << std::endl;
	std::cout << "Среднее время ожидания: " << TimeWaitAvg << " Среднее время выполнения: " << TimeToDoAvg << std::endl;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	int h = 20, w = 5;
	const char* FileName = "Test.txt";

	int** DataFIFO;
	int** DataMLFQ;
	DataFIFO = new int* [h];
	DataMLFQ = new int* [h];
	for (int k = 0; k < h; k++)
	{
		DataFIFO[k] = new int[w];
		DataMLFQ[k] = new int[w];
	}

	for (int k = 0; k < h; k++)
	{
		DataFIFO[k][0] = k;
		DataMLFQ[k][0] = k;
	}
	Input(h, w, FileName, DataFIFO, DataMLFQ);

	std::cout << "******************************************" << std::endl;
	std::cout << "*                  FIFO                  *" << std::endl;
	std::cout << "******************************************" << std::endl;
	FIFO(DataFIFO, h, w);
	std::cout << "******************************************" << std::endl;
	std::cout << "*         Multilevel Feedback Queue      *" << std::endl;
	std::cout << "******************************************" << std::endl;
	MLFQ(DataMLFQ, h, w);
	return 0;
}
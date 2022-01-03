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
	int Time = 1; // ������� ����
	int CountStreamDone = 0; // ���������� ����������� ���������
	int CountStreamTotal = h; // ���������� ���������
	int* TimeWait = new int[h]; //����� �������� ��� ��������
	int* TimeToDo = new int[h]; //����� ���������� ��� ��������
	for (int i = 0; i < h; i++)
	{
		TimeWait[i] = 0;
		TimeToDo[i] = 0;
	}
	float TimeWaitAvg = 0; //������� ����� ��������
	float TimeToDoAvg = 0; //������� ����� ����������
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
			std::cout << "���� " << Time << ": ������� � ������� " << Data[Running][1] <<
				" ������� � ��������� ����������" << std::endl;
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
					std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
						" ������� �� ��������� ���������� � ��������� ����������" << std::endl;
					if ((Queue.size() == 1) && (Running == -1))
					{
						Queue.pop();
						Running = Data[i][0];
						std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
							" ������� � ��������� ����������" << std::endl;
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
					std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
						" ������� �������� ����������" << std::endl;
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

	std::cout << "����� ���������� ������ �� ���������� ���������: " << Time << std::endl;
	std::cout << "������� ����� ��������: " << TimeWaitAvg << " ������� ����� ����������: " << TimeToDoAvg << std::endl;
}

void MLFQ(int** Data, int h, int w)
{
	bool Flag = false;
	int Time = 1; // ������� ����

	int CountStreamDone = 0; // ���������� ����������� ���������
	int CountStreamTotal = h; // ���������� ���������

	int* TimeWait = new int[h]; //����� �������� ��� ��������
	int* TimeToDo = new int[h]; //����� ���������� ��� ��������
	for (int i = 0; i < h; i++)
	{
		TimeWait[i] = 0;
		TimeToDo[i] = 0;
	}

	float TimeWaitAvg = 0; //������� ����� ��������
	float TimeToDoAvg = 0; //������� ����� ����������

	std::queue <int> FIFO1; //������ �������
	std::queue <int> FIFO2; //������ �������
	std::deque <int> SJR;   //������ �������

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
				std::cout << "���� " << Time << ": ������� � ������� " << Data[Running][1] <<
					" ������� � ��������� ���������� �� ������� 1 (FIFO)" << std::endl;
			}
			else if (!FIFO2.empty())
			{
				Running = FIFO2.front();
				RunningQueue = 2;
				FIFO2.pop();
				std::cout << "���� " << Time << ": ������� � ������� " << Data[Running][1] <<
					" ������� � ��������� ���������� �� ������� 2 (FIFO)" << std::endl;
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
				std::cout << "���� " << Time << ": ������� � ������� " << Data[Running][1] <<
					" ������� � ��������� ���������� �� ������� 3 (SJR)" << std::endl;
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
					std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
						" ������� �� ��������� ���������� � ��������� ����������" << std::endl;
					if (Data[i][4] <= 10)
					{
						FIFO1.push(Data[i][0]);
						std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
							" ����� � ������� 1 (FIFO) �� ����������" << std::endl;
						if (RunningQueue == 3)
						{
							SJR.push_back(Running);
							FIFO1.pop();
							std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
								" �� ������� 1 (FIFO) �������� ������� c ������� " << Data[Running][1] <<" �� ������� 3 (SJR)" << std::endl;
							Running = Data[i][0];
							RunningQueue = 1;
						}
						else if (RunningQueue == 2)
						{
							FIFO2.push(Running);
							FIFO1.pop();
							std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
								" �� ������� 1 (FIFO) �������� ������� c ������� " << Data[Running][1] << " �� ������� 2 (FIFO)" << std::endl;
							Running = Data[i][0];
							RunningQueue = 1;
						}
					}
					else if (Data[i][4] >= 50)
					{
						SJR.push_back(Data[i][0]);
						std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
							" ����� � ������� 3 (SJR) �� ����������" << std::endl;
					}
					else
					{
						FIFO2.push(Data[i][0]);
						std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
							" ����� � ������� 2 (FIFO) �� ����������" << std::endl;
						if (RunningQueue == 3)
						{
							SJR.push_back(Running);
							FIFO2.pop();
							std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
								" �� ������� 2 (FIFO) �������� ������� c ������� " << Data[Running][1] << " �� ������� 3 (SJR) " << std::endl;
							Running = Data[i][0];
							RunningQueue = 2;
						}
					}

					if ((FIFO1.size() == 1) && (Running == -1))
					{
						FIFO1.pop();
						Running = Data[i][0];
						RunningQueue = 1;
						std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
							" ������� � ��������� ���������� �� ������� 1 (FIFO)" << std::endl;
					}
					else if ((FIFO2.size() == 1) && (FIFO1.size() == 0) && (Running == -1))
					{
						FIFO2.pop();
						Running = Data[i][0];
						RunningQueue = 2;
						std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
							" ������� � ��������� ���������� �� ������� 2 (FIFO)" << std::endl;
					}
					else if ((SJR.size() == 1) && (FIFO1.size() == 0) && (FIFO2.size() == 0) && (Running == -1))
					{
						SJR.pop_front();
						Running = Data[i][0];
						RunningQueue = 3;
						std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
							" ������� � ��������� ����������  �� ������� 3 (SJR)" << std::endl;
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
					std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
						" ������� �������� ����������" << std::endl;
				}
				else if ((RunningQueue == 1) && (TimeSlice == 20))
				{
					Flag = true;
					Running = -1;
					RunningQueue = -1;
					TimeSlice = 0;
					FIFO2.push(Data[i][0]);
					std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
						" �� ���������� � ���������� ����� ������� (10 ������) � ��������� � ������� 2 (FIFO)" << std::endl;
				}
				else if ((RunningQueue == 2) && (TimeSlice == 30))
				{
					Flag = true;
					Running = -1;
					RunningQueue = -1;
					TimeSlice = 0;
					SJR.push_back(Data[i][0]);
					std::cout << "���� " << Time << ": ������� � ������� " << Data[i][1] <<
						" �� ���������� � ���������� ����� ������� (15 ������) � ��������� � ������� 3 (SJR)" << std::endl;
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

	std::cout << "����� ���������� ������ �� ���������� ���������: " << Time << std::endl;
	std::cout << "������� ����� ��������: " << TimeWaitAvg << " ������� ����� ����������: " << TimeToDoAvg << std::endl;
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
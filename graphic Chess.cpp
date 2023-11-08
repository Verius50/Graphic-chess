#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdio>
#include <string>
#include <cmath>

using namespace sf;


RenderWindow window(VideoMode(800, 600), "Graphic Chess", ContextSettings::Debug);

Texture textures[15];
Sprite sprites[35];
FloatRect BloudingBoxes[32];

class piece		//фигура
{
private:
	int xcord, ycord;	//координаты
	std::string pc;		//какая фигура
	bool color;			//цвет
	bool be = true;		//на доске или нет
public:
	piece(int x, int y, std::string p, bool b) : xcord(x), ycord(y), pc(p), color(b) {}	//конструктор
	void display(float x, float y, int num)		//вывод
	{
		sprites[num + 1].setPosition(x, y);
		BloudingBoxes[num] = sprites[num + 1].getGlobalBounds();
		window.draw(sprites[num + 1]);
	}
	bool getpos(int x, int y) const
	{
		return x == xcord && y == ycord && be == 1;
	}		//возвращаем true если фигура по координатам
	void move(int x, int y)		//передвинуть фигуру
	{
		xcord = x;
		ycord = y;
	}
	void set(int n)
	{
		switch (n)
		{
		case 9:pc[0] = '9';
			break;
		case 3:pc[0] = '3';
			break;
		case 2:pc[0] = '2';
			break;
		case 5:pc[0] = '5';
			break;
		}

	}
	void remove(int num)		//удалить фигуру(сбить)
	{
		be = 0;
		switch (num / 8)
		{
		case 0:
			sprites[num + 1].setPosition(Vector2f(500, num * 30));
			break;
		case 1:
			sprites[num + 1].setPosition(Vector2f(550, (num-8) * 30));
			break;
		case 2:
			sprites[num + 1].setPosition(Vector2f(550, (num-8) * 30 + 10));
			break;
		case 3:
			sprites[num + 1].setPosition(Vector2f(500, (num-16)  * 30 + 10));
			break;
		}
		sprites[num + 1].setScale(0.5, 0.5);
	}
	bool ispawn()
	{
		return pc[0] == '1';
	}
	bool getcond() const		//возвращает состояние фигуры(на доске или сбита)
	{
		return be;
	}
	bool getcolor() const		//цвет фигуры
	{
		return color;
	}
	bool canmove(int x, int y, bool b)		//может ли фигура пойти по координатам
	{
		if (pc == "1W")			//для белой пешки
		{
			if (!b)
				if ((x == xcord && y == ycord - 1) || (x == xcord && y == 4))
					return true;
				else return false;
			if (b)
				if ((x == xcord + 1 || x == xcord - 1) && y == ycord - 1)
					return true;
				else return false;
		}
		if (pc == "1B")		//для чёрной пешки
		{
			if (!b)
				if ((x == xcord && y == ycord + 1) || (x == xcord && y == 3))
					return true;
				else return false;
			if (b)
				if ((x == xcord + 1 || x == xcord - 1) && y == ycord + 1)
					return true;
				else return false;
		}
		if (pc[0] == '2')		//для коней
		{
			if ((x == xcord + 1 && (y == ycord + 2 || y == ycord - 2)) || (x == xcord - 1 && (y == ycord + 2 || y == ycord - 2)) || (x == xcord + 2 && (y == ycord + 1 || y == ycord - 1)) || (x == xcord - 2 && (y == ycord + 1 || y == ycord - 1)))
				return true;
			else return false;
		}
		if (pc[0] == '3')		//для слонов
		{
			if (abs(x - xcord) == abs(y - ycord))
				return true;
			else return false;
		}
		if (pc[0] == '5')		//для ладей
		{
			if ((x != xcord && y == ycord) || (x == xcord && y != ycord))
				return true;
			else return false;
		}
		if (pc[0] == '9')		//для ферзей
		{
			if ((x != xcord && y == ycord) || (x == xcord && y != ycord) || (abs(x - xcord) == abs(y - ycord)))
				return true;
			else return false;
		}
		if (pc[0] == '0')		//для королей
		{
			if (abs(x - xcord) < 2 && abs(y - ycord) < 2)
				return true;
			else return false;
		}

		return 0;
	}
	Vector2i getxypos()
	{
		return Vector2i(xcord, ycord);
	}
};

piece* pieces[32];		//32 указателя на обьекты фигур, не std::vector чтобы можно было использовать конструктор


class board		//доска, её вывод на экран
{
private:
	bool pbl;
public:
	void draw(bool mv)		//рисуем доску
	{
		for (int i = 0; i < 8; i++)		//строки
			for (int n = 0; n < 8; n++)		//столбцы
			{
				for (int j = 0; j < 32; j++)
					if (mv) {
						if (pieces[j]->getpos(n, i))
						{
							pieces[j]->display(float(40 + n * 53), float(40 + i * 53), j);
							break;
						}
					}
					else
						if (pieces[j]->getpos(n, i))
						{
							pieces[j]->display(float(410 - (n * 53)), float(410 - (i * 53)), j);
							break;
						}		
			}
		for (int n = 0; n < 32; n++)	//вывести сбытие фигуры
			if (!pieces[n]->getcond())
				window.draw(sprites[n + 1]);
	}
};

int getnumxy(int x, int y)		//узнаёт номер фигуры по координатам
{
	for (int i = 0; i < 32; i++)
		if (pieces[i]->getpos(x, y))
			return i;
	return -1;
}

bool pathclear(int x1, int x2, int y1, int y2, int nm)		//проверяет, есть ли на пуи фигуры другие фигуры
{
	if (nm == 0 || nm == 7 || nm == 24 || nm == 31)		//для ладей 
	{
		if (x1 == x2)
		{
			for (int j = y1 - 1; j > y2; j--)
			{
				for (int n = 0; n < 32; n++)
					if (pieces[n]->getpos(x1, j))
						return false;
			}
			for (int j = y1 + 1; j < y2; j++)
			{
				for (int n = 0; n < 32; n++)
					if (pieces[n]->getpos(x1, j))
						return false;
			}
		}
		if (y1 == y2)
		{
			for (int j = x1 - 1; j > x2; j--)
			{
				for (int n = 0; n < 32; n++)
					if (pieces[n]->getpos(j, y1))
						return false;
			}
			for (int j = x1 + 1; j < x2; j++)
			{
				for (int n = 0; n < 32; n++)
					if (pieces[n]->getpos(j, y1))
						return false;
			}
		}
	}
	if (nm == 2 || nm == 5 || nm == 26 || nm == 29)			//для слонов			
	{
		if (y2 > y1)
		{
			if (x2 > x1)
			{
				for (int n = x1 + 1; n < x2; n++)
					for (int i = 0; i < 32; i++)
						if (pieces[i]->getpos(n, y1 + (n - x1)))
							return false;
			}
			else
				for (int n = x1 - 1; n > x2; n--)
					for (int i = 0; i < 32; i++)
						if (pieces[i]->getpos(n, y1 + (x1 - n)))
							return false;
		}
		else
			if (x2 > x1)
			{
				for (int n = x1 + 1; n < x2; n++)
					for (int i = 0; i < 32; i++)
						if (pieces[i]->getpos(n, y1 - (n - x1)))
							return false;
			}
			else
				for (int n = x1 - 1; n > x2; n--)
					for (int i = 0; i < 32; i++)
						if (pieces[i]->getpos(n, y1 - (x1 - n)))
							return false;

	}
	if (nm == 3 || nm == 27)				//для ферзей
	{
		if (x1 == x2)
		{
			for (int j = y1 - 1; j > y2; j--)
			{
				for (int n = 0; n < 32; n++)
					if (pieces[n]->getpos(x1, j))
						return false;
			}
			for (int j = y1 + 1; j < y2; j++)
			{
				for (int n = 0; n < 32; n++)
					if (pieces[n]->getpos(x1, j))
						return false;
			}
		}
		if (y1 == y2)
		{
			for (int j = x1 - 1; j > x2; j--)
			{
				for (int n = 0; n < 32; n++)
					if (pieces[n]->getpos(j, y1))
						return false;
			}
			for (int j = x1 + 1; j < x2; j++)
			{
				for (int n = 0; n < 32; n++)
					if (pieces[n]->getpos(j, y1))
						return false;
			}
		}

		if (y2 > y1&&x1 != x2)
		{
			if (x2 > x1)
			{
				for (int n = x1 + 1; n < x2; n++)
					for (int i = 0; i < 32; i++)
						if (pieces[i]->getpos(n, y1 + (n - x1)))
							return false;
			}
			else
				for (int n = x1 - 1; n > x2; n--)
					for (int i = 0; i < 32; i++)
						if (pieces[i]->getpos(n, y1 + (x1 - n)))
							return false;
		}
		else
			if (x2 > x1&&y1 != y2)
			{
				for (int n = x1 + 1; n < x2; n++)
					for (int i = 0; i < 32; i++)
						if (pieces[i]->getpos(n, y1 - (n - x1)))
							return false;
			}
			else
				for (int n = x1 - 1; n > x2; n--)
					for (int i = 0; i < 32; i++)
						if (pieces[i]->getpos(n, y1 - (x1 - n)))
							return false;
	}
	return true;	//если препятствий не оказалось
}

bool correctin(int x1, int y1, int x2, int y2, bool mv)		//проверяем, правильные ли координаты ввёл пользователь
{
	if(!mv)
	{
		x2 = 7 - x2; y2 = 7 - y2;
	}
	int num1 = getnumxy(x1, y1), num2 = getnumxy(x2, y2);	//порядковые номера двух фигур (какая идёт, и куда идёт)
	if (num1 == -1)		//если ходит пустое место, повторяем ввод 
		return false;
	bool b = 0;
	if (num2 != -1)
		b = true;		//ты хочешь ходить туда, где есть фигура (для ходов пешек)
	if (pieces[num1]->getcolor() == mv)		//если цвет фигуры, которой ходят, совпадает с цветом ходящего игрока
	{
		if (pieces[num1]->canmove(x2, y2, b) && pathclear(x1, x2, y1, y2, num1))		//если фигура может туда пойти
		{
			if (num2 == -1)		//если там пустое место
			{
				pieces[num1]->move(x2, y2);
				return true;	//двигаем фигуру
			}
			if (pieces[num1]->getcolor() != pieces[num2]->getcolor()) //если ты ходишь на фигуру противоположного цвета, то сбиваем её
			{
				pieces[num2]->remove(num2);
				pieces[num1]->move(x2, y2);
				return true;
			}
			else { std::cout << "\nThere an another your piece!"; return false; }		//если ходишь на свою фигуру
		}
		else { if (x1 != x2 || y1 != y2) std::cout << "\nYou can't go there!"; return false; }	//если фигура не может туда пойти
	}
	else { std::cout << "\nNow is not your turn!"; return false; }		//если сейчас не твой ход
}

int ifend()		//дошли ли какая-нибудь пешка до края доски
{
	for (int j = 8; j < 16; j++)
		for (int i = 0; i < 8; i++)
			if (pieces[j]->getpos(i, 7) && pieces[j]->ispawn())
				return j;
	for (int j = 16; j < 24; j++)
		for (int i = 0; i < 8; i++)
			if (pieces[j]->getpos(i, 0) && pieces[j]->ispawn())
				return j;
	return -1;
}

int rouding(float value)	//округляет число
{
	if ((value - (int)value) >= 0.5f)
	{
		value = (int)value;
		value++;
	}
	else value = (int)value;
	return value;
}


int main()
{
	textures[0].loadFromFile("images/ChessBoard.png");
	textures[1].loadFromFile("images/bB.png");
	textures[2].loadFromFile("images/bK.png");
	textures[3].loadFromFile("images/bN.png");
	textures[4].loadFromFile("images/bP.png");
	textures[5].loadFromFile("images/bQ.png");
	textures[6].loadFromFile("images/bR.png");
	textures[7].loadFromFile("images/wB.png");
	textures[8].loadFromFile("images/wK.png");
	textures[9].loadFromFile("images/wN.png");
	textures[10].loadFromFile("images/wP.png");
	textures[11].loadFromFile("images/wQ.png");
	textures[12].loadFromFile("images/wR.png");
	textures[13].loadFromFile("images/select.png");
	textures[14].loadFromFile("images/table.jpg");
	
	sprites[0].setTexture(textures[0]);
	sprites[0].setOrigin(Vector2f(249, 249));
	sprites[0].setPosition(Vector2f(249, 249));
	sprites[33].setTexture(textures[13]);
	sprites[34].setTexture(textures[14]);
	for (size_t i = 1; i < 33; i++)
	{
		if (i > 8 && i < 17)	//пешкиs
			sprites[i].setTexture(textures[4]);
		if (i > 16 && i < 25)
			sprites[i].setTexture(textures[10]);

		if (i == 1 || i == 8)	//ладьи
			sprites[i].setTexture(textures[6]);
		if (i == 25 || i == 32)
			sprites[i].setTexture(textures[12]);

		if (i == 2 || i == 7)	//кони
			sprites[i].setTexture(textures[3]);
		if (i == 26 || i == 31)
			sprites[i].setTexture(textures[9]);

		if (i == 3 || i == 6)	//слоны
			sprites[i].setTexture(textures[1]);
		if (i == 27 || i == 30)
			sprites[i].setTexture(textures[7]);
		sprites[i].setScale(0.6f, 0.6f);
	}
	sprites[4].setTexture(textures[5]);
	sprites[5].setTexture(textures[2]);
	sprites[28].setTexture(textures[11]);
	sprites[29].setTexture(textures[8]);

	board b;
	for (int i = 0; i < 64; i++)		//инициализируем указатели фигурами
	{
		if (i > 7 && i < 16)	//пешки
			pieces[i] = new piece((i % 8), (i / 8), "1B", 0);
		if (i > 47 && i < 56)
			pieces[i - 32] = new piece((i % 8), (i / 8), "1W", 1);

		if (i == 0 || i == 7)	//ладьи
			pieces[i] = new piece((i % 8), (i / 8), "5B", 0);
		if (i == 56 || i == 63)
			pieces[i - 32] = new piece((i % 8), (i / 8), "5W", 1);

		if (i == 1 || i == 6)	//кони
			pieces[i] = new piece((i % 8), (i / 8), "2B", 0);
		if (i == 57 || i == 62)
			pieces[i - 32] = new piece((i % 8), (i / 8), "2W", 1);

		if (i == 2 || i == 5)	//слоны
			pieces[i] = new piece((i % 8), (i / 8), "3B", 0);
		if (i == 58 || i == 61)
			pieces[i - 32] = new piece((i % 8), (i / 8), "3W", 1);
	}
	pieces[3] = new piece((3), (0), "9B", 0);		//ферзи
	pieces[27] = new piece((3), (7), "9W", 1);
	pieces[4] = new piece((4), (0), "0B", 0);		//короли
	pieces[28] = new piece((4), (7), "0W", 1);
	bool move = true;		//первые ходят белые
	

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed) 
				window.close();
		}
		window.draw(sprites[34]);
		window.draw(sprites[0]);
		
		b.draw(move);
		
		for (size_t i = 1; i < 33; i++)
		{
			if ((move && i < 17) || (!move && i > 16))
				continue;
			if (BloudingBoxes[i - 1].contains(Vector2f(Mouse::getPosition(window))) && pieces[i - 1]->getcond())
			{
				sprites[33].setPosition(sprites[i].getPosition());
				window.draw(sprites[33]);
				while (Mouse::isButtonPressed(Mouse::Left))
				{
					window.clear();
					window.draw(sprites[34]);
					for (size_t n = 0; n < 33; n++)
					{
						if(n != i)
							window.draw(sprites[n]);
					}
					sprites[i].setPosition(Vector2f(Mouse::getPosition(window)) - Vector2f(20, 20));
					window.draw(sprites[i]);
					window.display();
					if (!Mouse::isButtonPressed(Mouse::Left))
					{
						if (correctin(pieces[i - 1]->getxypos().x, pieces[i - 1]->getxypos().y,
							rouding((sprites[i].getPosition().x - 40) / 53), rouding((sprites[i].getPosition().y - 40) / 53), move))
						{
							move = !move;
							sprites[0].rotate(180);
						}
					}
				}
			}
		}
		window.display();
		window.clear();	
	}
	return 0;
}
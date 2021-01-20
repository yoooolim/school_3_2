#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "string"

using namespace std;

string r[13] = {
	"Ace","Two","Three","Four",
	"Five", "Six","Seven","Eight",
	"Nine","Ten","Jack", "Queen", "King"
};

string s[4] = { "Spades","Hearts","Clubs","Diamonds" };

class Card {
private:
	int value;
	string rank;
	string suit;

public:
	void set_rank(string r) { rank = r; };
	void set_suit(string s) { suit = s; };
	void set_value(int v) { value = v; };

	// getter functions
	string get_rank() { return rank; };
	string get_suit() { return suit; };
	int get_value() { return value; };
};

void shuffle(vector <Card> &d);
void NewCard(vector <Card> &d);
Card dealfrom(vector <Card> &d, int data[]); 
int HitStatus(vector <Card> &card, int data[]);
int calCard(int data[]);
int SumofNum(vector <Card> &c);
int RankToNum(string rank);
float Betting(int data[],vector <Card> &P, vector <Card> &D);

int main() {
	srand(time(0));
	Card temp;
	vector <Card> deck(312);
	vector <Card> hand(0);
	int playnum = 0;
	int count;
	bool playagain = true;
	int CardData[13] = { 0, };

	int result_Card_Counting_Player[3] = { 0, };
	int result_Simple_Player[3] = { 0, };

	int MoneyP =100000;
	int MoneySP = 100000;

	float PlayerBetting = 0.0;

	vector <Card> DealCard;
	vector <Card> PlayCard;
	vector <Card> SPlayCard;

	// assigning rank and suit and value to each Card object in vector deck
	NewCard(deck);
	/*for (Card c : deck) {
		cout << c.get_value() << " " << c.get_rank() << " " << c.get_suit() << endl;
	}
	cout << "==========================================" << endl;*/
	shuffle(deck);

	cout << "Play Game : ";
	cin >> playnum;

	for (int play = 0; play < playnum; play++) {

		if (deck.size() < (312 * 0.2)) {
			for (int i = 0; i < 13; i++) {
				CardData[i] = 0;
			}
			NewCard(deck);
			shuffle(deck);
		}

		PlayCard.push_back(dealfrom(deck, CardData));
		PlayCard.push_back(dealfrom(deck, CardData));

		DealCard.push_back(dealfrom(deck, CardData));
		DealCard.push_back(dealfrom(deck, CardData));

		SPlayCard.push_back(dealfrom(deck, CardData));
		SPlayCard.push_back(dealfrom(deck, CardData));

		PlayerBetting = Betting(CardData,PlayCard,DealCard);

		playagain = true;
		while (playagain) {
			if (SumofNum(PlayCard) >= 17)	break;
			int result = HitStatus(PlayCard,CardData);
			//Hit
			if (result == 0) {
				PlayCard.push_back(dealfrom(deck, CardData));
			}
			//Stand
			else {
				playagain = false;
			}
		}

		playagain = true;
		while (playagain) {
			if (SumofNum(SPlayCard) < 17)
				SPlayCard.push_back(dealfrom(deck, CardData));
			else
				playagain = false;
		}

		playagain = true;
		while (playagain) {
			if (SumofNum(DealCard) < 17)
				DealCard.push_back(dealfrom(deck, CardData));
			else
				playagain = false;
		}

		/* Compare the sum of Dealer's card to Player's Card*/
		/* When Dealer win => lose money that betted*/
		/* When Player win => get money that betted twice */
		/* When Player black jack win => get money that betted x 2.5*/
		if (SumofNum(PlayCard) == 21 && PlayCard.size() == 2) {
			//Both Black jack => Draw
			if (SumofNum(DealCard) == 21 && DealCard.size() == 2) result_Card_Counting_Player[1]++;
			//Only Player Black Jack => Player win
			else {
				result_Card_Counting_Player[0]++;
				MoneyP += PlayerBetting*1.5;
			}
		}
		else if (SumofNum(DealCard) < SumofNum(PlayCard)) {
			// Player Bust => Dealer win
			if (SumofNum(PlayCard) > 21) { 
				result_Card_Counting_Player[2]++;
				MoneyP -= PlayerBetting; 
			}
			//Player Not Bust and sum of card is bigger => Player win
			else { 
				result_Card_Counting_Player[0]++;
				MoneyP += PlayerBetting; 
			}
		}
		else {
			//Dealer Bust & Player not Bust => Player win
			if (SumofNum(DealCard) > 21 && SumofNum(PlayCard) <= 21) {
				result_Card_Counting_Player[0]++;
				MoneyP += PlayerBetting;
			}
			//When Dealer's card sum == Card Counting Player's card sum => Draw
			else if (SumofNum(DealCard) == SumofNum(PlayCard))	result_Card_Counting_Player[1]++;
			//Both Not Bust and Deal's card sum is bigger & Both Bust => Dealer win
			else
			{
				result_Card_Counting_Player[2]++;
				MoneyP -= PlayerBetting;
			}
			//When Dealer's card sum == Card Counting Player's card sum => Draw
		}

		/* Compare the sum of Dealer's card to Simple Player's Card*/
		/* When Dealer win => lose 100*/
		/* When Simple Player win => get 200 */
		/* When Player black jack win => get 250*/
		if (SumofNum(SPlayCard) == 21 && SPlayCard.size() == 2) {
			if (SumofNum(DealCard) == 21 && DealCard.size() == 2) result_Simple_Player[1]++;
			else {
				result_Simple_Player[0]++;
				MoneySP += 100 * 1.5;
			}
		}
		else if (SumofNum(DealCard) < SumofNum(SPlayCard)) {
			if (SumofNum(SPlayCard) > 21) { 
				result_Simple_Player[2]++;
				MoneySP -= 100; 
			}
			else { 
				result_Simple_Player[0]++;
				MoneySP += 100; 
			}
		}
		else {
			if (SumofNum(DealCard) > 21 && SumofNum(SPlayCard) <= 21) {
				result_Simple_Player[0]++;
				MoneySP += 100;
			}
			else if (SumofNum(DealCard) == SumofNum(SPlayCard)) result_Simple_Player[1]++;
			else { 
				result_Simple_Player[2]++;
				MoneySP -= 100; 
			}
		}
		PlayCard.clear();
		DealCard.clear();
		SPlayCard.clear();
	}

	cout << "Card counting Player win : " << result_Card_Counting_Player[0] << " / draw : " << result_Card_Counting_Player[1] << " / ½Â·ü : " << ((float)result_Card_Counting_Player[0]) / ((float)playnum - (float)result_Card_Counting_Player[1]) * 100.0 << "% / ³²Àº µ· : " << MoneyP << endl;
	cout << "Simple Player win : " << result_Simple_Player[0] << " / draw : " << result_Simple_Player[1] << " / ½Â·ü : " << ((float)result_Simple_Player[0]) / ((float)playnum - (float)result_Simple_Player[1]) * 100.0 << "% / ³²Àº µ· : " << MoneySP << endl;
	
	int seegood = 0;
	cin >> seegood ;
	return 0;
}// end of main()

 //function definitions
void shuffle(vector <Card> &d) {
	Card temp;
	for (int i = 0; i < d.size(); i++) {
		int r = rand() % d.size();
		temp = d[i];
		d[i] = d[r];
		d[r] = temp;
	}

}

void NewCard(vector <Card> &d) {
	d.resize(312);
	for (int j = 0; j < 6; j++) {
		for (int i = 0; i < 52; i++) {
			if (i % 13 == 0)
				d[52 * j + i].set_value(11);
			else if (i % 13 >= 9)
				d[52 * j + i].set_value(10);
			else
				d[52 * j + i].set_value(i % 13 + 1);
			d[52 * j + i].set_rank(r[i % 13]);
			d[52 * j + i].set_suit(s[i / 13]);
		}
	}
}

Card dealfrom(vector <Card> &d, int data[]) {
	Card temp;
	temp = d[d.size() - 1];
	d.pop_back();
	data[RankToNum(temp.get_rank())]++;
	return temp;
}

/* return 0 => Hit */
/* return 1 => Stand */
int HitStatus(vector <Card> &card, int data[])
{
	if (SumofNum(card) <= 11)
		return 0;
	int sum_all = 0;
	int sum_card = 0;
	int count = 0;
	for (int i = 0; i < 13; i++) {
		sum_all += data[i];
		if (i <= (21 - SumofNum(card))) {
			sum_card += data[i];
			count++;
		}
	}
	float check = ((6.0 * 4.0 * (float)count - (float)sum_card) / (312.0 - (float)sum_all)) * 100.0;
	//if too low card sum => Hit
	if (check>50)
		return 0;
	return 1;
}

int calCard(int data[])
{
	int sum = 0;
	sum -= data[0];
	for (int i = 1; i < 6; i++) sum += data[i];
	for (int i = 9; i < 13; i++) sum -= data[i];
	return sum;
}

int SumofNum(vector<Card>& c)
{
	int sum = 0;
	int ISAce=0;
	for (int i = 0; i < c.size(); i++) {
		sum += c[i].get_value();
	}
	if (sum > 21) {
		for (int i = 0; i < c.size(); i++) {
			if (c[i].get_rank() == "Ace") ISAce++;
		}
		//if There is Ace card
		while (ISAce>0) {
			if(sum>21)	sum -= 10;
			ISAce--;
		}
	}
	return sum;
}

int RankToNum(string rank) {
	if (rank == "Ace")
		return 0;
	else if (rank == "Two")
		return 1;
	else if (rank == "Three")
		return 2;
	else if (rank == "Four")
		return 3;
	else if (rank == "Five")
		return 4;
	else if (rank == "Six")
		return 5;
	else if (rank == "Seven")
		return 6;
	else if (rank == "Eight")
		return 7;
	else if (rank == "Nine")
		return 8;
	else if (rank == "Ten")
		return 9;
	else if (rank == "Jack")
		return 10;
	else if (rank == "Queen")
		return 11;
	else
		return 12;
}

float Betting(int data[], vector <Card> &P, vector <Card> &D)
{
	int dataSum = 0;
	for (int i = 0; i < 13; i++)	dataSum += data[i];
	float check = ((float)calCard(data) / (312.0 - (float)dataSum)*10.0)*100.0;

	if (check <= 100) check = 100;

	if (SumofNum(D) == 21)	return 100;
	else if(SumofNum(D) < 17) {
		if (calCard(data) >= 0)	return 100;
		else {
			return check;
		}
	}
	else {
		if(SumofNum(P)>SumofNum(D))
			return check;
		else {
			if (calCard(data) <= 0)	return 100;
			else
				return check;
		}
	}
}
